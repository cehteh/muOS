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

#ifndef MUOS_HW_ATMEL_ATMEGA328P_H
#define MUOS_HW_ATMEL_ATMEGA328P_H

#include <muos/hw/atmel/avr/atmega.h>



// the Following missing in the headers, fix here when eventually added
#define OC0A_DDR   DDRD
#define OC0A_PORT  PORTD
#define OC0A_PIN   PIND
#define OC0A_BIT   6

#define OC0B_DDR   DDRD
#define OC0B_PORT  PORTD
#define OC0B_PIN   PIND
#define OC0B_BIT   5

#define OC1A_DDR   DDRB
#define OC1A_PORT  PORTB
#define OC1A_PIN   PINB
#define OC1A_BIT   1

#define OC1B_DDR   DDRB
#define OC1B_PORT  PORTB
#define OC1B_PIN   PINB
#define OC1B_BIT   2

#define OC2A_DDR   DDRB
#define OC2A_PORT  PORTB
#define OC2A_PIN   PINB
#define OC2A_BIT   3

#define OC2B_DDR   DDRD
#define OC2B_PORT  PORTD
#define OC2B_PIN   PIND
#define OC12_BIT   3


// values guessed from datasheet
// Figure 29-176. Calibrated 8 MHz RC Oscillator Frequency vs. OSCCAL Value
// doesn't need to be exact
#define MUOS_HW_ATMEL_OSCAL_HIGHSWITCH 202
#define MUOS_HW_ATMEL_OSCAL_LOWSWITCH 76

#define MUOS_HW_CLOCK0_OFF      0
#define MUOS_HW_CLOCK0_DIV1     _BV(CS00)
#define MUOS_HW_CLOCK0_DIV8     _BV(CS01)
#define MUOS_HW_CLOCK0_DIV64    _BV(CS01) | _BV(CS00)
#define MUOS_HW_CLOCK0_DIV256   _BV(CS02)
#define MUOS_HW_CLOCK0_DIV1024  _BV(CS02) | _BV(CS00)

#define MUOS_HW_CLOCK1_OFF      0
#define MUOS_HW_CLOCK1_DIV1     _BV(CS10)
#define MUOS_HW_CLOCK1_DIV8     _BV(CS11)
#define MUOS_HW_CLOCK1_DIV64    _BV(CS11) | _BV(CS10)
#define MUOS_HW_CLOCK1_DIV256   _BV(CS12)
#define MUOS_HW_CLOCK1_DIV1024  _BV(CS12) | _BV(CS10)

#define ISRNAME_CAPTURE_(hw) TIMER##hw##_CAPT_vect
#define ISRNAME_CAPTURE(hw) ISRNAME_CAPTURE_ hw


static inline void
muos_hw_shutdown (void)
{
  MCUCR |= _BV(PUD);
  DDRB = 0;
  DDRC = 0;
  DDRD = 0;
  muos_hw_sleep_prepare (SLEEP_MODE_PWR_DOWN);
  for (;;)
    {
      muos_hw_sleep ();
    }
}

#endif
