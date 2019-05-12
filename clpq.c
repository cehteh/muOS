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
#include <muos/error.h>

#include <string.h>

//PLANNED: wrapper for recurring jobs
//PLANNED: implement minimal uint8_t array math lib (inc/compare/xor etc)
//PLANNED: wrap all clock math in functions for above conversion


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
typedef uint64_t muos_clpq_segment;
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


/*

  Tools

*/

static inline bool
clpq_segment_parity (muos_clock when)
{
  return when & 1ULL<<16;
}


static inline uint8_t
clpq_barrier (muos_clpq_function what)
{
  if (what && (uintptr_t)what <= MUOS_CLPQ_BARRIERS)
    return (uintptr_t)what;

  return 0;
}


static inline muos_clpq_segment
clpq_segment (muos_clock when)
{
  return when >> 16;
}



/*

  Pushing jobs

*/

muos_error
muos_clpq_at_isr (muos_clock when, muos_clpq_function what)
{
  if (what && (uintptr_t)what <= MUOS_CLPQ_BARRIERS)
    return muos_error_error;  /* programmers error, should never happen, but better safe than sorry */

  if (when < muos_clpq.now)
    when = muos_clpq.now;

  muos_clpq_segment segments = clpq_segment (when) - clpq_segment (muos_clpq.now);

  // increment segments when there are leftover jobs
  if (clpq_segment_parity (muos_clpq.now) != muos_status.clpq_parity && !clpq_barrier (muos_clpq.entries[muos_clpq.used-1].what))
    {
      ++segments;
    }

  if (!segments)
    {
      if (muos_clpq.used >= MUOS_CLPQ_LENGTH)
        return muos_error_clpq_overflow;

      muos_clpq_index i = muos_clpq.used;
      for (; i; --i)
        {
          if (muos_clpq.entries[i-1].when > (muos_clock16)when
              || (muos_clpq.entries[i-1].what && (uintptr_t)muos_clpq.entries[i-1].what <= MUOS_CLPQ_BARRIERS))
            break;

          muos_clpq.entries[i] = muos_clpq.entries[i-1];
        }

      muos_clpq.entries[i] = (struct muos_clpq_entry){when, what};
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
              if (muos_clpq.entries[i-1].when > (muos_clock16)when
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
      muos_clpq.entries[i] = (struct muos_clpq_entry){when, what};
    }
  return muos_success;
}



//PLANNED: delay relative to clpq.now
//muos_error
//muos_clpq16_in (muos_clock16 when, muos_clpq_function what)
//muos_error
//muos_clpq32_in (muos_clock32 when, muos_clpq_function what)




/*

  Removing Jobs

 */

bool
muos_clpq_remove_isr (muos_clock when, muos_clpq_function what)
{
  if (what && (uintptr_t)what <= MUOS_CLPQ_BARRIERS)
    return muos_error_error;  /* programmers error, should never happen, but better safe than sorry */

  muos_clpq_segment segments = clpq_segment (when) - clpq_segment (muos_clpq.now);

  if (!segments)
    {
      muos_clpq_index i = muos_clpq.used;
      for (; i; --i)
        {
          if (muos_clpq.entries[i-1].when == (muos_clock16)when && muos_clpq.entries[i-1].what == what)
            break;
        }

      if (!i)
        return false;

      //TODO: memmove?
      for (; i < muos_clpq.used; ++i)
        {
          muos_clpq.entries[i-1] = muos_clpq.entries[i];
        }

      --muos_clpq.used;
    }
  else
    {
      // remove with barriers

      //TODO: when removing last, remove all preceeding barriers
      //TODO: when exponential, then merge barriers

      uint8_t barrier = 0;
      muos_clpq_index i = muos_clpq.used;

      for (; i && segments; --i)
        {
          barrier = clpq_barrier (muos_clpq.entries[i-1].what);
          if (barrier > segments)
            return false;

          segments -= barrier;
        }

      if (!i)
        return false;

      for (; i; --i)
        {
          if (muos_clpq.entries[i-1].when == (muos_clock16)when && muos_clpq.entries[i-1].what == what)
            break;
        }

      if (!i)
        return false;

      //TODO: memmove?
      for (; i < muos_clpq.used; ++i)
        {
          muos_clpq.entries[i-1] = muos_clpq.entries[i];
        }

      --muos_clpq.used;
    }
  return true;
}


#if 0 //PLANNED: unimplemented
bool
muos_clpq_remove_first (muos_clpq_function what);

bool
muos_clpq_remove_last (muos_clpq_function what);
#endif



//PLANNED: function to scan for 'what', first or count (for recover after errors)



/*

  Scheduling

 */
bool
muos_clpq_schedule_isr (void)
{
  muos_clpq.now = muos_clock_now_isr ();

  if (!muos_clpq.used)
    return false;

  if (clpq_barrier (muos_clpq.entries[muos_clpq.used-1].what))
    {
      if (clpq_segment_parity (muos_clpq.now) != muos_status.clpq_parity)
        {
#ifdef MUOS_CLPQ_EXPONENTIAL
          //TODO: split barriers
#endif
          --muos_clpq.used;
          muos_status.clpq_parity ^= 1;
        }
      else
        {
          // stop at barrier
          return false;
        }
    }


  if (!clpq_barrier (muos_clpq.entries[muos_clpq.used-1].what))
    {
      if (muos_clpq.entries[muos_clpq.used-1].when <= (muos_clock16)muos_clpq.now)
        {
          --muos_clpq.used;

          muos_clpq_function what = muos_clpq.entries[muos_clpq.used].what;
          if (what)
            {
              muos_clock16 delay = muos_clock16_elapsed (muos_clpq.now, muos_clpq.entries[muos_clpq.used].when);

              if (delay > UINT16_MAX/2)
                muos_error_set_isr (muos_warn_clpq_delay);

              muos_interrupt_enable ();
              what (delay);
              muos_interrupt_disable ();
            }
          return true;
        }
    }

  return false;
}


#endif
