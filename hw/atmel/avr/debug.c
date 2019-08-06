/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2016                           Christian Thäter <ct@pipapo.org>
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

#include <muos/debug.h>
#include <muos/pp.h>

#include <muos/hw/atmel/avr/avr.h>

//PLANNED: generic macros -> muos.h INPUT, OUTPUT, PULLUP, PULLDOWN, NOP
#define CONFIGURE_OUTPUT_(port, pin) DDR##port |= _BV(DD##port##pin)
#define CONFIGURE_OUTPUT(hw) CONFIGURE_OUTPUT_ hw

void
muos_hw_debug_init (void)
{
#ifdef MUOS_DEBUG_BUSY
  CONFIGURE_OUTPUT (MUOS_DEBUG_BUSY);
#endif
#ifdef MUOS_DEBUG_INTR
  CONFIGURE_OUTPUT (MUOS_DEBUG_INTR);
#endif
#ifdef MUOS_DEBUG_ERROR
  CONFIGURE_OUTPUT (MUOS_DEBUG_ERROR);
#endif
#ifdef MUOS_DEBUG_SWITCH
  CONFIGURE_OUTPUT (MUOS_DEBUG_SWITCH);
#endif
#ifdef MUOS_DEBUG_C1
  CONFIGURE_OUTPUT (MUOS_DEBUG_C1);
#endif
#ifdef MUOS_DEBUG_C2
  CONFIGURE_OUTPUT (MUOS_DEBUG_C2);
#endif
#ifdef MUOS_DEBUG_C3
  CONFIGURE_OUTPUT (MUOS_DEBUG_C3);
#endif
#ifdef MUOS_DEBUG_C4
  CONFIGURE_OUTPUT (MUOS_DEBUG_C4);
#endif
  MUOS_DEBUG_BUSY_ON;
  MUOS_DEBUG_INTR_OFF;
  MUOS_DEBUG_ERROR_OFF;
  MUOS_DEBUG_SWITCH_OFF;
  MUOS_DEBUG_C1_OFF;
  MUOS_DEBUG_C2_OFF;
  MUOS_DEBUG_C3_OFF;
  MUOS_DEBUG_C4_OFF;
}
