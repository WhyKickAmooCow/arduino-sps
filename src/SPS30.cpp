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

#include "SPS30.h"

SPS30::SPS30(void)
{
  // Constructor
}

//Initialize the Serial port
boolean SPS30::begin(TwoWire &wirePort)
{
  _i2cPort = &wirePort; //Grab which port the user wants us to use

  //We expect caller to begin their I2C port, with the speed of their choice external to the library
  //But if they forget, we start the hardware here.
  _i2cPort->begin();

  /* Especially during obtaining the ACK BIT after a byte sent the SCD30 is using clock stretching  (but NOT only there)!
   * The need for clock stretching is described in the Sensirion_CO2_Sensors_SCD30_Interface_Description.pdf
   *
   * The default clock stretch (maximum wait time) on the ESP8266-library (2.4.2) is 230us which is set during _i2cPort->begin();
   * In the current implementation of the ESP8266 I2C driver there is NO error message when this time expired, while
   * the clock stretch is still happening, causing uncontrolled behaviour of the hardware combination.
   *
   * To set ClockStretchlimit() a check for ESP8266 boards has been added in the driver.
   *
   * With setting to 20000, we set a max timeout of 20mS (> 20x the maximum measured) basically disabling the time-out 
   * and now wait for clock stretch to be controlled by the client.
   */

#if defined(ARDUINO_ARCH_ESP8266)
  _i2cPort->setClockStretchLimit(200000);
#endif

  //Check for device to respond correctly
  if (beginMeasuring() == true) //Start continuous measurements
  {
    return (true);
  }

  return (false); //Something went wrong
}

//Returns the latest available CO2 level
//If the current level has already been reported, trigger a new read
void *SPS30::getMass(float array[4])
{
  if (massHasBeenReported == true) //Trigger a new read
    readMeasurement();             //Pull in new co2, humidity, and temp into global vars

  massHasBeenReported = true;

  array[0] = massPM1;
  array[1] = massPM25;
  array[2] = massPM4;
  array[3] = massPM10;
}

void *SPS30::getNum(float array[5])
{
  if (numberHasBeenReported == true) //Trigger a new read
    readMeasurement();               //Pull in new co2, humidity, and temp into global vars

  numberHasBeenReported = true;

  array[0] = numPM05;
  array[1] = numPM1;
  array[2] = numPM25;
  array[3] = numPM4;
  array[4] = numPM10;
}

//Begins continuous measurements
//Continuous measurement status is saved in non-volatile memory. When the sensor
//is powered down while continuous measurement mode is active SCD30 will measure
//continuously after repowering without sending the measurement command.
//Returns true if successful
boolean SPS30::beginMeasuring()
{
  return (sendCommand(COMMAND_START_MEASUREMENT, 0x0300));
}

// Stops measurements
boolean SPS30::stopMeasuring()
{
  return (sendCommand(COMMAND_STOP_MEASUREMENT));
}

//Sets interval between measurements
//2 seconds to 1800 seconds (30 minutes)
void SPS30::setCleaningInterval(uint16_t interval)
{
  sendCommand(COMMAND_AUTO_CLEAN_INTERVAL, interval);
}

//Returns true when data is available
boolean SPS30::dataAvailable()
{
  uint16_t response = readRegister(COMMAND_GET_DATA_READY);

  if (response == 1)
    return (true);
  return (false);
}

