//
//  ESP8266wifiClass.cpp
//
//
//  Created by Jonas Ekstrand on 2015-02-20.
//
//

#include "ESP8266wifi.h"
#ifndef DEBUG
#define DEBUG false
#endif // !DEBUG

//Setup I2C display 
#define I2C_ADDR 0x3F  // Define I2C Address where the PCF8574A is
const byte BACKLIGHT_PIN PROGMEM = 3;
const byte En_pin PROGMEM = 2;
const byte Rw_pin PROGMEM = 1;
const byte Rs_pin PROGMEM = 0;
const byte D4_pin PROGMEM = 4;
const byte D5_pin PROGMEM = 5;
const byte D6_pin PROGMEM = 6;
const byte D7_pin PROGMEM = 7;

const byte TouchSensor PROGMEM = 8;
unsigned long previousOndisplay = 0;
boolean lastState = LOW;
boolean hasLCDOn = false;

//lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin, BACKLIGHT_PIN, POSITIVE);  // Set the LCD I2C address

extern void turnOnOffLCD(long time, boolean force) {
	if (force) {
		lcd.on();
		lcd.backlight();
		hasLCDOn = true;
		previousOndisplay = millis();
	}
	else {
		if (hasLCDOn && (unsigned long)(time - previousOndisplay) >= 5000) {
			lcd.off();
			hasLCDOn = false;
		}
	}
};

extern void LCDTracking() {
	boolean currentState = LOW;
	currentState = digitalRead(TouchSensor);
	//currentState = digitalRead(TouchSensor);
	if (DEBUG  && currentState == HIGH && lastState == LOW) {
		Serial.println("Touch");
	}
	turnOnOffLCD(millis(), currentState == HIGH && lastState == LOW);
	lastState = currentState;
};


TimedAction touchThread = TimedAction(100, LCDTracking);

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif
const char OK[] PROGMEM = "OK";
const char AT[] PROGMEM = "AT";
const char FAIL[] PROGMEM = "FAIL";
const char ERROR[] PROGMEM = "ERROR";
const char NO_CHANGE[] PROGMEM = "no change";

const char SEND_OK[] PROGMEM = "SEND OK";
const char LINK_IS_NOT[] PROGMEM = "link is not";
const char LINK_IS_BUILDED[] PROGMEM = "link is builded";
const char PROMPT[] PROGMEM = ">";
const char BUSY[] PROGMEM = "busy";
const char LINKED[] PROGMEM = "Linked";
const char ALREADY[] PROGMEM = "ALREAY";//yes typo in firmware..
const char READY[] PROGMEM = "ready";
const char NO_IP[] PROGMEM = "0.0.0.0";

const char CIPSEND[] PROGMEM = "AT+CIPSEND=";
const char CIPSERVERSTART[] PROGMEM = "AT+CIPSERVER=1,";
const char CIPSERVERSTOP[] PROGMEM = "AT+CIPSERVER=0";
//const char CIPSTART[] PROGMEM = "AT+CIPSTART=4,\"";
//const char CIPCLOSE[] PROGMEM = "AT+CIPCLOSE=4";
const char CIPSTART[] PROGMEM = "AT+CIPSTART=1,\"";
const char CIPCLOSE[] PROGMEM = "AT+CIPCLOSE=1";
const char TCP[] PROGMEM = "TCP";
const char UDP[] PROGMEM = "UDP";

const char CWJAP[] PROGMEM = "AT+CWJAP=\"";

const char CWMODE_1[] PROGMEM = "AT+CWMODE=1";
const char CWMODE_3[] PROGMEM = "AT+CWMODE=3";
const char CWMODE_CHECK[] PROGMEM = "AT+CWMODE?";
const char CWMODE_OK[] PROGMEM = "+CWMODE:1";

const char CIFSR[] PROGMEM = "AT+CIFSR";
const char CIPMUX_1[] PROGMEM = "AT+CIPMUX=1";

const char ATE0[] PROGMEM = "ATE0";
const char ATE1[] PROGMEM = "ATE1";

const char CWSAP[] PROGMEM = "AT+CWSAP=\"";

const char IPD[] PROGMEM = "IPD,";
const char CONNECT[] PROGMEM = "CONNECT";
const char CLOSED[] PROGMEM = "CLOSED";

