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

#if MUOS_CLPQ_LENGTH > 0

#include <muos/clpq.h>
#include <muos/clock.h>
#include <muos/debug.h>
#include <muos/error.h>

#include <string.h>

//PLANNED: wrapper for recurring jobs
//PLANNED: implement minimal uint8_t array math lib (inc/compare/xor etc)
//PLANNED: wrap all clock math in functions for above conversion

#include <muos/io.h>

#ifndef MUOS_CLPQ_BARRIERS
#define MUOS_CLPQ_BARRIERS (MUOS_CLPQ_LENGTH<16?MUOS_CLPQ_LENGTH:16)
#endif

#if MUOS_CLPQ_BARRIERS > 1
// use exponential barriers (1,2,4,8,..) with splitting and merging
#define MUOS_CLPQ_EXPONENTIAL
#error Exponential Barriers are not implemented yet, configure MUOS_CLPQ_BARRIERS=1
#endif


// How many barriers can be held at most
#define MUOS_CLPQ_BARRIER_MAX ((1ULL<<(MUOS_CLPQ_BARRIERS-1))*MUOS_CLPQ_LENGTH)

#if MUOS_CLPQ_BARRIER_MAX <= UINT8_MAX
typedef uint8_t muos_clpq_segment;
#elif MUOS_CLPQ_BARRIER_MAX <= UINT16_MAX
typedef uint16_t muos_clpq_segment;
#elif MUOS_CLPQ_BARRIER_MAX <= UINT32_MAX
typedef uint32_t muos_clpq_segment;
#else
// should never happen
#error woaah, too many barriers
#endif



//clpq_impl:
//: Time Segments
//: -------------
//:
//: The time is sliced into segments of 65536 ticks. To reduce memory only the span since the begin of the
//: current segment is stored in entries. Later events need to Time barriers to separate the segment borders.
//:
//: Segments are alternating with the state stored in muos_status.clpq_parity.
//:
//:
//: Priority Queue
//: --------------
//:
//: Uses a 'sorted stack' approach because times are compressed by storing base relative intervals only.
//: A heap datastructure can not handle that. The general assumption is that pushing events in farer future
//: is allowed to be more expensive than close times. Also in most cases there wont be very much events on a
//: CLPQ at any time.
//:
//: Each entry in the clpq is defined by:
//:  'when'::
//:     the relative time to clpq_base() (that is 0..65535) any longer time spans need time barriers, see below
//:  'what'::
//:     the function to call or some special sentinel values implementing the barriers.
//:
//:
//:
//: Wakeup Mark
//: -----------
//:
//: Uses 'NULL' as 'what'. Nothing gets scheduled but a wakeup gets scheduled. Used to implement silent
//: timeouts (nothing else wakes the MPU like in muos_wait())
//:
//: Time Barriers
//: -------------
//:
//: Time barriers are inserted when a time exceeds into succeeding segments, the 'when' of a Time barrier is always 0
//: The MUOS_CLPQ_BARRIERS
//:
//: transition from barrier to non barrier toggles the wait flag.
//:
//:

muos_clpq_type muos_clpq;

static muos_clock16 clpq_delay;
static muos_clpq_function clpq_what;


/*

  Tools

*/

static inline bool
clpq_segment_parity (const muos_clock* when)
{
  return muos_barray_getbit (when->barray, 16);
}


static inline uint8_t
clpq_barrier (muos_clpq_function what)
{
  if (what && (uintptr_t)what <= MUOS_CLPQ_BARRIERS)
    return (uintptr_t)what;

  return 0;
}


static inline muos_clpq_segment
clpq_segment (const muos_clock* when)
{
#if MUOS_CLPQ_BARRIER_MAX <= UINT8_MAX
  return muos_barray_uint8 (when->barray, 2);
#elif MUOS_CLPQ_BARRIER_MAX <= UINT16_MAX
  return muos_barray_uint16 (when->barray, 2);
#elif MUOS_CLPQ_BARRIER_MAX <= UINT32_MAX
  return muos_barray_uint32 (when->barray, 2);
#endif
}


muos_clock16
muos_clpq_delayed (void)
{
  return clpq_delay;
}




/*

  Pushing jobs

*/

muos_error
muos_clpq_after (muos_clock32 when, muos_clpq_function what)
{
  muos_clock then;

  muos_clock_now (&then);
  muos_clock_add32 (&then, when);

  return muos_clpq_at (&then, what);
}


muos_error
muos_clpq_repeat (muos_clock32 when)
{
  if (!clpq_what)
    return muos_error_clpq_repeat;

  muos_clock at;
  muos_barray_copy (at.barray, muos_clpq.now.barray);
  muos_barray_add_uint32 (at.barray, when+clpq_delay);

  return muos_clpq_at (&at, clpq_what);
}


