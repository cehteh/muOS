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

extern volatile uint16_t muos_cppm_channel[MUOS_CPPM_CHANNELS];

void
muos_cppm_init (void);

#endif
#endif