const char COMMA[] PROGMEM = ",";
const char COMMA_1[] PROGMEM = "\",";
const char COMMA_2[] PROGMEM = "\",\"";
const char THREE_COMMA[] PROGMEM = ",3";
const char DOUBLE_QUOTE[] PROGMEM = "\"";
const char EOL[] PROGMEM = "\n";

const char STAIP[] PROGMEM = "STAIP,\"";
const char STAMAC[] PROGMEM = "STAMAC,\"";

//Define POST DATA
const char EOL_COMMAND[] PROGMEM = "\r\n";
const char POST[] PROGMEM = "POST ";
const char HTTP[] PROGMEM = " HTTP/1.1";
const char HOST[] PROGMEM = "Host: ";
const char CONTENT_LENGTH[] PROGMEM = "Content-Length: ";
const char CONTENT_TYPE[] PROGMEM = "Content-Type: application/x-www-form-urlencoded\r\n";

Stream *ESP8266wifiClass::_serialIn;
inline long mid_num(String& s) {
	return strtol(&s[s.indexOf('[') + 1], nullptr, 10);
}
ESP8266wifiClass::ESP8266wifiClass() {}
void ESP8266wifiClass::init(Stream *serialIn, byte resetPin) {
	ESP8266wifiClass::_serialIn = serialIn;
	_resetPin = resetPin;

	pinMode(_resetPin, OUTPUT);
	digitalWrite(_resetPin, LOW);//Start with radio off

	flags.connectToServerUsingTCP = true;
	flags.endSendWithNewline = true;
	flags.started = false;
	flags.localServerConfigured = false;
	flags.localApConfigured = false;
	flags.apConfigured = false;
	flags.serverConfigured = false;
	flags.isReady = false;
	flags.tcpServerOpen = false;

	flags.debug = false;
	flags.echoOnOff = false;

	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		_connections[i].channel = i + 0x30; // index to ASCII 
		_connections[i].connected = false;
	}
}

void ESP8266wifiClass::endSendWithNewline(bool endSendWithNewline) {
	flags.endSendWithNewline = endSendWithNewline;
}

bool ESP8266wifiClass::begin() {
	msgOut[0] = '\0';
	msgIn[0] = '\0';
	flags.connectedToServer = false;
	flags.localServerConfigured = false;
	flags.localApConfigured = false;
	serverRetries = 0;

	//Do a HW reset
	bool statusOk = false;
	byte i;
	/*
	Reset ESP wifi module.
	for(i =0; i<HW_RESET_RETRIES; i++){
		readCommand(10, NO_IP); //Cleanup
		digitalWrite(_resetPin, LOW);
		delay(500);
		digitalWrite(_resetPin, HIGH); // select the radio
		// Look for ready string from wifi module
		statusOk = readCommand(3000, READY) == 1;
		if(statusOk)
			break;
	}*/
	statusOk = isReady();
	if (!statusOk)
		return false;

	//Turn local AP = off
	//writeCommand(CWMODE_1, EOL);
	/*
	if (readCommand(1000, OK, NO_CHANGE) == 0)
		return false;

	// Set echo on/off
	if(flags.echoOnOff)//if echo = true
		writeCommand(ATE1, EOL);
	else
		writeCommand(ATE0, EOL);
	if (readCommand(1000, OK, NO_CHANGE) == 0)
		return false;

	// Set mux to enable multiple connections
	*/
	writeCommand(CIPMUX_1, EOL);
	flags.started = readCommand(3000, OK, LINK_IS_BUILDED) > 0;

	//flags.started = statusOk;
	//Serial.println(flags.started);
	return flags.started;
}

bool ESP8266wifiClass::isStarted() {
	return flags.started;
}

bool ESP8266wifiClass::restart() {
	return begin()
		&& (!flags.localApConfigured || startLocalAp())
		&& (!flags.localServerConfigured || startLocalServer())
		&& (!flags.apConfigured || connectToAP())
		&& (!flags.serverConfigured || connectToServer());
}


bool ESP8266wifiClass::connectToAP(const char* ssid, const char* password) {//TODO make timeout config or parameter??
	strncpy_P(_ssid, ssid, sizeof _ssid);
	strncpy_P(_password, password, sizeof _password);
	flags.apConfigured = true;
	//Serial.println("connectToAP _ PASS");
	return connectToAP();
}