//Get 60 bytes from SCD30
//Updates global variables with floats
//Returns true if success
boolean SPS30::readMeasurement()
{
  //Verify we have data from the sensor
  if (dataAvailable() == false)
    return (false);

  // Mass conentration of PM1.0 - PM10 (μg/m³)
  uint32_t tempMassPM1 = 0;
  uint32_t tempMassPM25 = 0;
  uint32_t tempMassPM4 = 0;
  uint32_t tempMassPM10 = 0;

  // Number concentration of PM0.5 - PM10 (#/cm³)
  uint32_t tempNumPM05 = 0;
  uint32_t tempNumPM1 = 0;
  uint32_t tempNumPM25 = 0;
  uint32_t tempNumPM4 = 0;
  uint32_t tempNumPM10 = 0;

  // Typical particle size (μm)
  uint32_t tempTypPartSize = 0;

  _i2cPort->beginTransmission(SPS30_ADDRESS);
  _i2cPort->write(COMMAND_READ_MEASUREMENT >> 8);   //MSB
  _i2cPort->write(COMMAND_READ_MEASUREMENT & 0xFF); //LSB
  if (_i2cPort->endTransmission() != 0)
    return (0); //Sensor did not ACK

  _i2cPort->requestFrom((uint8_t)SPS30_ADDRESS, (uint8_t)60);
  if (_i2cPort->available())
  {
    for (byte x = 0; x < 60; x++)
    {
      byte incoming = _i2cPort->read();

      switch (x)
      {
      case 0:
      case 1:
      case 3:
      case 4:
        // mass pm1.0
        tempMassPM1 <<= 8;
        tempMassPM1 |= incoming;
        break;
      case 6:
      case 7:
      case 9:
      case 10:
        // mass pm2.5
        tempMassPM25 <<= 8;
        tempMassPM25 |= incoming;
        break;
      case 12:
      case 13:
      case 15:
      case 16:
        // mass pm4.0
        tempMassPM4 <<= 8;
        tempMassPM4 |= incoming;
        break;
      case 18:
      case 19:
      case 21:
      case 22:
        // mass pm10
        tempMassPM10 <<= 8;
        tempMassPM10 |= incoming;
        break;
      case 24:
      case 25:
      case 27:
      case 28:
        // number pm0.5
        tempNumPM05 <<= 8;
        tempNumPM05 |= incoming;
        break;
      case 30:
      case 31:
      case 33:
      case 34:
        // number pm1.0
        tempNumPM1 <<= 8;
        tempNumPM1 |= incoming;
        break;
      case 36:
      case 37:
      case 39:
      case 40:
        // number pm2.5
        tempNumPM25 <<= 8;
        tempNumPM25 |= incoming;
        break;
      case 42:
      case 43:
      case 45:
      case 46:
        // number pm4.0
        tempNumPM4 <<= 8;
        tempNumPM4 |= incoming;
        break;
      case 48:
      case 49:
      case 51:
      case 52:
        // number pm10
        tempNumPM10 <<= 8;
        tempNumPM10 |= incoming;
        break;
      case 54:
      case 55:
      case 57:
      case 58:
        // typ particle size
        tempTypPartSize <<= 8;
        tempTypPartSize |= incoming;
        break;
      default:
        //Do nothing with the CRC bytes
        break;
      }
    }
  }

  //Now copy the uint32s into their associated floats

  // Mass concentrations
  memcpy(&massPM1, &tempMassPM1, sizeof(massPM1));
  memcpy(&massPM25, &tempMassPM25, sizeof(massPM25));
  memcpy(&massPM4, &tempMassPM4, sizeof(massPM4));
  memcpy(&massPM10, &tempMassPM10, sizeof(massPM10));

  // Number concentrations
  memcpy(&numPM05, &tempNumPM05, sizeof(numPM05));
  memcpy(&numPM1, &tempNumPM1, sizeof(numPM1));
  memcpy(&numPM25, &tempNumPM25, sizeof(numPM25));
  memcpy(&numPM4, &tempNumPM4, sizeof(numPM4));
  memcpy(&numPM10, &tempNumPM10, sizeof(numPM10));

  // Typ particle size
  memcpy(&typPartSize, &tempTypPartSize, sizeof(typPartSize));

  //Mark our global variables as fresh
  massHasBeenReported = false;
  numberHasBeenReported = false;
  typHasBeenReported = false;

  return (true); //Success! New data available in globals.
}

//Gets two bytes from SCD30
uint16_t SPS30::readRegister(uint16_t registerAddress)
{
  _i2cPort->beginTransmission(SPS30_ADDRESS);
  _i2cPort->write(registerAddress >> 8);   //MSB
  _i2cPort->write(registerAddress & 0xFF); //LSB
  if (_i2cPort->endTransmission() != 0)
    return (0); //Sensor did not ACK

  _i2cPort->requestFrom((uint8_t)SPS30_ADDRESS, (uint8_t)2);
  if (_i2cPort->available())
  {
    uint8_t msb = _i2cPort->read();
    uint8_t lsb = _i2cPort->read();
    return ((uint16_t)msb << 8 | lsb);
  }
  return (0); //Sensor did not respond
}

//Sends a command along with arguments and CRC
boolean SPS30::sendCommand(uint16_t command, uint16_t arguments)
{
  uint8_t data[2];
  data[0] = arguments >> 8;
  data[1] = arguments & 0xFF;
  uint8_t crc = computeCRC8(data, 2); //Calc CRC on the arguments only, not the command

  _i2cPort->beginTransmission(SPS30_ADDRESS);
  _i2cPort->write(command >> 8);     //MSB
  _i2cPort->write(command & 0xFF);   //LSB
  _i2cPort->write(arguments >> 8);   //MSB
  _i2cPort->write(arguments & 0xFF); //LSB
  _i2cPort->write(crc);
  if (_i2cPort->endTransmission() != 0)
    return (false); //Sensor did not ACK

  return (true);
}

//Sends just a command, no arguments, no CRC
boolean SPS30::sendCommand(uint16_t command)
{
  _i2cPort->beginTransmission(SPS30_ADDRESS);
  _i2cPort->write(command >> 8);   //MSB
  _i2cPort->write(command & 0xFF); //LSB
  if (_i2cPort->endTransmission() != 0)
    return (false); //Sensor did not ACK

  return (true);
}

//Given an array and a number of bytes, this calculate CRC8 for those bytes
//CRC is only calc'd on the data portion (two bytes) of the four bytes being sent
//From: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
//Tested with: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
//x^8+x^5+x^4+1 = 0x31
uint8_t SPS30::computeCRC8(uint8_t data[], uint8_t len)
{
  uint8_t crc = 0xFF; //Init with 0xFF

  for (uint8_t x = 0; x < len; x++)
  {
    crc ^= data[x]; // XOR-in the next input byte

    for (uint8_t i = 0; i < 8; i++)
    {
      if ((crc & 0x80) != 0)
        crc = (uint8_t)((crc << 1) ^ 0x31);
      else
        crc <<= 1;
    }
  }

  return crc; //No output reflection
}
