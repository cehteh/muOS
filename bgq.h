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
#include <muos/lib/queue.h>

#if MUOS_BGQ_LENGTH > 0

typedef MUOS_QUEUEDEF(MUOS_BGQ_LENGTH) muos_bgq_type;
extern muos_bgq_type muos_bgq;

static inline bool
muos_bgq_schedule (void)
{
  return MUOS_QUEUE_SCHEDULE (muos_bgq);
}


static inline void
muos_bgq_pushback (muos_queue_function f)
{
  MUOS_QUEUE_PUSHBACK(muos_bgq, (f));
}

static inline void
muos_bgq_pushback_arg (muos_queue_function_arg f, intptr_t a)
{
  MUOS_QUEUE_PUSHBACK_ARG(muos_bgq, (f), (a));
}

static inline void
muos_bgq_pushfront (muos_queue_function f)
{
  MUOS_QUEUE_PUSHFRONT(muos_bgq, (f));
}

static inline void
muos_bgq_pushfront_arg (muos_queue_function_arg f, intptr_t a)
{
  MUOS_QUEUE_PUSHFRONT_ARG(muos_bgq, (f), (a));
}
#else
// stub for the schedule loop
static inline bool
muos_bgq_schedule (void)
{
  return false;
}
#endif


#endif
