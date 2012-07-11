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
#include "TinyGPS.h"
#include <EEPROM.h>

// Definition flags -----------------------------------------------------------
#define USE_SOFTGPS // use software serial for GPS (Arduino Pro Mini)
//#define USE_HARDWARE_COUNTER // pulse on digital pin5
#define USE_OPENLOG // disable for debugging
//#define USE_MEDIATEK // MTK3339 initialization
//#define USE_SKYTRAQ // SkyTraq Venus 6 initialization
#define USE_EEPROM_ID // use device id stored in EEPROM
#define DEBUG // enable debug log output
//#define DEBUG_DIAGNOSTIC
#ifdef USE_HARDWARE_COUNTER
#define USE_SLEEPMODE 
#endif

// PINs definition ------------------------------------------------------------
#ifdef USE_HARDWARE_COUNTER
// Pin assignment for version 1.0.1
#warning Hardware counter is used !
#define MINIPRO_GPS_RX_PIN 6
#define MINIPRO_GPS_TX_PIN 7
#define OPENLOG_RX_PIN 8
#define OPENLOG_TX_PIN 9
#define OPENLOG_RST_PIN 10
#else
// Old Pin assignment for version 1.0.0
#warning Interrupt counter is used !
#define MINIPRO_GPS_RX_PIN 5
#define MINIPRO_GPS_TX_PIN 6
#define OPENLOG_RX_PIN 7
#define OPENLOG_TX_PIN 8
#define OPENLOG_RST_PIN 9
#endif
#define GPS_LED_PIN 13
#define VOLTAGE_PIN A7

// Geiger settings ------------------------------------------------------------
#define TIME_INTERVAL 5000
#define LINE_SZ 100
#define BUFFER_SZ 16
#define NX 12
#define AVAILABLE 'A'  // indicates geiger data are ready (available)
#define VOID      'V'  // indicates geiger data not ready (void)
#define BMRDD_EEPROM_ID 100
#define BMRDD_ID_LEN 3

// log file headers
char hdr[6] = "BNRDD";  // header for sentence
char fileHeader[] = "# NEW LOG\n# format=1.0.0nano\n";
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

// OpenLog settings -----------------------------------------------------------
#ifdef USE_OPENLOG
#define OPENLOG_RETRY 200
SoftwareSerial OpenLog(OPENLOG_RX_PIN, OPENLOG_TX_PIN); //Connect TXO of OpenLog to pin 8, RXI to pin 7
static const int resetOpenLog = OPENLOG_RST_PIN; //This pin resets OpenLog. Connect pin 9 to pin GRN on OpenLog.
#endif

// GpsBee settings ------------------------------------------------------------
TinyGPS gps;
#define GPS_INTERVAL 1000
char gps_status = VOID;
static const int ledPin = GPS_LED_PIN;

#ifdef USE_SOFTGPS
SoftwareSerial gpsSerial(MINIPRO_GPS_RX_PIN, MINIPRO_GPS_TX_PIN);
#endif

// Gps data buffers
static char lat[BUFFER_SZ];
static char lon[BUFFER_SZ];
static char alt[BUFFER_SZ];
static char spd[BUFFER_SZ];
static char sat[BUFFER_SZ];
static char pre[BUFFER_SZ];

// MTK33x9 chipset
#define PMTK_SET_NMEA_UPDATE_1HZ "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

// Debug definitions ----------------------------------------------------------
#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// Function definitions ---------------------------------------------------------
unsigned long cpm_gen();
bool gps_gen_filename(TinyGPS &gps, char *buf);
bool gps_gen_timestamp(TinyGPS &gps, char *buf, unsigned long counts, unsigned long cpm, unsigned long cpb);
#ifdef USE_OPENLOG
void setupOpenLog();
void createFile(char *fileName);
#endif
void gps_program_settings();
#ifdef USE_EEPROM_ID
void setEEPROMDevId(char * id);
void getEEPROMDevId();
#endif
float read_voltage(int pin);

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
void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  
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
  // Setup WDT
  // Clear the reset flag
  MCUSR &= ~(1<<WDRF);
  
  // Set WDCE (4 clock cycles updates)
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  // Set new watchdog timeout prescaler value
  WDTCSR = 1<<WDP3; // 4.0 seconds
  
  // Enable the WD interrupt
  WDTCSR |= _BV(WDIE);
#endif

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
#endif

#ifdef USE_SOFTGPS
  // Put GPS serial in listen mode
  gpsSerial.listen();
