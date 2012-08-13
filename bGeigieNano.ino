/*
   The bGeigie-nano
   A device for car-borne radiation measurement (aka Radiation War-driving).

   Copyright (c) 2012, Lionel Bergeret
   Copyright (c) 2011, Robin Scheibler aka FakuFaku, Christopher Wang aka Akiba
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <limits.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <stdlib.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include "TinyGPS.h"

// Definition flags -----------------------------------------------------------
//#define USE_SSD1306
//#define USE_SSD1306_DISTANCE
//#define USE_SSD1306_CARDINAL
#define USE_SOFTGPS // use software serial for GPS (Arduino Pro Mini)
//#define USE_STATIC_GPS // for test only
//#define USE_HARDWARE_COUNTER // pulse on digital pin5
#define USE_OPENLOG // disable for debugging
//#define USE_MEDIATEK // MTK3339 initialization
//#define USE_SKYTRAQ // SkyTraq Venus 6 initialization
#define USE_EEPROM_ID // use device id stored in EEPROM

#ifndef USE_SSD1306 // high memory usage
#define DEBUG // enable debug log output
//#define DEBUG_DIAGNOSTIC
#endif

#ifdef USE_HARDWARE_COUNTER
#define USE_SLEEPMODE 
#endif

// PINs definition ------------------------------------------------------------
#ifdef USE_HARDWARE_COUNTER
// Pin assignment for version 1.0.1
#warning Hardware counter is used !
#define OLED_RESET 4
#define MINIPRO_GPS_RX_PIN 6
#define MINIPRO_GPS_TX_PIN 7
#define OPENLOG_RX_PIN 8
#define OPENLOG_TX_PIN 9
#define OPENLOG_RST_PIN 10
#else
// Old Pin assignment for version 1.0.0
#warning Interrupt counter is used !
#define OLED_RESET 4
#define MINIPRO_GPS_RX_PIN 5
#define MINIPRO_GPS_TX_PIN 6
#define OPENLOG_RX_PIN 7
#define OPENLOG_TX_PIN 8
#define OPENLOG_RST_PIN 9
#endif
#define GPS_LED_PIN 13
#define VOLTAGE_PIN A7

// OLED settings --------------------------------------------------------------
#ifdef USE_SSD1306
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#ifdef USE_SSD1306_DISTANCE
bool gps_fix_first = true;
float gps_last_lon = 0, gps_last_lat = 0;
unsigned long int gps_distance = 0;
#endif

#endif

// Geiger settings ------------------------------------------------------------
#define TIME_INTERVAL 5000
#define LINE_SZ 100
#define BUFFER_SZ 12
#define STRBUFFER_SZ 32
#define NX 12
#define AVAILABLE 'A'  // indicates geiger data are ready (available)
#define VOID      'V'  // indicates geiger data not ready (void)
#define BMRDD_EEPROM_ID 100
#define BMRDD_ID_LEN 3

// log file headers
#define LOGFILE_HEADER "# NEW LOG\n# format=1.0.0nano\n"
char hdr[6] = "BNRDD";  // header for sentence
char logfile_name[13];  // placeholder for filename
bool logfile_ready = false;
char logfile_ext[] = ".log";

// geiger statistics
unsigned long shift_reg[NX] = {0};
unsigned long reg_index = 0;
unsigned long total_count = 0;
int str_count = 0;
char geiger_status = VOID;

// the line buffer for serial receive and send
static char line[LINE_SZ];
static char strbuffer[STRBUFFER_SZ];

// geiger id
char dev_id[BMRDD_ID_LEN+1] = {'2', '0', '0', 0};  // device id (default 200)

// Pulse counter --------------------------------------------------------------
#ifdef USE_HARDWARE_COUNTER
// Hardware counter
#include "HardwareCounter.h"
#define COUNTER_TIMER1 5  // the timer1 pin on the 328p is D5
HardwareCounter hwc(COUNTER_TIMER1, TIME_INTERVAL);
#else
// Interrupt counter
#include "InterruptCounter.h"
#define COUNTER_INTERRUPT 0 // 0 = dpin2, 1 = dpin3
#endif

#ifdef USE_HARDWARE_COUNTER
#ifdef USE_SLEEPMODE
#define IS_READY (1)
#else
#define IS_READY (hwc.available())
#endif
#else
#define IS_READY (interruptCounterAvailable())
#endif

// OpenLog settings -----------------------------------------------------------
#ifdef USE_OPENLOG
#define OPENLOG_RETRY 200
SoftwareSerial OpenLog(OPENLOG_RX_PIN, OPENLOG_TX_PIN); //Connect TXO of OpenLog to pin 8, RXI to pin 7
static const int resetOpenLog = OPENLOG_RST_PIN; //This pin resets OpenLog. Connect pin 9 to pin GRN on OpenLog.
#endif
bool openlog_ready = false;

// GpsBee settings ------------------------------------------------------------
TinyGPS gps(true);
#define GPS_INTERVAL 1000
char gps_status = VOID;
static const int ledPin = GPS_LED_PIN;

#ifdef USE_SOFTGPS
SoftwareSerial gpsSerial(MINIPRO_GPS_RX_PIN, MINIPRO_GPS_TX_PIN);
#endif

// Gps data buffers
static char lat[BUFFER_SZ];
static char lon[BUFFER_SZ];

// MTK33x9 chipset
#define PMTK_SET_NMEA_UPDATE_1HZ "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define SBAS_ENABLE "$PMTK313,1*2E\r\n"
#define DGPS_WAAS_ON "$PMTK301,2*2E\r\n" # 2 = WAAS

#ifdef USE_STATIC_GPS
#include <avr/pgmspace.h>
// GPS test sentences
char strGPRMC[] PROGMEM = "$GPRMC,201547.000,A,3014.5527,N,09749.5808,W,0.24,163.05,040109,,*1A";
char strGPGGA[] PROGMEM = "$GPGGA,201548.000,3014.5529,N,09749.5808,W,1,07,1.5,225.6,M,-22.5,M,18.8,0000*78";
char *teststrs[2] = {strGPRMC, strGPGGA};

static void sendstring(TinyGPS &gps, const PROGMEM char *str)
{
  while (true)
  {
    char c = pgm_read_byte_near(str++);
    if (!c) break;
    gps.encode(c);
  }
  gps.encode('\r');
  gps.encode('\n');
}
#endif

// Debug definitions ----------------------------------------------------------
#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// Function definitions ---------------------------------------------------------
// Atmel Tips and Tricks: 3.6 Tip #6 – Access types: Static
static unsigned long cpm_gen();
static bool gps_gen_filename(TinyGPS &gps, char *buf);
static bool gps_gen_timestamp(TinyGPS &gps, char *buf, unsigned long counts, unsigned long cpm, unsigned long cpb);
static char checksum(char *s, int N);
#ifdef USE_OPENLOG
static void setupOpenLog();
static void createFile(char *fileName);
#endif
static void gps_program_settings();
#ifdef USE_EEPROM_ID
static void setEEPROMDevId(char * id);
static void getEEPROMDevId();
#endif
static float read_voltage(int pin);
static int availableMemory();
static unsigned long elapsedTime(unsigned long startTime);

// Sleep mode -----------------------------------------------------------------
#ifdef USE_SLEEPMODE
#include <avr/sleep.h>
#include <avr/power.h>

volatile int f_wdt=1;

ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    DEBUG_PRINTLN("WARNING: WDT Overrun");
  }
}

void enableSleepTimer(void) 
{
  cli();
  wdt_reset();

  // Setup WDT
  // Clear the reset flag
  MCUSR &= ~(1<<WDRF);
  
  // Set WDCE (4 clock cycles updates)
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  // Set new watchdog timeout prescaler value
  WDTCSR = 1<<WDP3; // 4.0 seconds
  
  // Enable the WD interrupt
  WDTCSR |= _BV(WDIE);

  sei();
}

void disableSleepTimer(void) 
{
  cli();
  wdt_reset();

  // Setup WDT
  // Clear the reset flag
  MCUSR &= ~(1<<WDRF);
  
  // Keep old prescaler setting to prevent unintentional time-out
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  
  // Disable the WD interrupt
  WDTCSR |= 0x00;

  sei();
}

void enterSleep(void)
{
  power_all_disable();
  set_sleep_mode(SLEEP_MODE_STANDBY); // keep oscillator active
  
  // Enable and enter sleep mode
  sleep_enable();
  sleep_mode();
  
  // The program will continue from here after the WDT timeout

  // Disable sleep and re-enable the peripherals
  sleep_disable();
  power_all_enable();
}
#endif

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------
void setup()
{
  // Set the EEPPROM device id at first run
  //setEEPROMDevId("204");
  
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

#ifndef USE_SLEEPMODE
  // enable and reset the watchdog timer
  wdt_enable(WDTO_8S);
  wdt_reset();
#endif

#ifdef USE_OPENLOG
  DEBUG_PRINTLN("Initializing OpenLog.");
  OpenLog.begin(9600);
  setupOpenLog();
#endif

  DEBUG_PRINTLN("Initializing pulse counter.");

#ifdef USE_HARDWARE_COUNTER
  // Start the Pulse Counter!
  hwc.start();
#else
  // Create pulse counter on INT1
  interruptCounterSetup(COUNTER_INTERRUPT, TIME_INTERVAL);

  // And now Start the Pulse Counter!
  interruptCounterReset();
#endif

#ifdef USE_SOFTGPS
  DEBUG_PRINTLN("Initializing GPS.");
  gpsSerial.begin(9600);

  // Put GPS serial in listen mode
  gpsSerial.listen();

  // initialize and program the GPS module
  gps_program_settings();
#endif

#ifdef USE_EEPROM_ID
  getEEPROMDevId();
#endif

  DEBUG_PRINT("Devide id = ");
  DEBUG_PRINTLN(dev_id);

#ifdef DEBUG_DIAGNOSTIC
  // setup analog reference to read battery and boost voltage
  analogReference(INTERNAL);
#endif

#ifdef USE_SLEEPMODE
  enableSleepTimer();
#endif

#ifdef USE_SSD1306
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display(); // show splashscreen
#endif

  Serial.println(availableMemory());

  DEBUG_PRINTLN("Setup completed.");
}

// ----------------------------------------------------------------------------
// Main loop
// ----------------------------------------------------------------------------
void loop()
{
  bool gpsReady = false;

#ifdef USE_SLEEPMODE
  if(f_wdt == 1)
  {
    disableSleepTimer();
#endif

#ifdef USE_SOFTGPS
  // Put GPS serial in listen mode
  gpsSerial.listen();
#endif
  
  // For one second we parse GPS sentences
  for (unsigned long start = millis(); (elapsedTime(start) < GPS_INTERVAL) and !IS_READY;)
  {
#ifdef USE_STATIC_GPS
    for (int i=0; i<2; ++i)
    {
      sendstring(gps, teststrs[i]);
    }
#else
#ifdef USE_SOFTGPS
    while (gpsSerial.available())
    {
      char c = gpsSerial.read();
#else
    while (Serial.available())
    {
      char c = Serial.read();
#endif

      //Serial.print(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        gpsReady = true;
    }
#endif
  }

#ifdef USE_SLEEPMODE
  // Will wakeup in 4 seconds from now
  enableSleepTimer();
#endif
  
  if ((gpsReady) || (gps_status == AVAILABLE)) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  // generate CPM every TIME_INTERVAL seconds
  if IS_READY {
      unsigned long cpm=0, cpb=0;

#ifndef USE_SLEEPMODE
      // first, reset the watchdog timer
      wdt_reset();
#endif

#ifdef USE_HARDWARE_COUNTER
      // obtain the count in the last bin
      cpb = hwc.count();

      // reset the pulse counter
      hwc.start();
#else
      // obtain the count in the last bin
      cpb = interruptCounterCount();

      // reset the pulse counter
      interruptCounterReset();
#endif

      // insert count in sliding window and compute CPM
      shift_reg[reg_index] = cpb;     // put the count in the correct bin
      reg_index = (reg_index+1) % NX; // increment register index
      cpm = cpm_gen();                // compute sum over all bins

      // update the total counter
      total_count += cpb;
      
      // set status of Geiger
      if (str_count < NX)
      {
        geiger_status = VOID;
        str_count++;
      } else if (cpm == 0) {
        geiger_status = VOID;
      } else {
        geiger_status = AVAILABLE;
      }
      
      // generate timestamp. only update the start time if 
      // we printed the timestamp. otherwise, the GPS is still 
      // updating so wait until its finished and generate timestamp
      memset(line, 0, LINE_SZ);
      gps_gen_timestamp(gps, line, shift_reg[reg_index], cpm, cpb);

      if ((!logfile_ready) && (gps_status == AVAILABLE))
      {
         if (gps_gen_filename(gps, logfile_name)) {
           logfile_ready = true;

#ifdef USE_OPENLOG
           DEBUG_PRINTLN("Create new logfile.");
           createFile(logfile_name);
           // print header to serial
           sprintf_P(strbuffer, PSTR(LOGFILE_HEADER));
           OpenLog.print(strbuffer);
#endif
           DEBUG_PRINT(strbuffer);
         }
      }

      // Printout line
      DEBUG_PRINTLN(line);

#ifdef DEBUG_DIAGNOSTIC
      int v0 = (int)(read_voltage(VOLTAGE_PIN));
      DEBUG_PRINT("$DIAG,");
      DEBUG_PRINTLN(v0);
#endif
      
#ifdef USE_OPENLOG
      if (logfile_ready) {
        // Put OpenLog serial in listen mode
        OpenLog.listen();
        OpenLog.println(line);

#ifdef DEBUG_DIAGNOSTIC
        OpenLog.print("$DIAG,");
        OpenLog.println(v0);
#endif
      }
#endif
  }

#ifdef USE_SLEEPMODE
    // Leave some time to serial to flush data
    delay(100);

    // Clear the flag
    f_wdt = 0;
    
    // Re-enter sleep mode
    enterSleep();
  }
#endif
  
}

// ----------------------------------------------------------------------------
// Utility functions
// ----------------------------------------------------------------------------

/* calculate elapsed time. this takes into account rollover */
unsigned long elapsedTime(unsigned long startTime) {
  unsigned long stopTime = millis();

  if (startTime >= stopTime) {
    return startTime - stopTime;
  } else {
    return (ULONG_MAX - (startTime - stopTime));
  }
}

