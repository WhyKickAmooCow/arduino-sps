/*
  This is a library written for the SCD30
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/14751

  Written by Nathan Seidle @ SparkFun Electronics, May 22nd, 2018

  The SCD30 measures CO2 with accuracy of +/- 30ppm.

  This library handles the initialization of the SCD30 and outputs
  CO2 levels, relative humidty, and temperature.

  https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library

  Development environment specifics:
  Arduino IDE 1.8.5

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

//The default I2C address for the SCD30 is 0x61.
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

	boolean setCleaningInterval(uint16_t interval);

	boolean startCleaning();

	boolean dataAvailable();
	boolean readMeasurement();

	boolean sendCommand(uint16_t command, uint16_t arguments);
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
