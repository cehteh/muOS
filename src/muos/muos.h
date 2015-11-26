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
#include <muos/hwdef.h>

#ifndef MUOS_ATOMIC
#error MUOS_ATOMIC lacks hardware definiton
#endif

#ifndef MUOS_NONATOMIC
#error MUOS_NONATOMIC lacks hardware definiton
#endif

#ifndef MUOS_ATOMIC_RESTORE
#error MUOS_ATOMIC_RESTORE lacks hardware definiton
#endif

#ifndef MUOS_NONATOMIC_RESTORE
#error MUOS_NONATOMIC_RESTORE lacks hardware definiton
#endif

#ifndef MUOS_ATOMIC_FORCE
#error MUOS_ATOMIC_FORCE lacks hardware definiton
#endif

#ifndef MUOS_NONATOMIC_FORCE
#error MUOS_NONATOMIC_FORCE lacks hardware definiton
#endif


#define MUOS_TYPEDEF(basetype, size, name) MUOS_TYPEDEF_(basetype, size, name)
#define MUOS_TYPEDEF_(basetype, size, name) typedef basetype##size##_t name

#define MUOS_ARRAY_ELEMENTS(array) (sizeof(array)/sizeof(*(array)))


#define MUOS_NOINIT __attribute__ ((section (".noinit")))

extern uint8_t muos_overflow_count;
#define MUOS_OVERFLOW do { if(muos_overflow_count<255) {++muos_overflow_count;}} while(0)

//#define MUOS_INIT

//#define MUOS_FALIGN __attribute__ ((aligned (4)))


//#define MUOS_RUN_QUEUES
#define MUOS_SLEEP


#endif