#ifdef USE_OPENLOG
/* setups up the software serial, resets OpenLog */
void setupOpenLog() {
  int safeguard = 0;

  pinMode(resetOpenLog, OUTPUT);
  OpenLog.listen();

  // reset OpenLog
  DEBUG_PRINTLN(" - reset");
  digitalWrite(resetOpenLog, LOW);
  delay(100);
  digitalWrite(resetOpenLog, HIGH);

  safeguard = 0;
  while(safeguard < OPENLOG_RETRY) {
    safeguard++;
    if(OpenLog.available())
      if(OpenLog.read() == '>') break;
    delay(10);
  }

  if (safeguard >= OPENLOG_RETRY) {
    DEBUG_PRINTLN("OpenLog init failed ! Check if the CONFIG.TXT is set to 9600,26,3,2");
    // Assume "newlog mode" is active
    logfile_ready = true;
  } else {
    DEBUG_PRINTLN(" - ready");
    openlog_ready = true;
  }
}

/* create a new file */
void createFile(char *fileName) {
  int result = 0;
  int safeguard = 0;

  OpenLog.listen();

  do {
    result = 0;

    do {
#ifndef USE_SLEEPMODE
      // reset the watchdog timer
      wdt_reset();
#endif

      DEBUG_PRINT(" - append ");
      DEBUG_PRINTLN(fileName);

      OpenLog.print("append ");
      OpenLog.print(fileName);
      OpenLog.write(13); //This is \r

      // wait for OpenLog to indicate file is open and ready for writing
      DEBUG_PRINTLN(" - wait");
      safeguard = 0;
      while(safeguard < OPENLOG_RETRY) {
        safeguard++;
        if(OpenLog.available())
          if(OpenLog.read() == '<') break;
        delay(10);
      }
      if (safeguard >= OPENLOG_RETRY) {
        DEBUG_PRINTLN("Append file failed");
        break;
      } else {
        DEBUG_PRINTLN(" - ready");
      }
      result = 1;
    } while (0);

    if (0 == result) {
      // reset OpenLog
      DEBUG_PRINTLN(" - reset");
      digitalWrite(resetOpenLog, LOW);
      delay(100);
      digitalWrite(resetOpenLog, HIGH);

      //Wait for OpenLog to return to waiting for a command
      safeguard = 0;
      while(safeguard < OPENLOG_RETRY) {
        safeguard++;
        if(OpenLog.available())
          if(OpenLog.read() == '>') break;
        delay(10);
      }
#ifdef DEBUG
      if (safeguard >= OPENLOG_RETRY) {
        DEBUG_PRINTLN("OpenLog init failed ! Check if the CONFIG.TXT is set to 9600,26,3,2");
      } else {
        DEBUG_PRINTLN(" - ready");
      }
#endif
    }
  } while (0 == result);

  //OpenLog is now waiting for characters and will record them to the new file  
}
#endif

