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

typedef MUOS_CBUFFER_INDEX muos_cbuffer_index;

struct muos_cbuffer
{
  muos_cbuffer_index      start;
  muos_cbuffer_index      len;
  uint8_t                 cbuffer[];
};


//lib_cbuffer_api:
//: .Cbuffer definition
//: ----
//: MUOS_CBUFFERDEF(size)
//: ----
//:
//: +size+::
//:   number of elements
//:
//: Macro defining the type of a cbuffer for the given size.
//:
#define MUOS_CBUFFERDEF(size)                   \
struct                                          \
{                                               \
  struct muos_cbuffer descriptor;               \
  uint8_t       cbuffer[size];                  \
}


//lib_cbuffer_api:
//: .Cbuffer Initialization
//: ----
//: void muos_cbuffer_init (struct muos_cbuffer* cbuffer)
//: ----
//:
//: +cbuffer+::
//:   pointer to the cbuffer
//:
//: Initialization is not necessary at startup, it is only required
//: for to reinitialize and delete an existing queue.
//:
static inline void
muos_cbuffer_init (struct muos_cbuffer* cbuffer)
{
  cbuffer->start = 0;
  cbuffer->len = 0;
}


void
muos_cbuffer_push (struct muos_cbuffer* cbuffer, muos_cbuffer_index size, const uint8_t value);

uint8_t
muos_cbuffer_pop (struct muos_cbuffer* cbuffer, muos_cbuffer_index size);

void
muos_cbuffer_popn (struct muos_cbuffer* cbuffer, muos_cbuffer_index size, muos_cbuffer_index n);

uint8_t
muos_cbuffer_peek (struct muos_cbuffer* cbuffer, muos_cbuffer_index size, muos_cbuffer_index index);

void
muos_cbuffer_poke (struct muos_cbuffer* cbuffer, muos_cbuffer_index size, muos_cbuffer_index index, const uint8_t value);


//lib_cbuffer_api:
//: .Cbuffer API Macros
//: ----
//: MUOS_CBUFFER_SIZE(cbuffer)
//: MUOS_CBUFFER_FREE(cbuffer)
//: MUOS_CBUFFER_USED(cbuffer)
//: MUOS_CBUFFER_PUSH(cbuffer, value)
//: MUOS_CBUFFER_POP(cbuffer)
//: MUOS_CBUFFER_RPOP(cbuffer)
//: MUOS_CBUFFER_POPN(cbuffer, n)
//: MUOS_CBUFFER_PEEK(cbuffer, n)
//: MUOS_CBUFFER_POKE(cbuffer, n, value)
//: ----
//:
//:  +cbuffer+::
//:    the cbuffer as defined with +MUOS_CBUFFERDEF()+
//:  +value+::
//:    byte (+uint8_t+) value
//:  +n+::
//:    number or position of elements
//:
//: --
//: * +MUOS_CBUFFER_SIZE(cbuffer)+ returns the size
//: * +MUOS_CBUFFER_FREE(cbuffer)+ returns how many bytes are free
//: * +MUOS_CBUFFER_USED(cbuffer)+ returns how many bytes are used
//: * +MUOS_CBUFFER_PUSH(cbuffer, value)+ pushes a byte to the end
//: * +MUOS_CBUFFER_POP(cbuffer)+ pops and returns the first byte
//: * +MUOS_CBUFFER_RPOP(cbuffer)+ pops the last byte (no return)
//: * +MUOS_CBUFFER_POPN(cbuffer, n)+ pops 'n' bytes frome the begin (no return)
//: * +MUOS_CBUFFER_PEEK(cbuffer, n)+ returns the byte at position 'n'
//: * +MUOS_CBUFFER_POKE(cbuffer, n, value)+ changes the byte at position 'n' to 'value'
//: --
//:
#define MUOS_CBUFFER_SIZE(b)  MUOS_ARRAY_ELEMENTS((b).cbuffer)
#define MUOS_CBUFFER_FREE(b) MUOS_CBUFFER_SIZE(b) - (b).descriptor.len
#define MUOS_CBUFFER_USED(b) (b).descriptor.len
#define MUOS_CBUFFER_PUSH(b, v) muos_cbuffer_push (&(b).descriptor, MUOS_CBUFFER_SIZE(b), (v))
#define MUOS_CBUFFER_POP(b) muos_cbuffer_pop (&(b).descriptor, MUOS_CBUFFER_SIZE(b))
#define MUOS_CBUFFER_RPOP(b) --(b).descriptor.len
#define MUOS_CBUFFER_POPN(b, n) muos_cbuffer_popn (&(b).descriptor, MUOS_CBUFFER_SIZE(b), (n))
#define MUOS_CBUFFER_PEEK(b, pos) muos_cbuffer_peek (&(b).descriptor, MUOS_CBUFFER_SIZE(b), (pos))
#define MUOS_CBUFFER_POKE(b, pos, val) muos_cbuffer_poke (&(b).descriptor, MUOS_CBUFFER_SIZE(b), (pos), (val))


#endif
