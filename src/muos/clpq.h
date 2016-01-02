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
#include <muos/lib/spriq.h>

#if MUOS_CLPQ_LENGTH > 0

typedef struct
{
  volatile struct muos_spriq descriptor;
  struct muos_spriq_entry spriq[MUOS_CLPQ_LENGTH];
} muos_clpq_type;
extern muos_clpq_type muos_clpq;

bool
muos_clpq_schedule (muos_spriq_priority when);

void
muos_clpq_at_unsafe (muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function what);

static inline void
muos_clpq_at (muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function what)
{
  muos_interrupt_disable ();
  muos_clpq_at_unsafe (base, when, what);
  muos_interrupt_enable ();
}

static inline void
muos_clpq_repeat (const struct muos_spriq_entry* event, muos_spriq_priority when)
{
  muos_clpq_at (event->when, when, event->fn);
}


void
muos_clpq_set_compmatch (void);

#endif

#endif