bool ESP8266wifiClass::connectToAP() {
	writeCommand(CWJAP);
	_serialIn->print(_ssid);
	writeCommand(COMMA_2);
	_serialIn->print(_password);
	if (flags.debug) {
		Serial.println("connectToAP _ 1");
	}
	writeCommand(DOUBLE_QUOTE, EOL);

	byte code = readCommand(15000, OK, FAIL);
	if (code != 1) {
		return false;
	}
	//Serial.println("connectToAP");
	return isConnectedToAP();
}

bool ESP8266wifiClass::isConnectedToAP() {
	writeCommand(CIFSR, EOL);
	byte code = readCommand(350, NO_IP, ERROR);
	readCommand(10, OK); //cleanup
	Serial.print("CODE:");
	Serial.println(code);
	return code == 0;
}

bool ESP8266wifiClass::isReady()
{
	if (flags.isReady) return flags.isReady;

	bool ready = false;
	if (flags.debug) {
		Serial.println(F("Check is Ready"));
	}
	while (!ready) {
		writeCommand(AT, EOL);
		String recv = readResponse(2000);
		if (recv.indexOf("OK") != -1) {
			return true;
		}
		delay(2000);
	}

	return flags.isReady = ready;
}

char* ESP8266wifiClass::getIP() {
	msgIn[0] = '\0';
	writeCommand(CIFSR, EOL);
	byte code = readCommand(1000, STAIP, ERROR);
	if (code == 1) {
		// found staip
		readBuffer(&msgIn[0], sizeof(msgIn) - 1, '"');
		readCommand(10, OK, ERROR);
		return &msgIn[0];
	}
	readCommand(1000, OK, ERROR);
	return &msgIn[0];
}

char* ESP8266wifiClass::getMAC() {
	msgIn[0] = '\0';
	writeCommand(CIFSR, EOL);
	byte code = readCommand(1000, STAMAC, ERROR);
	if (code == 1) {
		// found stamac
		readBuffer(&msgIn[0], sizeof(msgIn) - 1, '"');
		readCommand(10, OK, ERROR);
		return &msgIn[0];
	}
	readCommand(1000, OK, ERROR);
	return &msgIn[0];
}

char* ESP8266wifiClass::debug(String command)
{
	return readResponse(command, 5000);
}


void ESP8266wifiClass::setTransportToUDP() {
	flags.connectToServerUsingTCP = false;
}

void ESP8266wifiClass::setTransportToTCP() {
	flags.connectToServerUsingTCP = true;
}

void ESP8266wifiClass::isDebug(bool debug)
{
	flags.debug = debug;
}

bool ESP8266wifiClass::connectToServer(const char* ip, const char* port) {//TODO make timeout config or parameter??
	strncpy(_ip, ip, sizeof _ip);
	strncpy(_port, port, sizeof _port);
	flags.serverConfigured = true;
	return connectToServer();
}


bool ESP8266wifiClass::connectToServer() {
	//Set multiple connection 
	writeCommand(CIPMUX_1, EOL);

	writeCommand(CIPSTART);
	if (flags.connectToServerUsingTCP)
		writeCommand(TCP);
	else
		writeCommand(UDP);
	writeCommand(COMMA_2);
	_serialIn->print(_ip);
	writeCommand(COMMA_1);
	_serialIn->println(_port);

	flags.connectedToServer = (readCommand(10000, LINKED, ALREADY) > 0);

	if (flags.connectedToServer)
		serverRetries = 0;
	return flags.connectedToServer;
}


void ESP8266wifiClass::disconnectFromServer() {
	flags.connectedToServer = false;
	flags.serverConfigured = false;//disable reconnect
	writeCommand(CIPCLOSE);
	readCommand(2000, OK); //fire and forget in this case..
}


bool ESP8266wifiClass::isConnectedToServer() {
	if (flags.connectedToServer)
		serverRetries = 0;
	return flags.connectedToServer;
}

