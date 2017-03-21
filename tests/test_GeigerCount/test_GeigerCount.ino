#if defined(ARDUINO)
SYSTEM_MODE(MANUAL);//do not connect to cloud
#else
SYSTEM_MODE(AUTOMATIC);//connect to cloud
#endif

/*
 * This code tests the Geiger count functionality
 * and the InterruptCounter library
 */

#include "InterruptCounter.h"

void setup()
{

  Serial.begin(9600);
  
  interruptCounterSetup(D10, 5000);
  interruptCounterReset();
}

void loop()
{
  if (interruptCounterAvailable())
  {
    Serial.print("Last 5s count: ");   
    Serial.println(interruptCounterCount());
    interruptCounterReset();
  }
}

