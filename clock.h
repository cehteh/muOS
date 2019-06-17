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

#include <stdbool.h>
#include <muos/lib/barray.h>

#ifdef MUOS_HW_HEADER
#include MUOS_HW_HEADER
#endif

//PLANNED: macros compensating drift over long timespans; using rationals thereof

//PLANNED: make it possible to configure muos w/o a clock

//PLANNED: only stop clock interrupts when querying time



//: .Clock Types
//: ----
//: typedef muos_clock
//: typedef muos_clock32
//: typedef muos_clock16
//: ----
//:
//: +muos_clock+::
//:   The configurable type using muos_barray for the main clock implementation.
//: +muos_clock32+::
//:   Clock type truncated to uint32_t for limited time ranges.
//: +muos_clock16+::
//:   Clock type truncated to uint16_t for limited time ranges.
//:
//: Muos provides 3 clock types. The main type is configureable and should be wide enough that
//: it will not overflow for the expected worst case application run time. The actual details will
//: vary depending on application and prescaler settings. The 2 truncated types can save  memory
//: and are useful when only shorter time spans are required or overflows are handled
//: properly in the software.
//:
typedef struct {
  MUOS_BARRAY (barray, MUOS_CLOCK_SIZE*8);  //PLANNED: remove hwclock bytes
} muos_clock;
typedef uint32_t muos_clock32;
typedef uint16_t muos_clock16;


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
//: and add some drift.
//:
//: The correct configuration of F_CPU is mandatory for these macros to work correctly.
//:
#define MUOS_CLOCK_SECONDS(s) (((uint64_t)s)*F_CPU/MUOS_CLOCK_PRESCALER)
#define MUOS_CLOCK_MILLISECONDS(s) (MUOS_CLOCK_SECONDS(s)/1000)
#define MUOS_CLOCK_MICROSECONDS(s) (MUOS_CLOCK_MILLISECONDS(s)/1000)
#define MUOS_CLOCK_NANOSECONDS(s) (MUOS_CLOCK_MICROSECONDS(s)/1000)

#define MUOS_CLOCK_REGISTER MUOS_HW_CLOCK_REGISTER(MUOS_CLOCK_HW)
#define MUOS_CLOCK_OVERFLOW MUOS_HW_CLOCK_OVERFLOW(MUOS_CLOCK_HW)
typedef typeof(MUOS_CLOCK_REGISTER) muos_hwclock;



void
muos_clock_90init (void);



//clock_api:
//: .Query Time
//: ----
//: void muos_clock_now (muos_clock* now)
//: void muos_clock_now_isr (muos_clock* now)
//: muos_clock32 muos_clock32_now (void)
//: muos_clock32 muos_clock32_now_isr (void)
//: ----
//:
//: Queries the current time from the hardware.
//: The clock32 functions return truncated time, overflows need to be
//: handled by the software.
//:
void
muos_clock_now (muos_clock* now);

void
muos_clock_now_isr (muos_clock* now);


muos_clock32
muos_clock32_now (void);

muos_clock32
muos_clock32_now_isr (void);





//clock_api:
//: .Timespan caclulations
//: ----
//: muos_clock32 muos_clock_since (const muos_clock* start)
//: muos_clock32 muos_clock32_elapsed (muos_clock32 end, muos_clock32 start)
//: muos_clock16 muos_clock16_elapsed (muos_clock16 end, muos_clock16 start)
//: ----
//:
//: +start+::
//:   Begin of the timespan to to be calculated
//: +end+::
//:   End of the timespan to be calculated
//:
//: 'muos_clock_since()' returns the time between 'start' to 'muos_clock_now()'
//:
//: The two 'muos_clock*_elapsed()' return the time difference between 'end' and 'start'.
//: The result is always positive or zero. Simple overflows on the arguments are respected.
//:
//: The *32 and *16 bit versions save memory, but it must be guaranteed that
//: the time spans are short enough to fall into the respective range.
//:
static inline muos_clock32
muos_clock_since (const muos_clock* start)
{
  muos_clock now;
  muos_clock_now (&now);

  muos_barray_sub (now.barray, start->barray);

  return muos_barray_uint32 (now.barray, 0);
}


static inline muos_clock32
muos_clock32_elapsed (muos_clock32 now, muos_clock32 start)
{
  if (now > start)
    return now - start;
  else
    return start - now;
}

static inline muos_clock16
muos_clock16_elapsed (muos_clock16 now, muos_clock16 start)
{
  if (now > start)
    return now - start;
  else
    return start - now;
}


//: ----
//: void muos_clock_add32 (muos_clock *dst, muos_clock32 src)
//: void muos_clock_add16 (muos_clock *dst, muos_clock16 src)
//: ----
//:
//: +dst+::
//:   The time to be modified
//: +src+::
//:   Timespan to be added to 'dst'
//:
//: Adding a 32 or 16 bit time value to a muos_clock
//:
static inline void
muos_clock_add32 (muos_clock *dst, muos_clock32 src)
{
  muos_barray_add_uint32 (dst->barray, src);
}

static inline void
muos_clock_add16 (muos_clock *dst, muos_clock16 src)
{
  muos_barray_add_uint16 (dst->barray, src);
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
