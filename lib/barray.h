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
//: Defines arrays of up to 255bytes (2040 bits).
//: Sizes are rounded to the next byte boundary (multiplies of 8 bits).
//:
//: Simple arithmetic and bit operations on these arrays are supported
//: Optimized for size, not speed. Does not do bounds checking.
//:
//: Most functions are implemented as macros to properly derive sizes from arrays
//: and allow proper 'volatile' propagation. This documentation still gives
//: prototypes as if they where functions. Refer to the source for the underlying
//: functions.
//:
//: Arithmetic does not handle over/underflows. It can be easily handled by
//: making barrays big enough and use the highest bit as indicator.
//:


//lib_barray_api:
//: .Constructors
//: ----
//: MUOS_BARRAY (name, uint8_t bits)
//: MUOS_BARRAY_SIZE (uint8_t bits)
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

typedef uint8_t muos_barray[];

//: .BArray Initialization
//: ----
//: void muos_barray_clear (muos_barray dst)
//: void muos_barray_copy (muos_barray dst, const muos_barray arc)
//: ----
//:
//: +dst+::
//:   target barray
//: +src+::
//:   source barray
//:
//: 'muos_barray_clear()' clears the barray.
//:
//: 'muos_barray_copy()' copies the barray (truncated or filled by zeros).
//:
#define muos_barray_clear(dst) for (uint8_t i = 0; i < sizeof(dst); ++i) dst[i] = 0; }

#define muos_barray_copy(dst, src)              \
  for (uint8_t i = 0; i < sizeof(dst); ++i)     \
    {                                           \
      if (i < sizeof(src))                      \
        dst[i] = src[i];                        \
      else                                      \
        dst[i] = 0;                             \
    }


//lib_barray_api:
//: .Single Bit Operations
//: ----
//: bool muos_barray_getbit (const muos_barray src, uint8_t bit)
//: void muos_barray_setbit (muos_barray dst, uint8_t bit)
//: void muos_barray_clearbit (muos_barray dst, uint8_t bit)
//: void muos_barray_setbit (muos_barray dst, uint8_t bit)
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
#define muos_barray_setbit(dst, bit) dst[(bit)>>3] |= 1<<((bit)&7)
#define muos_barray_clearbit(dst, bit) dst[(bit)>>3] &= ~(1<<((bit)&7))
#define muos_barray_togglebit(dst, bit) dst[(bit)>>3] ^= 1<<((bit)&7)


#if 0 //PLANNED: not implemented yet
// lib_barray_api:
// : .Shift Operations
// : ----
// : void muos_barray_r8shift(muos_barray dst, uint8_t pos)
// : void muos_barray_l8shift(muos_barray dst, uint8_t pos)
// : void muos_barray_r1shift(muos_barray dst, uint8_t pos)
// : void muos_barray_l1shift(muos_barray dst, uint8_t pos)
// : void muos_barray_rshift(muos_barray dst, uint8_t pos)
// : void muos_barray_lshift(muos_barray dst, uint8_t pos)
// : ----
// :
// :
// :
// :
// :
// :
#define muos_barray_r8shift(dst, pos)
#define muos_barray_l8shift(dst, pos)
#define muos_barray_r1shift(dst, pos)
#define muos_barray_l1shift(dst, pos)
#define muos_barray_rshift(dst, pos)
#define muos_barray_lshift(dst, pos)

static inline void
muos_barray_r8shift_ (muos_barray dest, uint8_t len, uint8_t bytes)
{
}

static inline void
muos_barray_l8shift_ (muos_barray dest, uint8_t len, uint8_t bytes)
{
}

static inline void
muos_barray_r1shift_ (muos_barray dest, uint8_t len, uint8_t bits)
{
}

static inline void
muos_barray_l1shift_ (muos_barray dest, uint8_t len, uint8_t bits)
{
}
#endif



//lib_barray_api:
//: .Comparsions
//: ----
//: bool muos_barray_is_zero (const muos_barray a)
//: bool muos_barray_is_eq (const muos_barray a, const muos_barray b)
//: bool muos_barray_is_lt (const muos_barray a, const muos_barray b)
//: bool muos_barray_is_lte (const muos_barray a, const muos_barray b)
//: ----
//:
//: 'muos_barray_is_zero()' returns 'true' when the all bits in the
//: barray are set to 0 and 'false' when not.
//:
//: 'muos_barray_is_eq()'
//: 'muos_barray_is_lt()'
//: 'muos_barray_is_lte()'
//: unsigned arithmetic for 'a == b', 'a < b', 'a <= b', returns 'true' when
//: the condition is met and 'false' otherwise. 'a' and 'b' can have different
//: sizes, the excess bytes on the longer argument are considered zero.
//:
#define muos_barray_is_zero(a) muos_barray_is_zero_ (a, sizeof(a))
#define muos_barray_is_eq(a, b) muos_barray_is_eq_ (a, sizeof(a), b, sizeof(b))
#define muos_barray_is_lt(a, b) muos_barray_is_lt_ (a, sizeof(a), b, sizeof(b))
#define muos_barray_is_lte(a, b) muos_barray_is_lte_ (a, sizeof(a), b, sizeof(b))

