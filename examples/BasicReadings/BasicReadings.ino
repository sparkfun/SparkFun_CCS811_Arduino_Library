/*
  CCS811 Air Quality Sensor Example Code
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 7th, 2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Read the TVOC and CO2 values from the SparkFun CSS811 breakout board

  A new sensor requires at 48-burn in. Once burned in a sensor requires 
  20 minutes of run in before readings are considered good.

  Hardware Connections (Breakoutboard to Arduino):
  3.3V = 3.3V
  GND = GND
  SDA = A4
  SCL = A5

  Serial.print it out at 9600 baud to serial monitor.
*/

#include <Wire.h>
#include "SparkFunCCS811.h"

#define CCS811_ADDR 0x5B //7-bit unshifted default I2C Address

CCS811 mySensor(CCS811_ADDR);

void setup()
{
  Serial.begin(9600);
  Serial.println("CCS811 Read Example");

	status_t returnCode = mySensor.begin();
	Serial.print("begin exited with: ");
	printDriverError( returnCode );
	Serial.println();
	
}

void loop()
{
  if (mySensor.dataAvailable())
  {
    mySensor.readAlgorithmResults();

    Serial.print("CO2[");
    Serial.print(mySensor.CO2);
    Serial.print("] tVOC[");
    Serial.print(mySensor.tVOC);
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
