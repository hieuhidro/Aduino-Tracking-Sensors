// PHSensor.h

#ifndef _PHSENSOR_h
#define _PHSENSOR_h

#ifndef _SENSORCLIENT_h
	#include "SensorClient.h"
#endif // !_SENSORCLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
static const int8_t PHSensorPin PROGMEM = A3;
class PHSensorClass : public SensorClient
{
 protected:
	float initValue();
public: PHSensorClass() : SensorClient("Do pH") {};
};

extern PHSensorClass PHSensor;

#endif

