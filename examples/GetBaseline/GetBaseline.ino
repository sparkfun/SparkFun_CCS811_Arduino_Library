/*
  CCS811 Air Quality Sensor Example Code
  By: Nathan Seidle
  SparkFun Electronics
  Date: February 7th, 2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Reads the baseline number from the CCS811. You are supposed to gather this value
  when the sensor is in normalized clean air and use it to adjust (calibrate) the sensor 
  over the life of the sensor as it changes due to age.

  Hardware Connections (Breakoutboard to Arduino):
  3.3V = 3.3V
  GND = GND
  SDA = A4
  SCL = A5
  WAKE = D5 - Optional, can be left unconnected

  Serial.print it out at 9600 baud to serial monitor.
*/

#include <Wire.h>
#include "SparkFunCCS811.h"

#define CCS811_ADDR 0x5B //7-bit unshifted default I2C Address

#define WAKE 13 //!Wake on breakout connected to pin 5 on Arduino

CCS811 mySensor(CCS811_ADDR);

void setup()
{
  pinMode(WAKE, OUTPUT);
  digitalWrite(WAKE, LOW);

  Serial.begin(9600);
  Serial.println("CCS811 Baseline Example");

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
  Serial.println("Done");
  delay(1000);
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