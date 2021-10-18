// 
// 
// 

#include "SHTTSensor.h"

float SHTTSensorClass::initValue()
{
	_curValue = -40;
	int counter = 40;
	while (-40 >= (int)_curValue && -1000 < (int)_curValue  && counter > 0) {
		_curValue = this->_sensor->readTemperatureC();
		counter--;
		if (counter == 1) {
			_curValue = -1000;
		}
	}
	if (DEBUG) {
		Serial.print(F("Nhiet do: "));
		Serial.println(_curValue);
	}
	return _curValue;
}

SHTTSensorClass::SHTTSensorClass(const char* uniq):SensorClient(uniq)
{
}

void SHTTSensorClass::init(SHT1x & sht)
{
	_sensor = &sht;
}