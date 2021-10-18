// RainSensor.h

#ifndef _RAINSENSOR_h
#define _RAINSENSOR_h

#ifndef _SENSORCLIENT_h
#include "SensorClient.h"
#endif // !_SENSORCLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class RainSensorClass : public SensorClient
{
 protected:
	 float initValue();
};

extern RainSensorClass RainSensor;

#endif

