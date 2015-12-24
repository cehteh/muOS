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

#ifndef MUOS_LIB_BUFFER_H
#define MUOS_LIB_BUFFER_H

#include <muos/muos.h>

#include <stdint.h>

typedef MUOS_BUFFER_INDEX muos_buffer_index;

struct muos_buffer
{
  muos_buffer_index      len;
  uint8_t                buffer[];
};

#define MUOS_BUFFERDEF(size)                    \
struct                                          \
{                                               \
  struct muos_buffer descriptor;                \
  uint8_t       buffer[size];                   \
}


typedef volatile struct muos_buffer* muos_buffer_vptr;


static inline void
muos_buffer_init (muos_buffer_vptr buffer)
{
  buffer->len = 0;
}


void
muos_buffer_push (muos_buffer_vptr cbuffer, const uint8_t value);

void
muos_buffer_pop (muos_buffer_vptr buffer, muos_buffer_index n);



#define MUOS_BUFFER_SIZE(b)  MUOS_ARRAY_ELEMENTS((b).buffer)
#define MUOS_BUFFER_FREE(b) MUOS_BUFFER_SIZE(b) - (b).descriptor.len
#define MUOS_BUFFER_USED(b) (b).descriptor.len
#define MUOS_BUFFER_PUSH(b, v) muos_buffer_push (&(b).descriptor, (v))
#define MUOS_BUFFER_POP(b, n) muos_buffer_pop (&(b).descriptor, (n))



#endif