muos_error
muos_clpq_at_isr (muos_clock* when, muos_clpq_function what)
{
  if (what && (uintptr_t)what <= MUOS_CLPQ_BARRIERS)
    return muos_error_error;  /* programmers error, should never happen, but better safe than sorry */

  if (muos_barray_is_lt (when->barray, muos_clpq.now.barray))
    {
      muos_barray_clear (when->barray);
      muos_barray_add (when->barray, muos_clpq.now.barray);
    }

  muos_clpq_segment segments = clpq_segment (when) - clpq_segment (&muos_clpq.now);

  // increment segments when there are leftover jobs
  if (clpq_segment_parity (&muos_clpq.now) != muos_status.clpq_parity && !clpq_barrier (muos_clpq.entries[muos_clpq.used-1].what))
    {
      ++segments;
    }

  const muos_clock16 when16 = muos_clock_clock16 (when);

  if (!segments)
    {
      if (muos_clpq.used >= MUOS_CLPQ_LENGTH)
        return muos_error_clpq_overflow;

      muos_clpq_index i = muos_clpq.used;
      for (; i; --i)
        {
          if (muos_clpq.entries[i-1].when > when16
              || (muos_clpq.entries[i-1].what && (uintptr_t)muos_clpq.entries[i-1].what <= MUOS_CLPQ_BARRIERS))
            break;

          muos_clpq.entries[i] = muos_clpq.entries[i-1];
        }

      muos_clpq.entries[i] = (struct muos_clpq_entry){when16, what};
      ++muos_clpq.used;
    }
  else
    {
      uint8_t barrier = 0;
      muos_clpq_index i = muos_clpq.used;

      // find respective barrier
      for (; i && segments; --i)
        {
          barrier = clpq_barrier (muos_clpq.entries[i-1].what);
          if (barrier >= segments)
            break;
          segments -= barrier;
        }

      //have to insert barrier or not?
      if (!barrier)
        {
          // create new (segments) barriers at end

          //PLANNED: calculation for exponential barriers (log2)
          if (MUOS_CLPQ_LENGTH-muos_clpq.used < segments+1)
            {
              return muos_error_clpq_overflow;
            }

          memmove (&muos_clpq.entries[i+segments+1], &muos_clpq.entries[i], sizeof (struct muos_clpq_entry) * (muos_clpq.used-i));
          muos_clpq.used += segments + 1;

#ifdef MUOS_CLPQ_EXPONENTIAL
          //TODO: insert exponential barriers
#else
          for (; segments; --segments)
            muos_clpq.entries[i+segments] = (struct muos_clpq_entry){0, (muos_clpq_function)0x1};
#endif
        }
      else if (barrier == 1)
        {
          // insert into barrier at i
          if (muos_clpq.used >= MUOS_CLPQ_LENGTH)
            {
              return muos_error_clpq_overflow;
            }

          for (--i; i; --i)
            {
              if (muos_clpq.entries[i-1].when > when16
                  || (muos_clpq.entries[i-1].what && (uintptr_t)muos_clpq.entries[i-1].what <= MUOS_CLPQ_BARRIERS))
                break;
            }

          memmove (&muos_clpq.entries[i+1], &muos_clpq.entries[i], sizeof (struct muos_clpq_entry) * (muos_clpq.used-i));
          muos_clpq.used += 1;
        }
#ifdef MUOS_CLPQ_EXPONENTIAL
      else if (barrier > 1)
        {
          //TODO: barrier splitting/reordering, check allocation
          //if (muos_clpq.used > MUOS_CLPQ_LENGTH-2)
          //  return muos_error_clpq_overflow;
        }
#endif

      // insert what
      muos_clpq.entries[i] = (struct muos_clpq_entry){when16, what};
    }

  return muos_success;
}




/*

  Removing Jobs

 */
bool
muos_clpq_remove_isr (const muos_clock* when, muos_clpq_function what)
{
  MUOS_ASSERT(true, !(what && (uintptr_t)what <= MUOS_CLPQ_BARRIERS));

  muos_clpq_index i;

  // skip barriers
  muos_clpq_segment segments = clpq_segment (when) - clpq_segment (&muos_clpq.now) ;

  for (i = muos_clpq.used; i && segments; --i)
    {
      uint8_t barrier = clpq_barrier (muos_clpq.entries[i-1].what);
      if (barrier > segments)
        return false;

      segments -= barrier;
    }

  if (!i)
    return false;

  // find entry
  const muos_clock16 when16 = muos_clock_clock16 (when);

  for (; i; --i)
    {
      if (muos_clpq.entries[i-1].when == when16 && muos_clpq.entries[i-1].what == what)
        break;
    }

  if (!i)
    return false;

  // remove
  for (; i < muos_clpq.used; ++i)
    {
      muos_clpq.entries[i-1] = muos_clpq.entries[i];
    }

  --muos_clpq.used;

  //TODO: remove/merge excess barriers (optional)
#ifdef MUOS_CLPQ_EXPONENTIAL
  // ... dito
#endif

  return true;
}




//PLANNED: function to scan for 'what', first or count (for recover after errors)



/*

  Scheduling

 */
bool
muos_clpq_schedule_isr (void)
{
  muos_clock_now_isr (&muos_clpq.now);

  if (!muos_clpq.used)
    return false;

  bool parity_unmatch = clpq_segment_parity (&muos_clpq.now) != muos_status.clpq_parity;

  if (clpq_barrier (muos_clpq.entries[muos_clpq.used-1].what))
    {

      if (parity_unmatch)
        {
#ifdef MUOS_CLPQ_EXPONENTIAL
          //TODO: split barriers
#endif
          --muos_clpq.used;
          muos_status.clpq_parity ^= 1;
          return true;
        }
    }
  else
    {
      const muos_clock16 now16 = muos_barray_uint16 (muos_clpq.now.barray, 0);

      if (muos_clpq.entries[muos_clpq.used-1].when <= now16
          || parity_unmatch)
        {
          --muos_clpq.used;

          clpq_what = muos_clpq.entries[muos_clpq.used].what;

          if (clpq_what)
            {
              clpq_delay = muos_clock16_elapsed (now16, muos_clpq.entries[muos_clpq.used].when);

              muos_interrupt_enable ();
              clpq_what ();
              muos_interrupt_disable ();
            }

          return true;
        }
    }

  return false;
}


#endif
