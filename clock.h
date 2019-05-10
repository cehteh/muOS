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

#ifdef MUOS_HW_HEADER
#include MUOS_HW_HEADER
#endif

//PLANNED: macros compensating drift over long timespans; using rationals thereof

//PLANNED: make it possible to configure muos w/o a clock

//PLANNED: only stop clock interrupts when querying time



//: .Clock Types
//: ----
//: typedef MUOS_CLOCK_TYPE muos_clock
//: typedef uint32_t muos_clock32
//: typedef uint16_t muos_clock16
//: ----
//:
//: +muos_clock+::
//:   The configureable uint32_t or uint64_t type used for the main clock implementation.
//: +muos_clock32+::
//:   Clock type truncated to uint32_t for limited time ranges.
//: +muos_clock16+::
//:   Clock type truncated to uint16_t for limited time ranges.
//:
//: Muos provides 3 clock types. The main type is configureable and should be wide enough that
//: it will not overflow for the expected worst case application runtime. The actual details will
//: vary depending on application and prescaler setttings. The 2 truncated types can save a lot
//: memory and are useful when only shorter timespans are required and overflows are handled
//: properly in the software.
//:
typedef MUOS_CLOCK_TYPE muos_clock;
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
//: and add a slight drift.
//:
//: The correct configuration of F_CPU is mandatory for these macros to work correctly.
#define MUOS_CLOCK_SECONDS(s) (((uint64_t)s)*F_CPU/MUOS_CLOCK_PRESCALER)
#define MUOS_CLOCK_MILLISECONDS(s) (MUOS_CLOCK_SECONDS(s)/1000)
#define MUOS_CLOCK_MICROSECONDS(s) (MUOS_CLOCK_MILLISECONDS(s)/1000)
#define MUOS_CLOCK_NANOSECONDS(s) (MUOS_CLOCK_MICROSECONDS(s)/1000)

#define MUOS_CLOCK_REGISTER MUOS_HW_CLOCK_REGISTER(MUOS_CLOCK_HW)
#define MUOS_CLOCK_OVERFLOW MUOS_HW_CLOCK_OVERFLOW(MUOS_CLOCK_HW)
typedef typeof(MUOS_CLOCK_REGISTER) muos_hwclock;

extern volatile muos_clock muos_clock_coarse;


void
muos_clock_90init (void);



//clock_api:
//: .Query Time
//: ----
//: muos_clock muos_clock_now (void)
//: muos_clock muos_clock_now_isr (void)
//: muos_clock32 muos_clock32_now (void)
//: muos_clock32 muos_clock32_now_isr (void)
//: ----
//:
//: Returns the current time as queried from the hardware.
//: The clock32 functions return truncated time, overflows need to be
//: handled by the software.
//:
muos_clock
muos_clock_now (void);

muos_clock
muos_clock_now_isr (void);


muos_clock32
muos_clock32_now (void);

muos_clock32
muos_clock_now_isr (void);





//clock_api:
//: .Time difference between two timestamps
//: ----
//: muos_clock muos_clock_elapsed (muos_clock end, muos_clock start)
//: muos_clock32 muos_clock32_elapsed (muos_clock end, muos_clock start)
//: ----
//:
//: +now+::
//:   End of the timespan to be calculated
//: +start+::
//:   Begin of the timespan to to be calculated
//:
//: returns the time difference between 'end' and 'start'. The result is
//: always positive or zero. Simple overflows on the arguments are respected.
//: Thus the full range of 'muos_clock' is available.
//:
//: The *32 and *16 bit versions save memory, but it must be guaranteed that
//: the time spans are short enough to fall into the respective range.
//:
muos_clock
muos_clock_elapsed (muos_clock end, muos_clock start);

muos_clock32
muos_clock32_elapsed (muos_clock32 end, muos_clock32 start);

muos_clock16
muos_clock16_elapsed (muos_clock16 end, muos_clock16 start);


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
muos_clock_calibrate (const muos_clock now, const muos_clock32 sync);
#endif

#endif
