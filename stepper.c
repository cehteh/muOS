/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2018                            Christian Thäter <ct@pipapo.org>
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

#ifdef MUOS_STEPPER

#define MUOS_HW_STEPPER_H
#include <muos/stepper.h>

struct stepper_state muos_steppers[MUOS_STEPPER_COUNT];

#ifdef MUOS_STEPPER_DISABLEALL_INOUT_HW
muos_error
muos_stepper_all_on (void)
{
  for (uint8_t i=0; i<MUOS_STEPPER_COUNT; ++i)
    {
      if (muos_steppers[i].state < MUOS_STEPPER_OFF)
        return muos_error_stepper_state;
    }

  muos_error ret = muos_hw_stepper_enableall ();

  if (ret == muos_success)
    for (uint8_t i=0; i<MUOS_STEPPER_COUNT; ++i)
      {
        if (muos_steppers[i].state == MUOS_STEPPER_OFF)
          muos_steppers[i].state = MUOS_STEPPER_HOLD;
      }

  return ret;
}



muos_error
muos_stepper_stop (uint8_t hw)
{
  muos_error ret = muos_hw_stepper_stop (hw);
  if (!ret)
    return ret;

  if (muos_steppers[hw].state == MUOS_STEPPER_SLOW)
    muos_steppers[hw].state = MUOS_STEPPER_ARMED;
  else if (muos_steppers[hw].state > MUOS_STEPPER_RAW)
    muos_steppers[hw].state = MUOS_STEPPER_OFF;
  else if (muos_steppers[hw].state > MUOS_STEPPER_SLOW)
    muos_steppers[hw].state = MUOS_STEPPER_HOLD;

  for (uint8_t i=0; i<MUOS_STEPPER_POSITION_SLOTS; ++i)
    {
      muos_steppers[hw].position_match[i].whattodo = 0;
    }

  return muos_success;
}

void
muos_stepper_all_stop (void)
{
    for (uint8_t i=0; i<MUOS_STEPPER_COUNT; ++i)
      muos_stepper_stop (i);
}


void
muos_stepper_all_off (void)
{
  muos_stepper_all_stop ();
  muos_hw_stepper_disableall ();

  for (uint8_t i=0; i<MUOS_STEPPER_COUNT; ++i)
      {
        if (muos_steppers[i].state > MUOS_STEPPER_OFF)
          muos_steppers[i].state = MUOS_STEPPER_OFF;
      }
}
#endif



muos_error
muos_stepper_set_zero (uint8_t hw, int32_t offset)
{
  if (hw >= MUOS_STEPPER_COUNT)
    return muos_error_nohw;

  if (muos_steppers[hw].state != MUOS_STEPPER_HOLD && muos_steppers[hw].state != MUOS_STEPPER_ARMED)
    return muos_error_stepper_state;

  muos_steppers[hw].position -= offset;

  for (uint8_t i=0; i<MUOS_STEPPER_POSITION_SLOTS; ++i)
    {
      muos_steppers[hw].position_match[i].position -= offset;
    }

  muos_steppers[hw].state = MUOS_STEPPER_ARMED;

  return muos_success;
}



static void
muos_stepper_raw_restorestate (void)
{
  for(uint8_t i = 0; i < MUOS_STEPPER_COUNT; ++i)
    muos_steppers[i].state = muos_steppers[i].before_raw;

  for (uint8_t i = 0; i<MUOS_STEPPER_COUNT; ++i)
    {
      if (!muos_stepper_mutable_state(i))
        return;
    }

  // disable when the steppers where disabled before calibration
  muos_hw_stepper_disableall ();
}


muos_error
muos_stepper_move_raw (uint8_t hw,
                       uint8_t prescale,
                       uint16_t speed_raw,
                       int32_t offset)
{
  if (hw >= MUOS_STEPPER_COUNT)
    return muos_error_nohw;

  for (uint8_t i = 0; i<MUOS_STEPPER_COUNT; ++i)
    {
      if (!muos_stepper_mutable_state(i))
        {
          return muos_error_stepper_state;
        }
    }

#ifndef MUOS_STEPPER_UNSAFE
  if (speed_raw < 8192 || !offset)
    {
      return muos_error_stepper_range;
    }
#endif


  muos_error err = muos_hw_stepper_enableall ();
  if(err) return err; //could not enable

  muos_hw_stepper_set_direction (hw, offset>0?1:0);
  muos_hw_stepper_register_action (hw,
                                   muos_steppers[hw].position + offset,
                                   MUOS_STEPPER_ACTION_STOP|MUOS_STEPPER_HPQ_BACK,
                                   (uintptr_t)muos_stepper_raw_restorestate);

  for(uint8_t i = 0; i < MUOS_STEPPER_COUNT; ++i)
    muos_steppers[i].before_raw=muos_steppers[i].state;

  muos_steppers[hw].state = MUOS_STEPPER_RAW;

  muos_hw_stepper_start (hw, prescale, speed_raw);

  return muos_success;
}



#endif