bool ESP8266wifiClass::startLocalAPAndServer(const char* ssid, const char* password, const char* channel, const char* port) {
	strncpy(_localAPSSID, ssid, sizeof _localAPSSID);
	strncpy(_localAPPassword, password, sizeof _localAPPassword);
	strncpy(_localAPChannel, channel, sizeof _localAPChannel);
	strncpy(_localServerPort, port, sizeof _localServerPort);

	flags.localApConfigured = true;
	flags.localServerConfigured = true;
	return startLocalAp() && startLocalServer();
}

bool ESP8266wifiClass::startLocalAP(const char* ssid, const char* password, const char* channel) {
	strncpy(_localAPSSID, ssid, sizeof _localAPSSID);
	strncpy(_localAPPassword, password, sizeof _localAPPassword);
	strncpy(_localAPChannel, channel, sizeof _localAPChannel);

	flags.localApConfigured = true;
	return startLocalAp();
}

bool ESP8266wifiClass::startLocalServer(const char* port) {
	strncpy(_localServerPort, port, sizeof _localServerPort);
	flags.localServerConfigured = true;
	return startLocalServer();
}

bool ESP8266wifiClass::startLocalServer() {
	// Start local server
	writeCommand(CIPSERVERSTART);
	_serialIn->println(_localServerPort);

	flags.localServerRunning = (readCommand(2000, OK, NO_CHANGE) > 0);
	return flags.localServerRunning;
}

bool ESP8266wifiClass::startLocalAp() {
	// Start local ap mode (eg both local ap and ap)
	writeCommand(CWMODE_3, EOL);
	if (!readCommand(2000, OK, NO_CHANGE))
		return false;

	// Configure the soft ap
	writeCommand(CWSAP);
	_serialIn->print(_localAPSSID);
	writeCommand(COMMA_2);
	_serialIn->print(_localAPPassword);
	writeCommand(COMMA_1);
	_serialIn->print(_localAPChannel);
	writeCommand(THREE_COMMA, EOL);

	flags.localApRunning = (readCommand(5000, OK, ERROR) == 1);
	return flags.localApRunning;
}

bool ESP8266wifiClass::stopLocalServer() {
	writeCommand(CIPSERVERSTOP, EOL);
	boolean stopped = (readCommand(2000, OK, NO_CHANGE) > 0);
	flags.localServerRunning = !stopped;
	flags.localServerConfigured = false; //to prevent autostart
	return stopped;
}

bool ESP8266wifiClass::stopLocalAP() {
	writeCommand(CWMODE_1, EOL);

	boolean stopped = (readCommand(2000, OK, NO_CHANGE) > 0);
	flags.localApRunning = !stopped;
	flags.localApConfigured = false; //to prevent autostart
	return stopped;
}

bool ESP8266wifiClass::stopLocalAPAndServer() {
	return stopLocalAP() && stopLocalServer();
}

bool ESP8266wifiClass::isLocalAPAndServerRunning() {
	return flags.localApRunning & flags.localServerRunning;
}

// Performs a connect retry (or hardware reset) if not connected
bool ESP8266wifiClass::watchdog() {
	if (serverRetries >= SERVER_CONNECT_RETRIES_BEFORE_HW_RESET) {
		// give up, do a hardware reset
		return restart();
	}
	if (flags.serverConfigured && !flags.connectedToServer) {
		serverRetries++;
		if (flags.apConfigured && !isConnectedToAP()) {
			if (!connectToAP()) {
				// wait a bit longer, then check again
				delay(2000);
				if (!isConnectedToAP()) {
					return restart();
				}
			}
		}
		return connectToServer();
	}
	return true;
}

/*
 * Send string (if channel is connected of course)
 */
bool ESP8266wifiClass::send(char channel, const char * message, bool sendNow) {
	watchdog();
	byte avail = sizeof(msgOut) - strlen(msgOut) - 1;
	strncat(msgOut, message, avail);
	if (!sendNow)
		return true;
	byte length = strlen(msgOut);

	if (flags.endSendWithNewline)
		length += 2;

	writeCommand(CIPSEND);
	_serialIn->print(channel);
	writeCommand(COMMA);
	_serialIn->println(length);
	byte prompt = readCommand(1000, PROMPT, LINK_IS_NOT);
	if (prompt != 2) {
		if (flags.endSendWithNewline)
			_serialIn->println(msgOut);
		else
			_serialIn->print(msgOut);
		byte sendStatus = readCommand(5000, SEND_OK, BUSY);
		if (sendStatus == 1) {
			msgOut[0] = '\0';
			if (channel == SERVER)
				flags.connectedToServer = true;
			return true;
		}
	}
	//else
	if (channel == SERVER)
		flags.connectedToServer = false;
	else
		_connections[channel - 0x30].connected = false;
	msgOut[0] = '\0';
	return false;
}


