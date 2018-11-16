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

extern muos_hpq_type muos_hpq;

static inline bool
muos_hpq_schedule (void)
{
  return muos_queue_schedule (&muos_hpq.descriptor, MUOS_HPQ_LENGTH);
}

static inline bool
muos_hpq_check (uint8_t need)
{
  return muos_queue_free (&muos_hpq.descriptor, MUOS_HPQ_LENGTH) >= need;
}


//hpq_api:
//: .Schedule functions at high priority
//: ----
//: muos_error muos_hpq_pushback (muos_queue_function fn)
//: muos_error muos_hpq_pushback_arg (muos_queue_function_arg fn, intptr_t arg)
//: muos_error muos_hpq_pushfront (muos_queue_function fn)
//: muos_error muos_hpq_pushfront_arg (muos_queue_function_arg fn, intptr_t arg)
//: muos_error muos_hpq_pushback_isr (muos_queue_function fn)
//: muos_error muos_hpq_pushback_arg_isr (muos_queue_function_arg fn, intptr_t arg)
//: muos_error muos_hpq_pushfront_isr (muos_queue_function fn)
//: muos_error muos_hpq_pushfront_arg_isr (muos_queue_function_arg fn, intptr_t arg)
//: ----
//:
//: +fn+::
//:  function to schedule
//: +arg+::
//:  argument to pass to the function
//:
//: The *_isr variants of these functions are intended to be called from Interrupt handlers
//: or contexts where interrupts are already disabled.
//:
//: These functions return 'muos_success' on success and 'muos_error_hpq_overflow' on error.
//:
//: ----
//: intptr_t muos_hpq_pop_isr (void)
//: ----
//:
//: removes and returns the first element (argument) from the hpq. Must be called
//: while interrupts are still disabled at the start of scheduled functions.
//: (Note: no safety net when the caller didn't push an argument)

muos_error
muos_hpq_pushback (muos_queue_function f);

muos_error
muos_hpq_pushback_arg (muos_queue_function_arg f, intptr_t a);

muos_error
muos_hpq_pushfront (muos_queue_function f);

muos_error
muos_hpq_pushfront_arg (muos_queue_function_arg f, intptr_t a);


muos_error
muos_hpq_pushback_isr (muos_queue_function f);

muos_error
muos_hpq_pushback_arg_isr (muos_queue_function_arg f, intptr_t a);

muos_error
muos_hpq_pushfront_isr (muos_queue_function f);

muos_error
muos_hpq_pushfront_arg_isr (muos_queue_function_arg f, intptr_t a);

intptr_t
muos_hpq_pop_isr (void);

#else
// stub for the schedule loop
static inline bool
muos_hpq_schedule (void)
{
  return false;
}
#endif


#endif
