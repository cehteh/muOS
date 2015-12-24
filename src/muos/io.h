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

#ifndef MUOS_IO_H
#define MUOS_IO_H

#include <stdint.h>
#include <stdbool.h>

#define MUOS_OUTPUTFNS                           \
  OUTPUTFN(char, char)                           \
  OUTPUTFN(cstr, const char*)                    \
  OUTPUTFN(mem, const uint8_t*, uint8_t)         \
  OUTPUTFN(intptr, intptr_t)                     \
  OUTPUTFN(uintptr, uintptr_t)                   \
  OUTPUTFN(int8, int8_t)                         \
  OUTPUTFN(uint8, uint8_t)                       \
  OUTPUTFN(int16, uint16_t)                      \
  OUTPUTFN(uint16, uint16_t)                     \
  OUTPUTFN(int32, int32_t)                       \
  OUTPUTFN(uint32, uint32_t)                     \
  OUTPUTFN(int64, int64_t)                       \
  OUTPUTFN(uint64, uint64_t)                     \
  OUTPUTFN(float, float)                         \
  OUTPUTFN(cstr_R)                               \
  OUTPUTFN(mem_R)                                \
  OUTPUTFN(int32_R)                              \
  OUTPUTFN(uint32_R)                             \
  OUTPUTFN(int64_R)                              \
  OUTPUTFN(uint64_R)                             \
  OUTPUTFN(float_R)                              \
  OUTPUTFN(upcase, bool)                         \
  OUTPUTFN(base, uint8_t)                        \
  OUTPUTFN(ifmt, uint8_t, uint8_t)               \
  OUTPUTFN(ffmt, char)                           \
  OUTPUTFN(pupcase, bool)                        \
  OUTPUTFN(pbase, uint8_t)                       \
  OUTPUTFN(pifmt, uint8_t, uint8_t)              \
  OUTPUTFN(pffmt, char)                          \
  OUTPUTFN(ctrl, uint8_t, uint8_t, uint8_t)


/*
ctrl
 style:
  normal
  bold
  italic
  underline

 fgcolor
  Black Red Green Yellow Blue Magenta Cyan White
 bgcolor
  Black Red Green Yellow Blue Magenta Cyan White
*/


#define OUTPUTFN(name, ...) void muos_output_##name (__VA_ARGS__);
MUOS_OUTPUTFNS
#undef OUTPUTFN


//PLANNED: printf compatible sequence parser
//PLANNED: simpler serquencer MUOS_OUTPUT(cstr("foo"),...)
//#define MUOS_OUTPUT(...) MUOS_OUTPUT_(__VA_ARGS__, END)


#endif
