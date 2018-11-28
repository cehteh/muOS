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
#include <muos/pp.h>

#ifndef MUOS_STEPPER_HW
#error need MUOS_STEPPER_HW configuration
#endif
#define MUOS_STEPPER_COUNT (MUOS_PP_NARGS(MUOS_STEPPER_HW))  //FIXME: unpack (...)



/*


  configstore:

  prescale       prescaler setting
  cal_speed      slow speed for calibration (zeroing)
                 starting from min speed, that could be the speed where it can reverse
                 direction w/o loosing steps with some safety marigin (25%)
  min_speed      fastest speed which can be accelerated/decelerated from/to zero w/o
                 loosing steps with some safety margin (10%)
  max_speed      maximum speed the steppers run stable w/ loosing steps (after acceleration)
  accel          acceleration factor
  decel          deceleration factor

  conf_valid     bitfield, which values are known good

//PLANNED: axis length/limit switches
  limit_deadband limit switch variations



*/


//stepper_api:
//: .States
//: ----
//: enum muos_stepper_state
//: ----
//:
//: MUOS_STEPPER_UNKNOWN;;
//:   not fully initialized yet
//: MUOS_STEPPER_OFF;;
//:   stepper not energized, position unknown
//: MUOS_STEPPER_CAL;;
//:   stepper energized, position unknown, only calibration movements,
//:   no configuration necessary
//: MUOS_STEPPER_HOLD;;
//:   stepper energized, position unknown, only relative movements
//: MUOS_STEPPER_SLOW;;
//:   stepper energized, position known, running slower than min_speed
//:   can be stopped instantly without loosing steps.
//: MUOS_STEPPER_ARMED;;
//    stepper energized, position known
//: MUOS_STEPPER_FAST;;
//:   stepper energized, position known, running faster than min_speed
//:   must decelerate for stopping w/o loosing steps.
//:
enum muos_stepper_arming_state
  {
   MUOS_STEPPER_UNKNOWN,
   MUOS_STEPPER_OFF,
   MUOS_STEPPER_CAL,
   MUOS_STEPPER_HOLD,
   MUOS_STEPPER_ARMED,
   MUOS_STEPPER_SLOW,
   MUOS_STEPPER_FAST   //PLANNED: have a 'VERYFAST' state with simpler isr procedure
  };


struct stepper_state
{
  // note: statically initialized to zero, that must be ok for all values
  enum muos_stepper_arming_state state;
  int32_t position;
  int32_t movement_start_position;
  //TODO: callback on pos
  //TODO: callback on dest
};

extern struct stepper_state muos_steppers[MUOS_STEPPER_COUNT];

static inline void
muos_stepper_50init (void)
{
  muos_hw_stepper_init ();
}




#ifdef MUOS_STEPPER_ENABLE_ALL_HW
//stepper_api:
//: .Switching steppers on and off
//: ----
//: muos_error
//: muos_stepper_all_on (void)
//:
//: muos_error
//: muos_stepper_all_off (void)
//: ----
//:
//: Both functions return 'muos_error_configstore_locked' in case
//: of an error. Other errors should be handled in 'callback'.
muos_error
muos_stepper_on (void);

muos_error
muos_stepper_all_off (void);
#endif // MUOS_STEPPER_ENABLE_ALL_HW







// move commands for calibration