/* compute check sum of N bytes in array s */
char checksum(char *s, int N)
{
  int i = 0;
  char chk = s[0];

  for (i=1 ; i < N ; i++)
    chk ^= s[i];

  return chk;
}

/* compute cpm */
unsigned long cpm_gen()
{
   unsigned int i;
   unsigned long c_p_m = 0;
   
   // sum up
   for (i=0 ; i < NX ; i++)
     c_p_m += shift_reg[i];
   
   return c_p_m;
}

/* generate log filename */
bool gps_gen_filename(TinyGPS &gps, char *buf) {
  int year = 2012;
  byte month = 0, day = 0, hour = 0, minute = 0, second = 0, hundredths = 0;
  unsigned long age;

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (TinyGPS::GPS_INVALID_AGE == age) {
    return false;
  }
  
  // Create the filename for that drive
  strcpy(buf, dev_id);
  strcat(buf, "-");
  sprintf_P(strbuffer, PSTR("%02d"),month); 
  strncat(buf, strbuffer, 2);
  sprintf_P(strbuffer, PSTR("%02d"),day);
  strncat(buf, strbuffer, 2);
  strcpy(buf+8, logfile_ext);

  return true;
}

/* convert long integer from TinyGPS to string "xxxxx.xxxx" */
void get_coordinate_string(unsigned long val, char *buf)
{
  unsigned long left = 0;
  unsigned long right = 0;

  left = val/100000.0;
  right = (val - left*100000)/10;
  sprintf_P(buf, PSTR("%ld.%04ld"), left, right);
}

