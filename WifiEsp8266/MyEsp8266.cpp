// 
// 
// 

#include "Myesp8266.h"

Stream *MyEsp8266Class::esp8266;

MyEsp8266Class::MyEsp8266Class() {}

void MyEsp8266Class::init(Stream *_esp8266) {
	MyEsp8266Class::esp8266 = _esp8266;
};

bool MyEsp8266Class::isReady() {
	bool ready = false;
	while (!ready) {
		String recv = getResponse("AT", 2000);
		if (recv.indexOf("OK") != -1){
			ready = true;
		}
		delay(2000);
	}
	return ready;
}

bool MyEsp8266Class::connectWiFi(String _ssid, String _pwd)
{
	ssid = _ssid;
	pwd = _pwd;
	Serial.println("Connect Wifi: ");
	if (isReady()) {
		if (isDebug) {
			Serial.println("ESP8266 Is Ready.");
		}
		reset();
		configMode(1);
		while (!AT_CWJAP(ssid, pwd)) {
			if (isDebug) {
				Serial.println("Error trying to join in access point " + ssid + " try again...");
			}
			delay(2000);
		}
		if (isDebug) {
			Serial.println("Connected in accesspoint: " + ssid);
		}
		delay(5000);
		AT_CIFSR(ipMac);
		if (isDebug) {
			Serial.println("IP/MAC: " + ipMac);
		}
		status = 1;
		return true;
	}
	else {
		if (isDebug) {
			Serial.println("Error on connect with Esp8266");
		}
	}
	return false;
}



String MyEsp8266Class::getResponse(String AT_Command, const uint32_t  timeout = 100) {

	String response = "";

	esp8266->println(AT_Command); // send the read character to the esp8266

	long int time = millis();

	while ((time + timeout) > millis())
	{
		while (esp8266->available())
		{
			// The esp has data so display its output to the serial window 
			int c = esp8266->read(); // read the next character.
			response += (char)c;
		}
	}
	if (isDebug)
	{
		Serial.print(response);
	}

	return response;
}

String MyEsp8266Class::getResponse(const uint32_t  wait = 100) {
	String tmpData = "";
	long int time = millis();
	while ((time + wait) > millis()) {
		while (esp8266->available() > 0) {
			int c = esp8266->read();
			if ((char)c == '\0') continue;
			tmpData += (char)c;
		}
	}
	tmpData.trim();
	return tmpData;
}

bool MyEsp8266Class::recvFind(String AT_Command, String target, uint32_t timeout = 100)
{
	String data_tmp;
	data_tmp = getResponse(AT_Command, timeout);
	if (data_tmp.indexOf(target) != -1) {
		return true;
	}
	return false;
}

bool MyEsp8266Class::recvFind(String target, uint32_t timeout = 100)
{
	String response = getResponse(timeout);
	
	return response.indexOf(target) != -1;
}

String MyEsp8266Class::recvString(String target, uint32_t timeout = 1000)
{
	String data = "";
	int a;
	unsigned long start = millis();
	while ((start + timeout) > millis()) {
		while (esp8266->available() > 0) {
			a = esp8266->read();
			if ((char)a == '\0') continue;
			data += (char)a;
		}
		if (data.indexOf(target) != -1) {
			break;
		}
	}
	data.trim();
	if (isDebug) {
		Serial.println(data);
	}
	return data;
}

String MyEsp8266Class::recvString(String target1, String target2, uint32_t timeout = 1000)
{
	String data = "";
	int a;
	unsigned long start = millis();
	bool isTimeout = true;
	while ((start + timeout) > millis()) {
		while (esp8266->available() > 0) {
			a = esp8266->read();
			if ((char)a == '\0') continue;
			data += (char)a;
		}
		if (data.indexOf(target1) != -1) {
			isTimeout = false;
			break;
		}
		else if (data.indexOf(target2) != -1) {
			isTimeout = false;
			break;
		}
	}
	if (isTimeout) {
		if (isDebug) {
			Serial.println("**TIMEOUT OCCURRED. Timeout " + String(timeout) + " was insufficient");
		}
	}
	data.trim();
	if (isDebug) {
		Serial.println(data);
	}
	return data;
}

String MyEsp8266Class::recvString(String target1, String target2, String target3, uint32_t timeout)
{
	String data = "";
	int a;
	unsigned long start = millis();
	while ((start + timeout) > millis()) {
		while (esp8266->available() > 0) {
			a = esp8266->read();
			if ((char)a == '\0') continue;
			data += a;
		}
		if (data.indexOf(target1) != -1) {
			if (isDebug) {
				Serial.println("saiu " + target1);
			}
			break;
		}
		else if (data.indexOf(target2) != -1) {
			if (isDebug) {
				Serial.println("saiu " + target2);
			}
			break;
		}
		else if (data.indexOf(target3) != -1) {
			if (isDebug) {
				Serial.println("saiu " + target3);
			}
			break;
		}
	}
	if (isDebug) {
		Serial.println(data);
	}
	return data;
}

void MyEsp8266Class::rx_empty(void)
{
	while (esp8266->available() > 0) {
		esp8266->read();
	}
}

bool MyEsp8266Class::reset(void)
{
	return recvFind("AT+RST", "ready", 2000);
}

