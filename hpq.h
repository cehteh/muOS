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

#ifndef MUOS_HPQ_H
#define MUOS_HPQ_H

#include <muos/muos.h>
#include <muos/error.h>
#include <muos/lib/queue.h>

#if MUOS_HPQ_LENGTH > 0

typedef MUOS_QUEUEDEF(MUOS_HPQ_LENGTH) muos_hpq_type;
extern volatile muos_hpq_type muos_hpq;


static inline bool
muos_hpq_schedule (void)
{
  return MUOS_QUEUE_SCHEDULE (muos_hpq);
}

static inline bool
muos_hpq_check (uint8_t need)
{
  if (MUOS_QUEUE_FREE (muos_hpq) >= need)
    {
      return true;
    }
  else
    {
      muos_error_set_unsafe (muos_error_hpq_overflow);
      return false;
    }
}

static inline void
muos_hpq_pushback_unsafe (muos_queue_function f)
{
  if (muos_hpq_check (1))
    {
      MUOS_QUEUE_PUSHBACK (muos_hpq, (f));
    }
}


//hpq_api:
//: .Schedule functions at high priority
//: ----
//: void muos_hpq_pushback (muos_queue_function fn)
//: void muos_hpq_pushback_arg (muos_queue_function_arg fn, intptr_t arg)
//: void muos_hpq_pushfront (muos_queue_function fn)
//: void muos_hpq_pushfront_arg (muos_queue_function_arg fn, intptr_t arg)
//: ----
//:
//: +fn+::
//: function to schedule
//: +arg+::
//:  argument to pass to the function when it gets called
//:

//FIXME: refine error handling, return bool or error number on synchronous calls
static inline void
muos_hpq_pushback (muos_queue_function f)
{
  muos_interrupt_disable ();
  muos_hpq_pushback_unsafe (f);
  muos_interrupt_enable ();
}

static inline void
muos_hpq_pushback_arg (muos_queue_function_arg f, intptr_t a)
{
  muos_interrupt_disable ();
  if (muos_hpq_check (2))
    {
      MUOS_QUEUE_PUSHBACK_ARG(muos_hpq, (f), (a));
    }
  muos_interrupt_enable ();
}

static inline void
muos_hpq_pushfront (muos_queue_function f)
{
  muos_interrupt_disable ();
  if (muos_hpq_check (1))
    {
      MUOS_QUEUE_PUSHFRONT(muos_hpq, (f));
    }
  muos_interrupt_enable ();
}

static inline void
muos_hpq_pushfront_arg (muos_queue_function_arg f, intptr_t a)
{
  muos_interrupt_disable ();
  if (muos_hpq_check (2))
    {
      MUOS_QUEUE_PUSHFRONT_ARG(muos_hpq, (f), (a));
    }
  muos_interrupt_enable ();
}

#else
// stub for the schedule loop
static inline bool
muos_hpq_schedule (void)
{
  return false;
}
#endif

#endif
