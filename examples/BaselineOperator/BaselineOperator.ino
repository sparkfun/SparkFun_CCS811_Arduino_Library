/*
CCS811 Air Quality Sensor Example Code
By: Nathan Seidle
SparkFun Electronics
Date: February 7th, 2017
License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

This example demonstrates usage of the baseline register.

To use, wait until the sensor is burned in, warmed up, and in clean air.  Then,
use the terminal to save the baseline to EEPROM.  Aftewards, the sensor can be
powered up in dirty air and the baseline can be restored to the CCS811 to help
the sensor stablize faster.

EEPROM memory usage:

  addr: data
  ----------
  0x00: 0xA5
  0x01: 0xB2
  0x02: 0xnn
  0x03: 0xmm

0xA5B2 is written as an indicator that 0x02 and 0x03 contain a valid number.
0xnnmm is the saved data.

The first time used, there will be no saved data

Hardware Connections (Breakoutboard to Arduino):
  3.3V to 3.3V pin
  GND to GND pin
  SDA to A4
  SCL to A5

*/

#include <Wire.h>
#include <SparkFunCCS811.h>
#include <EEPROM.h>

#define CCS811_ADDR 0x5B //7-bit unshifted default I2C Address

CCS811 mySensor(CCS811_ADDR);

void setup()
{
	Serial.begin(9600);
	Serial.println();
	Serial.println("CCS811 Baseline Example");

	status_t returnCode = mySensor.begin();
	Serial.print("begin exited with: ");
	printDriverError( returnCode );
	Serial.println();
	
	//Check for valid data already saved
	if((EEPROM.read(0) == 0xA5)&&(EEPROM.read(1) == 0xB2))
	{
		Serial.println("EEPROM contains saved data.");
	}
	else
	{
		Serial.println("Saved data not found!");
	}
	Serial.println();

	Serial.println("Program running.  Send the following characters to operate:");
	Serial.println(" 's' - save baseline into EEPROM");
	Serial.println(" 'l' - load and apply baseline from EEPROM");
	Serial.println(" 'c' - clear baseline from EEPROM");
	Serial.println(" 'r' - read and print sensor data");

}

void loop()
{
	char c;
	unsigned int result;
	unsigned int baselineToApply;
	status_t errorStatus;
	if(Serial.available())
	{
		c = Serial.read();
		switch(c)
		{
			case 's':
				result = mySensor.getBaseline();
				Serial.print("baseline for this sensor: 0x");
				if(result < 0x100) Serial.print("0");
				if(result < 0x10) Serial.print("0");
				Serial.println(result, HEX);
				EEPROM.write(0, 0xA5);
				EEPROM.write(1, 0xB2);
				EEPROM.write(2, (result >> 8) & 0x00FF);
				EEPROM.write(3, result & 0x00FF);
				break;
			case 'l':
				if((EEPROM.read(0) == 0xA5)&&(EEPROM.read(1) == 0xB2))
				{
					Serial.println("EEPROM contains saved data.");
					baselineToApply = ((unsigned int)EEPROM.read(2) << 8) | EEPROM.read(3);
					Serial.print("Saved baseline: 0x");
					if(baselineToApply < 0x100) Serial.print("0");
					if(baselineToApply < 0x10) Serial.print("0");
					Serial.println(baselineToApply, HEX);
					errorStatus = mySensor.setBaseline( baselineToApply );
					if( errorStatus == SENSOR_SUCCESS )
					{
						Serial.println("Baseline written to CCS811.");
					}
					else
					{
						printDriverError( errorStatus );
					}
				}
				else
				{
					Serial.println("Saved data not found!");
				}
				break;
			case 'c':
				Serial.println("Clearing EEPROM space.");
				EEPROM.write(0, 0x00);
				EEPROM.write(1, 0x00);
				EEPROM.write(2, 0x00);
				EEPROM.write(3, 0x00);
				break;
			case 'r':
				if (mySensor.dataAvailable())
				{
					mySensor.readAlgorithmResults();
					
					Serial.print("CO2[");
					Serial.print(mySensor.getCO2());
					Serial.print("] tVOC[");
					Serial.print(mySensor.getTVOC());
					Serial.print("]");
					Serial.println();
				}
				else
				{
					Serial.println("Sensor data not available.");
				}
				break;
			default:
			break;
		}
	}
	delay(10);
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