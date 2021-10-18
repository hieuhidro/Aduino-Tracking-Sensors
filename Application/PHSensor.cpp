// 
// 
// 

#include "PHSensor.h"

double avergearray(int* arr, int number) {
	int i;
	int max, min;
	double avg;
	long amount = 0;
	if (number <= 0) {
		Serial.println("Error number for the array to avraging!/n");
		return 0;
	}
	if (number<5) {   //less than 5, calculated directly statistics
		for (i = 0; i<number; i++) {
			amount += arr[i];
		}
		avg = amount / number;
		return avg;
	}
	else {
		if (arr[0]<arr[1]) {
			min = arr[0]; max = arr[1];
		}
		else {
			min = arr[1]; max = arr[0];
		}
		for (i = 2; i<number; i++) {
			if (arr[i]<min) {
				amount += min;        //arr<min
				min = arr[i];
			}
			else {
				if (arr[i]>max) {
					amount += max;    //arr>max
					max = arr[i];
				}
				else {
					amount += arr[i]; //min<=arr<=max
				}
			}//if
		}//for
		avg = (double)amount / (number - 2);
	}//if
	return avg;
}

float PHSensorClass::initValue()
{	
	static float pHValue, voltage;
	int ArrayLenth = 20;
	int pHArray[20];
	for (int i = 0; i< 20; i++)       //Get 10 sample value from the sensor for smooth the value
	{
		pHArray[i] = analogRead(PHSensorPin);
		delay(20);
	}
	voltage = avergearray(pHArray, ArrayLenth) * 5.0 / 1024;
	pHValue = 3.5 * voltage + 0.00;
	if (DEBUG) {
		Serial.print("pH Sensor: ");
		Serial.println(pHValue);
	}
	return pHValue;
}