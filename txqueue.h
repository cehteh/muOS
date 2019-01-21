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

#include <muos/io.h>
#include <muos/lib/cbuffer.h>

#include <stdint.h>

#define SERIAL(hw, txqsize)                     \
  MUOS_CBUFFERDEC(muos_txqueue##hw, txqsize);
MUOS_IO_TXQUEUE_SIZE
#undef SERIAL


#define MUOS_TXQUEUE_TAGS                         \
  TAG(NL)                                         \
  TAG(UINT8)                                      \
  TAG(UINT16)                                     \
  TAG(UINT32)                                     \
  TAG(BASE2)                                      \
  TAG(BASE8)                                      \
  TAG(BASE10)                                     \
  TAG(BASE16)                                     \
  TAG(BASEN)                                      \
  TAG(UPCASE)                                     \
  TAG(DOWNCASE)                                   \
  TAG(PBASE)                                      \
  TAG(PUPCASE)                                    \
  TAG(CSI)                                        \
  TAG(FSTR)                                       \


enum muos_txqueue_tags
{
  MUOS_TXTAG_NCHARS = 128,
  MUOS_TXTAG_NCHARS_END = 191,

#define TAG(name)  MUOS_TXTAG_##name,
  MUOS_TXQUEUE_TAGS
#undef TAG
};

muos_cbuffer_index
muos_txqueue_free MUOS_IO_HWPARAM();

void
muos_txqueue_push MUOS_IO_HWPARAM(const uint8_t value);

uint8_t
muos_txqueue_pop MUOS_IO_HWPARAM();

#endif
