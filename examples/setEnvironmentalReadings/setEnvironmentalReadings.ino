/*
CCS811 Air Quality Sensor Example Code
By: Nathan Seidle
SparkFun Electronics
Date: February 7th, 2017
License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

Sends the humidity and temperature from a separate sensor to the CCS811 so
that the CCS811 can adjust its algorithm.

Hardware Connections (Breakoutboard to Arduino):
3.3V = 3.3V
GND = GND
SDA = A4
SCL = A5
WAKE = D5 - Optional, can be left unconnected

Serial.print it out at 9600 baud to serial monitor.
*/

//To demonstrate, the humidity and temperature are hard coded here.
//Normally, one would use a sensor to collect this data.
float temperatureVariable = 25.0; //in degrees C
float humidityVariable = 65.0; //in % relative

#include <Wire.h>
#include "SparkFunCCS811.h"

#define CCS811_ADDR 0x5B //7-bit unshifted default I2C Address
#define PIN_NOT_WAKE 5

CCS811 mySensor(CCS811_ADDR);

void setup()
{
	pinMode(PIN_NOT_WAKE, OUTPUT);
	digitalWrite(PIN_NOT_WAKE, LOW);

	Serial.begin(9600);
	Serial.println("CCS811 EnvironmentalReadings Example");

	status_t returnCode = mySensor.begin();
	Serial.print("begin exited with: ");
	printDriverError( returnCode );
	Serial.println();

	unsigned int result = mySensor.getBaseline();
	Serial.print("baseline for this sensor: 0x");
	if(result < 0x100) Serial.print("0");
	if(result < 0x10) Serial.print("0");
	Serial.println(result, HEX);

}

void loop()
{
	Serial.println();
	//Randomize the Temperature and Humidity
	humidityVariable = (float)random(0, 10000)/100; //0 to 100%
	temperatureVariable = (float)random(500, 7000)/100; // 5C to 70C
	Serial.println("New humidity and temperature:");
	Serial.print("  Humidity: ");
	Serial.print(humidityVariable, 2);
	Serial.println("% relative");
	Serial.print("  Temperature: ");
	Serial.print(temperatureVariable, 2);
	Serial.println(" degrees C");
	mySensor.setEnvironmentalData(humidityVariable, temperatureVariable);

	Serial.println("Environmental data applied!");
	mySensor.readAlgorithmResults(); //Dump a reading and wait
	delay(1000);
	//Print data points
	for( int i = 0; i < 10; i++)
	{
		if (mySensor.dataAvailable())
		{
			mySensor.readAlgorithmResults(); //Calling this function updates the global tVOC and CO2 variables

			Serial.print("CO2[");
			Serial.print(mySensor.getCO2());
			Serial.print("] tVOC[");
			Serial.print(mySensor.getTVOC());
			Serial.print("] millis[");
			Serial.print(millis());
			Serial.print("]");
			Serial.println();
		}
		else if (mySensor.checkForStatusError())
		{
			printSensorError();
		}
		delay(1000); //Wait for next reading
	}
}


//Displays the type of error
//Calling this causes reading the contents of the ERROR register
//This should clear the ERROR_ID register
void printSensorError()
{
	uint8_t error = mySensor.getErrorRegister();

	if( error == 0xFF )//comm error
	{
		Serial.println("Failed to get ERROR_ID register.");
	}
	else
	{
		Serial.print("Error: ");
		if (error & 1 << 5) Serial.print("HeaterSupply");
		if (error & 1 << 4) Serial.print("HeaterFault");
		if (error & 1 << 3) Serial.print("MaxResistance");
		if (error & 1 << 2) Serial.print("MeasModeInvalid");
		if (error & 1 << 1) Serial.print("ReadRegInvalid");
		if (error & 1 << 0) Serial.print("MsgInvalid");
		Serial.println();
	}
}

void printDriverError( status_t errorCode )
{
	switch( errorCode )
	{
	case SENSOR_SUCCESS:
		Serial.print("SUCCESS");
		break;
	case SENSOR_ID_ERROR:
		Serial.print("ID_ERROR");
		break;
	case SENSOR_I2C_ERROR:
		Serial.print("I2C_ERROR");
		break;
	case SENSOR_INTERNAL_ERROR:
		Serial.print("INTERNAL_ERROR");
		break;
	default:
		Serial.print("Unspecified error.");
	}
}

