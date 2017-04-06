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

//This is the simplest example.  It throws away most error information and
//runs at the default 1 sample per second.

//#include <Wire.h>
#include "SparkFunCCS811.h"

#define CCS811_ADDR 0x5B //7-bit unshifted default I2C Address

CCS811 mySensor(CCS811_ADDR);

void setup()
{
  Serial.begin(9600);
  Serial.println("CCS811 Basic Example");

	status_t returnCode = mySensor.begin();
	if(returnCode != SENSOR_SUCCESS)
	{
		Serial.println(".begin() returned with an error.");
		while(1);
	}
}

void loop()
{
  if (mySensor.dataAvailable())
  {
    mySensor.readAlgorithmResults();

    Serial.print("CO2[");
    Serial.print(mySensor.getCO2());
    Serial.print("] tVOC[");
    Serial.print(mySensor.getTVOC());
    Serial.print("] millis[");
    Serial.print(millis());
    Serial.print("]");
    Serial.println();
  }

  delay(1000); //Wait for next reading
}
