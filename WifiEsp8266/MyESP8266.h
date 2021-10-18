// MyEsp8266.h
#ifndef SoftwareSerial_h
	#include <SoftwareSerial.h>
#endif
#ifndef _MYESP8266_h
#define _MYESP8266_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class MyEsp8266Class
{
 protected:
	 String ssid;
	 String pwd;
	 String ipMac;
	 uint32_t status;
	 static Stream *esp8266;
	 boolean isDebug = true;

	 void rx_empty(void);
	 String getResponse(String AT_Command, const uint32_t  wait);
	 String getResponse(const uint32_t  wait);
	 bool recvFind(String AT_Command, String target, uint32_t timeout);
	 bool recvFind(String target, uint32_t timeout);
	 String recvString(String target, uint32_t timeout);
	 String recvString(String target1, String target2, uint32_t timeout);
	 String recvString(String target1, String target2, String target3, uint32_t timeout);

	 //Define AT command
	 bool AT_CIFSR(String &list);
	 bool AT_CWJAP(String _ssid, String _pwd);
	 bool AT_CIPMUX(uint8_t mode);
	 bool AT_CIPSTART(String type, String host, uint32_t  port);
	 bool AT_CIPCLOSE(void);
	 bool AT_CIPSEND(const uint8_t *buffer, uint32_t len);
	 void AT_CWMODE(uint32_t  mode);
	 bool sendTcpData(char *hello);
	 bool closeTcpSocket();
	 bool openTcpSocket(String host, int port);
 public:
	 MyEsp8266Class();
	 void init(Stream *_esp8266);
	 bool isReady();
	 bool connectWiFi(String _ssid, String _pwd);
	 void configMode(uint32_t  mode);
	 void configMultiConnection(uint32_t  mode);
	 bool httpGET(String host, int port, String query);
	 bool httpPOST(String host, int port, String query);
	 bool reset(void);
};
extern MyEsp8266Class MyEsp8266;
#endif