#endif
  
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < GPS_INTERVAL;)
  {
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
  }
  
  if ((gpsReady) || (gps_status == AVAILABLE)) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  // generate CPM every TIME_INTERVAL seconds
#ifdef USE_HARDWARE_COUNTER 
  if (hwc.available()) {
#else
  if (interruptCounterAvailable())  {
#endif
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
           OpenLog.print(fileHeader);
#endif
           DEBUG_PRINT(fileHeader);
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

/* Convert a coordinate from Degrees to DegreesMinutes (NMEA) */
void convertGPStoNMEA(double coordinate, char *buf) {
    double Degrees          = ( int )coordinate;   
    double Minutes          = coordinate - Degrees;     
    double DegreesMinutes   = Minutes * 60 + ( Degrees * 10 * 10 );   

    dtostrf(DegreesMinutes, 0, 4, buf); // 4 decimal places
}

/* generate log filename */
bool gps_gen_filename(TinyGPS &gps, char *buf) {
  int year = 2012;
  byte month = 0, day = 0, hour = 0, minute = 0, second = 0, hundredths = 0;
  unsigned long age;
  char temp[8];

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (TinyGPS::GPS_INVALID_AGE == age) {
    return false;
  }
  
  // Create the filename for that drive
  strcpy(buf, dev_id);
  strcat(buf, "-");
  sprintf(temp, "%02d",month); 
  strncat(buf, temp, 2);
  sprintf(temp, "%02d",day);
  strncat(buf, temp, 2);
  strcpy(buf+8, logfile_ext);

  return true;
}

/* generate log result line */
bool gps_gen_timestamp(TinyGPS &gps, char *buf, unsigned long counts, unsigned long cpm, unsigned long cpb)
{
  int year = 2012;
  byte month = 0, day = 0, hour = 0, minute = 0, second = 0, hundredths = 0;
  float flat = 0, flon = 0, faltitude = 0, fspeed = 0;
  unsigned short nbsat = 0;
  unsigned long precission = 0;
  unsigned long age;
  byte len, chk;
  char NS = 'N';
  char WE = 'E';

  memset(lat, 0, BUFFER_SZ);
  memset(lon, 0, BUFFER_SZ);
  memset(alt, 0, BUFFER_SZ);
  memset(spd, 0, BUFFER_SZ);
  memset(sat, 0, BUFFER_SZ);
  memset(pre, 0, BUFFER_SZ);
  
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (TinyGPS::GPS_INVALID_AGE == age) {
    year = 2012, month = 0, day = 0, hour = 0, minute = 0, second = 0, hundredths = 0;
  }
  gps.f_get_position(&flat, &flon, &age);
  if (TinyGPS::GPS_INVALID_AGE == age) {
    gps_status = VOID;
  } else {
    gps_status = AVAILABLE;
  }
  
  faltitude = gps.f_altitude();
  fspeed = gps.f_speed_kmph();
  nbsat = gps.satellites();
  precission = gps.hdop();

  convertGPStoNMEA(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, lat);
  convertGPStoNMEA(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, lon);
  if (flat < 0) NS = 'S';
  if (flon < 0) WE = 'W';

  dtostrf(faltitude == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0.0 : faltitude, 0, 2, alt);
  dtostrf(fspeed == TinyGPS::GPS_INVALID_F_SPEED ? 0.0 : fspeed, 0, 2, spd);
  sprintf(sat, "%d", nbsat  == TinyGPS::GPS_INVALID_SATELLITES ? 0 : nbsat);
  sprintf(pre, "%ld", precission == TinyGPS::GPS_INVALID_HDOP ? 0 : precission);
  
  memset(buf, 0, LINE_SZ);

  sprintf(buf, "$%s,%s,%02d-%02d-%02dT%02d:%02d:%02dZ,%ld,%ld,%ld,%c,%s,%c,%s,%c,%s,%c,%s,%s",  \
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
              alt, \
              gps_status, \
              pre, \
              sat);

   len = strlen(buf);
   buf[len] = '\0';

   // generate checksum
   chk = checksum(buf+1, len);

   // add checksum to end of line before sending
   if (chk < 16)
     sprintf(buf + len, "*0%X", (int)chk);
   else
     sprintf(buf + len, "*%X", (int)chk);

   return (gps_status == AVAILABLE);
}

/* setup the GPS module to 1Hz and RMC+GGA messages only */
void gps_program_settings()
{
#ifdef USE_MEDIATEK
  gpsSerial.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gpsSerial.println(PMTK_SET_NMEA_UPDATE_1HZ);
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
