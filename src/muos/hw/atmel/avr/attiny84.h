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

#include <muos/hw/atmel/avr/attiny.h>




#define MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE_(hw)  \
  TIMSK##hw |= _BV(TOIE##hw)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE_(tmhw, cmhw, at)    \
  OCR##tmhw##cmhw = at;                                        \
  TIMSK##tmhw |= _BV(OCIE##tmhw##cmhw)

#define MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE_(tmhw, cmhw)       \
  TIMSK##tmhw &= ~_BV(OCIE##tmhw##cmhw)


// values guessed from datasheet
// Figure 21-42. Calibrated 8 MHz RC Oscillator Frequency vs. OSCCAL Value
// doesn't need to be exact
#define MUOS_HW_ATMEL_OSCAL_HIGHSWITCH 188
#define MUOS_HW_ATMEL_OSCAL_LOWSWITCH 92

static inline void
muos_hw_shutdown (void)
{
  MCUCR |= _BV(PUD);
  //FIXME:  DDRB = 0;
  //FIXME:  DDRC = 0;
  //FIXME:  DDRD = 0;
  muos_hw_sleep_prepare (SLEEP_MODE_PWR_DOWN);
  for (;;)
    {
      muos_hw_sleep ();
    }
}

#endif
