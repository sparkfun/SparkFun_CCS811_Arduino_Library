/******************************************************************************
SparkFunCCS811.h
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

#ifndef __CCS811_IMU_H__
#define __CCS811_IMU_H__

#include "stdint.h"

//Register addresses
#define CSS811_STATUS 0x00
#define CSS811_MEAS_MODE 0x01
#define CSS811_ALG_RESULT_DATA 0x02
#define CSS811_RAW_DATA 0x03
#define CSS811_ENV_DATA 0x05
#define CSS811_NTC 0x06
#define CSS811_THRESHOLDS 0x10
#define CSS811_BASELINE 0x11
#define CSS811_HW_ID 0x20
#define CSS811_HW_VERSION 0x21
#define CSS811_FW_BOOT_VERSION 0x23
#define CSS811_FW_APP_VERSION 0x24
#define CSS811_ERROR_ID 0xE0
#define CSS811_APP_START 0xF4
#define CSS811_SW_RESET 0xFF

// Return values 
typedef enum
{
	SENSOR_SUCCESS,
	SENSOR_ID_ERROR,
	SENSOR_I2C_ERROR,
	SENSOR_INTERNAL_ERROR
	//...
} status_t;

//This is the core operational class of the driver.
//  LIS3DHCore contains only read and write operations towards the IMU.
//  To use the higher level functions, use the class LIS3DH which inherits
//  this class.

class CCS811Core
{
public:
	CCS811Core( uint8_t );
//	LIS3DHCore( uint8_t, uint8_t );
//	~LIS3DHCore() = default;
//	
	status_t beginCore( void );
//	
//	//The following utilities read and write to the IMU
//
	//readRegister reads one 8-bit register
	status_t readRegister( uint8_t offset, uint8_t* outputPointer);
	//multiReadRegister takes a uint8 array address as input and performs
	//  a number of consecutive reads
	status_t multiReadRegister(uint8_t offset, uint8_t *outputPointer, uint8_t length);

	//Writes an 8-bit byte;
	status_t writeRegister(uint8_t offset, uint8_t dataToWrite);
	//multiWriteRegister takes a uint8 array address as input and performs
	//  a number of consecutive writes
	status_t multiWriteRegister(uint8_t offset, uint8_t *inputPointer, uint8_t length);

	protected:
//	//Communication stuff
	uint8_t I2CAddress;
};

//This is the highest level class of the driver.
//
//  class CCS811 inherits the core and makes use of the beginCore()
//method through it's own begin() method.  It also contains the
//settings struct to hold user settings.

class CCS811 : public CCS811Core
{
public:
	CCS811( uint8_t );

	//Call to check for errors, start app, and set default mode 1
	status_t begin( void );

	status_t readAlgorithmResults( void );
	void configureCCS811( void );
	bool checkForStatusError( void );
	bool dataAvailable( void );
	bool appValid( void );
	uint8_t getErrorRegister( void );
	uint16_t getBaseline( void );
	status_t enableInterrupts( void );
	status_t disableInterrupts( void );
	status_t setDriveMode( uint8_t mode );
	status_t setEnvironmentalData( float relativeHumidity, float temperature );

//private:
	//These are the air quality values obtained from the sensor
	uint16_t tVOC;
	uint16_t CO2;
};

#endif  // End of __LIS3DH_IMU_H__ definition check
