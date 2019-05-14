/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2019                            Christian Thäter <ct@pipapo.org>
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

#ifndef MUOS_LIB_BARRAY_H
#define MUOS_LIB_BARRAY_H

#include <stdint.h>
#include <stdbool.h>

//PLANNED: how about signed math?

//lib_barray_api:
//: Bit Array Library
//: -----------------
//:
//: Defines arrays of up to 2040 bits (255bytes).
//:
//: Simple arithmetic and bit operations on these arrays are supported
//: Optimized for size, not speed.
//: Does not do bounds checking.
//:
//: Most functions are implemented as macros to properly derive sizes from arrays
//: and allow proper 'volatile' propagation.
//:


//lib_barray_api:
//: .Constructors/Macros
//: ----
//: MUOS_BARRAY(name, bits)
//: MUOS_BARRAY_SIZE(bits)
//: ----
//:
//: +name+::
//:   C identifier for the array.
//: +bits+::
//:   number of bits to store
//:
//: 'MUOS_BARRAY()' declares/defines an bit array to 'name' holding up 'bits'.
//: declarations/definitinons can be qualified with storage specifiers:
//:  extern volatile MUOS_BARRAY(muos_errors_, muos_errors_end);
//:
//: 'MUOS_BARRAY_SIZE()' returns the size in bytes required to hold 'bits'.
//:
#define MUOS_BARRAY_SIZE(bits) (((bits)+7)/8)
#define MUOS_BARRAY(name, bits) uint8_t name[MUOS_BARRAY_SIZE(bits)]

//: .Whole Array Operations
//: ----
//: muos_barray_clear(dst)
//: ----
//:
//: +dst+::
//:   barray to clear
//:
//: 'muos_barray_clear()' clears the entire array.
//:
#define muos_barray_clear(dst) do {for (uint8_t i = 0; i < sizeof(dst); ++i) dst[i] = 0; } while (0)


//lib_barray_api:
//: .Single Bit Operations
//: ----
//: muos_barray_getbit(src, bit)
//: muos_barray_setbit(dst, bit)
//: muos_barray_clearbit(dst, bit)
//: muos_barray_setbit(dst, bit)
//: ----
//:
//: 'muos_barray_getbit()'
//: returns +true+ or +false+ depending on the value of 'bit' in src.
//:
//: 'muos_barray_setbit()'
//: 'muos_barray_clearbit()'
//: 'muos_barray_toggle()'
//: set, clear or toggle the 'bit' in 'dst'.
//:
#define muos_barray_getbit(src, bit) !!(((src)[(bit)>>3] & 1<<((bit)&7)))
#define muos_barray_setbit(dst, bit) (dst)[(bit)>>3] |= 1<<((bit)&7)
#define muos_barray_clearbit(dst, bit) (dst)[(bit)>>3] &= ~(1<<((bit)&7))
#define muos_barray_togglebit(dst, bit) (dst)[(bit)>>3] ^= 1<<((bit)&7)




//TODO: implementme below

/*
 shifting by bytes
*/

//TODO: shift with copy

#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//: dest >>= bytes*8
static inline bool
muos_barray_r8shift (uint8_t* dest, uint8_t len, uint8_t bytes)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//: dest <<= bytes*8
static inline bool
muos_barray_l8shift (uint8_t* dest, uint8_t len, uint8_t bytes)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//: dest >>= bits (for bits < 8)
static inline bool
muos_barray_rshift (uint8_t* dest, uint8_t len, uint8_t bits)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//: dest <<= bits  (for bits < 8)
static inline bool
muos_barray_lshift (uint8_t* dest, uint8_t len, uint8_t bits)
{
}
#endif




















//lib_barray_api:
//: Arithmetic
//: ----------
//:
//:
//:






#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//:  dest += src
static inline bool
muos_barray_add(uint8_t* dest, uint8_t destlen, uint8_t* src, uint8_t srclen)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//:  dest += src<<(pos*8)
static inline bool
muos_barray_add_uint8 (uint8_t* dest, uint8_t len, uint8_t src, uint8_t pos)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//:  dest -= src
static inline bool
muos_barray_sub (uint8_t* dest, uint8_t destlen, uint8_t* src, uint8_t srclen)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//:  dest -= src<<(pos*8)
static inline bool
muos_barray_sub_uint8 (uint8_t* dest, uint8_t len, uint8_t src, uint8_t pos)
{
}
#endif


//lib_barray_api:
//: Comparsions
//: -----------
//:
//:
//:


#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//: a == 0
static inline bool
muos_barray_zero (uint8_t* a, uint8_t len)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//: a == b
static inline bool
muos_barray_eq (uint8_t* a, uint8_t* b, uint8_t len)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//: a < b
static inline bool
muos_barray_lt (uint8_t* a, uint8_t* b, uint8_t len)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_barray_api:
//:
//: a <= b
static inline bool
muos_barray_lte (uint8_t* a, uint8_t* b, uint8_t len)
{
}
#endif


#endif