// Checks to see if there is a client connection
bool ESP8266wifiClass::isConnection(void) {
	WifiConnection *connections;

	// return the first channel, assume single connection use
	return checkConnections(&connections);
}

// Updates private connections struct and make passed pointer point to data
bool ESP8266wifiClass::checkConnections(WifiConnection **pConnections) {
	watchdog();
	// setup buffers on stack & copy data from PROGMEM pointers
	char buf1[16] = { '\0' };
	char buf2[16] = { '\0' };
	char buf3[16] = { '\0' };
	strcpy_P(buf1, CONNECT);
	strcpy_P(buf2, READY);
	strcpy_P(buf3, CLOSED);
	byte len1 = strlen(buf1);
	byte len2 = strlen(buf2);
	byte len3 = strlen(buf3);
	byte pos = 0;
	byte pos1 = 0;
	byte pos2 = 0;
	byte pos3 = 0;
	byte ret = 0;
	char ch = '-';

	// unload buffer and check match
	while (_serialIn->available()) {
		char c = readChar();
		// skip white space
		if (c != ' ') {
			// get out of here if theres a message
			if (c == '+')
				break;
			// first char is channel
			if (pos == 0)
				ch = c;
			pos++;
			pos1 = (c == buf1[pos1]) ? pos1 + 1 : 0;
			pos2 = (c == buf2[pos2]) ? pos2 + 1 : 0;
			pos3 = (c == buf3[pos3]) ? pos3 + 1 : 0;
			if (len1 > 0 && pos1 == len1) {
				ret = 1;
				break;
			}
			if (len2 > 0 && pos2 == len2) {
				ret = 2;
				break;
			}
			if (len3 > 0 && pos3 == len3) {
				ret = 3;
				break;
			}
		}
	}

	if (ret == 2)
		restart();

	// new connection
	if (ret == 1) {
		_connections[ch - 0x30].connected = true;
		*pConnections = _connections;
		if (ch == SERVER)
			flags.connectedToServer = true;
		return 1;
	}

	// channel disconnected
	if (ret == 3) {
		_connections[ch - 0x30].connected = false;
		*pConnections = _connections;
		if (ch == SERVER)
			flags.connectedToServer = false;
		return 0;
	}

	// nothing has changed return single connection status
	*pConnections = _connections;
	return _connections[0].connected;
}

WifiMessage ESP8266wifiClass::listenForIncomingMessage(int timeout) {
	watchdog();
	char buf[16] = { '\0' };
	msgIn[0] = '\0';

	static WifiMessage msg;

	msg.hasData = false;
	msg.channel = '-';
	msg.message = msgIn;

	//TODO listen for unlink etc...
	byte msgOrRestart = readCommand(timeout, IPD, READY);

	//Detected a esp8266 restart
	if (msgOrRestart == 2) {
		restart();
		return msg;
	}
	//Message received..
	else if (msgOrRestart == 1) {
		char channel = readChar();
		if (channel == SERVER)
			flags.connectedToServer = true;
		readChar(); // removing comma
		readBuffer(&buf[0], sizeof(buf) - 1, ':'); // read char count
		readChar(); // removing ':' delim
		byte length = atoi(buf);
		readBuffer(&msgIn[0], min(length, sizeof(msgIn) - 1));
		msg.hasData = true;
		msg.channel = channel;
		msg.message = msgIn;
		readCommand(10, OK); // cleanup after rx
	}
	return msg;
}

