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
#include <avr/sleep.h>

#define MUOS_HW_ISR(what) ISR(what)
#define MUOS_HW_EMPTY_ISR(what) EMPTY_INTERRUPT(what)

static inline void
muos_interrupt_enable ()
{
  sei ();
}

static inline void
muos_interrupt_disable ()
{
  cli ();
}

static inline void
muos_hw_sleep_prepare (void)
{
  //TODO: configureable mode
  cli();
  set_sleep_mode (SLEEP_MODE_IDLE);
  sleep_enable();
  sei();
}

static inline void
muos_hw_sleep (void)
{
  sleep_cpu();
  sleep_disable();
}



#endif
