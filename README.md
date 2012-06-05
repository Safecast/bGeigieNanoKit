# Welcome to bGeigie Nano

This is a lighter version of bGeigie Mini which is meant to fix in a Pelican case 1040.

# Requirements
* Arduino Fio
* OpenLog
* GPSBee
* Inspector Alert (with audio jack connector)
* Lithium battery
* Pelican case 1040

# Extensions (optional)
* Solar panel
* Serial LCD Display

# Build process
## Using the Makefile
    export ARDUINODIR=/home/geigie/arduino-1.0.1/
    export SERIALDEV=/dev/ttyUSB0
    export BOARD=fio
    make
    make upload

## Using the prebuilt image
You can use directly the prebuilt image to flash the Arduino Fio, here is an example when Arduino Fio is connected to ttyUSB0:
    /usr/bin/avrdude -DV -p atmega328p -P /dev/ttyUSB0 -c arduino -b 57600 -U flash:w:bGeigieNano.hex:i

# Usage
Once powered on the bGeigieNano will initiliaze a new log file on the SD card, setup the GPS and start counting the CPM.

# Sample log
    # NEW LOG
    # format=1.0.0nano
    $BNRDD,1,2000-00-00T00:00:00Z,2,2,2,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,3,1,3,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,6,3,6,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,8,2,8,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,9,1,9,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,10,1,10,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,14,4,14,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,18,4,18,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,19,1,19,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,23,4,23,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,26,3,26,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,28,2,28,V,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,29,3,31,A,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,31,3,34,A,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,30,2,36,A,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,33,5,41,A,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,36,4,45,A,0.000000000,0.000000000,0.00,0,0
    $BNRDD,1,2000-00-00T00:00:00Z,38,3,48,A,0.000000000,0.000000000,0.00,0,0

