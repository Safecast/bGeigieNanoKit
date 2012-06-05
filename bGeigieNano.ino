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
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <stdlib.h>
#include <avr/wdt.h>
#include "TinyGPS.h"
#include "InterruptCounter.h"

// Geiger settings ------------------------------------------------------------
#define TIME_INTERVAL 5000
#define LINE_SZ 100
#define BUFFER_SZ 16
#define NX 12
#define AVAILABLE 'A'  // indicates geiger data are ready (available)
#define VOID      'V'  // indicates geiger data not ready (void)

// log file headers
char hdr[6] = "BNRDD";  // header for sentence
char fileHeader[] = "# NEW LOG\n# format=1.0.0nano\n";

// geiger statistics
unsigned long shift_reg[NX] = {0};
unsigned long reg_index = 0;
unsigned long total_count = 0;
int str_count = 0;
char geiger_status = VOID;

// the line buffer for serial receive and send
static char line[LINE_SZ];

// geiger id and interrupt
static const int interruptPin = 0; // 0 = pin2, 1= pin3
static const int dev_id = 1;

// OpenLog settings -----------------------------------------------------------
SoftwareSerial OpenLog(8, 7); //Connect TXO of OpenLog to pin 8, RXI to pin 7
static const int resetOpenLog = 9; //This pin resets OpenLog. Connect pin 9 to pin GRN on OpenLog.

// GpsBee settings ------------------------------------------------------------
TinyGPS gps;
static const int ledPin = 13;

// Global definitions ---------------------------------------------------------
unsigned long cpm_gen();
byte gps_gen_timestamp(TinyGPS &gps, char *buf, unsigned long counts, unsigned long cpm, unsigned long cpb);
void setupOpenLog(void);
void createFile(char *fileName);
void gotoCommandMode(void);

//#define DEBUG_LOG

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------
void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  setupOpenLog(); //Resets logger and waits for the '<' I'm alive character

  // print header to serial
#ifdef DEBUG_LOG
  Serial.print(fileHeader);
#endif
  OpenLog.print(fileHeader);

  // Create pulse counter on INT2
  interruptCounterSetup(interruptPin, TIME_INTERVAL);

  // And now Start the Pulse Counter!
  interruptCounterReset();
}

// ----------------------------------------------------------------------------
// Main loop
// ----------------------------------------------------------------------------
void loop()
{
  bool gpsReady = false;
  
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      // OpenLog.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        gpsReady = true;
    }
  }
  
  // generate CPM every TIME_INTERVAL seconds
  if (interruptCounterAvailable())
  {
      unsigned long cpm=0, cpb=0;
      byte line_len;
      
      if (gpsReady) digitalWrite(ledPin, HIGH);

      // obtain the count in the last bin
      cpb = interruptCounterCount();

      // reset the pulse counter
      interruptCounterReset();

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
      line_len = gps_gen_timestamp(gps, line, shift_reg[reg_index], cpm, cpb);

      // Printout line
#ifdef DEBUG_LOG
      Serial.println(line);
#endif
      OpenLog.println(line);
      
      if (gpsReady) digitalWrite(ledPin, LOW);
  }
}

// ----------------------------------------------------------------------------
// Utility functions
// ----------------------------------------------------------------------------

//Setups up the software serial, resets OpenLog so we know what state it's in, and waits
//for OpenLog to come online and report '<' that it is ready to receive characters to record
void setupOpenLog(void) {
  pinMode(resetOpenLog, OUTPUT);
  OpenLog.begin(9600);

  //Reset OpenLog
  digitalWrite(resetOpenLog, LOW);
  delay(100);
  digitalWrite(resetOpenLog, HIGH);

  //Wait for OpenLog to respond with '<' to indicate it is alive and recording to a file
  while(1) {
    if(OpenLog.available())
      if(OpenLog.read() == '<') break;
  }
}

//This function creates a given file and then opens it in append mode (ready to record characters to the file)
//Then returns to listening mode
void createFile(char *fileName) {

  //Old way
  OpenLog.print("new ");
  OpenLog.print(fileName);
  OpenLog.write(13); //This is \r

  //New way
  //OpenLog.print("new ");
  //OpenLog.println(filename); //regular println works with OpenLog v2.51 and above

  //Wait for OpenLog to return to waiting for a command
  while(1) {
    if(OpenLog.available())
      if(OpenLog.read() == '>') break;
  }

  OpenLog.print("append ");
  OpenLog.print(fileName);
  OpenLog.write(13); //This is \r

  //Wait for OpenLog to indicate file is open and ready for writing
  while(1) {
    if(OpenLog.available())
      if(OpenLog.read() == '<') break;
  }

  //OpenLog is now waiting for characters and will record them to the new file  
}

//This function pushes OpenLog into command mode
void gotoCommandMode(void) {
  //Send three control z to enter OpenLog command mode
  //Works with Arduino v1.0
  OpenLog.write(26);
  OpenLog.write(26);
  OpenLog.write(26);

  //Wait for OpenLog to respond with '>' to indicate we are in command mode
  while(1) {
    if(OpenLog.available())
      if(OpenLog.read() == '>') break;
  }
}

unsigned long cpm_gen()
{
   unsigned int i;
   unsigned long c_p_m = 0;
   
   // sum up
   for (i=0 ; i < NX ; i++)
     c_p_m += shift_reg[i];
   
   return c_p_m;
}

byte gps_gen_timestamp(TinyGPS &gps, char *buf, unsigned long counts, unsigned long cpm, unsigned long cpb)
{
  int year = 2012;
  byte month = 0, day = 0, hour = 0, minute = 0, second = 0, hundredths = 0;
  float flat = 0, flon = 0, faltitude = 0;
  unsigned short nbsat = 0;
  unsigned long precission = 0;
  unsigned long age;
  byte len;

  char lat[BUFFER_SZ];
  char lon[BUFFER_SZ];
  char alt[BUFFER_SZ];
  char sat[BUFFER_SZ];
  char pre[BUFFER_SZ];
  memset(lat, 0, BUFFER_SZ);
  memset(lon, 0, BUFFER_SZ);
  memset(alt, 0, BUFFER_SZ);
  memset(sat, 0, BUFFER_SZ);
  memset(pre, 0, BUFFER_SZ);
  
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (TinyGPS::GPS_INVALID_AGE == age) {
    year = 2012, month = 0, day = 0, hour = 0, minute = 0, second = 0, hundredths = 0;
  }
  gps.f_get_position(&flat, &flon, &age);
  faltitude = gps.f_altitude();
  nbsat = gps.satellites();
  precission = gps.hdop();

  dtostrf(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 0, 6, lat);
  dtostrf(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 0, 6, lon);
  dtostrf(faltitude == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0.0 : faltitude, 0, 2, alt);
  sprintf(sat, "%d", nbsat  == TinyGPS::GPS_INVALID_SATELLITES ? 0 : nbsat);
  sprintf(pre, "%ld", precission == TinyGPS::GPS_INVALID_HDOP ? 0 : precission);
  
  memset(buf, 0, LINE_SZ);

  sprintf(buf, "$%s,%d,%02d-%02d-%02dT%02d:%02d:%02dZ,%ld,%ld,%ld,%c,%s,%s,%s,%s,%s",  \
              hdr, \
              dev_id, \
              year, month, day,  \
              hour, minute, second, \
              cpm, \
              cpb, \
              total_count, \
              geiger_status, \
              lat,\
              lon, \
              alt, \
              sat, \
              pre);

   len = strlen(buf);
   buf[len] = '\0';

   return len;
}

