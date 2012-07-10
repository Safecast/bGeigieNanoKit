/*
   Simple library for Arduino implementing a hardware counter
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

#include "HardwareCounter.h"
#include <limits.h>

// Constructor
HardwareCounter::HardwareCounter(int timer_pin, long delay)
{
  // register delay
  _delay = delay;
  // set pin as digital input
  pinMode(timer_pin, INPUT);
}

// call this to start the counter
void HardwareCounter::start()
{

  // hardware counter setup ( refer atmega168.pdf chapter 16-bit counter1)
  TCCRnA=0;     // reset timer/countern control register A
  TCCRnB=0;     // reset timer/countern control register A
  // set timer/counter1 hardware as counter , counts events on pin Tn ( arduino pin 5 on 168, pin 47 on Mega )
  // normal mode, wgm10 .. wgm13 = 0
  sbi (TCCRnB ,CS10);  // External clock source on Tn pin. Clock on rising edge.
  sbi (TCCRnB ,CS11);
  sbi (TCCRnB ,CS12);
  TCCRnB = TCCRnB | 7;  //  Counter Clock source = pin Tn , start counting now

  // set start time
  _start_time = millis();

  // The counter needs to be reset after
  // the counter is setup (This is important)!
  TCNTn=0;      // counter value = 0

  // set count to zero (optional)
  _count = 0;
}

// call this to read the current count and save it
unsigned int HardwareCounter::count()
{

  TCCRnB = TCCRnB & ~7;   // Gate Off  / Counter Tn stopped
  _count = TCNTn;         // Set the count in object variable
  TCCRnB = TCCRnB | 7;    // restart counting
  return _count;

}

// This indicates when the count over the determined period is over
int HardwareCounter::available()
{
  // get current time
  unsigned long now = millis();
  // do basic check for millis overflow
  if (now >= _start_time)
    return (now - _start_time >= _delay);
  else
    return (ULONG_MAX + now - _start_time >= _delay);
}


