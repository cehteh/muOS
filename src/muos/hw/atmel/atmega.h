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


#define MUOS_HW_CLOCK_REGISTER_(hw) TCNT##hw
#define MUOS_HW_CLOCK_REGISTER(hw) MUOS_HW_CLOCK_REGISTER_(hw)


#define MUOS_HW_CLOCK_PRESCALE_SET_(hw, prescale)  \
  TCCR##hw##B = MUOS_HW_CLOCK_DIV##prescale

#define MUOS_HW_CLOCK_PRESCALE_SET(hw, prescale)   \
  MUOS_HW_CLOCK_PRESCALE_SET_(hw, prescale)


#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE_(hw)  \
  TIMSK##hw |= _BV(TOIE##hw)

#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE(hw)   \
  MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE_(hw)


#define MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE_(tmhw, cmhw, at)     \
  OCR##tmhw##cmhw = at;                                        \
  TIMSK##tmhw |= _BV(OCIE##tmhw##cmhw)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE(tmhw, cmhw, at)   \
  MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE_(tmhw, cmhw, at)


#define MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE_(tmhw, cmhw)       \
  TIMSK##tmhw &= ~_BV(OCIE##tmhw##cmhw)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE(tmhw, cmhw)       \
  MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE_(tmhw, cmhw)


/*
  Serial
*/


#define MUOS_HW_SERIAL_TX_REGISTER_(hw) UDR##hw

#define MUOS_HW_SERIAL_TX_REGISTER(hw)                  \
  MUOS_HW_SERIAL_TX_REGISTER_(hw)


#define MUOS_HW_SERIAL_RX_REGISTER_(hw) UDR##hw

#define MUOS_HW_SERIAL_RX_REGISTER(hw)                  \
  MUOS_HW_SERIAL_RX_REGISTER_(hw)


void
muos_hw_serial_init (void);


static inline void
muos_hw_tx_enable (void)
{
  UCSR0B |= _BV(TXEN0);
}

static inline void
muos_hw_rx_enable (void)
{
  UCSR0B |= _BV(RXEN0) | _BV(RXCIE0);
}

static inline void
muos_hw_serial_rx_run (void)
{
  UCSR0B |= _BV(RXCIE0);
}

static inline void
muos_hw_serial_rx_stop (void)
{
  UCSR0B &= ~_BV(RXCIE0);
}

static inline void
muos_hw_serial_tx_run (void)
{
  UCSR0B |= _BV(UDRIE0);
}

static inline void
muos_hw_serial_tx_stop (void)
{
  UCSR0B &= ~_BV(UDRIE0);
}


#endif
