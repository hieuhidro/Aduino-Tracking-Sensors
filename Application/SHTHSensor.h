// STHHSensor.h

#ifndef _SHTHSENSOR_h
#define _SHTHSENSOR_h

#ifndef _SENSORCLIENT_h
	#include "SensorClient.h"
#endif // !_SENSORCLIENT_h

#ifndef SHT1x_h
	#include <SHT1x.h>
#endif // SHT1x_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class SHTHSensorClass : public SensorClient
{
 protected:
	 float initValue();
	 SHT1x *_sensor;
 public:
	 SHTHSensorClass(const char *);
	 void init(SHT1x &sht);
};
#endif

