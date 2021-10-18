// SODSensor.h
// Cảm biến bụi

#ifndef _SODSENSOR_h
#define _SODSENSOR_h

#ifndef _SENSORCLIENT_h
#include "SensorClient.h"
#endif // !_SENSORCLIENT_h


#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class SODSensorClass : public SensorClient
{
 protected:
		float initValue();
};
#endif

