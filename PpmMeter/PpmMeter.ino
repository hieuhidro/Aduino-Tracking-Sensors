
#include <OneWire.h>
//#include <DallasTemperature.h>

int R1 = 500;
int Ra = 25; //Resistance of powering Pins
int ECPin = A0;
int ECGround = A1;
int ECPower = A4;

float PPMconversion = 0.7;
float TemperatureCoef = 0.019;
float K = 0.04;// 2.88;

#define ONE_WIRE_BUS 10          // Data wire For Temp Probe is plugged into pin 10 on the Arduino
const int TempProbePossitive = 8;  //Temp Probe power connected to pin 9
const int TempProbeNegative = 9;    //Temp Probe Negative connected to pin 8

OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.


float Temperature = 10;
float EC = 0;
float EC25 = 0;
int ppm = 0;


float raw = 0;
float Vin = 5;
float Vdrop = 0;
float Rc = 0;
float buffer = 0;


void setup()
{
	Serial.begin(9600);
	pinMode(TempProbeNegative, OUTPUT); //seting ground pin as output for tmp probe
	digitalWrite(TempProbeNegative, LOW);//Seting it to ground so it can sink current
	pinMode(TempProbePossitive, OUTPUT);//ditto but for positive
	digitalWrite(TempProbePossitive, HIGH);
	pinMode(ECPin, INPUT);
	pinMode(ECPower, OUTPUT);//Setting pin for sourcing current
	pinMode(ECGround, OUTPUT);//setting pin for sinking current
	digitalWrite(ECGround, LOW);//We can leave the ground connected permanantly

	delay(100);// gives sensor time to settle
	//sensors.begin();
	//delay(100);
	R1 = (R1 + Ra);// Taking into acount Powering Pin Resitance

};

void loop()
{
	GetEC();
	PrintReadings();  // Cals Print routine [below main loop]
	delay(5000);
}

void GetEC() {
	//sensors.requestTemperatures();// Send the command to get temperatures
	Temperature = 29.; // sensors.getTempCByIndex(0); //Stores Value in Variable
	digitalWrite(ECPower, HIGH);
	raw = analogRead(ECPin);
	raw = analogRead(ECPin);// This is not a mistake, First reading will be low beause if charged a capacitor
	digitalWrite(ECPower, LOW);

	Vdrop = (Vin*raw) / 1024.0;
	Rc = (Vdrop*R1) / (Vin - Vdrop);
	Rc = Rc - Ra; //acounting for Digital Pin Resitance
	EC = 1000 / (Rc*K);

	EC25 = EC / (1 + TemperatureCoef*(Temperature - 25.0));
	ppm = (EC25)*(PPMconversion * 1000);


}

void PrintReadings() {
	Serial.print("Rc: ");
	Serial.print(Rc);
	Serial.print(" EC: ");
	Serial.print(EC25);
	Serial.print(" Simens  ");
	Serial.print(ppm);
	Serial.print(" ppm  ");
	Serial.print(Temperature);
	Serial.println(" *C ");
	Serial.print("Vdrop: ");
	Serial.println(Vdrop);
	Serial.print("Rc: ");
	Serial.println(Rc);
	Serial.print(EC);
	Serial.println("Siemens");
};