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

#ifndef MUOS_TXQUEUE_H
#define MUOS_TXQUEUE_H

#include <stdint.h>
#include <muos/lib/cbuffer.h>

#if MUOS_SERIAL_TXQUEUE > 1
typedef MUOS_CBUFFERDEF(MUOS_SERIAL_TXQUEUE)
muos_txqueue_type;
#endif

#define MUOS_TXQUEUE_TAGS                       \
  TAG()                                         \

enum muos_txqueue_tags
{
  MUOS_TXTAG_NCHARS = 128,
  MUOS_TXTAG_NCHARS_END = 191,
};

muos_cbuffer_index
muos_txqueue_free (void);

void
muos_txqueue_push (const uint8_t value);

uint8_t
muos_txqueue_pop (void);

void
muos_txqueue_run (void);

#endif
