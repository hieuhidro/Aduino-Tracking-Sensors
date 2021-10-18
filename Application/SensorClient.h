// SensorClient.h

#ifndef ESP8266wifi_h
	#include "ESP8266wifi.h"
#endif // !ESP8266wifi_h

#ifndef _SERVERCONFIG_h
	#include "ServerConfig.h"
#endif // !_SERVERCONFIG_h

#ifndef _SENSORCLIENT_h
#define _SENSORCLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class SensorClient
{
 protected:
	 virtual float initValue() = 0;
	 String getUri();
	 const char * c_k();
	 const char * u_k();
	 static ServerConfigClass *_server;
	 static ESP8266wifiClass *_currentEsp;
	 String uniq;
	 static bool hasInit;
	 float _curValue;
	 int freeRam()
	 {
		 extern int __heap_start, *__brkval;
		 int v;
		 return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
	 }
 public:
	 float getValue() {
		 return _curValue;
	 }

	SensorClient(const char* uniq);
	static void initConnection(ESP8266wifiClass *_ESP, ServerConfigClass *);
	bool sendData(boolean disconnect);
};

#endif

