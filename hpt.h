/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2019                            Christian Thäter <ct@pipapo.org>
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

#ifndef MUOS_HPT_H
#define MUOS_HPT_H

#include <muos/clock.h>


typedef muos_hwclock (*muos_hpt_fn) (void);

//hpt_api:
//: .Registering a HPT function
//: ----
//: typedef muos_hwclock (*muos_hpt_fn) (void);
//:
//: muos_error muos_hpt_after (muos_hwclock when, muos_hpt_fn what)
//: ----
//:
//: +when+::
//:   When to schedule
//: +what+::
//:   Function to be called
//:
//: 'muos_hpt_after()' schedules the function 'what' to be called
//: after 'now+when' ticks.
//:
//: Returns 'muos_error_hpt_active' when a function is already scheduled.
//:
//: 'muos_hpt_fn' functions must return 0 when done or 1 to the max value
//: of 'muos_hwclock' to schedule the function in that much ticks again.
//:
//: Note that thr initial delay when calling 'muos_hpt_after()'can have
//: some jitter because the call is usually not synchronized with clock
//: ticks.
//:
muos_error
muos_hpt_after (muos_hwclock when, muos_hpt_fn what);

#endif
