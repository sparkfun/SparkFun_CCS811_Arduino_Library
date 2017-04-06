//This example configures the nWAKE and nINT pins.
//The interrupt pin is configured to pull low when the data is
//ready to be collected.
//The wake pin is configured to enable the sensor during I2C communications

//It is still a mystery if the wake pin is soldered correctly.

#include <SparkFunCCS811.h>

#define CCS811_ADDR 0x5B //7-bit unshifted default I2C Address

#define PIN_NOT_WAKE 5
#define PIN_NOT_INT 6

CCS811 mySensor(CCS811_ADDR);

//Global sensor object
//---------------------------------------------------------------
void setup()
{
	//Start the serial
	Serial.begin(9600);
	Serial.println();
	Serial.println("...");

	status_t returnCode;
	
	//Start the sensor
	returnCode = mySensor.begin();
	Serial.print("CCS811 begin exited with: ");
	printDriverError( returnCode );
	Serial.println();

	returnCode = mySensor.setDriveMode(2);
	Serial.print("Mode request exited with: ");
	printDriverError( returnCode );
	Serial.println();
	
	//Configure and enable the interrupt link
	pinMode(PIN_NOT_INT, INPUT_PULLUP);
	returnCode = mySensor.enableInterrupts();
	Serial.print("Interrupt configuation exited with: ");
	printDriverError( returnCode );
	Serial.println();
	
	//Configure the wake link
	pinMode(PIN_NOT_WAKE, OUTPUT);
	digitalWrite(PIN_NOT_WAKE, 1); //Start asleep
	
}
//---------------------------------------------------------------
void loop()
{
	if(digitalRead(PIN_NOT_INT) == 0)
	{
		digitalWrite(PIN_NOT_WAKE, 0);
		//Need to wait at least 50 us
		delay(1);
		//Interrupt signal caught
		mySensor.readAlgorithmResults(); //Calling this function updates the global tVOC and CO2 variables

		Serial.print("CO2[");
		Serial.print(mySensor.getCO2());
		Serial.print("] tVOC[");
		Serial.print(mySensor.getTVOC());
		Serial.print("] millis[");
		Serial.print(millis());
		Serial.print("]");
		Serial.println();

		digitalWrite(PIN_NOT_WAKE, 1);
		//Need to be asleep for at least 20 us
		delay(1);
	}
	delay(1); //cycle kinda fast
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