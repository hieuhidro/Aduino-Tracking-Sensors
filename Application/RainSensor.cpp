#include "RainSensor.h"

#ifndef RAINANALOG
#define RAINANALOG A0
#endif // !RAINANALOG

float RainSensorClass::initValue()
{
	const uint8_t sensorMin = 0;     // sensor minimum
	const uint16_t sensorMax = 1024;  // sensor maximum

	int sensorReading = analogRead(RAINANALOG);
	// map the sensor range (four options):
	// ex: 'long int map(long int, long int, long int, long int, long int)'
	int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
	if (DEBUG) {
		// range value:
		switch (range) {
		case 0:    // Sensor getting wet
			Serial.println(F("Flood"));
			break;
		case 1:    // Sensor getting wet
			Serial.println(F("Rain Warning"));
			break;
		case 2:    // Sensor dry - To shut this up delete the " Serial.println("Not Raining"); " below.
			Serial.println(F("Not Raining"));
			break;
		}
	}
	return range;
}
