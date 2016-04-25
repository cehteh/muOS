/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2016                            Christian Thäter <ct@pipapo.org>
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

#if MUOS_CPPM_CHANNELS > 0

#include <muos/clock.h>
#include <muos/cppm.h>

#ifdef MUOS_CPPM_RAW
volatile uint16_t muos_cppm_channel_raw[MUOS_CPPM_CHANNELS];
#endif

#ifdef MUOS_CPPM_COOKED
volatile int8_t muos_cppm_channel_cooked[MUOS_CPPM_CHANNELS];
#endif

void
muos_hw_cppm_init (void);

void
muos_cppm_50init (void)
{
  muos_hw_cppm_init ();
}

#endif