WifiMessage ESP8266wifiClass::getIncomingMessage(void) {
	watchdog();
	char buf[16] = { '\0' };
	msgIn[0] = '\0';

	static WifiMessage msg;

	msg.hasData = false;
	msg.channel = '-';
	msg.message = msgIn;

	// See if a message has come in (block 1s otherwise misses?)
	byte msgOrRestart = readCommand(10, IPD, READY);

	//Detected a esp8266 restart
	if (msgOrRestart == 2) {
		restart();
		return msg;
	}
	//Message received..
	else if (msgOrRestart == 1) {
		char channel = readChar();
		if (channel == SERVER)
			flags.connectedToServer = true;
		readChar(); // removing comma
		readBuffer(&buf[0], sizeof(buf) - 1, ':'); // read char count
		readChar(); // removing ':' delim
		byte length = atoi(buf);
		readBuffer(&msgIn[0], min(length, sizeof(msgIn) - 1));
		msg.hasData = true;
		msg.channel = channel;
		msg.message = msgIn;
		readCommand(10, OK); // cleanup after rx
	}
	return msg;
}

// Writes commands (from PROGMEM) to serial output
void ESP8266wifiClass::writeCommand(const char* text1 = NULL, const char* text2) {
	char buf[100] = { '\0' };
	strcpy_P(buf, (char *)text1);
	_serialIn->print(buf);
	//Serial.println("Send Command: ");
	//Serial.println(buf);
	if (text2 == EOL) {
		_serialIn->println();
	}
	else if (text2 != NULL) {
		strcpy_P(buf, (char *)text2);
		_serialIn->print(buf);
	}
}

// Reads from serial input until a expected string is found (or until timeout)
// NOTE: strings are stored in PROGMEM (auto-copied by this method)
byte ESP8266wifiClass::readCommand(int timeout, const char* text1, const char* text2) {
	// setup buffers on stack & copy data from PROGMEM pointers
	char buf1[16] = { '\0' };
	char buf2[16] = { '\0' };
	if (text1 != NULL)
		strcpy_P(buf1, (char *)text1);
	if (text2 != NULL)
		strcpy_P(buf2, (char *)text2);
	byte len1 = strlen(buf1);
	byte len2 = strlen(buf2);
	byte pos1 = 0;
	byte pos2 = 0;
	// read chars until first match or timeout
	unsigned long stop = millis() + timeout;
	do {
		while (_serialIn->available()) {
			char c = readChar();
			pos1 = (c == buf1[pos1]) ? pos1 + 1 : 0;
			pos2 = (c == buf2[pos2]) ? pos2 + 1 : 0;
			if (len1 > 0 && pos1 == len1)
				return 1;
			if (len2 > 0 && pos2 == len2)
				return 2;
		}
		touchThread.check();
		delay(10);
	} while (millis() < stop);
	return 0;
}


String ESP8266wifiClass::readResponse(int timeout)
{
	String tmpData = "";
	long int time = millis();
	while ((time + timeout) > millis()) {
		while (_serialIn->available() > 0) {
			int c = _serialIn->read();
			if ((char)c == '\0') continue;
			tmpData += (char)c;
		}
	}
	if (flags.debug)
	{
		//Serial.println(tmpData);
	}
	tmpData.trim();
	return tmpData;
}

char* ESP8266wifiClass::readResponse(String ATCommand, int timeout)
{
	long int time = millis();
	_serialIn->print(ATCommand + EOL);
	msgIn[0] = '\0';
	// found stamac
	readBuffer(&msgIn[0], sizeof(msgIn) - 1, '"');
	return &msgIn[0];
}

// Unload buffer without delay
/*byte ESP8266wifiClass::readCommand(const char* text1, const char* text2) {
	// setup buffers on stack & copy data from PROGMEM pointers
	char buf1[16] = {'\0'};
	char buf2[16] = {'\0'};
	if (text1 != NULL)
		strcpy_P(buf1, (char *) text1);
	if (text2 != NULL)
		strcpy_P(buf2, (char *) text2);
	byte len1 = strlen(buf1);
	byte len2 = strlen(buf2);
	byte pos1 = 0;
	byte pos2 = 0;

	// read chars until first match or timeout
	while (_serialIn->available()) {
		char c = readChar();
		pos1 = (c == buf1[pos1]) ? pos1 + 1 : 0;
		pos2 = (c == buf2[pos2]) ? pos2 + 1 : 0;
		if (len1 > 0 && pos1 == len1)
			return 1;
		if (len2 > 0 && pos2 == len2)
			return 2;
	}
	return 0;
}*/

