
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
#include <SPI.h>
#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>

#define CCS811_ADDR 0x5B //7-bit unshifted default I2C Address
#define PIN_NOT_WAKE 5

CCS811 myCCS811(CCS811_ADDR);

//Global sensor object
BME280 myBME280;
//---------------------------------------------------------------
void setup()
{
	Serial.begin(9600);
	Serial.println();
	Serial.println("Apply BME280 data to CCS811 for compensation.");

	status_t returnCode = myCCS811.begin();
	Serial.print("CCS811 begin exited with: ");
	printDriverError( returnCode );
	Serial.println();

	//For I2C, enable the following and disable the SPI section
	myBME280.settings.commInterface = I2C_MODE;
	myBME280.settings.I2CAddress = 0x77;

	//Initialize BME280
	//For I2C, enable the following and disable the SPI section
	myBME280.settings.commInterface = I2C_MODE;
	myBME280.settings.I2CAddress = 0x77;
	myBME280.settings.runMode = 3; //Normal mode
	myBME280.settings.tStandby = 0;
	myBME280.settings.filter = 4;
	myBME280.settings.tempOverSample = 5;
	myBME280.settings.pressOverSample = 5;
	myBME280.settings.humidOverSample = 5;
	
	//Calling .begin() causes the settings to be loaded
	delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
	myBME280.begin();
	

}
//---------------------------------------------------------------
void loop()
{
	if (myCCS811.dataAvailable())
	{
		myCCS811.readAlgorithmResults(); //Calling this function updates the global tVOC and CO2 variables
		printInfoSerial();

		float BMEtempC = myBME280.readTempC();
		float BMEhumid = myBME280.readFloatHumidity();

		Serial.print("Applying new values (deg C, %): ");
		Serial.print(BMEtempC);
		Serial.print(",");
		Serial.println(BMEhumid);
		Serial.println();
		
		myCCS811.setEnvironmentalData(BMEhumid, BMEtempC);
	}
	else if (myCCS811.checkForStatusError())
	{
		printSensorError();
	}

	delay(2000); //Wait for next reading
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

	Serial.println("BME280 data:");
	Serial.print(" Temperature: ");
	Serial.print(myBME280.readTempC(), 2);
	Serial.println(" degrees C");

	Serial.print(" Temperature: ");
	Serial.print(myBME280.readTempF(), 2);
	Serial.println(" degrees F");

	Serial.print(" Pressure: ");
	Serial.print(myBME280.readFloatPressure(), 2);
	Serial.println(" Pa");

	Serial.print(" Pressure: ");
	Serial.print((myBME280.readFloatPressure() * 0.0002953), 2);
	Serial.println(" InHg");

	Serial.print(" Altitude: ");
	Serial.print(myBME280.readFloatAltitudeMeters(), 2);
	Serial.println("m");

	Serial.print(" Altitude: ");
	Serial.print(myBME280.readFloatAltitudeFeet(), 2);
	Serial.println("ft");

	Serial.print(" %RH: ");
	Serial.print(myBME280.readFloatHumidity(), 2);
	Serial.println(" %");

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