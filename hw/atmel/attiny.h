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

#ifndef MUOS_HW_ATMEL_ATMEGA_H
#define MUOS_HW_ATMEL_ATMEGA_H

#include <muos/hw/atmel/avr.h>

// one hardware timer is used for the wall clock

#define MUOS_HW_CLOCK_OFF      0
#define MUOS_HW_CLOCK_DIV0     _BV(CS00)
#define MUOS_HW_CLOCK_DIV8     _BV(CS01)
#define MUOS_HW_CLOCK_DIV64    _BV(CS01) | _BV(CS00)
#define MUOS_HW_CLOCK_DIV256   _BV(CS02)
#define MUOS_HW_CLOCK_DIV1024  _BV(CS02) | _BV(CS00)

#define ISRNAME_OVERFLOW_(hw) TIM##hw##_OVF_vect
#define ISRNAME_OVERFLOW(hw) ISRNAME_OVERFLOW_(hw)

#define ISRNAME_COMPMATCH_(tmhw,cmhw) TIM##tmhw##_COMP##cmhw##_vect
#define ISRNAME_COMPMATCH(tmhw,cmhw) ISRNAME_COMPMATCH_(tmhw,cmhw)

#define MUOS_HW_CLOCK_REGISTER_(hw) TCNT##hw
#define MUOS_HW_CLOCK_REGISTER(hw) MUOS_HW_CLOCK_REGISTER_(hw)


#define MUOS_HW_CLOCK_PRESCALE_SET_(hw, prescale)  \
  TCCR##hw##B = MUOS_HW_CLOCK_DIV##prescale

#define MUOS_HW_CLOCK_PRESCALE_SET(hw, prescale)   \
  MUOS_HW_CLOCK_PRESCALE_SET_(hw, prescale)


#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE_(hw)  \
  TIMSK |= _BV(TOIE##hw)

#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE(hw)   \
  MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE_(hw)


#define MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE_(tmhw, cmhw, at)     \
  OCR##tmhw##cmhw = at;                                        \
  TIMSK |= _BV(OCIE##tmhw##cmhw)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE(tmhw, cmhw, at)   \
  MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE_(tmhw, cmhw, at)


#define MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE_(tmhw, cmhw)       \
  TIMSK &= ~_BV(OCIE##tmhw##cmhw)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE(tmhw, cmhw)       \
  MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE_(tmhw, cmhw)



#endif
