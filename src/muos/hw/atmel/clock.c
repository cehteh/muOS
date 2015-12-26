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

#ifdef __AVR_ATmega328P__

#define ISRNAME_OVERFLOW_(hw) TIMER##hw##_OVF_vect
#define ISRNAME_OVERFLOW(hw) ISRNAME_OVERFLOW_(hw)

ISR(ISRNAME_OVERFLOW(MUOS_CLOCK_HW))
{
  ++muos_clock_count_;
}

#define ISRNAME_COMPMATCH_(tmhw,cmhw) TIMER##tmhw##_COMP##cmhw##_vect
#define ISRNAME_COMPMATCH(tmhw,cmhw) ISRNAME_COMPMATCH_(tmhw,cmhw)

// compmatch interrupt is only used to wake the mainloop
EMPTY_INTERRUPT(ISRNAME_COMPMATCH(MUOS_CLOCK_HW, MUOS_CLOCK_HW_COMPAREMATCH));

#endif


