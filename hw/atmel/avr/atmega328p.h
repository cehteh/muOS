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