#ifdef USE_SSD1306_DISTANCE
/* convert long integer from TinyGPS to float WGS84 degrees */
float get_wgs84_coordinate(unsigned long val)
{
  double result = 0.0;
  result = val/10000000.0;
  result = ((result-(int)result)/60.0)*100 + (int)result;
  return (float)result;
}
#endif

/* generate log result line */
bool gps_gen_timestamp(TinyGPS &gps, char *buf, unsigned long counts, unsigned long cpm, unsigned long cpb)
{
  int year = 2012;
  byte month = 0, day = 0, hour = 0, minute = 0, second = 0, hundredths = 0;
  long int x = 0, y = 0;
  float faltitude = 0, fspeed = 0;
  unsigned short nbsat = 0;
  unsigned long precission = 0;
  unsigned long age;
  byte len, chk;
  char NS = 'N';
  char WE = 'E';

  memset(lat, 0, BUFFER_SZ);
  memset(lon, 0, BUFFER_SZ);
  memset(strbuffer, 0, STRBUFFER_SZ);
  
  // get GPS date
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (TinyGPS::GPS_INVALID_AGE == age) {
    year = 2012, month = 0, day = 0, hour = 0, minute = 0, second = 0, hundredths = 0;
  }
  
  // get GPS position, altitude and speed
  gps.get_position(&x, &y, &age);

  if (!gps.status()) {
    gps_status = VOID;
  } else {
    gps_status = AVAILABLE;
  } 
  faltitude = gps.f_altitude();
  fspeed = gps.f_speed_kmph();
  nbsat = gps.satellites();
  precission = gps.hdop();

  if (x < 0) { NS = 'S'; x = -x;}
  if (y < 0) { WE = 'W'; y = -y;}
  get_coordinate_string(x == TinyGPS::GPS_INVALID_ANGLE ? 0 : x, lat);
  get_coordinate_string(y == TinyGPS::GPS_INVALID_ANGLE ? 0 : y, lon);
  dtostrf(faltitude == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0.0 : faltitude, 0, 2, strbuffer);
  
  // prepare the log entry
  memset(buf, 0, LINE_SZ);
  sprintf_P(buf, PSTR("$%s,%s,%02d-%02d-%02dT%02d:%02d:%02dZ,%ld,%ld,%ld,%c,%s,%c,%s,%c,%s,%c,%ld,%d"),  \
              hdr, \
              dev_id, \
              year, month, day,  \
              hour, minute, second, \
              cpm, \
              cpb, \
              total_count, \
              geiger_status, \
              lat, NS,\
              lon, WE,\
              strbuffer, \
              gps_status, \
              precission == TinyGPS::GPS_INVALID_HDOP ? 0 : precission, \
              nbsat  == TinyGPS::GPS_INVALID_SATELLITES ? 0 : nbsat);

   len = strlen(buf);
   buf[len] = '\0';

   // generate checksum
   chk = checksum(buf+1, len);

   // add checksum to end of line before sending
   if (chk < 16)
     sprintf_P(buf + len, PSTR("*0%X"), (int)chk);
   else
     sprintf_P(buf + len, PSTR("*%X"), (int)chk);
       
#ifdef USE_SSD1306
#ifdef USE_SSD1306_DISTANCE
   // compute distance
   if (gps.status()) {
     int trigger_dist = 25;
     float flat = get_wgs84_coordinate(x);
     float flon = get_wgs84_coordinate(y);
    
     if(fspeed > 5) 
       // fpspeed/3.6 * 5s = 6.94 m
       trigger_dist = 5;
     if(fspeed > 10) 
       trigger_dist = 10;
     if(fspeed > 15) 
       trigger_dist = 20;
    
     if(gps_fix_first) 
     {
       gps_last_lat = flat;
       gps_last_lon = flon;
       gps_fix_first = false;
     } 
     else 
     {
       // Distance in meters
       unsigned long int dist = (long int)TinyGPS::distance_between(flat, flon, gps_last_lat, gps_last_lon);

       if (dist > trigger_dist)   
       {
         gps_distance += dist;   
         gps_last_lat = flat;
         gps_last_lon = flon;
       }
     }    
   }
#endif

   // ready to display the data on screen
   display.clearDisplay();
   int offset = 0;
   
   // Display date
   sprintf_P(strbuffer, PSTR("%02d/%02d %02d:%02d:%02d"),  \
              day, month, \
              hour, minute, second);
   display.setCursor(2, offset+24); // textsize*8 
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.println(strbuffer);
 
   // Display CPM
   display.setCursor(2, offset);
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.print("CPM ");
   display.println(cpm);

   // Display SD, GPS and Geiger states
   if (openlog_ready) {
     display.setTextColor(WHITE);
   } else {
     display.setTextColor(BLACK, WHITE); // 'inverted' text
   }
   display.setCursor(116, offset);
   display.setTextSize(1);
   if (!gps.status()) {
     display.println(gps_status);
   } else {
     sprintf(strbuffer, "%X", nbsat);
     display.println(strbuffer);
   }
   display.setCursor(122, offset);
   display.println(geiger_status);
 
   // Display uSv/h
   dtostrf((float)(cpm/334.0), 0, 3, strbuffer);
   display.setTextColor(WHITE);
   display.setTextSize(1);
   display.setCursor(2, offset+16); // textsize*8 
   display.print(strbuffer);
   display.println(" uSv/h");

#ifdef USE_SSD1306_DISTANCE
   // Display distance
   dtostrf((float)(gps_distance/1000.0), 0, 1, strbuffer);
   display.setTextColor(WHITE);
   display.setTextSize(1);
   display.setCursor(116-(strlen(strbuffer)*6), offset+16); // textsize*8 
   display.print(strbuffer);
   display.println("km");
#endif
   
   // Display altidude
   if (gps.status()) {
#ifdef USE_SSD1306_CARDINAL
       sprintf(strbuffer, "%s", gps.f_course() == TinyGPS::GPS_INVALID_F_ANGLE ? "***" : TinyGPS::cardinal(gps.f_course()));
       display.setTextSize(1);
       display.setCursor(128-(strlen(strbuffer)*6), offset+8); // textsize*8 
       display.println(strbuffer);
#endif

       dtostrf(faltitude, 0, 0, strbuffer);
       display.setTextSize(1);
       display.setCursor(122-(strlen(strbuffer)*6), offset+24); // textsize*8 
       display.print(strbuffer);
       display.println("m");
   }
      
   display.display();
#endif

   return (gps_status == AVAILABLE);
}

