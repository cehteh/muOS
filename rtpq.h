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

#ifndef MUOS_RTPQ_H
#define MUOS_RTPQ_H

#include <muos/muos.h>

#if MUOS_RTPQ_LENGTH > 0

#include <muos/lib/spriq.h>

typedef struct
{
  struct muos_spriq descriptor;
  struct muos_spriq_entry spriq[MUOS_RTPQ_LENGTH];
} muos_rtpq_type;
extern muos_rtpq_type muos_rtpq;

bool
muos_rtpq_schedule (muos_spriq_priority when);

#endif
#endif
