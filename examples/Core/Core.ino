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

CCS811Core mySensor(CCS811_ADDR);

//These are the air quality values obtained from the sensor
unsigned int tVOC = 0;
unsigned int CO2 = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println("CCS811 Core Example");

//  configureCCS811(); //Turn on sensor
	status_t returnCode = mySensor.beginCore();
	Serial.print("beginCore exited with: ");
  switch( returnCode )
  {
	  case SENSOR_SUCCESS:
	  Serial.println("SUCCESS");
	  break;
	  case SENSOR_ID_ERROR:
	  Serial.println("ID_ERROR");
	  break;
	  case SENSOR_I2C_ERROR:
	  Serial.println("I2C_ERROR");
	  break;
	  case SENSOR_INTERNAL_ERROR:
	  Serial.println("INTERNAL_ERROR");
	  break;
	  default:
	  Serial.println("Unspecified error.");
  }
  //Write to this register to start app
  Wire.beginTransmission(CCS811_ADDR);
  Wire.write(CSS811_APP_START);
  Wire.endTransmission();

}

void loop()
{
	uint8_t arraySize = 10;
	uint8_t tempData[arraySize];
	
	tempData[0] = 0x18;
	tempData[1] = 0x27;
	tempData[2] = 0x36;
	tempData[3] = 0x45;

	mySensor.multiWriteRegister(0x11, tempData, 2);
	//mySensor.writeRegister(0x11, tempData[0]);
	
	tempData[0] = 0x00;
	tempData[1] = 0x00;
	tempData[2] = 0x00;
	tempData[3] = 0x00;
	
	mySensor.multiReadRegister(0x11, tempData, 3);
	//mySensor.readRegister(0x01, tempData);
	
  for( int i = 0; i < arraySize; i++)
  {
	  if(i%8 == 0)
	  {
		  Serial.println();
	  	  Serial.print("0x");
	  	  Serial.print(i, HEX);
	  	  Serial.print(":");
	  }

	  Serial.print(tempData[i], HEX);
	  Serial.print(" ");
  }
  
 
  Serial.println("\n");
  delay(1000); //Wait for next reading
}