/* setup the GPS module to 1Hz and RMC+GGA messages only */
void gps_program_settings()
{
#ifdef USE_MEDIATEK
  gpsSerial.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gpsSerial.println(PMTK_SET_NMEA_UPDATE_1HZ);
  gpsSerial.println(SBAS_ENABLE);
  gpsSerial.println(DGPS_WAAS_ON);
#endif

#ifdef USE_SKYTRAQ
  // all GPS command taken from datasheet
  // "Binary Messages Of SkyTraq Venus 6 GPS Receiver"

  // set GGA and RMC output at 1Hz
  uint8_t GPS_MSG_OUTPUT_GGARMC_1S[9] = { 0x08, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01 }; // with update to RAM and FLASH
  uint16_t GPS_MSG_OUTPUT_GGARMC_1S_L = 9;

  // Power Save mode (not sure what it is doing at the moment
  uint8_t GPS_MSG_PWR_SAVE[3] = { 0x0C, 0x01, 0x01 }; // update to FLASH too
  uint16_t GPS_MSG_PWR_SAVE_L = 3;

  // wait for GPS to start
  while(!gpsSerial.available())
    delay(10);

  // send all commands
  gps_send_message(GPS_MSG_OUTPUT_GGARMC_1S, GPS_MSG_OUTPUT_GGARMC_1S_L);
  gps_send_message(GPS_MSG_PWR_SAVE, GPS_MSG_PWR_SAVE_L);
#endif
}

