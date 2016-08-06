# spidevice
Generic SPI bus master driver

## Instructions (Arduino)
* Include the library by adding following headers to your sketch:
```
#include <SPI.h>
#include <SpiDevice.h>
```
* then declare an instance of the SPI bus driver:
```
SpiDevice<10> spi;
```
* Here the `SpiDevice` represents the  SPI device abstraction of the attached
device. In this case for example the device was attached to the SPI bus with
the Slave Select line `PIN_SS` connected to Arduino digital pin number 10.
* Further template parameters can be appended for the serial clock speed `F_SCK`
given in Hz, `BIT_ORDER` `SpiBitOrderLsbFirst` or `SpiBitOrderMsbFirst` and
`MODE` from `SpiMode0` to `SpiMode3`.
* The SPI bus must be initialized by calling `spi.master()`.

## Instructions (WiringPi)
* Make sure that the SPI module is either enabled in the Device Tree and/or
loaded by the current Linux kernel, ie. not blacklisted. For example on
Raspberry Pi:
```
cat /boot/config.txt | grep "^dtparam=spi=on"
dtparam=spi=on
cat /etc/modprobe.d/raspi-blacklist.conf
#blacklist spi_bcm2708
```
* Install WiringPi either with `sudo apt-get install wiringpi` or
`sudo pacman -S wiringpi`.
* Compile the RFM69 library with `g++` and `-lwiringPi`.
* Linux kernels less than version 4.1.x may require root for execution.
* The usage of the SpiDevice class is nearly identical as for the Arduino
except that the first template parameter gets used instead to describe the
selected SPI channel. For example channel 0 is SPI bus with CE0 pin, channel 1
is SPI bus with CE1 ect. it is not the actual Slave Select GPIO pin number as it
is used in the Arduino implementation.

### Further reading
* http://jeelabs.org/book/1522c
* http://jeelabs.org/wp-content/uploads/2015/05/20/rfm69-on-raspberry-pi
