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

#include <muos/clock.h>

MUOS_HW_ISR(MUOS_HW_CLOCK_ISRNAME_OVERFLOW(MUOS_CLOCK_HW))
{
    ++muos_clock_count;
    //TODO: init compmatch from rtpq
}

volatile muos_clock muos_clock_count;

#if MUOS_NOW == 1
muos_clock muos_now_;
#endif

EMPTY_INTERRUPT(TIMER0_COMPA_vect);





