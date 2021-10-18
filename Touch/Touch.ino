/*
Author: Danny van den Brande. Arduinosensors.nl. BlueCore Tech.
Hello world! Today i made a code for the TTP233B capacitive touch sensor.
Its very simple and you can toggle on and off a led/relay or whatever you wanna add.
*/

#include <SoftwareSerial.h>
#define TouchSensor 9 // Pin for capactitive touch sensor

int relay = 2;

boolean currentState = LOW;
boolean lastState = LOW;
boolean RelayState = LOW;

void setup() {
	Serial.begin(9600);
	pinMode(relay, OUTPUT);
	pinMode(TouchSensor, INPUT);
}

void loop() {
	currentState = digitalRead(TouchSensor);
	if (currentState == HIGH && lastState == LOW) {
		Serial.println("pressed");
		delay(1);
	}
	lastState = currentState;
}