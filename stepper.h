/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2018                           Christian Thäter <ct@pipapo.org>
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

#ifndef MUOS_STEPPER_H
#define MUOS_STEPPER_H

#include <stdint.h>
#include <muos/muos.h>

//stepper_api:
//: .Raw values
//: ----
//: uint16_t muos_cppm_channel_raw[MUOS_CPPM_CHANNELS]
//: ----
//:
//: Stores the time in timer ticks as measuered for each channel directly,
//: after applying some configureable filter. This gives the most precision
//: but also needs more memory. Must be enabled with 'MUOS_CPPM_RAW'.
//:

// array of positions, each num is mapped to a hardware by config



/*

  config:

  cal_speed     slow hardcoded speed for calibration (zeroing)
  min_speed     fastest speed which can be accelerated/decelerated from/to zero w/o loosing steps
  max_speed     maximum speed the motors run stable w/ loosing steps (after acceleration)
  accel         acceleration factor
  decel         deceleration factor

  conf_valid    bitfield, which values are known good

*/



//void
//muos_stepper_50init (void);




void
muos_stepper_on (uint8_t hw);

void
muos_stepper_off (uint8_t hw);

void
muos_stepper_zero (uint8_t hw, int32_t offset);

muos_error
muos_stepper_mov_const (uint8_t hw, int16_t speed);


muos_error
muos_stepper_mov_rel (uint8_t hw, int32_t offset, int32_t speed);


uint32_t
muos_stepper_abs_estim (uint8_t hw, int32_t position);

muos_error
muos_stepper_mov_abs (uint8_t hw, int32_t position, uint8_t speed);




#endif
