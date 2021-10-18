//
//  ESP8266wifi.h
//
//
//  Created by Jonas Ekstrand on 2015-02-20.
//  ESP8266 AT cmd ref from https://github.com/espressif/esp8266_at/wiki/CIPSERVER
//
//

#ifndef ESP8266wifi_h
#define ESP8266wifi_h

#define HW_RESET_RETRIES 3
#define SERVER_CONNECT_RETRIES_BEFORE_HW_RESET 3

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <inttypes.h>


#include "HardwareSerial.h"

#include <LiquidCrystal_I2C.h>
#include <TimedAction.h>

//lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
extern LiquidCrystal_I2C lcd; //(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin, BACKLIGHT_PIN, POSITIVE);  // Set the LCD I2C address
extern TimedAction touchThread;// = TimedAction(100, LCDTracking);



//Define Touch Sensor ----------------- 

extern const byte TouchSensor;
extern unsigned long previousOndisplay;
extern boolean lastState;
extern boolean hasLCDOn;
//Define Touch Sensor ----------------- 

#define SERVER '4'
#define WEBSITE '1'
#define MAX_CONNECTIONS 3

#define MSG_BUFFER_MAX 128

struct WifiMessage{
public:
    bool hasData:1;
    char channel;
    char * message;
};

struct WifiConnection{
public:
    char channel;
    bool connected:1;
};

struct Flags   // 1 byte value (on a system where 8 bits is a byte
{
    bool isReady: 1, 
		 started:1, 
         echoOnOff:1, 
         debug:1, 
         serverConfigured:1,            // true if a connection to a remote server is configured
         connectedToServer:1,           // true if a connection to a remote server is established
         apConfigured:1,                // true if the module is configured as a client station
         localApConfigured:1,
         localServerConfigured:1,
         localApRunning:1, 
         localServerRunning:1, 
		 endSendWithNewline:1, 
		 tcpServerOpen: 1,
         connectToServerUsingTCP:1;
};

class ESP8266wifiClass
{
    
public:
    /*
     * Will pull resetPin low then high to reset esp8266, connect this pin to CHPD pin
     */
	ESP8266wifiClass();
    void init(Stream *serialIn, byte resetPin);
     
    /*
     * Will do hw reset and set inital configuration, will try this HW_RESET_RETRIES times.
     */
    bool begin(); // reset and set echo and other stuff
    
    bool isStarted();
    
    /*
     * Connect to AP using wpa encryption
     * (reconnect logic is applied, if conn lost or not established, or esp8266 restarted)
     */
    //bool connectToAP(String& ssid, String& password);
    bool connectToAP(const char* ssid, const char* password);
    bool isConnectedToAP();
	bool isReady();
    char* getIP();
    char* getMAC();
	char* debug(String command);
    /*
     * Connecting with TCP to server
     * (reconnect logic is applied, if conn lost or not established, or esp8266 restarted)
     */
    
    void setTransportToUDP();
    //Default..
    void setTransportToTCP();
	void isDebug(bool);
    bool connectToServer(const char* ip, const char* port);
    void disconnectFromServer();
    bool isConnectedToServer();
    
    /*
     * Starting local AP and local TCP-server
     * (reconnect logic is applied, if conn lost or not established, or esp8266 restarted)
     */
    bool startLocalAPAndServer(const char* ssid, const char* password, const char* channel,const char* port);
    bool startLocalAP(const char* ssid, const char* password, const char* channel);
    bool startLocalServer(const char* port);
    bool stopLocalAPAndServer();
    bool stopLocalAP();
    bool stopLocalServer();
    bool isLocalAPAndServerRunning();

	bool httpGET(const char* host, const char* port, String & uri, String& query);
	bool httpPOST(const char* host, const char* port, const char* uri, const char* query);
    
    
    /*
     * Send string (if channel is connected of course)
     */
    bool send(char channel, const char * message, bool sendNow = true);
    /*
     * Default is true.
     */
    void endSendWithNewline(bool endSendWithNewline);
    
    /*
     * Scan for incoming message, do this as often and as long as you can (use as sleep in loop)
     */
    WifiMessage listenForIncomingMessage(int timeoutMillis);
    WifiMessage getIncomingMessage(void);
    bool isConnection(void);
    bool checkConnections(WifiConnection **pConnections);
    
private:
	static Stream* _serialIn;
    byte _resetPin;
    
    Flags flags;
    
    bool connectToServer();
    char _ip[16];
    char _port[6];
    
    bool connectToAP();
    char _ssid[20];
    char _password[20];
    
    bool startLocalAp();
    bool startLocalServer();
    char _localAPSSID[16];
    char _localAPPassword[16];
    char _localAPChannel[3];
    char _localServerPort[6];
    WifiConnection _connections[MAX_CONNECTIONS];
    
    bool restart();
    
    byte serverRetries;
    bool watchdog();
	//bool AT_CIPSEND(const char * buffer, uint32_t len);
	//bool sendTcpData(const char *queryContent, uint32_t len);
    char msgOut[MSG_BUFFER_MAX];//buffer for send method
    char msgIn[MSG_BUFFER_MAX]; //buffer for listen method = limit of incoming message..

    void writeCommand(const char* text1, const char* text2 = NULL);
	void rx_empty(void);
    byte readCommand(int timeout, const char* text1 = NULL, const char* text2 = NULL);
	char* readResponse(String ATCommand, int timeout);
	String readResponse(int timeout);
    //byte readCommand(const char* text1, const char* text2);
    byte readBuffer(char* buf, byte count, char delim = '\0');
    char readChar();
};
extern ESP8266wifiClass ESP8266wifi;
#endif
