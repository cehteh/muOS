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

#ifndef UINT8A_H
#define UINT8A_H

#include <stdint.h>
#include <stdbool.h>


//PLANNED: how about signed math?
//PLANNED: implementme

//lib_uint8a_api:
//: Bit Array Library
//: -----------------
//:
//: Allows simple arithmetic and bit operations on array of up to 255 bytes.
//: Optimized for size, not speed.
//:
//:
//:

//lib_uint8a_api:
//: Arithmetic
//: ----------
//:
//:
//:






#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//:  dest += src
static inline bool
muos_uint8a_add(uint8_t* dest, uint8_t destlen, uint8_t* src, uint8_t srclen)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//:  dest += src<<(pos*8)
static inline bool
muos_uint8a_add_uint8 (uint8_t* dest, uint8_t len, uint8_t src, uint8_t pos)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//:  dest -= src
static inline bool
muos_uint8a_sub (uint8_t* dest, uint8_t destlen, uint8_t* src, uint8_t srclen)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//:  dest -= src<<(pos*8)
static inline bool
muos_uint8a_sub_uint8 (uint8_t* dest, uint8_t len, uint8_t src, uint8_t pos)
{
}
#endif


//lib_uint8a_api:
//: Comparsions
//: -----------
//:
//:
//:


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: a == 0
static inline bool
muos_uint8a_zero (uint8_t* a, uint8_t len)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: a == b
static inline bool
muos_uint8a_eq (uint8_t* a, uint8_t* b, uint8_t len)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: a < b
static inline bool
muos_uint8a_lt (uint8_t* a, uint8_t* b, uint8_t len)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: a <= b
static inline bool
muos_uint8a_lte (uint8_t* a, uint8_t* b, uint8_t len)
{
}
#endif


//lib_uint8a_api:
//: Bitops
//: ------
//:
//:
//:



/*
 shifting by bytes
*/

//TODO: shift with copy

#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: dest >>= bytes*8
static inline bool
muos_uint8a_r8shift (uint8_t* dest, uint8_t len, uint8_t bytes)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: dest <<= bytes*8
static inline bool
muos_uint8a_l8shift (uint8_t* dest, uint8_t len, uint8_t bytes)
{
}
#endif


if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: dest >>= bits (for bits < 8)
static inline bool
muos_uint8a_rshift (uint8_t* dest, uint8_t len, uint8_t bits)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: dest <<= bits  (for bits < 8)
static inline bool
muos_uint8a_lshift (uint8_t* dest, uint8_t len, uint8_t bits)
{
}
#endif


/*
  single bit manipulations
*/

#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: (dest & 1<<bit) == true
static inline bool
muos_uint8a_getbit (uint8_t* src, uint8_t len, uint16_t bit)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: dest |= 1<<bit
static inline void
muos_uint8a_setbit (uint8_t* src, uint8_t len, uint16_t bit)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: dest &= ~1<<bit
static inline void
muos_uint8a_clearbit (uint8_t* src, uint8_t len, uint16_t bit)
{
}
#endif


#if 0 //PLANNED: implementme
//lib_uint8a_api:
//:
//: dest &= 1<<bit
static inline void
muos_uint8a_togglebit (uint8_t* src, uint8_t len, uint16_t bit)
{
}
#endif

#endif
