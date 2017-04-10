
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
WAKE = D2

Serial.print it out at 9600 baud to serial monitor.
*/

#include <Wire.h>
#include <SparkFunCCS811.h>

#define CCS811_ADDR 0x5B //7-bit unshifted default I2C Address
#define PIN_TRIG 8

CCS811 myCCS811(CCS811_ADDR);

//---------------------------------------------------------------
void setup()
{
	pinMode(PIN_TRIG, OUTPUT);
	Serial.begin(9600);
	Serial.println();
	Serial.println("Apply NTC data to CCS811 for compensation.");

	status_t returnCode = myCCS811.begin();
	Serial.print("CCS811 begin exited with: ");
	printDriverError( returnCode );
	Serial.println();
	
	myCCS811.setRefResistance( 9950 );

}
//---------------------------------------------------------------
void loop()
{
	if (myCCS811.dataAvailable())
	{
		myCCS811.readAlgorithmResults(); //Calling this function updates the global tVOC and CO2 variables
		printInfoSerial();

		//float BMEtempC = myBME280.readTempC();
		//float BMEhumid = myBME280.readFloatHumidity();
        //
		//Serial.print("Applying new values (deg C, %): ");
		//Serial.print(BMEtempC);
		//Serial.print(",");
		//Serial.println(BMEhumid);
		//Serial.println();
		//
		//myCCS811.setEnvironmentalData(BMEhumid, BMEtempC);
	}
	else if (myCCS811.checkForStatusError())
	{
		printSensorError();
	}
	
	delay(500); //Wait for next reading
}

//---------------------------------------------------------------
void printInfoSerial()
{
	Serial.println("CCS811 data:");
	Serial.print(" CO2 concentration : "); 
	Serial.print(myCCS811.getCO2());
	Serial.println(" ppm");

	Serial.print(" TVOC concentration : "); 
	Serial.print(myCCS811.getTVOC());
	Serial.println(" ppb");

	myCCS811.readNTC();
	Serial.print(" Measured resistance : "); 
	Serial.print( myCCS811.getResistance() );
	Serial.println(" ohms");
	
	Serial.print(" Converted temperature : "); 

	float readTemperature = myCCS811.getTemperature();
	Serial.print( readTemperature, 2);
	Serial.println(" deg C");

	myCCS811.setEnvironmentalData( 50, readTemperature);

	Serial.println();
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
	case SENSOR_GENERIC_ERROR:
		Serial.print("GENERIC_ERROR");
		break;
	default:
		Serial.print("Unspecified error.");
	}
}

//Displays the type of error
//Calling this causes reading the contents of the ERROR register
//This should clear the ERROR_ID register
void printSensorError()
{
	uint8_t error = myCCS811.getErrorRegister();

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