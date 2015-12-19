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

#ifndef MUOS_HWDEF_H
#define MUOS_HWDEF_H

#ifdef __AVR_ATmega328P__
#include <muos/hw/atmel/atmega328p.h>
#endif

#ifdef __AVR_ATtiny85__
#include <muos/hw/atmel/attiny85.h>
#endif

#ifdef MUOS_LINUX
#include <muos/hw/linux/linux.h>
#endif

#endif