static inline bool
muos_barray_is_zero_ (const muos_barray a, uint8_t len)
{
  for (uint8_t i = 0; i < len; ++i)
    if (a[i]) return false;

  return true;
}

static inline bool
muos_barray_is_eq_ (const muos_barray a, uint8_t alen, const muos_barray b, uint8_t blen)
{
  for (uint8_t i = 0; i < alen ; ++i)
    if (alen <= blen)
      {
        if (a[i] != b[i]) return false;
      }
    else
      {
        if (a[i]) return false;
      }

  return true;
}

static inline bool
muos_barray_is_lt_ (const muos_barray a, uint8_t alen, const muos_barray b, uint8_t blen)
{
  for (uint8_t i = alen>blen?alen:blen; i; --i)
    if (alen <= blen)
      {
        if (a[i-1] >= b[i-1]) return false;
      }
    else
      {
        if (a[i]) return false;
      }

  return true;
}

static inline bool
muos_barray_is_lte_ (const muos_barray a, uint8_t alen, const muos_barray b, uint8_t blen)
{
  for (uint8_t i = alen>blen?alen:blen; i; --i)
    if (alen <= blen)
      {
        if (a[i-1] > b[i-1]) return false;
      }
    else
      {
        if (a[i]) return false;
      }

  return true;
}


//lib_barray_api:
//: .Arithmetic With Standard Types
//: ----
//: void muos_barray_add_uint8 (muos_barray dst, uint8_t src, uint8_t l8shift)
//: void muos_barray_sub_uint8 (muos_barray dst, uint8_t src, uint8_t l8shift)
//: void muos_barray_add_uint16 (muos_barray dst, uint16_t src)
//: void muos_barray_sub_uint16 (muos_barray dst, uint16_t src)
//: void muos_barray_add_uint32 (muos_barray dst, uint32_t src)
//: void muos_barray_sub_uint32 (muos_barray dst, uint32_t src)
//: ----
//:
//: +dst+::
//:   Destination operand for the operation which gets mutated.
//: +src+::
//:   Second operand for the operation which stays unchanged
//: +l8shift+::
//:   left shift in bytes (8 bits) applied to the src operand
//:
//: 'muos_barray_add_uint*()' and 'muos_barray_sub_uint*()' add/substract an C standard type
//: values to an barray. For the '*uint8' variant the value can be left shifted in 8 bit
//: increments to extend the magnitude of the operation.
//:
#define muos_barray_add_uint8(dst, src, l8shift) muos_barray_add_uint8_ (dst, sizeof(dst), src, l8shift)
#define muos_barray_sub_uint8(dst, src, l8shift) muos_barray_sub_uint8_ (dst, sizeof(dst), src, l8shift)
#define muos_barray_add_uint16(dst, src) muos_barray_add_uint16_ (dst, sizeof(dst), src)
#define muos_barray_sub_uint16(dst, src) muos_barray_sub_uint16_ (dst, sizeof(dst), src)
#define muos_barray_add_uint32(dst, src) muos_barray_add_uint32_ (dst, sizeof(dst), src)
#define muos_barray_sub_uint32(dst, src) muos_barray_sub_uint32_ (dst, sizeof(dst), src)

static inline void
muos_barray_add_uint8_ (muos_barray dst, uint8_t len, uint8_t src, uint8_t l8shift)
{
  if (l8shift >= len)
    return;

  dst[l8shift] += src;

  if (dst[l8shift] < src)
    {
      for (l8shift++; l8shift <= len; ++l8shift)
        {
          if (++dst[l8shift])
            break;
        }
    }
}


static inline void
muos_barray_sub_uint8_ (muos_barray dst, uint8_t len, uint8_t src, uint8_t l8shift)
{
  if (l8shift >= len)
    return;

  uint8_t tmp = dst[l8shift];
  dst[l8shift] -= src;

  if (dst[l8shift] > tmp)
    {
      for (l8shift++; l8shift <= len; ++l8shift)
        {
          if (dst[l8shift]--)
            break;
        }
    }
}


static inline void
muos_barray_add_uint16_ (muos_barray dst, uint8_t len, uint16_t src)
{
  muos_barray_add_uint8_ (dst, len, src, 0);
  muos_barray_add_uint8_ (dst, len, src>>8, 1);
}

