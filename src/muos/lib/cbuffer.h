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

#ifndef MUOS_LIB_CBUFFER_H
#define MUOS_LIB_CBUFFER_H

#include <muos/muos.h>

#include <stdint.h>

typedef MUOS_BUFFER_INDEX muos_cbuffer_index;

struct muos_cbuffer
{
  muos_cbuffer_index      start;
  muos_cbuffer_index      len;
  uint8_t                 cbuffer[];
};

#define MUOS_CBUFFERDEF(size)                   \
struct                                          \
{                                               \
  struct muos_cbuffer descriptor;               \
  uint8_t       cbuffer[size];                  \
}

typedef volatile struct muos_cbuffer* muos_cbuffer_vptr;

static inline void
muos_cbuffer_init (muos_cbuffer_vptr cbuffer)
{
  cbuffer->start = 0;
  cbuffer->len = 0;
}


void
muos_cbuffer_push (muos_cbuffer_vptr cbuffer, muos_cbuffer_index size, const uint8_t value);


uint8_t
muos_cbuffer_pop (muos_cbuffer_vptr cbuffer, muos_cbuffer_index size);


void
muos_cbuffer_reserve (muos_cbuffer_vptr cbuffer, muos_cbuffer_index size, muos_cbuffer_index need);



#define MUOS_CBUFFER_SIZE(b)  MUOS_ARRAY_ELEMENTS((b).cbuffer)

#define MUOS_CBUFFER_FREE(b) MUOS_CBUFFER_SIZE(b) - (b).descriptor.len
#define MUOS_CBUFFER_PUSH(b, v) muos_cbuffer_push (&(b).descriptor, MUOS_CBUFFER_SIZE(b), (v))
#define MUOS_CBUFFER_POP(b) muos_cbuffer_pop (&(b).descriptor, MUOS_CBUFFER_SIZE(b))


#endif
