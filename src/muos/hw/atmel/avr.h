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

#ifndef MUOS_HW_ATMEL_AVR_H
#define MUOS_HW_ATMEL_AVR_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define MUOS_ATOMIC ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#define MUOS_NONATOMIC NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)

static __inline__ uint8_t
__iOuter(const  uint8_t *__s)
{
    SREG = *__s;
    __asm__ volatile ("" ::: "memory");
    return 1;
}

#define MUOS_ATOMIC_RESTORE for ( uint8_t __restore __attribute__((__cleanup__(__iCliParam))) = 0, __ToDo = __iOuter(&sreg_save); \
                                 __ToDo ;  __ToDo = 0 )

#define MUOS_NONATOMIC_RESTORE for ( uint8_t __restore __attribute__((__cleanup__(__iSeiParam))) = 0, __ToDo = __iOuter(&sreg_save); \
                                 __ToDo ;  __ToDo = 0 )


#define MUOS_ATOMIC_FORCE ATOMIC_BLOCK(ATOMIC_FORCEON)
#define MUOS_NONATOMIC_FORCE NONATOMIC_BLOCK(NONATOMIC_FORCEOFF)



#endif
