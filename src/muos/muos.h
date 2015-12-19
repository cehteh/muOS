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

#ifndef MUOS_H
#define MUOS_H

#include <stdint.h>

#define MUOS_EVAL(v) v
#define MUOS_CONCAT_(a,b) a##b
#define MUOS_CONCAT2(a,b) MUOS_CONCAT_(a,b)
#define MUOS_CONCAT3(a,b,c) MUOS_CONCAT2(a,MUOS_CONCAT_(b,c))
#define MUOS_CONCAT4(a,b,c,d) MUOS_CONCAT2(MUOS_CONCAT_(a,b),MUOS_CONCAT_(b,c))
#define MUOS_CONCAT5(a,b,c,d,e) MUOS_CONCAT3(MUOS_CONCAT_(a,b),MUOS_CONCAT_(c,d),e)


#include <muos/hwdef.h>

#define MUOS_ARRAY_ELEMENTS(array) (sizeof(array)/sizeof(*(array)))


#define MUOS_NOINIT __attribute__ ((section (".noinit")))
//#define MUOS_EXPLICIT_INIT MUOS_NOINIT


#endif
