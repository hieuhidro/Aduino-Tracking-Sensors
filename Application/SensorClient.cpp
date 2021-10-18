// 
// 
// 

#include "SensorClient.h"

bool SensorClient::hasInit = false;
ESP8266wifiClass *SensorClient::_currentEsp;
ServerConfigClass *SensorClient::_server;

String SensorClient::getUri()
{
	return "/api/sensor/results/";
}
const char* SensorClient::c_k() {
	//const char signMessage[] PROGMEM = "COM590b2b408bbfc";
	return "COM590b2b408bbfc";
}

const char *SensorClient::u_k() {
	//const char signMessage[] PROGMEM = "KEY58f4ba20ae130";
	return "KEY58f4ba20ae130";
}

SensorClient::SensorClient(const char * uniq)
{
	this->uniq = uniq;
}

void SensorClient::initConnection(ESP8266wifiClass *_ESP, ServerConfigClass* server)
{
	if (hasInit == false) {
		_currentEsp = _ESP;
		_server = server;
		hasInit = true;
	}
}

bool SensorClient::sendData(boolean disconnect)
{
	//if (DEBUG) {
	//	Serial.print(F("FREE RAM: "));
	Serial.println(freeRam());
	//	Serial.println(F("Build Query"));
	//}
	_curValue = this->initValue();
	if (-1000 != _curValue) {
		String *query = new String("sen=" + this->uniq + "&u_k=" + u_k() + "&c_k=" + c_k() + "&value=" + _curValue);
		String *uri = new String(getUri());
		if (DEBUG) {
			Serial.println(F("Begin Send Result "));
		}
		_currentEsp->httpPOST(_server->_host, _server->_port, uri->c_str(), query->c_str());
		if (disconnect) {
			_currentEsp->disconnectFromServer();
		}
		delete query;
		delete uri;
		if (DEBUG) {
			Serial.println(F("End Send Result "));
		}

		return true;
	}
	return false;
}