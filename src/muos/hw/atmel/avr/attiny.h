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


#include <muos/pp.h>
#include <muos/hw/atmel/avr/avr.h>

// one hardware timer is used for the wall clock



#define ISRNAME_OVERFLOW_(hw, _) TIM##hw##_OVF_vect
#define ISRNAME_OVERFLOW(hw) ISRNAME_OVERFLOW_ hw

#define ISRNAME_COMPMATCH_(tmhw,cmhw) TIM##tmhw##_COMP##cmhw##_vect
#define ISRNAME_COMPMATCH(hw) ISRNAME_COMPMATCH_ hw

#define ISRNAME_CAPTURE_(hw) TIM##hw##_CAPT_vect
#define ISRNAME_CAPTURE(hw) ISRNAME_CAPTURE_ hw

#define MUOS_HW_CLOCK_REGISTER_(hw, ...) TCNT##hw
#define MUOS_HW_CLOCK_REGISTER(hw) MUOS_HW_CLOCK_REGISTER_ hw

#define MUOS_HW_CLOCK_PRESCALE_SET__(hw, prescale)  \
  TCCR##hw##B = MUOS_HW_CLOCK##hw##_DIV##prescale

#define MUOS_HW_CLOCK_PRESCALE_SET_(hw, prescale)   \
  MUOS_HW_CLOCK_PRESCALE_SET__(hw, prescale)

#define MUOS_HW_CLOCK_PRESCALE_SET(hw, prescale)                \
  MUOS_HW_CLOCK_PRESCALE_SET_(MUOS_PP_FIRST hw, prescale)



#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE(hw)   \
  MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE_ hw





#endif
