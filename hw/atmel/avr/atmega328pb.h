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
#define OC2B_BIT   3

#define OC3A_DDR   DDRD
#define OC3A_PORT  PORTD
#define OC3A_PIN   PIND
#define OC3A_BIT   0

#define OC3B_DDR   DDRD
#define OC3B_PORT  PORTD
#define OC3B_PIN   PIND
#define OC3B_BIT   2

#define OC4A_DDR   DDRD
#define OC4A_PORT  PORTD
#define OC4A_PIN   PIND
#define OC4A_BIT   1

#define OC4B_DDR   DDRD
#define OC4B_PORT  PORTD
#define OC4B_PIN   PIND
#define OC4B_BIT   2


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

#define MUOS_HW_CLOCK4_OFF      0
#define MUOS_HW_CLOCK4_DIV1     _BV(CS40)
#define MUOS_HW_CLOCK4_DIV8     _BV(CS41)
#define MUOS_HW_CLOCK4_DIV64    _BV(CS41) | _BV(CS40)
#define MUOS_HW_CLOCK4_DIV256   _BV(CS42)
#define MUOS_HW_CLOCK4_DIV1024  _BV(CS42) | _BV(CS40)

#define ISRNAME_CAPTURE_(hw) TIMER##hw##_CAPT_vect
#define ISRNAME_CAPTURE(hw) ISRNAME_CAPTURE_ hw


static inline void
muos_hw_shutdown (void)
{
  //PLANNED: configurable shutdown possibly calling HW_INIT, make HW_INIT configurable then
  MCUCR |= _BV(PUD);
  DDRB = 0;
  DDRC = 0;
  DDRD = 0;
  muos_hw_sleep_prepare (SLEEP_MODE_PWR_DOWN);
  for (;;)
    {
      cli();
      muos_hw_sleep ();
    }
}

#endif
