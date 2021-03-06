/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2019                            Christian Thäter <ct@pipapo.org>
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

//#include <stdbool.h>
#include <muos/lib/clock.h>

#ifdef MUOS_HW_HEADER
#include MUOS_HW_HEADER
#endif


//PLANNED: make it possible to configure muos w/o a clock

//PLANNED: only stop clock interrupts when querying time


#define MUOS_CLOCK_REGISTER MUOS_HW_CLOCK_REGISTER(MUOS_CLOCK_HW)
#define MUOS_CLOCK_OVERFLOW MUOS_HW_CLOCK_OVERFLOW(MUOS_CLOCK_HW)
typedef MUOS_HW_CLOCK_TYPE(MUOS_CLOCK_HW) muos_hwclock;


void
muos_clock_90init (void);



//clock_api:
//: .Query Time
//: ----
//: void muos_clock_now (muos_clock* now)
//: void muos_clock_now_isr (muos_clock* now)
//: muos_clock16 muos_clock_now16_isr (void)
//: ----
//:
//: Queries the current time from the hardware and
//: stores it in 'now' or returns a 16bit truncated
//: result.
//:
void
muos_clock_now (muos_clock* now);

void
muos_clock_now_isr (muos_clock* now);


muos_clock16
muos_clock_now16_isr (void);



//: ----
//: muos_clock32 muos_clock_since (const muos_clock* start)
//: muos_clock32 muos_clock_since_isr (const muos_clock* start)
//: bool muos_clock_is_expired (const muos_clock* when)
//: bool muos_clock_is_expired_isr (const muos_clock* when)
//: ----
//:
//: +start+::
//:   Begin of the timespan to to be calculated
//: +when+::
//:   Timestamp for the expire time
//:
//: 'muos_clock_since*()' returns the time between 'start' to 'muos_clock_now()'.
//:
//: 'muos_clock_is_expired*()' check whenever 'muos_clock_now()' is past 'when' or not.
//:
//:
static inline muos_clock32
muos_clock_since (const muos_clock* start)
{
  muos_clock now;
  muos_clock_now (&now);

  muos_clock_sub (&now, start);

  return muos_clock_clock32 (&now);
}


static inline muos_clock32
muos_clock_since_isr (const muos_clock* start)
{
  muos_clock now;
  muos_clock_now_isr (&now);

  muos_clock_sub (&now, start);

  return muos_clock_clock32 (&now);
}



static inline bool
muos_clock_is_expired (const muos_clock* when)
{
  muos_clock now;
  muos_clock_now (&now);

  return muos_barray_is_lte (when->barray, now.barray);
}

static inline bool
muos_clock_is_expired_isr (const muos_clock* when)
{
  muos_clock now;
  muos_clock_now_isr (&now);

  return muos_barray_is_lte (when->barray, now.barray);
}



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
muos_clock_calibrate (const muos_clock* now, const muos_clock32 sync);
#endif

#endif