// Reads count chars to a buffer, or until delim char is found
byte ESP8266wifiClass::readBuffer(char* buf, byte count, char delim) {
	byte pos = 0;
	while (_serialIn->available() && pos < count) {
		if (_serialIn->peek() == delim)
			break;
		buf[pos++] = readChar();
	}
	buf[pos] = '\0';
	return pos;
}

// Reads a single char from serial input (with debug printout if configured)
char ESP8266wifiClass::readChar() {
	char c = (char)_serialIn->read();
	if (flags.debug)
		Serial.print(c);
	else
		delayMicroseconds(50); // don't know why
	return c;
}


void ESP8266wifiClass::rx_empty(void)
{
	while (_serialIn->available() > 0) {
		_serialIn->read();
	}
}

/**
Custom by Hidro
*/

/**
 *	url: "/abc/"
 *  query: key=value&key2=value2
 */
bool ESP8266wifiClass::httpGET(const char* host, const char* port, String & uri, String& query) {
	if (isConnectedToServer()) {
		connectToServer(host, port);
	}
	if (isConnectedToServer()) {
		String httpQuery = "GET " + uri + "?" + query + " HTTP/1.1\r\nHost: " + host + ":" + String(port) + "\r\nConnection: close\r\n\r\n";
		send(WEBSITE, (&httpQuery)->c_str(), true);
		disconnectFromServer();
	}
}

/**
*	url: "/abc/"
*	query: key=value&key2=value2
*/
bool ESP8266wifiClass::httpPOST(const char* host, const char* port, const char* uri,const char* query) {

	watchdog();
	//esp.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80"); //start a TCP connection.
	while (!isConnectedToServer()) {
		if (flags.debug) {
			Serial.println(F("Connect To Server"));
		}
		connectToServer(host, port);
	}
	if (isConnectedToServer()) {
		byte sendStatus;
		if (flags.debug) {
			Serial.println(F("Connected To Server"));
			Serial.println(F("Before Send"));
		}
		
		uint8_t	length = 93;
		//length += 5; // strlen("POST "); //5
		length += strlen(uri);
		//length += 11; //strlen(" HTTP/1.1\r\n"); //11
		//length += 6; //strlen("Host: "); //6
		length += strlen(host);
		//length += 2; //strlen("\r\n");
		//length += 16; //strlen("Content-Length: "); //16
		uint8_t queryLength = strlen(query);
		uint8_t _queryLength = queryLength;
		uint8_t lengthCount = 0;
		for (; queryLength != 0; queryLength /= 10, lengthCount++) {};
		length += lengthCount;
		//delete &lengthCount;
		//delete &queryLength;
		

		//length += 2; // strlen("\r\n"); //2
		//length += 51; //strlen("Content-Type: application/x-www-form-urlencoded\r\n\r\n"); //51
		length += _queryLength;

		//delete &_queryLength;
		if (flags.endSendWithNewline)
			length += 2;
		
		writeCommand(CIPSEND);
		_serialIn->print(WEBSITE);
		writeCommand(COMMA);
		_serialIn->println(length);

		byte prompt = readCommand(1000, PROMPT, LINK_IS_NOT);
		if (flags.debug) {
			Serial.print(F("DEBUG HERE (prompt): "));
			Serial.println(prompt);
		}
		if (prompt == 1) {
			
			
			//Send Header
			writeCommand(POST);
			_serialIn->print(uri);
			writeCommand(HTTP);
			writeCommand(EOL_COMMAND);

			writeCommand(HOST);
			_serialIn->println(host);
			

			writeCommand(CONTENT_LENGTH);
			_serialIn->println(strlen(query));


			writeCommand(CONTENT_TYPE, EOL_COMMAND);
			_serialIn->println(query);

			
			byte sendStatus = readCommand(5000, SEND_OK, BUSY);
			if (sendStatus == 1) {
				//msgOut[0] = '\0';
				if (flags.debug) {
					Serial.println(F("Send Completed"));
					/*String response = _serialIn->readString();
					long num; 
					num = mid_num(response);
					Serial.print(num);
					Serial.println(F("---"));*/
				}
				//disconnectFromServer();
				return true;
			}
		}
		//disconnectFromServer();
		if (flags.debug) {
			Serial.println(F("Send failed"));
		}
	}
	//disconnectFromServer();
	return false;
}