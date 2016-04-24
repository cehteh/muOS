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

#ifndef MUOS_CPPM_H
#define MUOS_CPPM_H

#include <muos/muos.h>

#if MUOS_CPPM_CHANNELS > 0

#ifdef MUOS_CPPM_RAW
//cppm_api:
//: .Raw values
//: ----
//: uint16_t muos_cppm_channel_raw[MUOS_CPPM_CHANNELS]
//: ----
//:
//: Stores the time in timer ticks as measuered for each channel directly,
//: after applying some configureable filter. This gives the most precision
//: but also needs more memory. Must be enabled with 'MUOS_CPPM_RAW'.
//:
extern volatile uint16_t muos_cppm_channel_raw[MUOS_CPPM_CHANNELS];
#endif

#ifdef MUOS_CPPM_COOKED
//cppm_api:
//: .Raw values
//: ----
//: int8_t muos_cppm_channel_cooked[MUOS_CPPM_CHANNELS]
//: ----
//:
//: Stores the channel data as values from -125 to 125 mapping to
//: the range from 'MUOS_CPPM_COOKED_MIN' to 'MUOS_CPPM_COOKED_MAX'.
//: Little overflows from -128 to 127 are tolerated.
//: Cooked values need less memory and are more stable, but have lower precision.
//: Must be enabled with 'MUOS_CPPM_COOKED'.
//:
extern volatile int8_t muos_cppm_channel_cooked[MUOS_CPPM_CHANNELS];
#endif


void
muos_cppm_init (void);

#endif
#endif
