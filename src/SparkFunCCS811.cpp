/******************************************************************************
SparkFunCCS811.cpp
LIS3DH Arduino and Teensy Driver

Marshall Taylor @ SparkFun Electronics
Nov 16, 2016
https://github.com/sparkfun/LIS3DH_Breakout
https://github.com/sparkfun/SparkFun_LIS3DH_Arduino_Library

Resources:
Uses Wire.h for i2c operation
Uses SPI.h for SPI operation
Either can be omitted if not used

Development environment specifics:
Arduino IDE 1.6.4
Teensy loader 1.23

This code is released under the [MIT License](http://opensource.org/licenses/MIT).

Please review the LICENSE.md file included with this example. If you have any questions 
or concerns with licensing, please contact techsupport@sparkfun.com.

Distributed as-is; no warranty is given.
******************************************************************************/
//Use VERBOSE_SERIAL to add debug serial to an existing Serial object.
//Note:  Use of VERBOSE_SERIAL adds delays surround RW ops, and should not be used
//for functional testing.
//#define VERBOSE_SERIAL

//See SparkFunCCS811.h for additional topology notes.

#include "SparkFunCCS811.h"
#include "stdint.h"

#include <Arduino.h>
#include "Wire.h"

//****************************************************************************//
//
//  LIS3DHCore functions
//
//  For I2C, construct LIS3DHCore myIMU(<address>);
//
//  Default <address> is 0x5B.
//
//****************************************************************************//
CCS811Core::CCS811Core( uint8_t inputArg ) : I2CAddress(0x5B)
{
	I2CAddress = inputArg;

}

status_t CCS811Core::beginCore(void)
{
	status_t returnError = SENSOR_SUCCESS;

	Wire.begin();

#ifdef __AVR__
#else
#endif

#ifdef __MK20DX256__
#else
#endif

	//Spin for a few ms
	volatile uint8_t temp = 0;
	for( uint16_t i = 0; i < 10000; i++ )
	{
		temp++;
	}

	while ( Wire.available() ) //Clear wire as a precaution
	{
		Wire.read();
	}
	
	//Check the ID register to determine if the operation was a success.
	uint8_t readCheck;
	readCheck = 0;
	returnError = readRegister(CSS811_HW_ID, &readCheck);

	if( returnError != SENSOR_SUCCESS ) return returnError;
	
	if( readCheck != 0x81 )
	{
		returnError = SENSOR_ID_ERROR;
	}

	return returnError;

}

//****************************************************************************//
//
//  ReadRegister
//
//  Parameters:
//    offset -- register to read
//    *outputPointer -- Pass &variable (address of) to save read data to
//
//****************************************************************************//
status_t CCS811Core::readRegister(uint8_t offset, uint8_t* outputPointer)
{
	//Return value
	uint8_t result;
	uint8_t numBytes = 1;
	status_t returnError = SENSOR_SUCCESS;

	Wire.beginTransmission(I2CAddress);
	Wire.write(offset);
	if( Wire.endTransmission() != 0 )
	{
		returnError = SENSOR_I2C_ERROR;
	}
	Wire.requestFrom(I2CAddress, numBytes);
	while ( Wire.available() ) // slave may send less than requested
	{
		result = Wire.read(); // receive a byte as a proper uint8_t
	}
	*outputPointer = result;

	return returnError;
}

//****************************************************************************//
//
//  multiReadRegister
//
//  Parameters:
//    offset -- register to read
//    *outputPointer -- Pass &variable (base address of) to save read data to
//    length -- number of bytes to read
//
//  Note:  Does not know if the target memory space is an array or not, or
//    if there is the array is big enough.  if the variable passed is only
//    two bytes long and 3 bytes are requested, this will over-write some
//    other memory!
//
//****************************************************************************//
status_t CCS811Core::multiReadRegister(uint8_t offset, uint8_t *outputPointer, uint8_t length)
{
	status_t returnError = SENSOR_SUCCESS;

	//define pointer that will point to the external space
	uint8_t i = 0;
	uint8_t c = 0;
	//Set the address
	Wire.beginTransmission(I2CAddress);
	//NOT SURE IF AUTOINC REQUIRED!!!
	//offset |= 0x80; //turn auto-increment bit on, bit 7 for I2C
	Wire.write(offset);
	if( Wire.endTransmission() != 0 )
	{
		returnError = SENSOR_I2C_ERROR;
	}
	else  //OK, all worked, keep going
	{
		// request 6 bytes from slave device
		Wire.requestFrom(I2CAddress, length);
		while ( (Wire.available()) && (i < length))  // slave may send less than requested
		{
			c = Wire.read(); // receive a byte as character
			*outputPointer = c;
			outputPointer++;
			i++;
		}
		//dump extra
		while(Wire.available())
		{
			Wire.read();
		}
	}


	return returnError;
}

