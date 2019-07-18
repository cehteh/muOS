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

#ifndef MUOS_LIB_CLOCK_H
#define MUOS_LIB_CLOCK_H

#include <muos/lib/barray.h>

//PLANNED: macros compensating drift over long timespans; using rationals thereof

//clock_api:
//: .Timespan calculation macros
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



//: .Clock Type Conversion
//: ----
//: muos_clock16 muos_clock_clock16 (const muos_clock* src)
//: muos_clock32 muos_clock_clock32 (const muos_clock* src)
//: ----
//:
//: +src+::
//:   time to be converted
//:
//: These functions convert a full 'muos_clock' into a truncated 16 or 32 bit clock type.
//:
static inline muos_clock16
muos_clock_clock16 (const muos_clock* src)
{
  return muos_barray_uint16 (src->barray, 0);
}

static inline muos_clock32
muos_clock_clock32 (const muos_clock* src)
{
  return muos_barray_uint32 (src->barray, 0);
}






//clock_api:
//: .Timespan calculations
//: ----
//: void muos_clock_add (muos_clock *dst, const muos_clock* src)
//: void muos_clock_sub (muos_clock *dst, const muos_clock* src)
//: void muos_clock_add32 (muos_clock *dst, muos_clock32 src)
//: void muos_clock_add16 (muos_clock *dst, muos_clock16 src)
//: void muos_clock_add8 (muos_clock *dst, uint8_t src)
//: void muos_clock_sub32 (muos_clock *dst, muos_clock32 src)
//: void muos_clock_sub16 (muos_clock *dst, muos_clock16 src)
//: void muos_clock_sub8 (muos_clock *dst, uint8_t src)
//: ----
//:
//: +dst+::
//:   The time to be modified
//: +src+::
//:   Timespan to be added/substracted to 'dst'
//:
//: Adding or substracting a full, 32, 16 or 8 bit value to a muos_clock
//:
static inline void
muos_clock_copy (muos_clock *dst, const muos_clock* src)
{
  muos_barray_copy (dst->barray, src->barray);
}

static inline bool
muos_clock_is_lt (const muos_clock *a, const muos_clock* b)
{
  return muos_barray_is_lt (a->barray, b->barray);
}

static inline void
muos_clock_add (muos_clock *dst, const muos_clock* src)
{
  muos_barray_add (dst->barray, src->barray);
}

static inline void
muos_clock_sub (muos_clock *dst, const muos_clock* src)
{
  muos_barray_sub (dst->barray, src->barray);
}


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

static inline void
muos_clock_add8 (muos_clock *dst, uint8_t src)
{
  muos_barray_add_uint8 (dst->barray, src, 0);
}


static inline void
muos_clock_sub32 (muos_clock *dst, muos_clock32 src)
{
  muos_barray_sub_uint32 (dst->barray, src);
}

static inline void
muos_clock_sub16 (muos_clock *dst, muos_clock16 src)
{
  muos_barray_sub_uint16 (dst->barray, src);
}

static inline void
muos_clock_sub8 (muos_clock *dst, uint8_t src)
{
  muos_barray_sub_uint8 (dst->barray, src, 0);
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


#endif
