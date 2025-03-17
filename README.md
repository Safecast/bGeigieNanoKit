## bGeigie Nano Kit Operating Instructions (March 2017)

# Table of Contents

1.  [Introduction](#introduction)
2.  [Powering the Device On](#powering-the-device-on)
3.  [Switching Between Modes](#switching-between-modes)
4.  [Measuring Ionizing Radiation in Logging Mode](#measuring-ionizing-radiation-in-logging-mode)
5.  [Measuring Ionizing Radiation in Surface Mode](#measuring-ionizing-radiation-in-surface-mode)
    1. [Alarm](#alarm)
    2. [Data Log](#data-log)
6.  [Uploading Data to the Safecast Database With the Safecast API](#uploading-data-to-the-safecast-database-with-the-safecast-api)
7.  [Uploading Data to Other Datasets](#uploading-data-to-other-datasets)
8. [Using Safecast With Mobile Apps](#using-safecast-with-mobile-apps)
9. [Troubleshooting](#troubleshooting)
    1. [Troubleshooting the iSafecast Geiger Bot App for iOS](#troubleshooting-the-isafecast-geiger-bot-app-for-ios)
    2. [Troubleshooting the iSafecast Geiger Bot App's API](#troubleshooting-the-isafecast-geiger-bot-apps-api)
    3. [Troubleshooting Other Issues](#troubleshooting-other-issues)
        1. [Firmware](#firmware)
    4. [Cleaning](#cleaning)
10. [Warranty Information](#warranty-information)
11. [Hardware Cautions](#hardware-cautions)
12. [Resources and Support](#resources-and-support)

## Introduction
The Safecast bGeigie Nano kit is a *geo-tagged mobile sensor of ionizing radiation*. It is equipped with Internet data sharing capability and optional wireless capability. 

The term "radiation" refers to energy emitted by a source. This includes light, heat, microwaves, and other types of energy. Ionizing radiation is a type radiation that has so much energy that it removes electrons from the atoms and molecules that make up air, water, and living tissue. This activity can harm the molecules in our bodies, leading to diseases like cancer. Radiation can come from either natural or human-made sources. Natural sources include radioactive rocks, elements, and a radioactive gas called radon. Human-made sources include nuclear power plants and medical diagnostic tests, such as x-rays.

The bGeigie Nano measures alpha, beta, and gamma radiation. 

| Type | Description |
| ---- | ----------- |
| Alpha Radiation | Alpha particles are charged particles that have a large mass and charge. As a result, they don't penetrate materials as easily as other types of ionizing radiation. They also don't travel very far. |
| Beta Radiation | Beta particles are similar to electrons. They are lighter than alpha particles and travel farther, up to a few feet through air. |
| Gamma Radiation | Gamma particles are the most powerful form of ionizing radiation. They travel very far at the speed of light and penetrate most materials easily. Thick layers of concrete, lead, steel, or similar materials are required to stop gamma particles. |

The Nano's maximum operating range is about 350,000cpm, or 1mSv/h (1 millisievert per hour dose rate or 1000µSv/h microsieverts per hour). 

The Nano can be mounted on a car window, or used for static or spot radiation detection. Nano users can submit their mobile radiation measurements to Safecast, an online global mapping system developed by Safecast and MIT Media Lab. 

The Nano also records location, date, and time. The location data allows Safecast to map the measurements and understand the distribution of ionizing radiation. Safecast uses the date and time information to track how ionizing radiation concentrations change over time. The do-it-yourself kit enables customization, cost savings, and learning. 


## Powering the Device On

To turn the unit on, slide the switch at the lower right to the up position. 

The SAFECAST logo will appear on the display for approximately one second. The next start-up screen displays the name of model (bGeigie Nano), the firmware version number, battery charge level, basic settings, and the user’s name (or other customizable information).  

## Switching Between Modes

* The bGeigie Nano has two operating modes, "display" and "recording". To alternate between them, use the toggle switch at the upper right. Label on the transparent top panel: (to the right) "bq/m^2; uS/h", (to left) "log; cpm".

+ The “up” position (to the right) switches the unit into display mode (in which there is no data logging). The fields on the OLED indicate uSv/h dose-rate (Cs137), max dose-rate, dosimeter, Bq/m2 display (Cs137), time stamp, and alarm. 

+ The “down” position puts the Nano into recording mode. The OLED shows indicators for CPM and µSv/h, the number of satellites locked, altitude (meters), distance traversed (km), total duration of measurement (h:m), and time stamp (dd:hh:mm:ss). When the Nano is in recording mode, the OLED shows whether a micro-SD memory card is currently inserted. 

## Measuring Ionizing Radiation in Logging Mode

While in recording mode, the OLED display also shows a GPS lock indicator. When the device locks onto a GPS signal, it will show the number of satellites found, and a small red LED will glow. If the device isn't getting a lock, try placing it near a window for a few minutes.

The small red LED will only glow if the GPS is locked, the SD card is present, the battery has more than 10% charge left, the unit has been on for one minute, and the Geiger tube is providing a pulse. It can be dimmed by setting DIP switch #2 to "Off."

The speaker "clicks" and a small blue LED blinks with each pulse from the Geiger tube. It can be dimmed by setting DIP switch #1 to "Off."

The red LED on the GPS unit blinks every 1 second when the GPS is not locked. Once locked, it blinks every 10 seconds.

The date and time are in __UTC__ (formerly called GMT), rather than local time. The __date/time stamp__ refreshes when log data is written to the card. The LOG's file name is based on the time stamp   (e.g., 21080716.LOG).  The UTC log filename may be +/- one day off from the local date of the measurement, depending on the difference between time zones and distance from the International Date Line (IDL). The file metadata is not updated, which means the log's file creation date may appear with a default date, such as 01/01/2000. For this reason, we recommend that you sort your logs by filename instead of the date field.  When uploading a log, please enter its metadata in the required fields, and submit it for approval. 

**GPS Reset Procedure** (ver 1.3.0 and later)

To reset the GPS device: 

1) Turn off the bGeigie
2) Remove the SD card
3) Turn the bGeigie back on and leave it on for one minute 

    After approximately 10 seconds, the display will show the message "NO SD CARD/GPS Reset." Note that powering the unit on without the SD card will cause an error and **re-initialize the unit, including the GPS controller**.

4) Turn the device off again
5) Insert the SD card

## Measuring Ionizing Radiation in Surface Mode

Use surface mode to detect surface contamination. Removing the Nano from its case while in surface mode can increase the Nano's ability to detect alpha and beta radiation.

1. Remove the Nano from its case (with or without the rubber liner). Use both thumbs to push the unit in and then swivel it out. When you reinsert the Nano into the case later on, you can do it either above or inside the removable rubber liner.

While in uSv/Bq mode, the display also shows the peak dose rate (in uSv/h), total accumulated dose (in uSv), and CPM. 

The mode switch sets the bGeigie Nano into "Log Mode" (switch down "log cpm"). In Log Mode, the largest area of the display ("the big numbers") shows the count rate in CPM, and data will be written to the SD card.  In "Survey Mode" (switch up "Bq/m2 uSv/h"), the main part of the display will show the dose rate in microsieverts per hour.

- To measure the uSv dose rate accurately, put the Nano in the case, hold it 1m above the ground, tilt it around 45 degrees, and then wait for approximately one minute for a stable reading.

- To measure bQ accurately, take the Nano out of the case, keep it within 5cm to 1cm from the surface, and wait approximately one minute for a stable reading. Then, read the value on the second line in Bq/m2. The Nano will emit a speaker sound to aid in finding hotspots.

- The dose rate is based on the assumptions that the Nano is in its case; it is about 1 meter (3ft) away from any object or surface; any gamma rays are of moderate energy (600~700KeV); and the unit has met the previous conditions for at least one minute.

- Also in Survey Mode, the second line on the display ("the small numbers") will rotate through several other values. One of those will be Bq/m2. That is only a locally displayed interpretation of the data, and requires very different conditions to be correct: the Nano is _out_ of its case and held 1cm (~half inch) from a surface contaminated with radio cesium. This usage probably only works in Fukushima and Chernobyl.


### Limitations  

The grid on the pancake sensor is only useful when you remove the unit from the case to look for surface contamination. The grid itself has a very small shielding effect, mostly for alpha/beta and weak gamma radiation. The effect is minimal, and can be ignored.

When averaging readings from two different Nanos, you should only compare long-term counting (e.g. number of counts for 10 minutes).
 
### Alarm
- A speaker "clicks" and a small blue LED “Count” blinks with each pulse from the Geiger tube. It can be dimmed by setting DIP switch #1 to "Off."
- A small Red LED “Log/Alarm” flickers on pulse, and remains lit on alarm. It will only glow if the GPS is locked, the SD card is present, the battery has more than 10% charge left, the unit has been on for one minute, and the Geiger tube is providing a pulse. It can be dimmed by setting DIP switch #2 to "Off."
- The Piezo buzzer may not loud enough for all users. There are solder pads available on the Nano main board for an optional audio-out connector that can be used to connect an amplifier or sound recorder to get louder clicks, if desired.


### Data Log

In recording mode, the data log file writes to the micro-SD card. A key to the fields in the Data Log is available in the [[bGeigie library README.md by fakufaku|https://github.com/Safecast/SafecastBGeigie]]. 

The data is formatted similarly to the NMEA sentences that GPS uses. It begins with a dollar sign ($) and ends with an asterisk (*). A checksum follows the star.

>Radiation Data Sentence: This is the basic message containing the geo-located radiation measurement.

>EXAMPLE:

>$BNRDD,300,2012-12-16T17:58:31Z,30,1,116,A,4618.9612,N,00658.4831,E,443.7,A,5,1.28*6D

**KEY**

| Field | Description | Example|
| ----- | ----------- | ---- |
| Header   | the device model header | Mini=BMRDD, Nano=BNRDD, NX=BNXRDD |
| Device ID | the device serial number | 300 |
| Date  | the date, formatted according to the ISO-8601 standard, usually in UTC | 2012-12-16T17:58:31Z |
| Radiation 1 minute  | the number of pulses given by the Geiger tube in the last minute | 30 |
| Radiation 5 seconds  | the number of pulses given by the Geiger tube in the last 5 seconds | 1 |
| Radiation total count   | the total number of pulses recorded since startup |  116  |
| Radiation count validity flag  | 'A' indicates the counter has been running for more than one minute, and the one-minute count is not zero. Otherwise, the flag is 'V' (void) | A |
| Latitude | as given by GPS; the format is ddmm.mmmm, where dd is decimal degrees and mm.mmmm is decimal minutes |  4618.9612 |
| Hemisphere |  'N' (north), or 'S' (south) | N |
| Longitude | as given by GPS; he format is dddmm.mmmm where ddd is decimal degrees and mm.mmmm is decimal minutes | 00658.4831 |
| East/West | 'W' (west) or 'E' (east) from the Prime Meridian | E |
| Altitude |  the altitude (above sea level), as given by the GPS, in meters | 443.7 |
| GPS validity | 'A' = Available or OK, 'V' = Void or invalid | A |
| Number of satellites | the number of satellites used by the GPS | 5 |
| HDOP | the Horizontal Dilution of Precision (HDOP), or relative accuracy of the horizontal position | 1.28 |
|Checksum |  | *6D |

*(For possible updates, see the current [README.md](https://github.com/Safecast/SafecastBGeigie/blob/master/README.md) in the technical GitHub repository)*

The data log file name is comprised of three parts: The DID number, the month the log was initiated, and the day the log was initiated. For example, "21080716.LOG" is the data log for unit 2108 from 16 July. Due to space constraints, the file date property is 01/01/2000. However, every line has UTC time date stamp reading.  The log file name may appear to be a day off due to time zone differences.

A data log or section begins with several comment lines, which begin with hash signs (#).

EXAMPLE:

>\# NEW LOG

>\# format=1.2.9nano

>\# deadtime=on

>$BNRDD,2108,2013-12-06T13:03:58Z,22,2,115,A,3145.7607,N,03510.1975,E,734.90,V,3,908*64

One of the comment lines contains the deadtime. [Deadtime](http://en.wikipedia.org/wiki/Dead_time) is "the time after each event during which the system is not able to record another event."  You can find the *ENABLE_LND_DEADTIME compensation formula* in the [bGeigieNano.ino master repository](https://github.com/Safecast/bGeigieNanoKit/blob/master/bGeigieNano.ino). 

### Uploading Data to the Safecast Database With the Safecast API

Your device is designed to submit data to the Safecast database through the Safecast API. To enable data submission, you’ll need to obtain an API key. The key is a credential that gives you access to the API, much like a username and password give you access to an email account.

You can [register on the Safecast site](https://api.safecast.org/en-US/users/sign_up) to get an API key. 

You can learn about how the Safecast API works by visiting the [API documentation](https://api.safecast.org/).

### Uploading Data to Other Datasets

Nano data can be submitted to other radiation mapping datasets. For example, the free [Geiger Bot iOS app](https://sites.google.com/site/geigerbot/) allows you to upload data to servers other than the [Safecast API dataset](https://api.safecast.org/). To do so, you'll need to configure the app with the details of your device, the particular sensor, data format, connection, and other information.  

### Using Safecast With Mobile Apps 

The official [Safecast app for iOS](https://itunes.apple.com/us/app/safecast/id571167450?mt=8) brings our extensive dataset of radiation measurements to your mobile device, and provides a full toolset to help you perform measurements with your own instrument such as a Geiger or scintillation counter (not included). The app functions as a virtual Geiger counter, allowing you to see your location on a map while displaying radiation readings that have been taken nearby. The app is currently for iOS only. 

In addition, the Safecast Drive app for [iOS](https://itunes.apple.com/us/app/safecast-drive/id996229604?mt=8) and [Android](https://play.google.com/store/apps/details?id=io.wizkers.safecast.drive&hl=en) enables you to:

- Connect with your bGeigieNano (if you have the Bluetooth LE module installed), record data, and upload the data to Safecast directly, without removing your MicroSD card
- Access previous logs and update their status

### Troubleshooting

#### Troubleshooting the iSafecast Geiger Bot App for iOS

In principle, the app's analog click counting is simple; it looks for samples in the recording buffer from the OS that exceed threshold X.

However, a variety of factors can affect it. App author Nick Dolezal recommends taking the following steps:

1. Verifying the output of the pulse with an oscilloscope (if you don't have an oscilloscope, you might want to buy an inexpensive DSO Nano online)
2. Ensuring that the voltage is in the [consumer line-level range](http://en.wikipedia.org/wiki/Line_level)
3. Ensuring that the pulse width (duration) is at least [two samples at 44.1 khz](http://en.wikipedia.org/wiki/Nyquist%E2%80%93Shannon_sampling_theorem)
4. Verifying that the pin-out is correct, both at the unit and at the end of the cable. The cable or device must also short mic and ground with a resistor of a certain impedance to be detected by the iPhone. If you overdo that, or if it's done with the cable and unit, you may not see any input. 

Software factors that may affect input include: 

- App settings
- OS settings (such as accessibility features)
- iOS 7 microphone permissions

Hardware factors that may affect output include:

- Differing impedance in electronics or cables
- Non-voltage regulated output + low battery
- Noise from external power (interferes with gamma spectroscopy)
- Configuration settings

**Common Issues**

1. Wrong cable - A "null modem" cable that swaps wires internally may cause problems. The cable should be straight/passthrough.
2. Input voltage too high - Interferes with the audio route and the audio engine's watchdog timer.
3. Mic-ground in the cable not shorted correctly - The app will report an audio route change to "Headphones" on the console. The audio route required for a line input cable is "Headset."

>>Nick Dolezal is working hard on the new version of the Safecast app (which will allow on demand refreshing of the map data) decided to whip up these alternate visualizations of the Safecast data. These maps show the frequency of samples taken – NOT READINGS OF THOSE SAMPLES – just showing how often a specific place has been measured. In this example, a location (like Fukushima) that has been measured repeatedly would show hot. Thought this was a really interesting look at the work Safecast has done over the last 2+ years

#### Troubleshooting the iSafecast Geiger Bot App's API

On the main numeric display of the app, you can tap on the top part of the screen to show detailed log messages containing information on connection attempts, and which input device is selected. If successful, the API upload will return HTTP 201 CREATED.

**Verify Input**

1. Tap the min/max arrow button in the lower-right hand corner of the map to resize it into a small window. The interface will display fixed buttons at the bottom, and four possible display screens at the top. These four screens are "Main Numeric Display," "CPS/CPM graph," "Audio input monitor graph," and "Map."

2. Tap the "<" button next to the white page dots under the map. The audio input monitor will appear.

3. Verify that the counts from the Onyx appear and that they're correct. The black line represents the input. The stationary red line represents the threshold the input volume must exceed for it to be counted. The black line will appear blue when it has been counted as a pulse.

If you're unable to verify input, you may have a defective serial cable. A "Geiger Pulse" setting on the Onyx of other than the default value of 6 may also cause problems. 

4. Configure the App's Settings

a. Tap the "Settings" icon on the fixed button panel (the machine gear). Scroll to the bottom of the list and select "Safecast."
b. Enter your API key. Change the upload interval as desired, but do not change the Safecast device back to Nano. 

#### Troubleshooting Other Issues

If you experience problems with the display, check all solder joints carefully to verify that you haven't missed a joint, or that you don't have a cold solder joint. These issues can cause the display to act unpredictably.

You can report bugs and other issues at the [issues page](https://github.com/Safecast/bGeigieNanoKit/issues) of the Safecast GitHub repository. Volunteer users report problems and bugs in the devices discussion group. 

##### Firmware
* For the current version of the firmware, see the the .hex file in the [bGeigieNanoKit repository](https://github.com/Safecast/bGeigieNanoKit).

* To learn how to update the firmware of the Fio via FTDI, see the ["How to Update the Firmware" page](https://github.com/Safecast/bGeigieNanoKit/wiki/How-to-Update-the-Firmware) of this wiki.

### Cleaning

To clean the unit:

1. Remove the Nano from the case. Never wet the Nano's interior. 
2. Use a soft brush and air blower on the Nano, carefully. A vacuum may be another option (electronics labs use unheated air blowers for dusting off parts.)
3. Rinse the liner in clean water, if necessary. 
4. The case may be rinsed with a clean, slightly damp microfiber cloth. 
5. Turn the screen display off and wait until it's cool before wiping the LEDs. Wipe the screen using light pressure.  Use clean, lint-free cotton, microfiber cloths or low-lint wipes. 

### Warranty Information

For warranty information, email [contact@safecast.org](contact@safecast.org). 

**Calibration** 

“The sensor is pre-calibrated. The calibration of the unit for Bq for Cs assumes the grid is in front of the pancake. The difference is around 5%. For gamma only, the grid has a negligible effect on the measured value.

The uSv/h conversion from CPM is calibrated assuming the grid is on the tube. As all Safecast measurements are done with the pancake WITH grid, please keep the grid on the tube. The grid itself will not block alpha or beta radiation, but will reduce the sensitivity slightly, as the grid covers a small portion of the sensor. In practice, the difference with and without grid will be around 4-5% for a beta source, and less for a gamma source, according to bGeigie Nano user Jam.

The software we have preloaded on the device takes the sensor type into account and calibrates readings for Cs137. All bGeigie Nanos have the same configuration and can be compared against one other. A bGeigie Nano's calibration should never change, unless the device is broken. 

### Hardware Cautions

- The Pelican micro case is shockproof and water-resistant. However, it is not waterproof and it cannot be used underwater. Condensation can build up inside the case, due to temperature differences. While this condensation will evaporate with time, you can also remove it by opening the unit and drying it with fiber-free cloth. 

- Do not use the unit outside of its water-resistant case with wet hands, in the rain, or close to a dripping tap. 

- The Micro-SD cannot be read through a USB port. Use a micro-SD card reader instead.

**Battery Cautions:** 

Charge the unit with the mini-USB cable. The yellow LED will dim once the unit is fully charged. A full charge takes 5-6 hours. The bGeigie Nano runs approx 35-40 hours on a full charge in recording mode. 

The main battery indicator is the icon at the bottom right of the display. The percent of battery charge appears on the startup screen.

**NEVER charge the bGeigie while the unit is on.** The power MUST be turned OFF before charging to avoid permanent damage to the charge circuit. 

+ Replace a broken battery (that has a detached wire) with one that has the same rating (2000mAh 3.7V Lithium Polymer (Li-Po, Li-Poly). The Nano charge circuit may overheat if you use a larger battery.

+ A Li-Po battery can be hazardous if the mylar membrane is punctured. Please handle batteries carefully! 

**SENSOR Cautions:**

Although the Nano sensor inside its closed Pelican micro-case is well protected from shock and pressure transients, sudden changes in pressure (such as slamming a car door) may damage the sensor. The pressure inside the tube is very low, and the membrane is under constant tension from atmospheric pressure. Being at altitude *reduces* the strain on it. Because the membrane is mica (basically extremely thin glass), it is low-density and very brittle. A sharp shock can shatter it, and it will pop if something pokes it. 

- To avoid damage to the pancake sensor, NEVER touch the membrane! 

**Onyx Hardware Precautions:**

To keep the Onyx™ in good condition, handle it with care, and observe the following precautions:

- Do not contaminate the Onyx™ by touching it to radioactive surfaces or materials. Instead, hold it just above the surface that is suspected of contamination to take readings. 

- Do not leave the Onyx™ in temperatures over 122° F (50° C), or in direct sunlight for extended periods of time. 

- If the surface of the mica on your pancake detector becomes scratched or loses its coating, avoid making measurements with the detector window in direct sunlight, as this could affect the readings. 

- Do not put the Onyx™ in a microwave oven. It cannot measure microwaves, and you may damage it or the oven. 


#### Resources and Support

- [Nano catch-all landing page](http://blog.safecast.org/bgeigie-nano/)

- [Safecast website](http://blog.safecast.org/) - articles, news, notices, API uploads, crowdsourced radiation data and maps, and more

- [Safecast Devices Discussions and Support group](https://groups.google.com/forum/?hl=en#!forum/safecast-devices) 
