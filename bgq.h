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

#ifndef MUOS_BGQ_H
#define MUOS_BGQ_H

#include <muos/muos.h>
#include <muos/error.h>
#include <muos/lib/queue.h>

#if MUOS_BGQ_LENGTH > 0

typedef MUOS_QUEUEDEF(MUOS_BGQ_LENGTH) muos_bgq_type;
extern muos_bgq_type muos_bgq;

static inline bool
muos_bgq_schedule (void)
{
  return MUOS_QUEUE_SCHEDULE (muos_bgq);
}

static inline bool
muos_bgq_check (uint8_t need)
{
  return MUOS_QUEUE_FREE (muos_bgq) >= need;
}


//bgq_api:
//: .Schedule functions at background priority
//: ----
//: muos_error muos_bgq_pushback (muos_queue_function fn)
//: muos_error muos_bgq_pushback_arg (muos_queue_function_arg fn, intptr_t arg)
//: muos_error muos_bgq_pushfront (muos_queue_function fn)
//: muos_error muos_bgq_pushfront_arg (muos_queue_function_arg fn, intptr_t arg)
//: muos_error muos_bgq_pushback_unsafe (muos_queue_function fn)
//: muos_error muos_bgq_pushback_arg_unsafe (muos_queue_function_arg fn, intptr_t arg)
//: muos_error muos_bgq_pushfront_unsafe (muos_queue_function fn)
//: muos_error muos_bgq_pushfront_arg_unsafe (muos_queue_function_arg fn, intptr_t arg)
//: ----
//:
//: +fn+::
//: function to schedule
//: +arg+::
//:  argument to pass to the function
//:
//: The *_unsafe variants of these functions are intended to be called from Interrupt handlers
//: or contexts where interrupts are already disabled.
//:
//: These functions return 'muos_success' on success and 'muos_error_bgq_overflow' on error.
//:

static inline muos_error
muos_bgq_pushback_unsafe (muos_queue_function f)
{
  if (!muos_bgq_check (1))
    return muos_error_bgq_overflow;

  MUOS_QUEUE_PUSHBACK(muos_bgq, (f));
  return muos_success;
}

static inline muos_error
muos_bgq_pushback_arg_unsafe (muos_queue_function_arg f, intptr_t a)
{
  if (!muos_bgq_check (2))
    return muos_error_bgq_overflow;

  MUOS_QUEUE_PUSHBACK_ARG(muos_bgq, (f), (a));
  return muos_success;
}

static inline muos_error
muos_bgq_pushfront_unsafe (muos_queue_function f)
{
  if (!muos_bgq_check (1))
    return muos_error_bgq_overflow;

  MUOS_QUEUE_PUSHFRONT(muos_bgq, (f));
  return muos_success;
}

static inline muos_error
muos_bgq_pushfront_arg_unsafe (muos_queue_function_arg f, intptr_t a)
{
  if (!muos_bgq_check (2))
    return muos_error_bgq_overflow;

  MUOS_QUEUE_PUSHFRONT_ARG(muos_bgq, (f), (a));
  return muos_success;
}


muos_error
muos_bgq_pushback (muos_queue_function f);

muos_error
muos_bgq_pushback_arg (muos_queue_function_arg f, intptr_t a);

muos_error
muos_bgq_pushfront (muos_queue_function f);

muos_error
muos_bgq_pushfront_arg (muos_queue_function_arg f, intptr_t a);


#else
// stub for the schedule loop
static inline bool
muos_bgq_schedule (void)
{
  return false;
}
#endif


#endif
