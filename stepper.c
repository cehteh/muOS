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

#include <muos/stepper.h>

struct stepper_state muos_steppers[MUOS_STEPPER_COUNT];



#ifdef MUOS_STEPPER_ENABLE_ALL_HW
muos_error
muos_stepper_all_on (void)
{

  for (uint8_t i=0; i<MUOS_STEPPER_COUNT)
    {
      //muos_steppers[i]
    }

  //  if state < unknown
  // && config is valid
  // check configuration
  // init prescaler / but timer is stopped
  // enable controller
  // start timers
  // set state to on
}

muos_error
muos_stepper_all_off (void)
{
  //if state > off
  //   stop timers
  //   disable controller
  //   state = off
}
#endif


// zeros the axis relative to the current position
#if 0 //TODO: implement
muos_error
muos_stepper_set_zero (uint8_t hw, int32_t offset)
{
  //  if state == ON || READY
                //zero
}
#endif


muos_error
muos_hw_stepper_register_action (uint8_t hw,
                                 int32_t position,
                                 uint8_t action,
                                 uintptr_t arg);

muos_error
muos_hw_stepper_remove_action (uint8_t hw,
                               int32_t position,
                               uint8_t action,
                               uintptr_t arg);

muos_error
muos_hw_stepper_enableall (void);

muos_error
muos_hw_stepper_disableall (void);

muos_error
muos_hw_stepper_start (uint8_t hw, uint8_t prescale, uint16_t speed_raw);




static void
muos_stepper_cal_restorestate (void)
{
  for(uint8_t i = 0; i < MUOS_STEPPER_COUNT; ++i)
    muos_steppers[i].state = muos_steppers[i].before_calibration;

  for (uint8_t i = 0; i<MUOS_STEPPER_COUNT; ++i)
    {
      if (!muos_stepper_mutable_state(i))
        return;
    }

  muos_hw_stepper_disableall ();
}


muos_error
muos_stepper_cal_mov (uint8_t hw,
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

  muos_hw_stepper_register_action (hw,
                                   muos_steppers[hw].position - offset,
                                   MUOS_STEPPER_ACTION_STOP|MUOS_STEPPER_HPQ_BACK,
                                   (uintptr_t)muos_stepper_cal_restorestate);
#if 0
  muos_hw_stepper_register_action (hw,
                                   muos_steppers[hw].position + offset,
                                   MUOS_STEPPER_ACTION_STOP|MUOS_STEPPER_HPQ_BACK,
                                   (uintptr_t)muos_stepper_cal_restorestate);


  if (offset<0)
    muos_hw_stepper_set_direction (hw, 0);
  else
    muos_hw_stepper_set_direction (hw, 1);
#endif
  //TODO: set enable & handle direction

  for(uint8_t i = 0; i < MUOS_STEPPER_COUNT; ++i)
    muos_steppers[i].before_calibration=muos_steppers[i].state;

  muos_steppers[hw].state = MUOS_STEPPER_CAL;

  muos_hw_stepper_start (hw, prescale, speed_raw);

  return muos_success;
}



#endif

