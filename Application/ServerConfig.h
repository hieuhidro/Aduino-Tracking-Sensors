// ServerConfig.h

#ifndef _SERVERCONFIG_h
#define _SERVERCONFIG_h
#define DEBUG true

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ServerConfigClass
{
 public:
	 char _host[30];
	 char _port[7];
	 void init(const char *host, const char *port) {
		 strncpy_P(_host, host, sizeof _host);
		 strncpy_P(_port, port, sizeof _port);
		 Serial.println(F("Config Server"));
	 }
};
extern ServerConfigClass ServerConfig;
#endif

