/*
   Simple library for Arduino implementing a counter using the interrupt pin
   for a Geigier counter for example

   Copyright (c) 2011, Robin Scheibler aka FakuFaku
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

#include "InterruptCounter.h"
#include <limits.h>

// Declare variables here
int _interrupt_pin;
unsigned long _start_time;
unsigned long _delay;
COUNTER_TYPE _count;

// private methods here
void interrupt_routine();

// Constructor
void interruptCounterSetup(int interrupt_pin, unsigned long delay)
{
  _interrupt_pin = interrupt_pin;
  _delay = delay;
  _count = 0;
  attachInterrupt(_interrupt_pin, interrupt_routine, RISING);
}

// call this to start the counter
void interruptCounterReset()
{
  // set start time
  _start_time = millis();
  // set count to zero (optional)
  _count = 0;
}

// This indicates when the count over the determined period is over
int interruptCounterAvailable()
{
  // get current time
  unsigned long now = millis();
  // do basic check for millis overflow
  if (now >= _start_time)
    return (now - _start_time >= _delay);
  else
    return (ULONG_MAX + now - _start_time >= _delay);
}

// return current number of counts
COUNTER_TYPE interruptCounterCount()
{
  _start_time = millis();   // reset time for next sample 
  return _count;
}

// The interrupt routine, simply increment count on every event
void interrupt_routine()
{
  _count++;
}

