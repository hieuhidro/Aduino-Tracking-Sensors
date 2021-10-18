/*
Name:		WifiEsp8266.ino
Created:	3/13/2017 22:49:53
Author:	Hidro Le
*/
#include <SoftwareSerial.h>
#include "MyEsp8266.h"

SoftwareSerial _esp(10, 11); //RX, TX

							 // WIFI Settings
String ssid = "Coffee Chapel";
String pass = "136chapel";

//WiFiEspServer server(80);
//MyEsp8266 wifi(&_esp);

void setup() {
	Serial.begin(9600);
	_esp.begin(9600);
	MyEsp8266.init(&_esp);

	if (MyEsp8266.connectWiFi(ssid, pass)) {

	}
}
// the loop function runs over and over again until power down or reset
void loop() {

}