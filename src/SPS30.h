#pragma once

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

//The default I2C address for the SPS30 is 0x69.
#define SPS30_ADDRESS 0x69

//Available commands
#define COMMAND_START_MEASUREMENT 0x0010
#define COMMAND_STOP_MEASUREMENT 0x0104

#define COMMAND_GET_DATA_READY 0x0202
#define COMMAND_READ_MEASUREMENT 0x0300

#define COMMAND_AUTO_CLEAN_INTERVAL 0x8004
#define COMMAND_START_FAN_CLEANING 0x5607
#define COMMAND_READ_ARTICLE_CODE 0xD025
#define COMMAND_READ_SERIAL_NUMBER 0xD033
#define COMMAND_DEVICE_RESET 0xD304

class SPS30
{
public:
	SPS30(void);

	boolean begin(TwoWire &wirePort = Wire); //By default use Wire port

	boolean beginMeasuring(void);
	boolean stopMeasuring(void);

	void *getMass(float array[4]);
	void *getNum(float array[5]);

	boolean setCleaningInterval(uint32_t interval);

	boolean startCleaning();

	boolean dataAvailable();
	boolean readMeasurement();

	boolean sendCommand(uint16_t command, uint16_t arguments);
	boolean sendCommand(uint16_t command, uint32_t arguments);
	boolean sendCommand(uint16_t command);

	uint16_t readRegister(uint16_t registerAddress);

	uint8_t computeCRC8(uint8_t data[], uint8_t len);

public:
	//Variables
	TwoWire *_i2cPort; //The generic connection to user's chosen I2C hardware

	//Global main datums

	// Mass conentration of PM1.0 - PM10 (μg/m³)
	float massPM1 = 0;
	float massPM25 = 0;
	float massPM4 = 0;
	float massPM10 = 0;

	// Number concentration of PM0.5 - PM10 (#/cm³)
	float numPM05 = 0;
	float numPM1 = 0;
	float numPM25 = 0;
	float numPM4 = 0;
	float numPM10 = 0;

	// Typical particle size (μm)
	float typPartSize = 0;

	//These track the staleness of the current data
	//This allows us to avoid calling readMeasurement() every time individual datums are requested
	boolean massHasBeenReported = true;
	boolean numberHasBeenReported = true;
	boolean typHasBeenReported = true;
};
