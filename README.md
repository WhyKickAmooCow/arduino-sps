Sensirion SPS30 Particulate Sensor Library
===========================================================

![SparkFun SPS30 Particulate Sensor](https://cdn.sparkfun.com//assets/parts/1/3/4/6/3/15103-Particulate_Matter_Sensor_-_SPS30-01.jpg)


An Arduino library for the SPS30 particulate sensor from Sensirion. The SPS30 is a high quality [LASER diffratction](https://en.wikipedia.org/wiki/Laser_diffraction_analysis) based particulate sensor capable of detecting particles between 0.3μm and 10μm. To maintian performance, the SPS30 has the ability to clean itself using the built in fan, there are commands to manually start this process as well as to set the interval at which it automatically starts the process.


This library makes reading the particulate matter concentration (μg/m³, and #/m³) easy. The pinouts are described in datasheet from Sensirion, the required configuration to use this library is:

| Pin | Use |
| --- | --- |
| 1   | 5v  |
| 2   | SDA |
| 3   | SCL |
| 4   | GND |
| 5   | GND |

Note: The SPS30 has an automatic self-cleaning routine. Sensirion recommends at most 7 days of use between cleanings.


Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **library.properties** - General library properties for the Arduino package manager. 

Where to Buy
------------
#### I got my one from FutureElectronics, however I needed to buy a ZH5-1.5mm cable separately, Sparkfun advertises a nice looking one included.
[*Sparkfun*](https://www.sparkfun.com/products/15103)

[*FutureElectronics*](https://www.futureelectronics.com/p/analog--sensors--air-quality/sps30-sensirion-9104459)

[*Element14*](https://nz.element14.com/sensirion/sps30/particulate-sensor-dust-0-5um/dp/2925311?st=sps30)

[*Digi-Key*](https://www.digikey.com/product-detail/en/sensirion-ag/SPS30/1649-1106-ND/9598990)

[*Mouser*](https://nz.mouser.com/ProductDetail/Sensirion/SPS30?qs=sGAEpiMZZMve4%2FbfQkoj%252BB8Y4iWciyxKvMQm%2F4%252Bfm8U%3D)

Notice
------
This library is a derivative of the excellent [Sparkfun SCD30 Library](https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library). It has been modified to work with the SPS30 but retains much of the original work. Thank you to the people at Sparkfun for open sourcing your library.

License Information
-------------------
This library uses the MIT liscense.