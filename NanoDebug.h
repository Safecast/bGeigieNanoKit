#ifndef _NANO_DEBUG_H_
#define _NANO_DEBUG_H_

#include "NanoConfig.h"

// Debug definitions ----------------------------------------------------------
#ifdef ENABLE_DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

#endif
