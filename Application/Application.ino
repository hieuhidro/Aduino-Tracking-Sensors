/*
 Name:		Application.ino
 Created:	4/22/2017 17:33:33
 Author:	Hidro Le
*/
#ifndef DEBUG
#define DEBUG false
#endif // !DEBUG

#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
#include "SHTTSensor.h"
#include "SHTHSensor.h"
#include "SODSensor.h"
#include "PHSensor.h"
#include "RainSensor.h"
#include <SPI.h>
#include <MFRC522.h>
//#define sw_serial_rx_pin 10 //  Connect this pin to TX on the esp8266
//#define sw_serial_tx_pin 11 //  Connect this pin to RX on the esp8266
//#define esp8266_reset_pin 9 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)
//#define dataPin 5
//#define clockPin 6

SHT1x sht1x(5, 6);

SoftwareSerial swSerial(10, 11);
ESP8266wifiClass MyEsp8266;
ServerConfigClass _config;
const unsigned long interval PROGMEM  = 10000; // the time we need to wait
unsigned long previousMillis = 0; // millis() returns an unsigned long.

const char IP_SERVER[] PROGMEM = "139.59.105.228";
const char SERVER_PORT[] PROGMEM = "80";
const char WIFI_NAME[] PROGMEM = "TP-LINK_91D6"; // "WIFI_IOT" //  "Tenda_0A8BC8"; // "Forix"; // "Coffee Chapel"; //".SEVENDAYSCOFFEE_2", "TP-LINK_91D6"
const char WIFI_PASS[] PROGMEM = "01581522";// "1234567801"  //"12345678901"; // "FOR!x@6789"; // "136chapel"; // "7dayscoffee", "01581522"


SHTTSensorClass _shtTSensor("Nhiet Do");
SHTHSensorClass _shtHSensor("Do Am");
PHSensorClass _phSensor;

//create a couple timers that will fire repeatedly every x ms
//TimedAction sendphthread = TimedAction(10000, sendPhSensor);
//TimedAction sendHthread = TimedAction(10000, sendHSensor);
//TimedAction sendTthread = TimedAction(10000, sendTSensor);
//TimedAction sendAllThread = TimedAction(10000, sendAllSensor);


void sendPhSensor() {
	if (_phSensor.sendData(true)) {
		//lcd.setCursor(0, 1);
		lcd.print(F(" pH:"));
		lcd.print(_phSensor.getValue(), 1);
	}
}
void sendHSensor() {
	if (_shtHSensor.sendData(false)) {
		lcd.setCursor(0, 1);
		lcd.print(F("D.A:"));
		lcd.print(_shtHSensor.getValue(), 1);
	}
}
void sendTSensor() {
	if (_shtTSensor.sendData(false)) {
		lcd.setCursor(0, 0);
		lcd.print(F("Nhiet Do:"));
		lcd.print(_shtTSensor.getValue(), 1);
		lcd.print(" ");
	}
}

void sendAllSensor() {
	sendTSensor();
	touchThread.check();
	sendHSensor();
	touchThread.check();
	sendPhSensor();
}


// the setup function runs once when you press reset or power the board
void setLCD() {
	lcd.begin(16, 2);    // initialize the lcd for 16 chars 2 lines, turn on backlight
	// ------- Quick 3 blinks of backlight  -------------

	lcd.backlight(); // finish with backlight on  
	hasLCDOn = true;
					 //-------- Write characters on the display ------------------
					 // NOTE: Cursor Position: (CHAR, LINE) start at 0  
	lcd.setCursor(3, 0); //Start at character 4 on line 0
	lcd.print(F("Hello, world!"));
	delay(1000);
	lcd.setCursor(2, 1);
	lcd.print(F("From ShrimpIoT"));
	delay(1000);
	lcd.clear();
	lcd.home();
}

void setup() {
	swSerial.begin(9600);
	Serial.begin(115200);
	pinMode(TouchSensor, INPUT);
	setLCD();
	_shtTSensor.init(sht1x);
	_shtHSensor.init(sht1x);

	while (!Serial);
	
	_config.init(IP_SERVER, SERVER_PORT);

	MyEsp8266.init(&swSerial, 9);
	MyEsp8266.isDebug(DEBUG);
	SensorClient::initConnection(&MyEsp8266, &_config);
	if (DEBUG) {
		Serial.println(F("Starting wifi"));
	}
	lcd.setCursor(0, 0);
	lcd.print(F("Starting wifi"));

	MyEsp8266.setTransportToTCP();// this is also default

	//MyEsp8266.endSendWithNewline(true); // Will end all transmissions with a newline and carrage return ie println.. default is true

	while(!MyEsp8266.isStarted()) {
		MyEsp8266.begin();
		if (DEBUG) {
			Serial.println(F("Wifi Ready"));
		}
		lcd.setCursor(0, 0);
		lcd.print(F("Wifi Ready"));
	}
	while (true) {
		lcd.setCursor(0, 0);
		lcd.print(F("Connect Wifi"));
		if (MyEsp8266.connectToAP(WIFI_NAME, WIFI_PASS)) {
			lcd.setCursor(0, 0);
			lcd.print(F("Done Start App"));
			break;
		}
	}
	previousOndisplay = millis();
}

// the loop function runs over and over again until power down or reset
void loop() {
	unsigned long currentMillis = millis();

	touchThread.check();
	//sendAllThread.check();
	//LCDTracking(currentMillis);	
	if ((unsigned long)(currentMillis - previousMillis) >= interval) {
		sendAllSensor();
		previousMillis = millis();
	}
}
