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

#ifndef MUOS_RTQ_H
#define MUOS_RTQ_H

#include <muos/muos.h>
#include <muos/lib/queue.h>

#if MUOS_RTQ_LENGTH > 0

typedef MUOS_QUEUEDEF(MUOS_RTQ_LENGTH) muos_rtq_type;
extern muos_rtq_type muos_rtq;

static inline bool
muos_rtq_schedule (void)
{
  return MUOS_QUEUE_SCHEDULE (muos_rtq);
}

static inline void
muos_rtq_pushback (muos_queue_function f)
{
  MUOS_QUEUE_PUSHBACK(muos_rtq, (f));
}

static inline void
muos_rtq_pushback_arg (muos_queue_function_arg f, intptr_t a)
{
  MUOS_QUEUE_PUSHBACK_ARG(muos_rtq, (f), (a));
}

static inline void
muos_rtq_pushfront (muos_queue_function f)
{
  MUOS_QUEUE_PUSHFRONT(muos_rtq, (f));
}

static inline void
muos_rtq_pushfront_arg (muos_queue_function_arg f, intptr_t a)
{
  MUOS_QUEUE_PUSHFRONT_ARG(muos_rtq, (f), (a));
}

#else
// stub for the schedule loop
static inline bool
muos_rtq_schedule (void)
{
  return false;
}
#endif

#endif
