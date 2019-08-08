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

/*
  ISR names
*/

#define ISRNAME_OVERFLOW_(tm, cm, type, prescale) TIMER##tm##_OVF_vect
#define ISRNAME_OVERFLOW(hw) ISRNAME_OVERFLOW_ hw

#define ISRNAME_COMPMATCH_(tm, cm, type, prescale) TIMER##tm##_COMP##cm##_vect
#define ISRNAME_COMPMATCH(hw) ISRNAME_COMPMATCH_ hw

#define ISRNAME_EEPROM_READY EE_READY_vect

/*
  CLOCK defs
*/


#define MUOS_HW_CLOCK_TM_(tm, cm, type, prescale) tm
#define MUOS_HW_CLOCK_TM(hw) MUOS_HW_CLOCK_TM_ hw

#define MUOS_HW_CLOCK_CM_(tm, cm, type, prescale) cm
#define MUOS_HW_CLOCK_CM(hw) MUOS_HW_CLOCK_CM_ hw

#define MUOS_HW_CLOCK_TYPE_(tm, cm, type, prescale) type
#define MUOS_HW_CLOCK_TYPE(hw) MUOS_HW_CLOCK_TYPE_ hw


#define MUOS_HW_CLOCK_REGISTER_(tm, cm, type, prescale) TCNT##tm
#define MUOS_HW_CLOCK_REGISTER(hw) MUOS_HW_CLOCK_REGISTER_ hw

#define MUOS_HW_CLOCK_OVERFLOW_(tm, cm, type, prescale) (!!(TIFR##tm & _BV(TOV##tm)))
#define MUOS_HW_CLOCK_OVERFLOW(hw) MUOS_HW_CLOCK_OVERFLOW_ hw




#define MUOS_HW_CLOCK_PRESCALE_SET_(tm, cm, type, prescale)  \
  TCCR##tm##B = MUOS_HW_CLOCK##tm##_DIV##prescale

#define MUOS_HW_CLOCK_PRESCALE_SET(hw)                \
  MUOS_HW_CLOCK_PRESCALE_SET_ hw

#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE_(tm, cm, type, prescale) \
  TIMSK##tm |= _BV(TOIE##tm)

#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE(hw)   \
  MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE_ hw


#define MUOS_HW_CLOCK_ISR_COMPMATCH_REG_(tm, cm, type, prescale)       \
  OCR##tm##cm

#define MUOS_HW_CLOCK_ISR_COMPMATCH_REG(hw)              \
  MUOS_HW_CLOCK_ISR_COMPMATCH_REG_ hw


#define MUOS_HW_CLOCK_ISR_COMPMATCH_CLEAR_(tm, cm, type, prescale)    \
  TIFR##tm |= _BV(OCF##tm##cm)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_CLEAR(hw)                  \
  MUOS_HW_CLOCK_ISR_COMPMATCH_CLEAR_ hw


#define MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE_(tm, cm, type, prescale)    \
  TIMSK##tm |= _BV(OCIE##tm##cm)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE(hw)                  \
  MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE_ hw


#define MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE_(tm, cm, type, prescale)      \
  TIMSK##tm &= ~_BV(OCIE##tm##cm)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE(hw) \
  MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE_ hw

/*
  serial
*/

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
  }                                                     \
                                                        \
  static inline void                                    \
  muos_hw_serial_tx##hw##_stop (void)                   \
  {                                                     \
    UCSR##hw##B &= ~_BV(UDRIE##hw);                     \
  }

MUOS_SERIAL_HW
#undef UART

muos_error
muos_hw_serial_start (uint8_t hw, uint32_t baud, const char config[3], int rxsync);



#endif
