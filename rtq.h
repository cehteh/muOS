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

#ifndef MUOS_RTQ_H
#define MUOS_RTQ_H

#include <muos/muos.h>
#include <muos/error.h>
#include <muos/lib/queue.h>

#if MUOS_RTQ_LENGTH > 0

typedef MUOS_QUEUEDEF(MUOS_RTQ_LENGTH) muos_rtq_type;

extern muos_rtq_type muos_rtq;

static inline bool
muos_rtq_schedule (void)
{
  return muos_queue_schedule (&muos_rtq.descriptor, MUOS_RTQ_LENGTH);
}

static inline bool
muos_rtq_check (uint8_t need)
{
  return muos_queue_free (&muos_rtq.descriptor, MUOS_RTQ_LENGTH) >= need;
}


//rtq_api:
//: .Schedule functions at realtime priority
//: ----
//: muos_error muos_rtq_pushback (muos_queue_function fn)
//: muos_error muos_rtq_pushback_arg (muos_queue_function_arg fn, intptr_t arg)
//: muos_error muos_rtq_pushfront (muos_queue_function fn)
//: muos_error muos_rtq_pushfront_arg (muos_queue_function_arg fn, intptr_t arg)
//: muos_error muos_rtq_pushback_isr (muos_queue_function fn, bool schedule)
//: muos_error muos_rtq_pushback_arg_isr (muos_queue_function_arg fn, intptr_t arg, bool schedule)
//: muos_error muos_rtq_pushfront_isr (muos_queue_function fn, bool schedule)
//: muos_error muos_rtq_pushfront_arg_isr (muos_queue_function_arg fn, intptr_t arg, bool schedule)
//: ----
//:
//: +fn+::
//:  function to schedule
//: +arg+::
//:  argument to pass to the function
//: +schedule+::
//:  when false the scheduler will not check for functions scheduled
//:  after a wake up from sleep.
//:
//: The *_isr variants of these functions are intended to be called from Interrupt handlers
//: or contexts where interrupts are already disabled.
//:
//: These functions return 'muos_success' on success and 'muos_error_rtq_overflow' on error.
//:
//: ----
//: intptr_t muos_rtq_pop_isr (void)
//: ----
//:
//: removes and returns the first element (argument) from the rtq. Must be called
//: while interrupts are still disabled at the start of scheduled functions.
//: (Note: no safety net when the caller didn't push an argument)

muos_error
muos_rtq_pushback (muos_queue_function f);

muos_error
muos_rtq_pushback_arg (muos_queue_function f, intptr_t a);

muos_error
muos_rtq_pushfront (muos_queue_function f);

muos_error
muos_rtq_pushfront_arg (muos_queue_function f, intptr_t a);


muos_error
muos_rtq_pushback_isr (muos_queue_function f, bool schedule);

muos_error
muos_rtq_pushback_arg_isr (muos_queue_function f, intptr_t a, bool schedule);

muos_error
muos_rtq_pushfront_isr (muos_queue_function f, bool schedule);

muos_error
muos_rtq_pushfront_arg_isr (muos_queue_function f, intptr_t a, bool schedule);

intptr_t
muos_rtq_pop_isr (void);

#else
// stub for the schedule loop
static inline bool
muos_rtq_schedule (void)
{
  return false;
}
#endif


#endif