static inline void
muos_barray_sub_uint16_ (muos_barray dst, uint8_t len, uint16_t src)
{
  muos_barray_sub_uint8_ (dst, len, src, 0);
  muos_barray_sub_uint8_ (dst, len, src>>8, 1);
}


static inline void
muos_barray_add_uint32_ (muos_barray dst, uint8_t len, uint32_t src)
{
  muos_barray_add_uint8_ (dst, len, src, 0);
  muos_barray_add_uint8_ (dst, len, src>>=8, 1);
  muos_barray_add_uint8_ (dst, len, src>>=8, 2);
  muos_barray_add_uint8_ (dst, len, src>>=8, 3);
}


static inline void
muos_barray_sub_uint32_ (muos_barray dst, uint8_t len, uint32_t src)
{
  muos_barray_sub_uint8_ (dst, len, src, 0);
  muos_barray_sub_uint8_ (dst, len, src>>=8, 1);
  muos_barray_sub_uint8_ (dst, len, src>>=8, 2);
  muos_barray_sub_uint8_ (dst, len, src>>=8, 3);
}


//lib_barray_api:
//: .BArray Arithmetic
//: ----
//: void muos_barray_add (muos_barray dst, const muos_barray src)
//: void muos_barray_sub (muos_barray dst, const muos_barray src)
//: ----
//:
//: +dst+::
//:   Destination barray for the operation which gets mutated.
//: +src+::
//:   Second operand for the operation which stays unchanged
//:
//: 'muos_barray_add()' and 'muos_barray_sub()' are generic operations for adding/subtracting
//: 2 barrays.
//:
#define muos_barray_add(dst, src) muos_barray_add_ (dst, sizeof(dst), src, sizeof(src))
#define muos_barray_sub(dst, src) muos_barray_sub_ (dst, sizeof(dst), src, sizeof(src))


static inline void
muos_barray_add_ (muos_barray dst, uint8_t dlen, const muos_barray src, uint8_t slen)
{
  for (uint8_t i = 0; i <= slen; ++i)
    {
      if (src[i])
        muos_barray_add_uint8_ (dst, dlen, src[i], i);
    }
}



static inline void
muos_barray_sub_ (muos_barray dst, uint8_t dlen, const muos_barray src, uint8_t slen)
{
  for (uint8_t i = 0; i <= slen; ++i)
    {
      if (src[i])
        muos_barray_sub_uint8_ (dst, dlen, src[i], i);
    }
}



//: .BArray Type Conversion
//: ----
//: uint8_t muos_barray_uint8(src, r8shift)
//: uint16_t muos_barray_uint16(src, r8shift)
//: uint32_t muos_barray_uint32(src, r8shift)
//: ----
//:
//: +src+::
//:   Source barray
//: +r8shift+::
//:   right shift by bytes (8 bits) applied to the src operand
//:
//: Returns the barray integer value shifted and truncated to the destination C standard type.
//:
#define muos_barray_uint8(src, r8shift) muos_barray_uint8_ (src, sizeof(src), r8shift)
#define muos_barray_uint16(src, r8shift) muos_barray_uint16_ (src, sizeof(src), r8shift)
#define muos_barray_uint32(src, r8shift) muos_barray_uint32_ (src, sizeof(src), r8shift)

static inline uint8_t
muos_barray_uint8_ (muos_barray src, uint8_t len, uint8_t r8shift)
{
  if (len <= r8shift) return 0;

  return src[r8shift];
}

static inline uint16_t
muos_barray_uint16_ (muos_barray src, uint8_t len, uint8_t r8shift)
{
  union {
    uint32_t u16;
    uint32_t u8[2];
  } ret = { .u16 = 0 };

  if (len > 2+r8shift) len = 2+r8shift;

  switch (len-r8shift)
    {
    case 2:
      ret.u8[0] = src[1+r8shift];
    case 1:
      ret.u8[1] = src[0+r8shift];
    }

  return ret.u16;
}

static inline uint32_t
muos_barray_uint32_ (muos_barray src, uint8_t len, uint8_t r8shift)
{
  union {
    uint32_t u32;
    uint32_t u8[4];
  } ret = { .u32 = 0 };

  if (len > 4+r8shift) len = 4+r8shift;

  switch (len-r8shift)
    {
    case 4:
      ret.u8[0] = src[3+r8shift];
    case 3:
      ret.u8[1] = src[2+r8shift];
    case 2:
      ret.u8[2] = src[1+r8shift];
    case 1:
      ret.u8[3] = src[0+r8shift];
    }

  return ret.u32;
}



#if 0 //PLANNED: not implemented yet
// lib_barray_api:
// : .Logic Operations
// : ----
// : ----
// : and or xor not
// :
#endif

#endif
