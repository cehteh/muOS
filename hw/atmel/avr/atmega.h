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

#include <muos/error.h>

#define ISRNAME_OVERFLOW_(hw, _) TIMER##hw##_OVF_vect
#define ISRNAME_OVERFLOW(hw) ISRNAME_OVERFLOW_ hw

#define ISRNAME_COMPMATCH_(tmhw,cmhw) TIMER##tmhw##_COMP##cmhw##_vect
#define ISRNAME_COMPMATCH(hw) ISRNAME_COMPMATCH_ hw

#define ISRNAME_EEPROM_READY EE_READY_vect

#define MUOS_HW_CLOCK_REGISTER_(hw, _) TCNT##hw
#define MUOS_HW_CLOCK_REGISTER(hw) MUOS_HW_CLOCK_REGISTER_ hw

#define MUOS_HW_CLOCK_OVERFLOW_(hw, _) (!!(TIFR##hw & _BV(TOV##hw)))
#define MUOS_HW_CLOCK_OVERFLOW(hw) MUOS_HW_CLOCK_OVERFLOW_ hw


#define MUOS_HW_CLOCK_PRESCALE_SET__(hw, prescale)  \
  TCCR##hw##B = MUOS_HW_CLOCK##hw##_DIV##prescale

#define MUOS_HW_CLOCK_PRESCALE_SET_(hw, prescale)   \
  MUOS_HW_CLOCK_PRESCALE_SET__(hw, prescale)

#define MUOS_HW_CLOCK_PRESCALE_SET(hw, prescale)                \
  MUOS_HW_CLOCK_PRESCALE_SET_(MUOS_PP_FIRST hw, prescale)


#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE__(hw) \
  TIMSK##hw |= _BV(TOIE##hw)

#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE_(hw, _)       \
  MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE__(hw)

#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE(hw)   \
  MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE_ hw


#define MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE__(tmhw, cmhw, at)   \
  OCR##tmhw##cmhw = at;                                        \
  TIMSK##tmhw |= _BV(OCIE##tmhw##cmhw)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE_(hw, at)     \
  MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE__(hw, at)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE(hw, at)              \
  MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE_(MUOS_PP_BOTH hw, at)





#define MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE__(tmhw, cmhw)       \
  TIMSK##tmhw &= ~_BV(OCIE##tmhw##cmhw)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE_(tmhw, cmhw)        \
  MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE__(tmhw, cmhw)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE(hw) \
  MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE_ hw


#define UART(hw, txsize, rxsize)                        \
  static inline void                                    \
  muos_hw_tx##hw##_enable (void)                        \
  {                                                     \
    UCSR##hw##B |= _BV(TXEN##hw);                       \
  }                                                     \
                                                        \
  static inline void                                    \
  muos_hw_rx##hw##_enable (void)                        \
  {                                                     \
    UCSR##hw##B |= _BV(RXEN##hw) | _BV(RXCIE##hw);      \
  }                                                     \
                                                        \
  static inline void                                    \
  muos_hw_serial_rx##hw##_run (void)                    \
  {                                                     \
    UCSR##hw##B |= _BV(RXCIE##hw);                      \
  }                                                     \
                                                        \
  static inline void                                    \
  muos_hw_serial_rx##hw##_stop (void)                   \
  {                                                     \
    UCSR##hw##B &= ~_BV(RXCIE##hw);                     \
  }                                                     \
                                                        \
  static inline void                                    \
  muos_hw_serial_tx##hw##_run (void)                    \
  {                                                     \
    UCSR##hw##B |= _BV(UDRIE##hw);                      \
}                                                       \
                                                        \
  static inline void                                    \
  muos_hw_serial_tx##hw##_stop (void)                   \
  {                                                     \
    UCSR##hw##B &= ~_BV(UDRIE##hw);                     \
  }

MUOS_SERIAL_HW
#undef UART

muos_error
muos_hw_serial_start (uint8_t hw, uint32_t baud, char config[3], int rxsync);



#endif
