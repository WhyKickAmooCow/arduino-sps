#include <SPS30.h>

SPS30 Sensor;

void setup()
{
  Serial.begin(9600);

  Wire1.begin(); //Start the wire hardware that may be supported by your platform

  Sensor.begin(Wire1); //Pass the Wire port to the .begin() function

  //The library will now use Wire1 for all communication
}

void loop()
{
  if (Sensor.dataAvailable())
  {
    float mass_concen[4];
    Sensor.getMass(mass_concen);

    float num_concen[5];
    Sensor.getNum(num_concen);

    char *pm[5] = {"PM0.5", "PM1.0", "PM2.5", "PM4.0", "PM10"};

    Serial.println("--- Mass Concentration ---");

    for (int i = 0; i < 4; i++)
    {
      Serial.printf("%s: %.2f\n", pm[i + 1], mass_concen[i]);
    }

    Serial.println("--- Number Concentration ---");

    for (int i = 0; i < 5; i++)
    {
      Serial.printf("%s: %.2f\n", pm[i], num_concen[i]);
    }
  }
}
