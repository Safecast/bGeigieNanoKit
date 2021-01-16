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

#include "NanoSetup.h"
#include "NanoConfig.h"
#include "NanoDebug.h"

NanoSetup::NanoSetup(SoftwareSerial &openlog, ConfigType &config, DoseType &dose, 
    char * buffer, size_t buffer_size):
    mOpenlog(openlog), mConfig(config), mDose(dose), mBuffer(buffer), mBufferSize(buffer_size) {
}

void NanoSetup::initialize() {
  // Configuration 
  DEBUG_PRINTLN("Loading EEPROM configuration");
  memset(&mConfig, 0, sizeof(mConfig));
  EEPROM_readAnything(BMRDD_EEPROM_SETUP, mConfig);

  if (mConfig.marker != BMRDD_EEPROM_MARKER) {
    DEBUG_PRINTLN("  - First time setup");
    // First run, time to set default values
    memset(&mConfig, 0, sizeof(mConfig));
    mConfig.marker = BMRDD_EEPROM_MARKER;
    mConfig.device_id = NANO_DEVICE_ID;
    mConfig.timezone = 9;
    sprintf_P(mConfig.country_code, PSTR("JPN"));
    mConfig.cpm_window = 60;
    mConfig.cpm_factor = NANO_CPM_FACTOR;
    mConfig.bqm_factor = NANO_BQM2_FACTOR;
    mConfig.sensor_height = 100;
    EEPROM_writeAnything(BMRDD_EEPROM_SETUP, mConfig);

#if ENABLE_EEPROM_DOSE
    memset(&mDose, 0, sizeof(mDose));
    EEPROM_writeAnything(BMRDD_EEPROM_DOSE, mDose);
#endif
  }
}

