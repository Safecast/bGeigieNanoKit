# Welcome to bGeigie Nano project

This is a lighter version of the bGeigie Mini which is meant to fit in a Pelican Micro Case 1040.

# Requirements
* Arduino Fio
* [OpenLog][1]
* [GPSBee][2]
* [Inspector Alert][3] (with audio jack connector)
* 3.7V 850mAh Lithium battery (*~15 hours logging*)
* [Pelican Micro Case 1040][4]

![bGeigieNano](https://bitbucket.org/lbergeret/bgeigienano/raw/89f6a92e662f/assembly/bGeigieNano_bb.jpg)

# Extensions (optional)
* Solar panel
* Serial LCD Display

# Power consumption

## Estimation
* **Fio**: 0.045mA sleep, 6mA at run time
* **OpenLog**: 2mA idle, 6mA at maximum recording rate
* **GPSBee**: 102mA acquisition, 44mA tracking

The total current used at run time can be estimated around 56mA (= 6+6+44) per second which will result in a consumption of 0.0155mAh (= 56mA/3600). So
the total log duration if using a battery of 850mAh will be (850/.0155)/3600 = 15.17 = 15h10m

## Summary table

| Battery capacity (mAh) | Estimated log duration (days hh:mm) |
| :-----------: | :-----------: |
| 850 |	0d 15:10 |
| 1000 | 0d 17:51 |
| 2000 | 1d 11:42 |
| 6000 | 4d 11:08 |

# Build process
## Using the Makefile
    export ARDUINODIR=/home/geigie/arduino-1.0.1/
    export SERIALDEV=/dev/ttyUSB0
    export BOARD=fio
    make
    make upload

## Using the prebuilt image
You can use directly the prebuilt image to flash the Arduino Fio. Here is an example with Arduino Fio connected to ttyUSB0:

    /usr/bin/avrdude -DV -p atmega328p -P /dev/ttyUSB0 -c arduino -b 57600 -U flash:w:bGeigieNano.hex:i

# Usage
Once powered on the bGeigieNano will initiliaze a new log file on the SD card, setup the GPS and start counting the CPM.

# Sample log

    # NEW LOG
    # format=1.0.0nano
    $BNRDD,1,2012-00-00T00:00:00Z,0,0,0,V,0.000000,0.000000,0.00,0.00,V,0,0
    $BNRDD,1,2012-00-00T00:00:00Z,0,0,0,V,0.000000,0.000000,0.00,0.00,V,0,0
    $BNRDD,1,2012-00-00T00:00:00Z,0,0,0,V,0.000000,0.000000,0.00,0.00,V,0,0
    $BNRDD,1,2012-06-05T07:57:26Z,0,0,0,V,35.622318,139.749220,0.10,0.13,A,3,525
    $BNRDD,1,2012-06-05T07:57:31Z,0,0,0,V,35.622581,139.748660,68.30,0.02,A,5,520
    $BNRDD,1,2012-06-05T07:57:36Z,0,0,0,V,35.622581,139.748660,69.40,0.11,A,5,520
    $BNRDD,1,2012-06-05T07:57:41Z,0,0,0,V,35.622601,139.748700,66.60,0.06,A,5,521
    $BNRDD,1,2012-06-05T07:57:46Z,0,0,0,V,35.622620,139.748720,63.90,0.11,A,5,522
    $BNRDD,1,2012-06-05T07:57:51Z,0,0,0,V,35.622669,139.748690,66.10,0.09,A,5,523
    $BNRDD,1,2012-06-05T07:57:56Z,0,0,0,V,35.622711,139.748660,67.90,0.41,A,5,524
    $BNRDD,1,2012-06-05T07:58:02Z,0,0,0,V,35.622730,139.748660,67.30,0.02,A,6,524
    $BNRDD,1,2012-06-05T07:58:07Z,0,0,0,V,35.622742,139.748630,69.10,0.13,A,6,525
    $BNRDD,1,2012-06-05T07:58:12Z,0,0,0,V,35.622749,139.748600,74.00,0.07,A,6,526
    $BNRDD,1,2012-06-05T07:58:17Z,0,0,0,V,35.622742,139.748600,75.40,0.33,A,6,527
    $BNRDD,1,2012-06-05T07:58:22Z,0,0,0,V,35.622719,139.748600,74.90,0.02,A,6,528
    $BNRDD,1,2012-06-05T07:58:27Z,0,0,0,V,35.622719,139.748600,75.40,0.15,A,6,529
    $BNRDD,1,2012-06-05T07:58:32Z,0,0,0,V,35.622711,139.748610,75.30,0.07,A,6,530
    $BNRDD,1,2012-06-05T07:58:37Z,0,0,0,V,35.622700,139.748610,75.60,0.02,A,6,530

# Licenses
 * [InterruptHandler and bGeigieMini code][5] - Copyright (c) 2011, Robin Scheibler aka FakuFaku
 * [TinyGPS][6] - Copyright (C) 2008-2012 Mikal Hart
 * bGeigieNano - Copyright (c) 2012, Lionel Bergeret


  [1]: https://github.com/sparkfun/OpenLog "OpenLog"
  [2]: http://www.seeedstudio.com/wiki/GPS_Bee_kit_%28with_Mini_Embedded_Antenna%29 "GPSBee"
  [3]: http://medcom.com/products/inspector-alert "Inspector Alert"
  [4]: http://pelican.com/cases_detail.php?Case=1040 "Pelican Micro Case 1040"
  [5]: https://github.com/fakufaku/SafecastBGeigie-firmware "SafecastBGeigie-firmware"
  [6]: http://arduiniana.org/libraries/tinygps/ "TinyGPS"
