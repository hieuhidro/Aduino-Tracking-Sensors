/*
Arduino Protothreading Example v1.0
by Drew Alden (@ReanimationXP) 1/12/2016
*/

//COMPONENTS

/*
This code was made using the Sunfounder Arduino starter kit's blue LCD.
It can be found at Amazon.com in a variety of kits.
*/

//THIRD-PARTY LIBRARIES

//these must be manually added to your Arduino IDE installation
//see sites for details.

//gives us the ability to do a foreach loop:
//http://playground.arduino.cc/Code/Utility
//#include <Utility.h>

//allows us to set actions to perform on separate timed intervals
//http://playground.arduino.cc/Code/TimedAction
//http://wiring.uniandes.edu.co/source/trunk/wiring/firmware/libraries/TimedAction
#include <TimedAction.h>


//NATIVE LIBRARIES

#include <LiquidCrystal_I2C.h>
/*
LiquidCrystal Library - Hello World

Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
library works with all LCD displays that are compatible with the
Hitachi HD44780 driver. There are many of them out there, and you
can usually tell them by the 16-pin interface.

This sketch prints "Hello World!" to the LCD
and shows the time.

The circuit:
* LCD RS pin to digital pin 12.

* LCD Enable pin to digital pin 11
* LCD D4 pin to digital pin 5
* LCD D5 pin to digital pin 4
* LCD D6 pin to digital pin 3
* LCD D7 pin to digital pin 2
* LCD R/W pin to ground
* LCD VSS pin to ground
* LCD VCC pin to 5V
* 10K resistor:
* ends to +5V and ground
* wiper to LCD VO pin (pin 3)

Library originally added 18 Apr 2008
by David A. Mellis
library modified 5 Jul 2009
by Limor Fried (http://www.ladyada.net)
example added 9 Jul 2009
by Tom Igoe
modified 22 Nov 2010
by Tom Igoe

This example code is in the public domain.

http://www.arduino.cc/en/Tutorial/LiquidCrystal
*/

//GLOBALS
int backlightPin = 9;   // used for backlight fading
int timerCounter = 0;   // incrementing counter. will crash eventually.
int stringNo = 0;       //which text string to show
						//                   "16 CHARACTER MAX"
char* stringArray[] = { "Check it out... ",
"I have 3 threads",
"going at once...",
"Bitches! :D     " };

//INITIALIZATION
#define I2C_ADDR 0x3F  // Define I2C Address where the PCF8574A is
const byte BACKLIGHT_PIN PROGMEM = 3;
const byte En_pin PROGMEM = 2;
const byte Rw_pin PROGMEM = 1;
const byte Rs_pin PROGMEM = 0;
const byte D4_pin PROGMEM = 4;
const byte D5_pin PROGMEM = 5;
const byte D6_pin PROGMEM = 6;
const byte D7_pin PROGMEM = 7;
//lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin, BACKLIGHT_PIN, POSITIVE);  // Set the LCD I2C address

//FUNCTIONS

//create a couple timers that will fire repeatedly every x ms
//TimedAction numberThread = TimedAction(700, incrementNumber);
TimedAction textThread = TimedAction(3000, changeText);

//this is our first task, print an incrementing number to the LCD
void incrementNumber() {
	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):
	lcd.setCursor(0, 1);
	// add one to the counter, then display it.
	timerCounter = timerCounter + 1;
	lcd.print(timerCounter);
}

//our second task, fires every few seconds and rotates text strings
void changeText() {
	// Print a message to the LCD.
	lcd.setCursor(0, 0);
	lcd.print(stringArray[stringNo]);
	//next string
	if (stringNo >= sizeof(stringArray) / sizeof(char *)) {  //nasty hack to get number of Array elements
		stringNo = 0;
		changeText();  //not sure if this is technically right, but it works?
	}
	else {
		stringNo = stringNo + 1;
	}
}


// where's our third task? well, it's the loop itself :) the task
// which repeats most often should be used as the loop. other
// tasks are able to "interrupt" the fastest repeating task.



void setup() {
	//define the LCD's number of columns and rows:
	lcd.begin(16, 2);
	//fire changeText once to paint the initial string [0]
	changeText();
}


void loop() {

	//check on our threads. based on how long the system has been
	//running, do they need to fire and do work? if so, do it!
	numberThread.check();
	textThread.check();

	//third task, fade in backlight from min to max brightness
	//in increments of 5 points:
	digitalWrite(13, HIGH);
	for (int fadeValue = 0; fadeValue <= 255; fadeValue += 10) {

		//wait a second, why am i checking on the threads here? because
		//this is a for loop. you must check on your threads during ANY
		//loops that occur, including the main one!
		//numberThread.check();
		textThread.check();

		//sets the value (range from 0 to 255):
		analogWrite(backlightPin, fadeValue);

		// wait for 20 milliseconds to see the dimming effect
		// keep delays on the main loop SHORT. these WILL prevent
		// other threads from firing on time.
		delay(20);
	}

	//fade out from max to min in increments of 5 points:
	digitalWrite(13, LOW);
	for (int fadeValue = 255; fadeValue >= 0; fadeValue -= 10) {

		//check on our threads again
		//numberThread.check();
		textThread.check();

		//sets the value (range from 0 to 255):
		analogWrite(backlightPin, fadeValue);

		//wait for 20 milliseconds to see the dimming effect
		delay(20);
	}

	/*

	For some scrolling message fun in the future...

	lcd.setCursor(15,0);  // set the cursor to column 15, line 0
	for (int positionCounter1 = 0; positionCounter1 < 26; positionCounter1++)
	{
	lcd.scrollDisplayLeft();  //Scrolls the contents of the display one space to the left.
	lcd.print(array1[positionCounter1]);  // Print a message to the LCD.
	delay(tim);  //wait for 250 microseconds
	}
	lcd.clear();  //Clears the LCD screen and positions the cursor in the upper-left corner.
	lcd.setCursor(15,1);  // set the cursor to column 15, line 1
	for (int positionCounter = 0; positionCounter < 26; positionCounter++)
	{
	lcd.scrollDisplayLeft();  //Scrolls the contents of the display one space to the left.
	lcd.print(array2[positionCounter]);  // Print a message to the LCD.
	delay(tim);  //wait for 250 microseconds
	}
	lcd.clear();  //Clears the LCD screen and positions the cursor in the upper-left corner.
	*/

}