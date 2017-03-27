/*
   The bGeigie-nano
   A device for car-borne radiation measurement (aka Radiation War-driving).

   Copyright (c) 2013, Lionel Bergeret and Rob Oudendijk
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

SYSTEM_MODE(MANUAL);//do not connect to cloud

#include "NanoSetup.h"
#include "NanoConfig.h"
#include "NanoDebug.h"

// For some reason (ask Musti) ARDUINO
// must be undef before loading SdFat library
// (2017 01 22 Robin: test works without these two lines. commenting out)
// #undef ARDUINO
// #define PLATFORM_ID 3
#include "SdFat.h"


// the line buffer for serial receive and send
#define LINE_SZ 100
static char line[LINE_SZ];

// SD FAT object
SdFat sd(1);

// Nano Settings --------------------------------------------------------------
static ConfigType config;
static DoseType dose;
NanoSetup nanoSetup(sd, config, dose, line, LINE_SZ);

// ****************************************************************************
// Setup
// ****************************************************************************
void setup()
{
  Serial.begin(9600);

  // Wait for USB Serial
  while (!Serial) {
    SysCall::yield();
  }

  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // Change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(CS_LOG, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  }

  // Load EEPROM settings
  nanoSetup.initialize();
  nanoSetup.loadFromFile("SAFECAST.TXT");

  // Read the dose from EEPROM
  EEPROM_readAnything(BMRDD_EEPROM_DOSE, dose);

}



// ****************************************************************************
// Main loop
// ****************************************************************************
void loop()
{
  Serial.println("***********");
  Serial.println("Dump Setup:");
  Serial.print("marker: ");
  Serial.println(config.marker);
  Serial.print("type: ");
  Serial.println(config.type);
  Serial.print("mode: ");
  Serial.println(config.mode);
  Serial.print("user_name: ");
  Serial.println(config.user_name);
  Serial.print("device_id: ");
  Serial.println(config.device_id);
  Serial.print("cpm_window: ");
  Serial.println(config.cpm_window);
  Serial.print("cpm_factor: ");
  Serial.println(config.cpm_factor);
  Serial.print("bqm_factor: ");
  Serial.println(config.bqm_factor);
  Serial.print("alarm_level: ");
  Serial.println(config.alarm_level);
  Serial.print("timezone: ");
  Serial.println(config.timezone);
  Serial.print("country_code: ");
  Serial.println(config.country_code);
  Serial.print("sensor_type: ");
  Serial.println(config.sensor_type);
  Serial.print("sensor_shield: ");
  Serial.println(config.sensor_shield);
  Serial.print("sensor_height: ");
  Serial.println(config.sensor_height);
  Serial.print("sensor_mode: ");
  Serial.println(config.sensor_mode);
  delay(1000);
}

// ****************************************************************************
// Utility functions
// ****************************************************************************
