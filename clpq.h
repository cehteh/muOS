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

#ifndef MUOS_CLPQ_H
#define MUOS_CLPQ_H

#include <muos/muos.h>
#include <muos/clock.h>

#if MUOS_CLPQ_LENGTH > 0

typedef struct
{
  muos_clock now;
  //FIXME: new queue in place struct muos_spriq descriptor;
  //FIXME: struct muos_spriq_entry spriq[MUOS_CLPQ_LENGTH];
} muos_clpq_type;
extern muos_clpq_type muos_clpq;

typedef void (*muos_clpq_function)(muos_clock when);

//clock_api:
//: .Event time
//: ----
//: muos_clock muos_clpq_now (void)
//: muos_clock32 muos_clpq32_now (void)
//: ----
//:
//: The time the last CLPQ event was scheduled. This functon gives consistent time in a very efficient way.
//: The time is only updated on CLPQ scheduling. Also HPQ and BGQ entries can benefit from this, but not RTQ
//: since that has higher priority than the CLPQ.
//:
static inline muos_clock
muos_clpq_now (void)
{
  return muos_clpq.now;
}

static inline muos_clock32
muos_clpq32_now (void)
{
  return muos_clpq.now;
}


bool
muos_clpq_schedule (void);


muos_error
muos_clpq_at_isr (muos_clock when, muos_clpq_function what);

static inline bool
muos_clpq_remove_isr (muos_clock when, muos_clpq_function what)
{
  (void)when; (void)what; //FIXME:
  return false;
  //return muos_spriq_remove (&muos_clpq.descriptor, base, when, what);
}


//clpq_api:
//: .Schedule a function at the given time
//: ----
//: void muos_clpq_at (
//:        muos_clock when,
//:        muos_spriq_function what
//: )
//: ----
//:
//: +base+::
//:   time used as relative base for the timing calculation
//: +when+::
//:   offset to base for the destination time
//: +what+::
//:   function to be scheduled
//:
//: For scheduling on time +base+ has some more contraints to handle
//: overflows correctly. The 'clpq' handles that, refer to the source
//: for details.
//:
static inline muos_error
muos_clpq_at (muos_clock when, muos_clpq_function what)
{
  muos_error ret;
  muos_interrupt_disable ();
  ret = muos_clpq_at_isr (when, what);
  muos_interrupt_enable ();

  return ret;
}


//clpq_api:
//:
//: .Reschedule an event
//: ----
//: void muos_repeat (
//:        const struct muos_spriq_entry* event,
//:        muos_spriq_priority when
//: )
//: ----
//:
//:
//: +event+::
//:   the event pointer passed which get passed into queued functions
//: +when+::
//:   time when to repeat this event
//:
//static inline void
//muos_clpq_repeat (const struct muos_spriq_entry* event, muos_spriq_priority when)
//{
  //FIXME: new clpq
  //muos_error_set (muos_clpq_at (event->when, when, event->fn));
//}


bool
muos_hw_clpq_set_compmatch (void);

#endif

#endif