//****************************************************************************//
//
//  writeRegister
//
//  Parameters:
//    offset -- register to write
//    dataToWrite -- 8 bit data to write to register
//
//****************************************************************************//
status_t CCS811Core::writeRegister(uint8_t offset, uint8_t dataToWrite) {
	status_t returnError = SENSOR_SUCCESS;
	
	Wire.beginTransmission(I2CAddress);
	Wire.write(offset);
	Wire.write(dataToWrite);
	if( Wire.endTransmission() != 0 )
	{
		returnError = SENSOR_I2C_ERROR;
	}
	return returnError;
}

//****************************************************************************//
//
//  multiReadRegister
//
//  Parameters:
//    offset -- register to read
//    *inputPointer -- Pass &variable (base address of) to save read data to
//    length -- number of bytes to read
//
//  Note:  Does not know if the target memory space is an array or not, or
//    if there is the array is big enough.  if the variable passed is only
//    two bytes long and 3 bytes are requested, this will over-write some
//    other memory!
//
//****************************************************************************//
status_t CCS811Core::multiWriteRegister(uint8_t offset, uint8_t *inputPointer, uint8_t length)
{
	status_t returnError = SENSOR_SUCCESS;
	//define pointer that will point to the external space
	uint8_t i = 0;
	//Set the address
	Wire.beginTransmission(I2CAddress);
	Wire.write(offset);
	while ( i < length )  // send data bytes
	{
		Wire.write(*inputPointer); // receive a byte as character
		inputPointer++;
		i++;
	}
	if( Wire.endTransmission() != 0 )
	{
		returnError = SENSOR_I2C_ERROR;
	}
	return returnError;
}


//****************************************************************************//
//
//  Main user class -- wrapper for the core class + maths
//
//  Construct with same rules as the core ( uint8_t busType, uint8_t inputArg )
//
//****************************************************************************//
CCS811::CCS811( uint8_t inputArg ) : CCS811Core( inputArg )
{
	refResistance = 10000;
	resistance = 0;
	tVOC = 0;
	CO2 = 0;
}

//****************************************************************************//
//
//  Begin
//
//  This starts the lower level begin, then applies settings
//
//****************************************************************************//
status_t CCS811::begin( void )
{
	uint8_t data[4] = {0x11,0xE5,0x72,0x8A}; //Reset key
	status_t returnError = SENSOR_SUCCESS; //Default error state

	//restart the core
	returnError = beginCore();
	//Reset the device
	multiWriteRegister(CSS811_SW_RESET, data, 4);
	//Tclk = 1/16MHz = 0x0000000625
	//0.001 s / tclk = 16000 counts
	volatile uint8_t temp = 0;
	for( uint16_t i = 0; i < 16000; i++ ) //This waits > 1ms @ 16MHz clock
	{
		temp++;
	}
	
	if( checkForStatusError() == true ) return SENSOR_INTERNAL_ERROR;
	
	if( appValid() == false ) return SENSOR_INTERNAL_ERROR;
	
	//Write 0 bytes to this register to start app
	Wire.beginTransmission(I2CAddress);
	Wire.write(CSS811_APP_START);
	if( Wire.endTransmission() != 0 )
	{
		return SENSOR_I2C_ERROR;
	}	
	
	returnError = setDriveMode(1); //Read every second
	
	return returnError;
}


//****************************************************************************//
//
//  Sensor functions
//
//****************************************************************************//
//Updates the total voltatile organic compounds (TVOC) in parts per billion (PPB)
//and the CO2 value
//Returns nothing
status_t CCS811::readAlgorithmResults( void )
{
  uint8_t data[4];
  status_t returnError = multiReadRegister(CSS811_ALG_RESULT_DATA, data, 4);
  if( returnError != SENSOR_SUCCESS ) return 0;
  // Data ordered:
  // co2MSB, co2LSB, tvocMSB, tvocLSB

  CO2 = ((uint16_t)data[0] << 8) | data[1];
  tVOC = ((uint16_t)data[2] << 8) | data[3];
  return SENSOR_SUCCESS;
}

//Checks to see if error bit is set
bool CCS811::checkForStatusError( void )
{
  uint8_t value;
  //return the status bit
  readRegister( CSS811_STATUS, &value );
  return (value & 1 << 0);
}

//Checks to see if DATA_READ flag is set in the status register
bool CCS811::dataAvailable( void )
{
  uint8_t value;
  status_t returnError = readRegister( CSS811_STATUS, &value );
  if( returnError != SENSOR_SUCCESS )
  {
	  return 0;
  }
  else
  {
	  return (value & 1 << 3);
  }
}