//stepper_api:
//: .Basic Calibration Movements
//: ----
//: muos_error
//: muos_stepper_cal_mov (uint8_t hw,
//:                       uint8_t prescale,
//:                       uint16_t speed_raw,
//:                       int32_t offset)
//:
//: muos_error
//: muos_stepper_cal_zigzag (uint8_t hw,
//:                          uint8_t prescale,
//:                          uint16_t speed_raw,
//:                          int16_t steps,
//:                          uint8_t rep)
//:
//: muos_error
//: muos_stepper_cal_zigzagpause (uint8_t hw,
//:                               uint8_t prescale,
//:                               int16_t speed_raw,
//:                               int16_t steps,
//:                               uint8_t rep)
//: ----
//:
//: +hw+;;
//:   Stepper to control.
//: +prescale+;;
//;   Hardware specific prescaler selection.
//: +speed_raw+;;
//;   Direct frequency generator value.
//: +offset+;;
//:   Distance and direction to travel.
//: +steps+;;
//:   Steps to zigzag (and pause).
//: +rep+;;
//:   Repetitions.
//:
//: Calibration needs needs some special commands bypassing the normal logic and
//: being functional even when there is no configuration available yet. The calibration
//: commands will turn the steppers (all, when MUOS_STEPPER_ENABLE_ALL_HW is
//: set) on.
//:
//: The speed here is defined as direct 16 bit counter value for the underlying hardware.
//: 65535 is the slowest possible speed. 8192 is the fastest speed. These functions
//  restrict the speed for safety.
//:
//: The 'prescale' parameter has the greatest effect on speeds and should be used with
//: uttermost care. The actual values are hardware implementation dependent.
//:
//: muos_stepper_mov_cal;;
//:   Moves for 'offset' steps at 'speed', negative values give  reverse direction.
//:
//: muos_stepper_cal_zigzag;;
//:   Moves 'steps' for 'rep' times forth and back. With instant direction change at then ends.
//:
//: muos_stepper_cal_zigzagpause;;
//:   Moves 'steps' for 'rep' times forth and back. Does a short pause before changing direction.
//:
//: WARNING: wrong use of this functions can damage the hardware.
//:
//:
muos_error
muos_stepper_cal_mov (uint8_t hw,
                      uint8_t prescale,
                      uint16_t speed_raw,
                      int32_t offset);

#if 0 //PLANNED:
muos_error
muos_stepper_cal_zigzag (uint8_t hw,
                         uint8_t prescale,
                         uint16_t speed_raw,
                         int16_t steps,
                         uint8_t rep);

muos_error
muos_stepper_cal_zigzagpause (uint8_t hw,
                              uint8_t prescale,
                              unt16_t speed_raw,
                              int16_t steps,
                              uint8_t rep);
#endif


// zeros the axis relative to the current position
//: .Axis zeroing
//: ----
//: muos_error
//: muos_stepper_set_zero (uint8_t hw, int32_t offset)
//: ----
//:
//: +hw+;;
//:   Stepper to control.
//: +offset+;;
//:   Offset to the current position.
//:
//: Zeros the step counter to the given offset.
//: Only available when the stepper is energized but not moving.
//:
muos_error
muos_stepper_set_zero (uint8_t hw, int32_t offset);


/*

// READY|RUNNING->ON
void
muos_stepper_invalidate (uint8_t hw);

void
muos_stepper_get_status (uint8_t hw);


//
muos_error
muos_stepper_mov_raw (uint8_t hw, uint16_t rawspeed, bool dir);





// speed is slowest_speed..min_speed/127, negative speed for reverse,
// 0==1 (saves some error handling)
muos_error
muos_stepper_mov_const (uint8_t hw, int8_t speed);


muos_error
muos_stepper_mov_rel (uint8_t hw, int32_t offset, int8_t speed);




// returns time when moving at full speed
uint32_t
muos_stepper_abs_estim (uint8_t hw, int32_t position);

// speedf is 1/256 factor to min_speed..max_speed (and accel/decel)
muos_error
muos_stepper_mov_abs (uint8_t hw, int32_t position, uint8_t speedf);
*/


#if 0 // TODO from DESIGN.orgin
/*


config:
  backlash compensation
       * offset
       * speed




   * later: have a small queue for planning ahead that allows smooth
     movements without stopping


**** Backlash compensation -> higher level

     Two possible strategies:

     1. speed=0
        Offset is used directly as distance to the current position.
        Can be inaccurate but is fast

     2. speed!=0
        Overshoot by offset steps, then move back offset steps with 'speed'
        Slow but accurate


*** API

    disclaimer: function and type names below are just for functional description



**** Configuration structure
     defined in source, values are directly written there

     * axis limits
     * max speed change for constant speed movements
     * max speed
     * backlash compensation values

**** State structure
     defined in source, values are directly written there

     * position
     * position valid flag
     * arming state
     * current speed -> derrived from frequency generator register
     * movement phase?

**** Configuration commands
     zero_axis(offset)
        sets the axis zero to offset from the current position





**** callback/query

     set_callback_at (position, function)
       calls function when position is touched (w/o backlash compensation)

     set_callback (function)
       calls function absolute positioning is done (w/ backlash commpensation)

     position query_position ()
     position query_speed ()



 */
#endif






#endif
