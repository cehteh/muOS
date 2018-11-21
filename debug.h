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

#ifndef MUOS_DEBUG_H
#define MUOS_DEBUG_H



//debug_api:
//: .Debug GPIO Macros
//: ----
//: MUOS_DEBUG_Cx_ON
//: MUOS_DEBUG_Cx_OFF
//: MUOS_DEBUG_Cx_TOGGLE
//: ----
//:
//: +x+::
//:   Debug channel (1-4)
//:
//: Sets the debug channel to the given state.
//:
//: ----
//: MUOS_DEBUG_INTR_ON
//: MUOS_DEBUG_INTR_OFF
//: ----
//:
//: MµOS Interrupt debug channel. User defined interrupts should turn
//: the GPIO on right at the start and off when done.
//:

#if defined(MUOS_DEBUG) && defined(MUOS_DEBUG_BUSY)
#define MUOS_DEBUG_BUSY_ON MUOS_GPIO_SET_(MUOS_DEBUG_BUSY)
#define MUOS_DEBUG_BUSY_OFF MUOS_GPIO_CLEAR_(MUOS_DEBUG_BUSY)
#else
#define MUOS_DEBUG_BUSY_ON
#define MUOS_DEBUG_BUSY_OFF
#endif

#if defined(MUOS_DEBUG) && defined(MUOS_DEBUG_INTR)
#define MUOS_DEBUG_INTR_ON MUOS_GPIO_SET_(MUOS_DEBUG_INTR)
#define MUOS_DEBUG_INTR_OFF MUOS_GPIO_CLEAR_(MUOS_DEBUG_INTR)
#else
#define MUOS_DEBUG_INTR_ON
#define MUOS_DEBUG_INTR_OFF
#endif

#if defined(MUOS_DEBUG) && defined(MUOS_DEBUG_ERROR)
#define MUOS_DEBUG_ERROR_ON MUOS_GPIO_SET_(MUOS_DEBUG_ERROR)
#define MUOS_DEBUG_ERROR_OFF MUOS_GPIO_CLEAR_(MUOS_DEBUG_ERROR)
#define MUOS_DEBUG_ERROR_TOGGLE MUOS_GPIO_TOGGLE_(MUOS_DEBUG_ERROR)
#else
#define MUOS_DEBUG_ERROR_ON
#define MUOS_DEBUG_ERROR_OFF
#define MUOS_DEBUG_ERROR_TOGGLE
#endif

#if defined(MUOS_DEBUG) && defined(MUOS_DEBUG_SWITCH)
#define MUOS_DEBUG_SWITCH_ON MUOS_GPIO_SET_(MUOS_DEBUG_SWITCH)
#define MUOS_DEBUG_SWITCH_OFF MUOS_GPIO_CLEAR_(MUOS_DEBUG_SWITCH)
#define MUOS_DEBUG_SWITCH_TOGGLE MUOS_GPIO_TOGGLE_(MUOS_DEBUG_SWITCH)
#else
#define MUOS_DEBUG_SWITCH_ON
#define MUOS_DEBUG_SWITCH_OFF
#define MUOS_DEBUG_SWITCH_TOGGLE
#endif

#if defined(MUOS_DEBUG) && defined(MUOS_DEBUG_C1)
#define MUOS_DEBUG_C1_ON MUOS_GPIO_SET_(MUOS_DEBUG_C1)
#define MUOS_DEBUG_C1_OFF MUOS_GPIO_CLEAR_(MUOS_DEBUG_C1)
#define MUOS_DEBUG_C1_TOGGLE MUOS_GPIO_TOGGLE_(MUOS_DEBUG_C1)
#else
#define MUOS_DEBUG_C1_ON
#define MUOS_DEBUG_C1_OFF
#define MUOS_DEBUG_C1_TOGGLE
#endif

#if defined(MUOS_DEBUG) && defined(MUOS_DEBUG_C2)
#define MUOS_DEBUG_C2_ON MUOS_GPIO_SET_(MUOS_DEBUG_C2)
#define MUOS_DEBUG_C2_OFF MUOS_GPIO_CLEAR_(MUOS_DEBUG_C2)
#define MUOS_DEBUG_C2_TOGGLE MUOS_GPIO_TOGGLE_(MUOS_DEBUG_C2)
#else
#define MUOS_DEBUG_C2_ON
#define MUOS_DEBUG_C2_OFF
#define MUOS_DEBUG_C2_TOGGLE
#endif

#if defined(MUOS_DEBUG) && defined(MUOS_DEBUG_C3)
#define MUOS_DEBUG_C3_ON MUOS_GPIO_SET_(MUOS_DEBUG_C3)
#define MUOS_DEBUG_C3_OFF MUOS_GPIO_CLEAR_(MUOS_DEBUG_C3)
#define MUOS_DEBUG_C3_TOGGLE MUOS_GPIO_TOGGLE_(MUOS_DEBUG_C3)
#else
#define MUOS_DEBUG_C3_ON
#define MUOS_DEBUG_C3_OFF
#define MUOS_DEBUG_C3_TOGGLE
#endif

#if defined(MUOS_DEBUG) && defined(MUOS_DEBUG_C4)
#define MUOS_DEBUG_C4_ON MUOS_GPIO_SET_(MUOS_DEBUG_C4)
#define MUOS_DEBUG_C4_OFF MUOS_GPIO_CLEAR_(MUOS_DEBUG_C4)
#define MUOS_DEBUG_C4_TOGGLE MUOS_GPIO_TOGGLE_(MUOS_DEBUG_C4)
#else
#define MUOS_DEBUG_C4_ON
#define MUOS_DEBUG_C4_OFF
#define MUOS_DEBUG_C4_TOGGLE
#endif

void
muos_hw_debug_init (void);







#endif
