// 
// 
// 

#include "SHTHSensor.h"

float SHTHSensorClass::initValue()
{
	_curValue = -40;
	int counter = 40;
	while (0 > (int)_curValue  && counter > 0) {
		_curValue = this->_sensor->readHumidity();
		counter--;
		if (counter == 1) {
			_curValue = -1000;
		}
	}
	if (DEBUG) {
		Serial.print(F("Do Am: "));
		Serial.println(_curValue);
	}
	return _curValue;
}

SHTHSensorClass::SHTHSensorClass(const char*uniq) :SensorClient(uniq)
{	
}

void SHTHSensorClass::init(SHT1x & sht)
{
	_sensor = &sht;
}

