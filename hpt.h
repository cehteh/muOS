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

/*
  High precision timer

  using 2nd compmatch from clock

  only one job can be running

  called at high prio in interrupt context

  returns 0 for stop or hw_clock_type 1..max for rescheduling

  no extra check about missed times
*/

typedef muos_hwclock (*muos_hpt_function) (uintptr_t);

static inline muos_error
muos_hpt_register (muos_hwclock when, muos_hpt_function what)
{
  return muos_fatal_error;
}

#endif
