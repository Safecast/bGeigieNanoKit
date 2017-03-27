/* FatLib Library
 * Copyright (C) 2013 by William Greiman
 *
 * This file is part of the FatLib Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the FatLib Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SysCall_h
#define SysCall_h

#undef ARDUINO
#define PLATFORM_ID 3

/**
 * \file
 * \brief SysCall class
 */
#if defined(ARDUINO)
#include <Arduino.h>
#include <SPI.h>
#elif defined(PLATFORM_ID)  // Only defined if a Particle device
#include "application.h"
#else  // defined(ARDUINO)
#error "Unknown system"
#endif  // defined(ARDUINO)
#ifndef F
/** Define macro for strings stored in flash. */
#define F(str) (str)
#endif  // F
/**
 * \class SysCall
 * \brief SysCall - Class to wrap system calls.
 */
class SysCall {
 public:
  /** Halt execution of this thread. */
  static void halt() {
    while (1) {
      yield();
    }
  }
  /** Yield to other threads. */
  static void yield();
};

#if defined(ARDUINO)
inline void SysCall::yield() {
  // Use the external Arduino yield() function.
  ::yield();
}
#elif defined(PLATFORM_ID)  // Only defined if a Particle device
inline void SysCall::yield() {
  Particle.process();
}
#else  // defined(ARDUINO)
inline void SysCall::yield() {}
#endif  // defined(ARDUINO)

#endif  // SysCall_h
