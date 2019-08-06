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

//PLANNED: incremental barriers instead exponential
//PLANNED: tagged barriers, the 'what' in the following entry defines the timeout

#ifndef MUOS_CLPQ_DEBUG
#define MUOS_CLPQ_DEBUG 0
#endif

#define CLPQ_ASSERT(pred) MUOS_ASSERT(MUOS_CLPQ_DEBUG, pred)
#define CLPQ_ASSURE(pred, expect) MUOS_ASSURE(MUOS_CLPQ_DEBUG, expr, expect)

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
#endif



//clpq_impl:
//: Time Segments
//: -------------
//:
//: The time is sliced into segments of 65536 ticks (16bit). Only the span since the begin
//: of the current segment is stored in entries. Segments for later events are separated by
//: barriers at the segment borders.
//:
//: Priority Queue
//: --------------
//:
//: Uses a 'sorted stack' approach because times are compressed by storing base relative intervals only.
//: A heap data structure can not handle that. The general assumption is that pushing events in farer future
//: is allowed to be more expensive than close times. Also in most cases there wont be very much events on a
//: CLPQ at any time.
//:
//: Each entry in the clpq is defined by:
//:  'when'::
//:     the relative time to clpq_base() (that is 0..65535) any longer time spans need time barriers.
//:  'what'::
//:     the function to call or some special sentinel values implementing the barriers.
//:
//:
//: Wakeup Mark
//: -----------
//:
//: Uses 'NULL' as 'what'. Nothing gets scheduled but a wakeup gets scheduled. Used to implement silent
//: timeouts (example: timeout wakeup in muos_wait())
//:
//: Time Barriers
//: -------------
//:
//: Time barriers are inserted when a time exceeds into succeeding segments, the 'when' of a Time barrier is
//: always 0. The sync flag toggles with each segment to keep segments in synchronization.
//:
//:

muos_clpq_type muos_clpq;

static muos_clock16 clpq_delay;
static muos_clpq_function clpq_what;



/*

  Tools

*/

static inline bool
clpq_segment_sync (const muos_clock* when)
{
  return muos_barray_getbit (when->barray, 16);
}



static inline muos_clpq_segment
clpq_segmentdiff (const muos_clock* start, const muos_clock* end)
{
  //PLANNED: optimize as barray submod8 16 32 operation
  return muos_barray_uint32 (end->barray, 2) - muos_barray_uint32 (start->barray, 2);
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
  muos_clpq_now (&then);
  muos_clock_add32 (&then, when);

  return muos_clpq_at (&then, what, false);
}


muos_error
muos_clpq_repeat (muos_clock32 when)
{
  if (!clpq_what)
    return muos_fatal_error;

  muos_clock at;
  muos_clock_copy (&at, &muos_clpq.now);
  muos_clock_add32 (&at, when-clpq_delay);

  muos_error ret;
  while (1)
    {
      ret = muos_clpq_at (&at, clpq_what, false);
      if (ret != muos_error_clpq_past)
        break;

      muos_clock_add32 (&at, when);
    }

  return ret;
}


muos_error
muos_clpq_at_isr (const muos_clock* when, muos_clpq_function what, bool unique)
{
  CLPQ_ASSERT (!(what && (uintptr_t)what <= MUOS_CLPQ_BARRIERS));

  // 'when' already passed
  if (muos_clock_is_lt (when, &muos_clpq.now))
    return muos_error_clpq_past;

  muos_clpq_segment segments = clpq_segmentdiff (&muos_clpq.now, when);

  // increment segments when there are expired jobs
  if (clpq_segment_sync (&muos_clpq.now) != muos_clpq.sync)
    {
      ++segments;
    }

  // skip barriers
  muos_clpq_index i = muos_clpq.used;

  if (i && segments)
    {
      uint8_t barrier = 0;

      // find barrier
      for (; i && segments; --i)
        {
          barrier = clpq_barrier (muos_clpq.entries[i-1].what);
          if (barrier && barrier > segments)
            break;
          segments -= barrier;
        }

#ifdef MUOS_CLPQ_EXPONENTIAL
      if (i && !barrier)
        {
          //PLANNED: split barrier
        }
#endif
    }

  //check available space
  //PLANNED: exponential compresses number of segments required
  if (MUOS_CLPQ_LENGTH - muos_clpq.used < segments + 1)
    {
      return muos_error_clpq_overflow;
    }

  //find position for entry
  const muos_clock16 when16 = muos_clock_clock16 (when);

  muos_clpq_index seg_start = i;

  for (; i; --i)
    {
      if (unique)
        {
          if (muos_clpq.entries[i-1].when == when16
              && muos_clpq.entries[i-1].what == what)
            return muos_error_clpq_nounique;
        }

      if (muos_clpq.entries[i-1].when > when16
          || clpq_barrier (muos_clpq.entries[i-1].what))
        break;
    }

#ifdef MUOS_CLPQ_EXPONENTIAL
  //TODO: insert exponential barriers
#else
  //move and insert barriers
  if (muos_clpq.used-seg_start)
    {
      memmove (&muos_clpq.entries[seg_start+segments+1],
               &muos_clpq.entries[seg_start], sizeof (struct muos_clpq_entry) * (muos_clpq.used-seg_start));
    }

  for (; segments; --segments)
    {
      muos_clpq.entries[seg_start+segments] = (struct muos_clpq_entry){0, (muos_clpq_function)0x1};
      ++muos_clpq.used;
    }
#endif

  // move and insert job
  if (seg_start-i)
    {
      memmove (&muos_clpq.entries[i+1],
               &muos_clpq.entries[i], sizeof (struct muos_clpq_entry) * (seg_start-i));
    }

  muos_clpq.entries[i] = (struct muos_clpq_entry){when16, what};

  ++muos_clpq.used;

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
  muos_clpq_segment segments = clpq_segmentdiff (&muos_clpq.now, when);

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

  //FIXME: restart sync when queue was empty at 'clpq_at' time

  uint8_t barrier = clpq_barrier (muos_clpq.entries[muos_clpq.used-1].what);
  const muos_clock16 now16 = muos_clock_clock16 (&muos_clpq.now);

  if (clpq_segment_sync (&muos_clpq.now) == muos_clpq.sync)
    {
      if (barrier)
        {
          // barrier wait
          return false;
        }
      else
        {
          if (muos_clpq.entries[muos_clpq.used-1].when <= now16)
            {
              // job schedule
              clpq_delay = now16 - muos_clpq.entries[muos_clpq.used-1].when;
              clpq_what = muos_clpq.entries[muos_clpq.used-1].what;
            }
          else
            {
              // job wait
              return false;
            }
        }
    }
  else
    {
      if (barrier)
        {
          // remove barrier to next segment, sync
          muos_clpq.sync = !muos_clpq.sync;
        }
      else
        {
          // job is expired from previous segment
          clpq_delay = now16 - muos_clpq.entries[muos_clpq.used-1].when;
          clpq_what = muos_clpq.entries[muos_clpq.used-1].what;
        }
    }

  --muos_clpq.used;

  if (clpq_what)
    {
      muos_interrupt_enable ();
      clpq_what ();
      clpq_what = NULL;
      muos_interrupt_disable ();
    }

  // schedule again
  return true;
}

#endif
