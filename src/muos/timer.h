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
#ifndef MUOS_TIMER_H
#define MUOS_TIMER_H



#include "muos_config.h"

#include "muos/hwdef.h"


MUOS_TYPEDEF(uint, MUOS_TIMER_TYPE, muos_timer);
MUOS_TYPEDEF(uint, MUOS_TIMER_SHORT_TYPE, muos_shorttimer);


#define MUOS_TIMERDEF(hw, ...)                  \
  extern volatile muos_timer muos_timer_##hw;   \
  ISR (MUOS_TIMER_ISRNAME_OVERFLOW(hw))         \
  {                                             \
    ++muos_timer_##hw;                          \
    (void) __VA_ARGS__;                         \
  }                                             \
  volatile muos_timer muos_timer_##hw


#define MUOS_TIMER_INIT(hw, prescale)           \
  MUOS_HW_TIMER_ISR_OVERFLOW_ENABLE(hw);        \
  MUOS_HW_TIMER_PRESCALE_SET(hw, prescale)


#define MUOS_TIMER_NOW(hw)                                                             \
  ((muos_timer_##hw<<(sizeof(MUOS_HW_TIMER_REGISTER(hw)) * 8)) + MUOS_HW_TIMER_REGISTER(hw))


//TODO: match interrupt

#endif
