/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2015                            Christian Thäter <ct@pipapo.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MUOS_CLOCK_H
#define MUOS_CLOCK_H

#include <stdbool.h>

#include <muos/muos.h>

//PLANNED: dont inline clock functions

//PLANNED: macros compensating drift over long timespans; using rationals thereof

//PLANNED: make it possible to configure muos w/o a clock

//clock_api:
//: .Time calculation macros
//: ----
//: MUOS_CLOCK_SECONDS(t)
//: MUOS_CLOCK_MILLISECONDS(t)
//: MUOS_CLOCK_MICROSECONDS(t)
//: ----
//:
//: +t+::
//:   time to be converted
//:
//: The clock runs on timer ticks. These macros convert time from second, ms, µs to ticks.
//: Because this needs to result in a integer number of ticks, the result might be inexact
//: and add a slight drift.
//:
//: The correct configuration of F_CPU is mandatory for these macros to work correctly.
#define MUOS_CLOCK_SECONDS(s) (((uint64_t)s)*F_CPU/MUOS_CLOCK_PRESCALER)
#define MUOS_CLOCK_MILLISECONDS(s) (MUOS_CLOCK_SECONDS(s)/1000)
#define MUOS_CLOCK_MICROSECONDS(s) (MUOS_CLOCK_MILLISECONDS(s)/1000)

#define MUOS_CLOCK_REGISTER MUOS_HW_CLOCK_REGISTER(MUOS_CLOCK_HW)
#define MUOS_CLOCK_OVERFLOW MUOS_HW_CLOCK_OVERFLOW(MUOS_CLOCK_HW)
typedef typeof(MUOS_CLOCK_REGISTER) muos_hwclock;

typedef MUOS_CLOCK_TYPE muos_clock;
typedef MUOS_CLOCK_SHORT_TYPE muos_shortclock;

//clock_api:
//: .The fullclock datatype
//: ----
//: typedef struct \{
//:  muos_clock coarse;
//:  muos_hwclock fine;
//: \} muos_fullclock;
//: ----
//:
//: +coarse+::
//:   the global counter for hardware overflows
//: +fine+::
//:   the hardware part of the clock
//:
typedef struct {
  muos_clock coarse;
  muos_hwclock fine;
} muos_fullclock;


extern volatile muos_clock muos_clock_count_;

extern muos_clock muos_now_;

//clock_api:
//: .Event time
//: ----
//: muos_clock muos_now (void)
//: ----
//:
//: This function is very cheap and gives a consistent time throughout the whole mainloop iteration.
//:
//: Returns time if each mainloop (also recursive from 'muos_wait()' and 'muos_yield()') iteration start.
static inline muos_clock
muos_now (void)
{
  return muos_now_;
}

static inline void
muos_clock_start (void)
{
  MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE(MUOS_CLOCK_HW);
  MUOS_HW_CLOCK_PRESCALE_SET(MUOS_CLOCK_HW, MUOS_CLOCK_PRESCALER);
}


//clock_api:
//: .Query Time
//: ----
//: muos_clock muos_clock_now (void)
//: ----
//:
//: Returns the current time as queried from the hardware.
static inline muos_clock
muos_clock_now (void)
{
  muos_clock counter;
  muos_hwclock hw;
  do
    {
      counter = muos_clock_count_ + MUOS_CLOCK_OVERFLOW;
      hw = MUOS_CLOCK_REGISTER;
    }
  while ((uint8_t)counter != (uint8_t)muos_clock_count_ + MUOS_CLOCK_OVERFLOW);

  return (counter<<(sizeof(MUOS_CLOCK_REGISTER) * 8)) + hw;
}

//clock_api:
//: ----
//: muos_shortclock muos_clock_shortnow (void)
//: ----
//:
//: Returns the current time as queried from the hardware, using the 'muos_shortclock' datatype.
static inline muos_shortclock
muos_clock_shortnow (void)
{
  if (sizeof(MUOS_CLOCK_REGISTER) < sizeof(muos_shortclock))
    {
      muos_shortclock counter;
      muos_hwclock hw;
      do
        {
          counter = (muos_shortclock)muos_clock_count_ + MUOS_CLOCK_OVERFLOW;
          hw = MUOS_CLOCK_REGISTER;
        }
      while ((uint8_t)counter != (uint8_t)muos_clock_count_ + MUOS_CLOCK_OVERFLOW);

      return ((counter<<((sizeof(MUOS_CLOCK_REGISTER) * 8)-1))<<1) + hw;
    }
  else
    return MUOS_CLOCK_REGISTER;
}


//clock_api:
//: ----
//: muos_fullclock muos_clock_fullnow (void)
//: ----
//:
//: Returns the current time as queried from the hardware, using the 'muos_fullclock' datatype.
static inline muos_fullclock
muos_clock_fullnow (void)
{
  muos_fullclock clock;
  do
    {
      clock.coarse = muos_clock_count_ + MUOS_CLOCK_OVERFLOW;
      clock.fine = MUOS_CLOCK_REGISTER;
    }
  while ((uint8_t)clock.coarse != (uint8_t)muos_clock_count_ + MUOS_CLOCK_OVERFLOW);

  return clock;
}

//clock_api:
//: .Time difference between two timestamps
//: ----
//: muos_clock muos_clock_elapsed (muos_clock now, muos_clock start)
//: ----
//:
//: +now+::
//:   End of the timespan to be calculated
//: +start+::
//:   Begin of the timespan to to be calculated
//:
//: returns the time difference between 'now' and 'start'. The result is
//: always positive or zero. Simple overflows on the arguments are respected.
//: Thus the full range of 'muos_clock' is available.
muos_clock
muos_clock_elapsed (muos_clock now, muos_clock start);


#ifdef MUOS_CLOCK_CALIBRATE
//clock_api:
//: .Calibrate the µC Frequency
//: ----
//: void muos_clock_calibrate (const muos_clock now, const muos_clock sync)
//: ----
//:
//: +now+::
//:   Time at which the external sync signal happened
//: +sync+::
//:   Timespan which should be elapsed since the last call of this function
//:
//: For µC's which support it, the main frequency can be calibrated by some external signal.
//: For example with some 1 second pulse from a RTC one could call
//: +muos_clock_calibrate (muos_clock_now(), MUOS_CLOCK_SECONDS(1))+ upon receiving this signal.
//:
//: There is a special case that if 'sync' is 0 then only the internal state is recorded but
//: no frequency calibration is executed. This is to be used for initialization or when the time elapsed
//: since the last call can't be determined.
//:
//: Note that frequency calibration is often too coarse to archive perfect synchronization. You should
//: expect some drift remaining. The configuration specially includes a deadband for this. Otherwise
//: when calibration tries to constantly change the frequency there would be very much jitter on the timing.
void
muos_clock_calibrate (const muos_clock now, const muos_clock sync);
#endif

#endif
