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

#ifndef MUOS_CLPQ_H
#define MUOS_CLPQ_H

#if MUOS_CLPQ_LENGTH > 0

#include <muos/muos.h>
#include <muos/clock.h>
#include <muos/debug.h>

#if MUOS_CLPQ_LENGTH < 255
typedef uint8_t muos_clpq_index;
#else
typedef uint16_t muos_clpq_index;
#endif


//clpq_api:
//: .Callback Function Type
//: ----
//  typedef void (*muos_clpq_function)(muos_clock16 delayed);
//: ----
//:
//: User functions to be scheduled must be of type 'muos_clpq_function'.
//: There is no return value, errors need to be handled within that function,
//: delegated to another scheduled function or set asynchronously.
//:
//: Since functions are scheduled at or after the time registered they may become
//: delayed some timer ticks. The +delayed+ parameter holds this number.
//: This can be used to consistently repeat a function at given intervals.
//:
//: The delay is calculated relative to 'muos_clock_now()' which is set by
//: the scheduler. Thus recursive calls to the scheduler will update the 'now'
//: time and by that make keeping track of recurring jobs a bit more complicated.
//: If possible do not invoke any recursive scheduler calls (muos_wait/muos_yield)
//: from recurring jobs but delegate work that may wait to the hpq or bgq.
//:
typedef void (*muos_clpq_function)(void);


struct muos_clpq_entry
{
  muos_clock16 when;
  muos_clpq_function what;
};

typedef struct
{
  muos_clock now;
  bool sync;
  muos_clpq_index used;
  struct muos_clpq_entry entries[MUOS_CLPQ_LENGTH];
} muos_clpq_type;

extern muos_clpq_type muos_clpq;


//clpq_api:
//: .Event time
//: ----
//: muos_clock muos_clpq_now (muos_clock* now)
//: muos_clock32 muos_clpq_now32 (void)
//: muos_clock16 muos_clpq_delayed (void)
//: bool muos_clpq_is_expired (muos_clock* when)
//: ----
//:
//: Each clpq scheduling keeps the time. This time can be efficiently
//: queried because no hardware access or locks are needed and gives
//: a stable timestamp throughout the whole job.
//:
//: 'muos_clpq_now*()' returns the time the last CLPQ event was scheduled.
//: This functon gives consistent time in a efficient way. The time is only updated
//: on CLPQ scheduling. Also HPQ and BGQ entries can benefit from this, but not the RTQ
//: since that has higher priority than the CLPQ. The 32bit variant may roll over every
//: once a while.
//:
//: 'muos_clpq_delayed()' Makes only sense within a clpq scheduled function, it returns
//: the number of ticks the scheduler is behind the requested time.
//:
//: 'muos_clpq_delayed()' Makes only sense within a clpq scheduled function, it returns
//: the number of ticks the scheduler is behind the requested time.
//:
//: 'muos_clpq_is_expired (muos_clock* when)' compares the current 'clpq_now()' time with
//: the given 'when' timestamp. When 'now >= when' it returns 'true' and 'false' otherwise.
//:
static inline void
muos_clpq_now (muos_clock* now)
{
  muos_barray_copy (now->barray, muos_clpq.now.barray);
}

static inline muos_clock32
muos_clpq_now32 (void)
{
  return muos_barray_uint32 (muos_clpq.now.barray, 0);
}

muos_clock16
muos_clpq_delayed (void);


static inline bool
muos_clpq_is_expired (muos_clock* when)
{
  return muos_barray_is_lt (when->barray, muos_clpq.now.barray);
}



//clpq_api:
//: .Scheduling a function call
//: ----
//: muos_error
//: muos_clpq_at (const muos_clock* when, muos_clpq_function what, bool unique)
//:
//: muos_error
//: muos_clpq_at_isr (const muos_clock* when, muos_clpq_function what, bool unique)
//:
//: muos_error
//: muos_clpq_after (muos_clock32 when, muos_clpq_function what)
//:
//: muos_error
//: muos_clpq_repeat (muos_clock32 when)
//: ----
//:
//: +when+::
//:   Time when the event should be scheduled
//: +what+::
//:   Function to push or NULL for a scheduler wakeup
//: +unique+::
//:   Request that the when/what must be unique
//:
//: 'muos_clpq_at()/muos_clpq_at_isr()' Pushes the function 'what' one the clpq to
//: be scheduled not before 'when'. Pushing is stable ordered when 2 functions are
//: to be scheduled at the same time the order in which they where pushed is preserved.
//: When uniqe operation is requested the function fails when a there is already a
//: when/what pair scheduled with the same parameters.
//:
//: 'muos_clpq_after()' schedules 'what' to be executed after 'muos_clpq_now()+when'
//:
//: 'muos_clpq_repeat()' may be called from a clpq scheduled function to repeat the same
//: function after 'when' time. This gives consistent timing. In case the designated
//: time has already passed, the time gets incremented by 'when' to skip expired repetitions.
//:
//: returns::
//: 'muos_success':::
//:   Everything ok.
//: 'muos_error_clpq_nounique':::
//:   Cant insert uniquely.
//: 'muos_error_clpq_overflow':::
//:   The clpq has not enough capacity to add the request.
//: 'muos_error_clpq_past':::
//:   Can't schedule something in the past.
//: 'muos_fatal_error':::
//:   Programming error. 'what' is not a function or
//:   'clpq_repeat()' called from outside of a clpq scheduled function.
//:
muos_error
muos_clpq_at_isr (const muos_clock* when, muos_clpq_function what, bool unique);


static inline muos_error
muos_clpq_at (const muos_clock* when, muos_clpq_function what, bool unique)
{
  muos_error ret;
  muos_interrupt_disable ();
  ret = muos_clpq_at_isr (when, what, unique);
  muos_interrupt_enable ();

  return ret;
}



muos_error
muos_clpq_after (muos_clock32 when, muos_clpq_function what);

muos_error
muos_clpq_repeat (muos_clock32 when);



//clpq_api:
//: .Remove entries
//: ----
//: bool
//: muos_clpq_remove (muos_clock when, muos_clpq_function what)
//:
//: bool
//: muos_clpq_remove_isr (muos_clock when, muos_clpq_function what)
// :
// : bool
// : muos_clpq_remove_first (muos_clpq_function what)
// :
// : bool
// : muos_clpq_remove_last (muos_clpq_function what)
//: ----
//:
//: +when+::
//:   time at when it was scheduled
//: +what+::
//:   Function to remove
//:
//: Removes one element inserted with the same parameters previously.
//: Returns true when element got removed or false when element was not found.
//:

bool
muos_clpq_remove_isr (const muos_clock* when, muos_clpq_function what);

static inline bool
muos_clpq_remove (const muos_clock* when, muos_clpq_function what)
{
  bool ret;
  muos_interrupt_disable ();
  ret = muos_clpq_remove_isr (when, what);
  muos_interrupt_enable ();

  return ret;
}



bool
muos_clpq_schedule_isr (void);


bool
muos_hw_clpq_wake_isr (void);


/*

  Tools

*/

static inline uint8_t
clpq_barrier (muos_clpq_function what)
{
  if (what && (uintptr_t)what <= MUOS_CLPQ_BARRIERS)
    return (uintptr_t)what;

  return 0;
}





#endif
#endif
