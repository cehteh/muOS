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

#ifndef MUOS_LINEEDIT_H
#define MUOS_LINEEDIT_H

#include <muos/io.h>

#include <stdbool.h>

//PLANNED: strategy on input errors, desync?
//PLANNED: full line overflow, because lineedit buffer > txqueue, needs some strategy (blocking_io/atomic_io)
//PLANNED: ^^ -DMUOS_LINEDIT_BLOCKING
//FIXME: per queue sizes
//#if defined(MUOS_IO_TXQUEUE_SIZE) && MUOS_LINEEDIT_BUFFER >= MUOS_IO_TXQUEUE
//#error MUOS_IO_TXQUEUE must be bigger than MUOS_LINEEDIT_BUFFER
//#endif

//PLANNED: docme Serial init, clear line "\x1b\x5b\x31\x7e\x0b\x0d"

void
muos_lineedit_echo MUOS_IO_HWPARAM(bool echo);

bool
muos_lineedit MUOS_IO_HWPARAM();


#endif
