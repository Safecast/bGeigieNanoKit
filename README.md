# Welcome to bGeigie Nano Kit project  
[Main bGeigie Nano page](http://blog.safecast.org/bgeigie-nano/)  
Assembly and Operations Manual can be found on the [Wiki](https://github.com/Safecast/bGeigieNanoKit/wiki)  
  
The Safecast bGeigie Nano is a mobile, GPS enabled, logging, radiation sensor. It is designed for mounting on the outside of a car window but can be used on bicycles, trains, planes, and other modes of transportation.  
  
It can also be used for taking static readings and contamination findings. In logging mode, the Nano takes a measurement every 5 seconds and logs the time and location coordinates.  



# Acknowledgements
This work would not exist without the original development by [bidouilles](https://github.com/bidouilles/bGeigieNano) and contributions of other [safecast](https://github.com/Safecast/bGeigieNano) developers.

# Requirements
* bGeigieNanoKit can be bought online from [KitHub](https://shop.kithub.cc/products/safecast-bgeigie-nano)
* for programing/updating software you need a FTDI cable drivers are at http://www.ftdichip.com/Drivers/VCP.htm
* solder iron
* time to make the kit (3-4 hours is normal)


# Build process 
(Needs compiler for example Crosspack-AVR on Mac)
http://www.ladyada.net/learn/avr/setup-mac.html

## Using the Makefile
    export ARDUINODIR=/home/geigie/arduino-1.0.1/
    export SERIALDEV=/dev/ttyUSB0
    export BOARD=fio
    cp -r libraries /home/geigie/arduino-1.0.1/
    make
    make upload

## Using the prebuilt image
You can use directly the prebuilt image to flash the Arduino Fio. Here is an example with Arduino Fio connected to ttyUSB0:

    /usr/bin/avrdude -DV -p atmega328p -P /dev/ttyUSB0 -c arduino -b 57600 -U flash:w:bGeigieNano.hex:i
    
Or Windows Users can download a small program called Xloader (http://russemotto.com/xloader/) and can directly flash a HEX file to the bGiegieNano (assuming you have a FDTI connection. Can be bought at many places like Adafruit, Seeedstudio, Switch-science etc.
Download and installed X-loader. Do not use the USB charging port on the Nano, but the 6 pin FTDI connector on the edge of the CPU board.
Purchase a   https://www.sparkfun.com/products/9873      FTDI breakout adapter.
 
When you plug this little FTDI breakout board onto the 6 pin header and then plug in via a usb cable into your computer, the Nano will power up by itself (even though the power switch is off).  Be careful here as you can plug it in upside down, so match the pins on the FTDI board with the pins on the cpu module.
 
Monitor the device manager com ports to see the device appear and what com # Windows assigned to the device.
 
Then Run X-loader, select com port, browse to HEX file, and I selected in the drop down Duemilanove/nano(ATmega328) device.  Leave the baud rate at 57600 and hit upload.   When it is done, the Nano rebooted/restarted all by itself and is running the new code.     Remove the FTDI board and usb cable to computer, and the Nano will power off. 

# Assembly

## Pins assignment

# Power consumption

* **Fio**: 0.045mA sleep, 6mA at run time
* **OpenLog**: 2mA idle, 6mA at maximum recording rate
* **Adafruit Ultimate GPS**: 25mA acquisition, 20mA tracking
* **Monochrome OLED 128x32 0.91"**: 4mA 50% turn-on, 7.8mA 100% turn-on

## Estimation
The total current used at run time can be estimated around 36mA (= 6+6+20+4) per second which will result in a consumption of 0.01mAh (= 36mA/3600). So
the total log duration if using a battery of 1300mAh will be (1300/.01)/3600 = 36.11 = **36h06m**

## Summary table

| Battery capacity (mAh) | Estimated log duration (days hh:mm) |
| :-----------: | :-----------: |
| 1300 | 1d 12:06 |
| 2600 |  3d 00:13 |
| 6600 |  7d 15:19 |

# Usage
Once powered on the bGeigieNano will initiliaze a new log file on the SD card, setup the GPS and start counting the CPM.

# Sample log

    # NEW LOG
    # format=1.0.0nano
    $BNRDD,204,2012-09-20T16:53:58Z,776,63,33895,A,5641.7788,N,1411.8820,E,9861.20,A,109,9*46
    $BNRDD,204,2012-09-20T16:54:03Z,771,61,33956,A,5642.2047,N,1412.9433,E,9862.60,A,109,9*4D
    $BNRDD,204,2012-09-20T16:54:08Z,773,70,34026,A,5642.6305,N,1414.0053,E,9865.00,A,109,9*40
    $BNRDD,204,2012-09-20T16:54:13Z,768,59,34085,A,5643.0562,N,1415.0662,E,9866.80,A,108,9*4D
    $BNRDD,204,2012-09-20T16:54:18Z,765,59,34144,A,5643.4820,N,1416.1277,E,9868.10,A,108,9*4D
    $BNRDD,204,2012-09-20T16:54:23Z,776,70,34214,A,5643.9077,N,1417.1884,E,9870.40,A,90,10*4E
    $BNRDD,204,2012-09-20T16:54:28Z,790,69,34283,A,5644.3330,N,1418.2491,E,9871.30,A,90,10*44
    $BNRDD,204,2012-09-20T16:54:33Z,792,77,34360,A,5644.7576,N,1419.3115,E,9871.40,A,90,10*41
    $BNRDD,204,2012-09-20T16:54:38Z,800,73,34433,A,5645.1819,N,1420.3749,E,9872.60,A,90,10*4C
    $BNRDD,204,2012-09-20T16:54:43Z,784,57,34490,A,5645.6060,N,1421.4371,E,9873.10,A,89,10*4A
    $BNRDD,204,2012-09-20T16:54:48Z,787,58,34548,A,5646.0298,N,1422.4998,E,9874.10,A,89,10*40
    $BNRDD,204,2012-09-20T16:54:53Z,792,76,34624,A,5646.4534,N,1423.5620,E,9874.80,A,98,9*73
    $BNRDD,204,2012-09-20T16:54:58Z,804,75,34699,A,5646.8769,N,1424.6242,E,9874.30,A,98,9*74
    $BNRDD,204,2012-09-20T16:55:03Z,808,65,34764,A,5647.3011,N,1425.6873,E,9877.10,A,98,9*7F
    $BNRDD,204,2012-09-20T16:55:08Z,793,55,34819,A,5647.7236,N,1426.7514,E,9876.30,A,89,10*49
    $BNRDD,204,2012-09-20T16:55:13Z,799,65,34884,A,5648.1464,N,1427.8159,E,9876.10,A,98,9*7F
    $BNRDD,204,2012-09-20T16:55:18Z,795,55,34939,A,5648.5688,N,1428.8810,E,9877.80,A,98,9*7B
    $BNRDD,204,2012-09-20T16:55:23Z,774,49,34988,A,5648.9898,N,1429.9472,E,9878.50,A,85,10*46
    $BNRDD,204,2012-09-20T16:55:28Z,768,63,35051,A,5649.4098,N,1431.0137,E,9877.40,A,224,8*4A
    $BNRDD,204,2012-09-20T16:55:33Z,754,63,35114,A,5649.8297,N,1432.0798,E,9876.30,A,223,8*4F
    $BNRDD,204,2012-09-20T16:55:38Z,752,71,35185,A,5650.2496,N,1433.1447,E,9873.20,A,106,9*4C
    $BNRDD,204,2012-09-20T16:55:43Z,753,58,35243,A,5650.6698,N,1434.2092,E,9871.30,A,106,9*40
    $BNRDD,204,2012-09-20T16:55:48Z,765,70,35313,A,5651.0904,N,1435.2737,E,9869.50,A,106,9*4B

# Notes
## OpenLog config

The OpenLog should start listening at 9600bps and in Command mode. Here is the content of the CONFIG.TXT file you have to create on the microSD card:

    9600,26,3,2

## SoftwareSerial update

To make sure all of the NMEA sentences can be received correctly, we will need to update the _SS_MAX_RX_BUFF definition from arduino-1.0.1/libraries/SoftwareSerial/SoftwareSerial.h header file. Here is the modification:

    //#define _SS_MAX_RX_BUFF 64 // RX buffer size -- Old Value is 64
    #define _SS_MAX_RX_BUFF 128 // RX buffer size for TinyGPS

# Licenses
 Licenses for this repository can be found at [here][10]
 
 * [InterruptHandler and bGeigieMini code][5] - Copyright (c) 2011, Robin Scheibler aka FakuFaku
 * [TinyGPS][6] - Copyright (C) 2008-2012 Mikal Hart
 * [Makefile][8] - Copyright (c) 2012, Tim Marston
 


  [1]: https://github.com/sparkfun/OpenLog "OpenLog"
  [2]: https://www.adafruit.com/products/661 "Monochrome 128x32 OLED"
  [3]: https://www.sparkfun.com/products/10116 "Arduino Fio"
  [4]: http://www.lndinc.com/products/17/ "LND-7317"
  [5]: https://github.com/fakufaku/SafecastBGeigie-firmware "SafecastBGeigie-firmware"
  [6]: http://arduiniana.org/libraries/tinygps/ "TinyGPS"
  [7]: https://www.adafruit.com/products/746 "Ultimate GPS"
  [8]: http://ed.am/dev/make/arduino-mk "Arduino Makefile"
  [9]: http://www.pelican.com/cases_detail.php?Case=1010 "Pelican 1010"
  [10]: https://blog.safecast.org/faq/licenses/