void MyEsp8266Class::configMode(uint32_t  mode)
{
	AT_CWMODE(mode);
}

void MyEsp8266Class::configMultiConnection(uint32_t  mode) {
	AT_CIPMUX(mode);
}

/* Join Access Point */
bool MyEsp8266Class::AT_CWJAP(String ssid, String pwd)
{
	String data;
	rx_empty();

	String cmd = "AT+CWJAP=\"";
	cmd += ssid;
	cmd += "\",\"";
	cmd += pwd;
	cmd += "\"";

	
	data = getResponse(cmd, 5000);
	return data.indexOf("OK") != -1;
}

/* Check IP address */
bool MyEsp8266Class::AT_CIFSR(String &list)
{
	rx_empty();
	String target = "OK";
	String begin = "\r\r\n";
	String end = "\r\n\r\nOK";

	String response = getResponse("AT+CIFSR", 3000);
	if (response.indexOf(target) != -1) {
		int32_t index1 = response.indexOf(begin);
		int32_t index2 = response.indexOf(end);
		if (index1 != -1 && index2 != -1) {
			index1 += begin.length();
			list = response.substring(index1, index2);
			return true;
		}
	}
	list = "";
	return false;
}

/* Set TCP/UDP Connections mode
1: Query
2: Single
3: Multiple
*/
bool MyEsp8266Class::AT_CIPMUX(uint8_t mode)
{
	String data;
	rx_empty();
	data = "AT+CIPMUX=" + String(mode);

	esp8266->println(mode);
	data = recvString("OK", "Link is builded");
	if (data.indexOf("OK") != -1) {
		return true;
	}
	return false;
}

/*
Set up TCP or UDP connection
*/
bool MyEsp8266Class::AT_CIPSTART(String type, String host, uint32_t  port)
{
	String data, cmd;
	rx_empty();
	cmd = "AT+CIPSTART=\"";
	cmd += type;
	cmd += "\",\"";
	cmd += host;
	cmd += "\",";
	cmd += String(port);
	esp8266->println(cmd);
	data = recvString("OK", "ERROR", "ALREADY CONNECT", 1000);
	Serial.println("***");
	Serial.println(data);
	Serial.println("***");
	if (data.indexOf("OK") != -1 || data.indexOf("ALREADY CONNECT") != -1) {
		return true;
	}
	return false;
}

/*
* Close TCP / UDP connection
*/
bool MyEsp8266Class::AT_CIPCLOSE(void)
{
	rx_empty();
	return recvFind("AT+CIPCLOSE", "OK", 1000);
}

/**
* Send TCP/IP data
*/
bool MyEsp8266Class::AT_CIPSEND(const uint8_t * buffer, uint32_t len)
{
	rx_empty();
	String cmd = "AT+CIPSEND=" + String(len);
	if (recvFind(cmd, ">", 1000)) {
		rx_empty();
		for (uint32_t i = 0; i < len; i++) {
			esp8266->write(buffer[i]);
		}
		return recvFind("SEND OK");
	}
	return false;
}

void MyEsp8266Class::AT_CWMODE(uint32_t mode)
{
	String cmd = "AT+CWMODE=" + String(mode);
	String response = getResponse(cmd, 1000);
	if (isDebug) {
		Serial.println(response);
	}
}


bool MyEsp8266Class::sendTcpData(char *hello) {
	if (AT_CIPSEND((const uint8_t*)hello, strlen(hello))) {
		Serial.println("Sended tcp data");
	}
	else {
		Serial.println("error in send tcp data");
		Serial.println("Not connected. Use openTcpSocket(host, port) before");
	}
}

bool MyEsp8266Class::closeTcpSocket() {
	if (AT_CIPCLOSE()) {
		status = 1;
		Serial.println("Connection closed successfully");
		return true;
	}
	else {
		status = 1;
		Serial.println("Connection closed with error");
		return false;
	}
}

bool MyEsp8266Class::openTcpSocket(String host, int port) {
	if (AT_CIPSTART("TCP", host, port)) {
		status = 2;
	}
}

bool MyEsp8266Class::httpGET(String host, int port, String query) {
	openTcpSocket(host, port);
	char *data;
	String httpQuery = "GET " + query + " HTTP/1.1\r\nHost: " + host + ":" + String(port) + "\r\nConnection: close\r\n\r\n";
	Serial.println(httpQuery);
	httpQuery.toCharArray(data, httpQuery.length());
	sendTcpData(data);
	closeTcpSocket();
}


bool MyEsp8266Class::httpPOST(String host, int port, String query) {

	//esp.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80"); //start a TCP connection.
	openTcpSocket(host, port);
	char *data;
	String postRequest =

		"POST " + query + " HTTP/1.0\r\n" +

		"Host: " + host + "\r\n" +

		"Accept: *" + "/" + "*\r\n" +

		"Content-Length: " + query.length() + "\r\n" +

		"Content-Type: application/x-www-form-urlencoded\r\n" +

		"\r\n" + query;

	postRequest.toCharArray(data, postRequest.length());
	sendTcpData(data);
	closeTcpSocket();
}

MyEsp8266Class MyEsp8266;