//Checks to see if APP_VALID flag is set in the status register
bool CCS811::appValid( void )
{
  uint8_t value;
  status_t returnError = readRegister( CSS811_STATUS, &value );
  if( returnError != SENSOR_SUCCESS )
  {
	  return 0;
  }
  else
  {
	  return (value & 1 << 4);
  }
}

uint8_t CCS811::getErrorRegister( void )
{
	uint8_t value;
	
  status_t returnError = readRegister( CSS811_ERROR_ID, &value );
  if( returnError != SENSOR_SUCCESS )
  {
	  return 0xFF;
  }
  else
  {
	  return value;  //Send all errors in the event of communication error
  }
}

//Returns the baseline value
//Used for telling sensor what 'clean' air is
//You must put the sensor in clean air and record this value
uint16_t CCS811::getBaseline( void )
{
  uint8_t data[2];
  status_t returnError = multiReadRegister(CSS811_BASELINE, data, 2);

  unsigned int baseline = ((uint16_t)data[0] << 8) | data[1];
  if( returnError != SENSOR_SUCCESS )
  {
	  return 0;
  }
  else
  {
	  return (baseline);
  }
}

//Enable the nINT signal
status_t CCS811::enableInterrupts( void )
{
  uint8_t value;
  status_t returnError = readRegister( CSS811_MEAS_MODE, &value ); //Read what's currently there
  if(returnError != SENSOR_SUCCESS) return returnError;
  Serial.println(value, HEX);
  value |= (1 << 3); //Set INTERRUPT bit
  writeRegister(CSS811_MEAS_MODE, value);
  Serial.println(value, HEX);
  return returnError;
}

//Disable the nINT signal
status_t CCS811::disableInterrupts( void )
{
  uint8_t value;
  status_t returnError = readRegister( CSS811_MEAS_MODE, &value ); //Read what's currently there
  if( returnError != SENSOR_SUCCESS ) return returnError;
  value &= ~(1 << 3); //Clear INTERRUPT bit
  returnError = writeRegister(CSS811_MEAS_MODE, value);
  return returnError;
}

//Mode 0 = Idle
//Mode 1 = read every 1s
//Mode 2 = every 10s
//Mode 3 = every 60s
//Mode 4 = RAW mode
status_t CCS811::setDriveMode( uint8_t mode )
{
  if (mode > 4) mode = 4; //sanitize input

  uint8_t value;
  status_t returnError = readRegister( CSS811_MEAS_MODE, &value ); //Read what's currently there
  if( returnError != SENSOR_SUCCESS ) return returnError;
  value &= ~(0b00000111 << 4); //Clear DRIVE_MODE bits
  value |= (mode << 4); //Mask in mode
  returnError = writeRegister(CSS811_MEAS_MODE, value);
  return returnError;
}

//Given a temp and humidity, write this data to the CSS811 for better compensation
//This function expects the humidity and temp to come in as floats
status_t CCS811::setEnvironmentalData( float relativeHumidity, float temperature )
{
  int rH = relativeHumidity * 1000; //42.348 becomes 42348
  int temp = temperature * 1000; //23.2 becomes 23200

  byte envData[4];

  //Split value into 7-bit integer and 9-bit fractional
  envData[0] = ((rH % 1000) / 100) > 7 ? (rH / 1000 + 1) << 1 : (rH / 1000) << 1;
  envData[1] = 0; //CCS811 only supports increments of 0.5 so bits 7-0 will always be zero
  if (((rH % 1000) / 100) > 2 && (((rH % 1000) / 100) < 8))
  {
    envData[0] |= 1; //Set 9th bit of fractional to indicate 0.5%
  }

  temp += 25000; //Add the 25C offset
  //Split value into 7-bit integer and 9-bit fractional
  envData[2] = ((temp % 1000) / 100) > 7 ? (temp / 1000 + 1) << 1 : (temp / 1000) << 1;
  envData[3] = 0;
  if (((temp % 1000) / 100) > 2 && (((temp % 1000) / 100) < 8))
  {
    envData[2] |= 1;  //Set 9th bit of fractional to indicate 0.5C
  }
  status_t returnError = multiWriteRegister(CSS811_ENV_DATA, envData, 4);
  return returnError;
}

void CCS811::setRefResistance( uint16_t input )
{
	refResistance = input;
}

status_t CCS811::readNTC( void )
{
	uint8_t data[4];
	status_t returnError = multiReadRegister(CSS811_NTC, data, 4);

	vrefCounts = ((uint16_t)data[0] << 8) | data[1];
	ntcCounts = ((uint16_t)data[2] << 8) | data[3];
	resistance = (float)ntcCounts * refResistance / (float)vrefCounts;
	
	
	return returnError;
}

uint16_t CCS811::getTVOC( void )
{
	return tVOC;
}

uint16_t CCS811::getCO2( void )
{
	return CO2;
}

uint16_t CCS811::getResistance( void )
{
	return resistance;
}