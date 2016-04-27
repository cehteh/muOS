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


#ifndef MUOS_UTF8_H
#define MUOS_UTF8_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

//lib_utf8_api:
//: .Character Tests
//: ----
//: bool muos_utf8ascii (const char c)
//: bool muos_utf8start (const char c)
//: bool muos_utf8cont (const char c)
//: ----
//:
//: +c+::
//:   character to check
//:
//: .Returns true
//: * +muos_utf8ascii (c)+ when 'c' is a latin1 character
//: * +muos_utf8start (c)+ when 'c' is the begin of a multibyte sequence
//: * +muos_utf8cont (c)+ when 'c' is the a continuation of a multibyte sequence
//:
static inline bool
muos_utf8start (const char c)
{
  return ((c & 192) == 192);
}

static inline bool
muos_utf8cont (const char c)
{
  return ((c & 192) == 128);
}

static inline bool
muos_utf8ascii (const char c)
{
  return !(c & 128);
}


//lib_utf8_api:
//: .String length
//: ----
//: size_t muos_utf8len (const char* str)
//: ----
//:
//: +str+::
//:   zero terminated c-string
//:
//: 'str' must not be a continuation byte
//:
//: Returns the length of a utf-8 encoded string in characters.
//:
size_t
muos_utf8len (const char* str);


//lib_utf8_api:
//: .Character size
//: ----
//: uint8_t muos_utf8size (const char* char)
//: ----
//:
//: +char+::
//:   character to analyze
//:
//: 'char' can point into the middle or to the end of a multibyte sequence, but
//: the sequence must have a proper start byte.
//:
//: Returns the size in bytes of the given multibyte character sequence.
//:
uint8_t
muos_utf8size (const char* start);



#endif