void gps_send_message(const uint8_t *msg, uint16_t len)
{
  uint8_t chk = 0x0;
  // header
  gpsSerial.write(0xA0);
  gpsSerial.write(0xA1);
  // send length
  gpsSerial.write(len >> 8);
  gpsSerial.write(len & 0xff);
  // send message
  for (unsigned int i = 0 ; i < len ; i++)
  {
    gpsSerial.write(msg[i]);
    chk ^= msg[i];
  }
  // checksum
  gpsSerial.write(chk);
  // end of message
  gpsSerial.write(0x0D);
  gpsSerial.write(0x0A);
  gpsSerial.write('\n');
}

#ifdef USE_EEPROM_ID
/* retrieve the device id from EEPROM */
void getEEPROMDevId()
{
  // counter for trials of reading EEPROM
  int n = 0;
  int N = 3;

  cli(); // disable all interrupts

  for (int i=0 ; i < BMRDD_ID_LEN ; i++)
  {
    // try to read one time
    dev_id[i] = (char)EEPROM.read(i+BMRDD_EEPROM_ID);
    n = 1;
    // while it's not numberic, and up to N times, try to reread.
    while ((dev_id[i] < '0' || dev_id[i] > '9') && n < N)
    {
      // wait a little before we retry
      delay(10);        
      // reread once and then increment the counter
      dev_id[i] = (char)EEPROM.read(i+BMRDD_EEPROM_ID);
      n++;
    }

    // catch when the read number is non-numeric, replace with capital X
    if (dev_id[i] < '0' || dev_id[i] > '9')
      dev_id[i] = 'X';
  }

  // set the end of string null
  dev_id[BMRDD_ID_LEN] = '\0';

  sei(); // re-enable all interrupts
}

void setEEPROMDevId(char * id)
{
 for (int i=0 ; i < BMRDD_ID_LEN ; i++)
  {
    EEPROM.write(BMRDD_EEPROM_ID+i, byte(id[i]));
  }
}
#endif

#ifdef DEBUG_DIAGNOSTIC
float read_voltage(int pin)
{
  return 1.1*analogRead(pin)*(3300/1024);
}
#endif

int availableMemory() 
{
  int size = 1024;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
}
