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
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include <muos/debug.h>

//PLANNED: cleanup macros when to use arguments, when tuple

//FIXME: mpu specific

//PLANNED: generic macros -> muos.h INPUT, OUTPUT, PULLUP, PULLDOWN, NOP
//#define CONFIGURE_GPIO_(port, pin) DDR##port |= _BV(DD##port##pin)
//#define CONFIGURE_GPIO(hw) CONFIGURE_OUTPUT_ hw
//PLANNED: also GPIO_SET for input states (config + set)

#define MUOS_HW_INIT  CLKPR = _BV(CLKPCE); CLKPR = 0

#define MUOS_GPIO_SET(port, pin) PORT##port |= _BV(PORT##port##pin)
#define MUOS_GPIO_SET_(hw) MUOS_GPIO_SET hw

#define MUOS_GPIO_CLEAR(port, pin) PORT##port &= ~_BV(PORT##port##pin)
#define MUOS_GPIO_CLEAR_(hw) MUOS_GPIO_CLEAR hw

#define MUOS_GPIO_TOGGLE(port, pin) PIN##port |= _BV(PIN##port##pin)
#define MUOS_GPIO_TOGGLE_(hw) MUOS_GPIO_TOGGLE hw

#define MUOS_PSTR(s) PSTR(s)
typedef const __flash char muos_flash_cstr[];


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
muos_hw_sleep_prepare (const uint8_t mode)
{
  //TODO: configureable mode
  set_sleep_mode (mode);
  sleep_enable();
}

static inline void
muos_hw_sleep (void)
{
  MUOS_DEBUG_BUSY_OFF;
  sei ();
  sleep_cpu ();
  cli ();
  MUOS_DEBUG_BUSY_ON;
}

static inline void
muos_hw_sleep_done (void)
{
  sleep_disable();
}


#endif
