/*
   Simple library to handle nano configuration from file and EEPROM

   Copyright (c) 2012, Lionel Bergeret
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

#ifndef _NANO_SETUP_H
#define _NANO_SETUP_H

// Link to arduino library
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <EEPROM.h>
#include <SoftwareSerial.h>

#define BMRDD_EEPROM_DOSE 200
#define BMRDD_EEPROM_SETUP 500
#define BMRDD_EEPROM_MARKER 0x5afeF00d

// geiger dose
// write every hours (eeprom ~ 100000 cycles -> ~ 11 years)
#define BMRDD_EEPROM_DOSE_WRITETIME 3600

typedef enum {
 GEIGIE_TYPE_B = 0,
 GEIGIE_TYPE_X
} GeigieType;

typedef enum {
 GEIGIE_MODE_USVH = 0,
 GEIGIE_MODE_BQM2
} GeigieMode;

typedef enum {
 SENSOR_TYPE_LND7317 = 0,
 SENSOR_TYPE_LND712,
} SensorType;

typedef enum {
 SENSOR_SHIELD_NONE = 0,
 SENSOR_SHIELD_ALPHA,
 SENSOR_SHIELD_ALPHABETA,
} SensorShield;

typedef enum {
 SENSOR_MODE_AIR = 0,
 SENSOR_MODE_SURFACE = 1,
 SENSOR_MODE_PLANE = 2
} SensorMode;

typedef struct {
  unsigned long total_count;
  unsigned long total_time;
} DoseType;

typedef struct {
  unsigned long marker; // set at first run
  byte type; // 0 for bGeigie, 1 for xGeigie
  byte mode; // 0 for uSv/h, 1 for Bq/m2
  char user_name[16];
  unsigned int device_id;
  byte cpm_window;
  float cpm_factor;
  float bqm_factor;
  unsigned int alarm_level; // in CPM
  byte timezone; // in hours
  char country_code[4];
  byte sensor_type;
  byte sensor_shield;
  unsigned int sensor_height; // in cm
  byte sensor_mode;
} ConfigType;

// Write a template value into EEPROM address [ee]
template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

// Read a template value from EEPROM address [ee]
template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}

class NanoSetup {

public:
  NanoSetup(SoftwareSerial &openlog, 
        ConfigType &config, 
        DoseType &dose,
        char * buffer, size_t buffer_size);
  void initialize();
  void loadFromFile(char * setupFile);

private:
  SoftwareSerial &mOpenlog;
  ConfigType &mConfig;
  DoseType &mDose;

  char * mBuffer;
  size_t mBufferSize;
};

#endif
