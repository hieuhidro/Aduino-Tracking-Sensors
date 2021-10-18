// 
// 
// 

#include "SODSensor.h"
#ifndef MEASUREPIN
	#define MEASUREPIN 5
#endif // MEASUREPIN

float SODSensorClass::initValue()
{
	const uint8_t sensorMin = 0;
	const uint16_t sensorMax = 1024;

	const uint16_t samplingTime = 280;

	float voMeasured = 0;
	float calcVoltage = 0;
	float dustDensity = 0;

	voMeasured = analogRead(MEASUREPIN); // read the dust value

	// 0 - 5.0V mapped to 0 - 1023 integer values 
	calcVoltage = voMeasured * (5.0 / 1024);

	// linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
	// Chris Nafis (c) 2012
	dustDensity = (0.17 * calcVoltage - 0.1) * 1000;

	if (DEBUG) {
		Serial.print(F("Raw Signal Value (0-1023): "));
		Serial.print(voMeasured);

		Serial.print(F(" - Voltage: "));
		Serial.print(calcVoltage);

		Serial.print(F(" - Dust Density [ug/m3]: "));
		Serial.println(dustDensity);
	}

	return dustDensity;
}
