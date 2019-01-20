/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2015, 2019                     Christian Thäter <ct@pipapo.org>
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

//lib_cbuffer_api:
//: .Cbuffer types
//: ----
//: typedef MUOS_CBUFFER_INDEX muos_cbuffer_index;
//:
//: struct muos_cbuffer {...}
//: ----
//:
//: All indexing is done with a configureable datatype which shall be
//: of unsigned type. When buffers are always smaller than 256 bytes
//: then uint8_t will save a few bytes RAM.
//:
//: The buffer datatype itself should be treated opaque and only be defined
//: with the macros below and accessed by API calls.
typedef MUOS_CBUFFER_INDEX muos_cbuffer_index;

struct muos_cbuffer
{
  const muos_cbuffer_index size;
  muos_cbuffer_index       start;
  muos_cbuffer_index       len;
  uint8_t                  cbuffer[];
};


//lib_cbuffer_api:
//: .Cbuffer definition
//: ----
//: MUOS_CBUFFERDEC(name, size)
//: MUOS_CBUFFERDEF(name, size)
//: ----
//:
//: +name+::
//:   name for the buffer
//:
//: +size+::
//:   number of elements
//:
//: Macro declaring and defining the type of a cbuffer for the given size.
//:
//: The resulting cbuffer will be named by the given name with '_muos_cbuffer'
//: appended.
//:
#define MUOS_CBUFFERDEC(name, sz)               \
  extern struct name##_muos_cbuffer             \
  {                                             \
    struct muos_cbuffer descriptor;             \
    uint8_t       cbuffer[sz];                  \
  } name

#define MUOS_CBUFFERDEF(name, sz)               \
  struct name##_muos_cbuffer name =             \
    {.descriptor = {.size = sz}}


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


//lib_cbuffer_api:
//: .Cbuffer API
//: ----
//: muos_cbuffer_index
//: muos_cbuffer_free (struct muos_cbuffer* cbuffer)
//:
//: static inline muos_cbuffer_index
//: muos_cbuffer_used (struct muos_cbuffer* cbuffer)
//:
//: muos_cbuffer_index
//: muos_cbuffer_size (struct muos_cbuffer* cbuffer)
//: ----
//:
//: +cbuffer+::
//:   the cbuffer as defined with +MUOS_CBUFFERDEF()+
//:
//: 'muos_cbuffer_free()'::
//:   returns the number of elements free in the buffer.
//: 'muos_cbuffer_used()'::
//:   returns the number of elements used on the buffer.
//: 'muos_cbuffer_size()'::
//:   returns the capacity of the buffer.
//:
//:
//: ----
//: void
//: muos_cbuffer_push (struct muos_cbuffer* cbuffer, const uint8_t value)
//:
//: uint8_t
//: muos_cbuffer_pop (struct muos_cbuffer* cbuffer)
//:
//: void
//: muos_cbuffer_popn (struct muos_cbuffer* cbuffer, muos_cbuffer_index n)
//:
//: void
//: muos_cbuffer_rpop (struct muos_cbuffer* cbuffer)
//: ----
//:
//: +value+::
//:    byte (+uint8_t+) value
//:  +n+::
//:    number of elements
//:
//: 'muos_cbuffer_push()'::
//:    Pushes a single value onto the buffer.
//: 'muos_cbuffer_pop (struct muos_cbuffer* cbuffer)'::
//:    Pops and returns the first element from the buffer.
//: 'muos_cbuffer_popn()'::
//:    Removes the n first elements from the buffer, no return.
//: 'muos_cbuffer_rpop()'::
//:    Removes the last element from the buffer, no return.
//:
//:
//: ----
//: uint8_t
//: muos_cbuffer_peek (struct muos_cbuffer* cbuffer, muos_cbuffer_index index)
//:
//: void
//: muos_cbuffer_poke (struct muos_cbuffer* cbuffer, muos_cbuffer_index index, const uint8_t value)
//: ----
//:
//:  +index+::
//:    position of element
//:
//:  These functions inspect or mutate an element in the buffer.
void
muos_cbuffer_push (struct muos_cbuffer* cbuffer, const uint8_t value);

uint8_t
muos_cbuffer_pop (struct muos_cbuffer* cbuffer);

void
muos_cbuffer_popn (struct muos_cbuffer* cbuffer, muos_cbuffer_index n);

uint8_t
muos_cbuffer_peek (struct muos_cbuffer* cbuffer, muos_cbuffer_index index);

void
muos_cbuffer_poke (struct muos_cbuffer* cbuffer, muos_cbuffer_index index, const uint8_t value);


static inline muos_cbuffer_index
muos_cbuffer_size (struct muos_cbuffer* cbuffer)
{
  return cbuffer->size;
}

static inline muos_cbuffer_index
muos_cbuffer_free (struct muos_cbuffer* cbuffer)
{
  return cbuffer->size - cbuffer->len;
}

static inline muos_cbuffer_index
muos_cbuffer_used (struct muos_cbuffer* cbuffer)
{
  return cbuffer->len;
}

static inline void
muos_cbuffer_rpop (struct muos_cbuffer* cbuffer)
{
  --cbuffer->len;
}



#endif