void NanoSetup::loadFromFile(char * setupFile) {
  bool config_changed = false;
  char *config_buffer, *key, *value;
  byte pos, line_lenght;
  byte i, buffer_lenght;

  mOpenlog.listen();

  // Send read command to OpenLog
  DEBUG_PRINT(" - read ");
  DEBUG_PRINTLN(setupFile);

  sprintf_P(mBuffer, PSTR("read %s 0 %d"), setupFile, mBufferSize);
  mOpenlog.print(mBuffer);
  mOpenlog.write(13); //This is \r

  while(1) {
    if(mOpenlog.available())
      if(mOpenlog.read() == '\r') break;
  }

  // Read config file in memory
  pos = 0;
  memset(mBuffer, 0, mBufferSize);
  for(int timeOut = 0 ; timeOut < 1000 ; timeOut++) {
    if(mOpenlog.available()) {
      mBuffer[pos++] = mOpenlog.read();
      timeOut = 0;
    }
    delay(1);

    if(pos == mBufferSize) {
      break;
    }
  }

  line_lenght = pos;
  pos = 0;
  
  // Process each config file lines
  while(pos < line_lenght){

    // Get a complete line
    i = 0;
    config_buffer = mBuffer + pos;
    while(mBuffer[pos++] != '\n') {
      i++;
      if(pos == mBufferSize) {
        break;
      }
    }
    buffer_lenght = i++;
    config_buffer[--i] = '\0';

    // Skip empty lines
    if(config_buffer[0] == '\0' || config_buffer[0] == '#' || buffer_lenght < 3) continue;

    // Search for keys
    i = 0;
    while(config_buffer[i] == ' ' || config_buffer[i] == '\t') {
      if(++i == buffer_lenght) break; // skip white spaces
    }
    if(i == buffer_lenght) continue;
    key = &config_buffer[i];

    // Search for '=' ignoring white spaces
    while(config_buffer[i] != '=') {
      if(config_buffer[i] == ' ' || config_buffer[i] == '\t') config_buffer[i] = '\0';
      if(++i == buffer_lenght) {
        break;
      }
    }
    if(i == buffer_lenght) continue;
    config_buffer[i++] = '\0';

    // Search for value ignoring white spaces
    while(config_buffer[i] == ' ' || config_buffer[i] == '\t') {
      if(++i == buffer_lenght) {
        break;
      }
    }
    if(i == buffer_lenght) continue;
    value = &config_buffer[i];
    
    //
    // Process matching keys
    //
    if(strcmp(key, "cpmf") == 0) {
      // Update cpm factor
      float factor = atoi(value);
      if (mConfig.cpm_factor != factor) {
        mConfig.cpm_factor = factor;
        config_changed = true;
        DEBUG_PRINTLN("   - Update cpm factor");
      }
    }
    else if(strcmp(key, "bqmf") == 0) {
      // Update bq/m2 factor
      float factor = atoi(value);
      if (mConfig.bqm_factor != factor) {
        mConfig.bqm_factor = factor;
        config_changed = true;
        DEBUG_PRINTLN("   - Update bq/m2 factor");
      }
    }
    else if(strcmp(key, "nm") == 0) {
      // Update name in EEPROM
      if (strcmp(mConfig.user_name, value) != 0 ) {
        strcpy(mConfig.user_name, value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update name in EEPROM");
      }
    }
    else if(strcmp(key, "did") == 0) {
      // Update device id in EEPROM
      if (mConfig.device_id != (unsigned int)atoi(value)) {
        mConfig.device_id = atoi(value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update devide id in EEPROM");
      }
    }
    else if(strcmp(key, "gt") == 0) {
      // Update geiger type in EEPROM
      if (mConfig.type != atoi(value)) {
        mConfig.type = atoi(value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update geiger type in EEPROM");
      }
    }
    else if(strcmp(key, "gm") == 0) {
      // Update geiger mode in EEPROM
      if (mConfig.mode != atoi(value)) {
        mConfig.mode = atoi(value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update geiger mode in EEPROM");
      }
    }
    else if(strcmp(key, "al") == 0) {
      // Update alarm threshold in EEPROM
      if (mConfig.alarm_level != (unsigned int)atoi(value)) {
        mConfig.alarm_level = atoi(value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update alarm threshold in EEPROM");
      }
    }
    else if(strcmp(key, "cn") == 0) {
      // Update country code in EEPROM
      value[3] = '\0'; // force 3 characters code
      if (strcmp(mConfig.country_code, value) != 0 ) {
        strcpy(mConfig.country_code, value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update country code in EEPROM");
      }
    }
    else if(strcmp(key, "tz") == 0) {
      // Update timezone in EEPROM
      if (mConfig.timezone != atoi(value)) {
        mConfig.timezone = atoi(value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update timezone in EEPROM");
      }
    }
    else if(strcmp(key, "st") == 0) {
      // Update sensor type in EEPROM
      if (mConfig.sensor_type != atoi(value)) {
        mConfig.sensor_type = atoi(value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update sensor type in EEPROM");
      }
    }
    else if(strcmp(key, "ss") == 0) {
      // Update sensor shield in EEPROM
      if (mConfig.sensor_shield != atoi(value)) {
        mConfig.sensor_shield = atoi(value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update sensor shield in EEPROM");
      }
    }
    else if(strcmp(key, "sh") == 0) {
      // Update sensor height in EEPROM
      if (mConfig.sensor_height != (unsigned int)atoi(value)) {
        mConfig.sensor_height = atoi(value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update sensor height in EEPROM");
      }
    }
    else if(strcmp(key, "sm") == 0) {
      // Update sensor mode in EEPROM
      if (mConfig.sensor_mode != atoi(value)) {
        mConfig.sensor_mode = atoi(value);
        config_changed = true;
        DEBUG_PRINTLN("   - Update sensor mode in EEPROM");
      }
    }
#if ENABLE_EEPROM_DOSE
    else if(strcmp(key, "dose") == 0) {
      // Reset total dose in EEPROM
      memset(&mDose, 0, sizeof(mDose));
      EEPROM_writeAnything(BMRDD_EEPROM_DOSE, mDose);
      DEBUG_PRINTLN("   - Reset total dose in EEPROM");
    }
#endif
  }
  DEBUG_PRINTLN("   - Done.");

  if (config_changed) {
    // Configuration is changed
    DEBUG_PRINTLN("Update configuration in EEPROM");
    EEPROM_writeAnything(BMRDD_EEPROM_SETUP, mConfig);
  }
}
