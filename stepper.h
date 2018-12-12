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

//PLANNED: do we need min_start / min_stop speed?

//PLANNED: have a sum-speed all steppers together wont go faster
//         bucket allocator for stepper speed

*/

#ifdef MUOS_HW_STEPPER_H
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
muos_hw_stepper_set_direction (uint8_t hw, bool dir);

muos_error
muos_hw_stepper_start (uint8_t hw, uint8_t prescale, uint16_t speed_raw);

muos_error
muos_hw_stepper_stop (uint8_t hw);

#endif

//stepper_api:
//: .States
//: ----
//: enum muos_stepper_state
//: ----
//:
//: MUOS_STEPPER_UNKNOWN;;
//:   not fully initialized yet
//: MUOS_STEPPER_DISABLED;;
//:   externally disabled (when inout pin is configured)
//: MUOS_STEPPER_RAW;;
//:   stepper energized, position unknown, only raw movements,
//:   no configuration necessary
//: MUOS_STEPPER_OFF;;
//:   stepper not energized, position unknown
//: MUOS_STEPPER_HOLD;;
//:   stepper energized, position unknown, only relative movements
//: MUOS_STEPPER_ARMED;;
//    stepper energized, position known
//: MUOS_STEPPER_STOPPED;;
//    stepper energized, position known, done moving, stopped
//: MUOS_STEPPER_SLOW;;
//:   stepper energized, position known, running slower than min_speed
//:   can be stopped instantly without loosing steps.
//: MUOS_STEPPER_ACCEL;;
//:   stepper energized, position known, accerating faster than min_speed
//:   must decelerate for stopping w/o loosing steps.
//: MUOS_STEPPER_FAST;;
//:   stepper energized, position known, running at max_speed
//:   must decelerate for stopping w/o loosing steps.
//: MUOS_STEPPER_DECEL;;
//:   stepper energized, position known, decelerating, faster than min_speed
//:
enum muos_stepper_arming_state
  {
   MUOS_STEPPER_UNKNOWN,
   MUOS_STEPPER_DISABLED,
   MUOS_STEPPER_RAW, //FIXME: check state checks
   MUOS_STEPPER_OFF,
   MUOS_STEPPER_HOLD,
   MUOS_STEPPER_ARMED,
   MUOS_STEPPER_STOPPED,
   MUOS_STEPPER_SLOW,
   MUOS_STEPPER_ACCEL,
   MUOS_STEPPER_FAST,
   MUOS_STEPPER_DECEL,
  };


struct stepper_state
{
  // note: statically initialized to zero, that must be ok for all values
  enum muos_stepper_arming_state state;
  enum muos_stepper_arming_state before_raw;
  int32_t position; // not volatile, should not be read when stepper is running
  // internal state for the ISR
  int32_t start;
  int32_t end;
  int32_t accel_end;
  int32_t decel_start;
  uint32_t ad;
  uint32_t slope;

  struct {
    int32_t position;
    uint8_t whattodo;
    uintptr_t arg;
  } position_match[MUOS_STEPPER_POSITION_SLOTS];
};

extern struct stepper_state muos_steppers[MUOS_STEPPER_COUNT];


//: .Check State
//: ----
//: static inline bool
//: muos_stepper_mutable_state (uint8_t hw)
//: ----
//:
//: +hw+;;
//:   Stepper to control.
//:
//: Returns 'true' when the state can be modified by the API.
//: States can only be changed when the motors are not moving and not externally disabled.
//:
static inline bool
muos_stepper_mutable_state (uint8_t hw)
{
  return muos_steppers[hw].state != MUOS_STEPPER_RAW
    && muos_steppers[hw].state >= MUOS_STEPPER_OFF
    && muos_steppers[hw].state < MUOS_STEPPER_SLOW;
}



static inline void
muos_stepper_50init (void)
{
  muos_hw_stepper_init ();
}



#ifdef MUOS_STEPPER_DISABLEALL_INOUT_HW
//stepper_api:
//: .Switching steppers on and off, stopping
//: ----
//: muos_error
//: muos_stepper_all_on (void)
//:
//: void
//: muos_stepper_all_off (void)
//:
//: void
//: muos_stepper_stop (uint8_t hw)
//:
//: void
//: muos_stepper_all_stop (void)
//: ----
//:
//: 'muos_stepper_all_on ()' tries to switch on all steppers.
//: When successful muos_success is returned.
//: The steppers are in state 'MUOS_STEPPER_HOLD' which
//: only allows relative and calibration movements. For arming the
//: Steppers fully the axis has to be zerored.
//: In case the steppers where already on, no state is changed.
//:
//: .On error following values are returned:
//: +muos_warn_sched_depth+;;
//    Scheduler depth exceeded
//: +muos_error_stepper_state+;;
//:   Steppers are externally disabled, uninitialized or in calibration mode.
//:
//: 'muos_stepper_all_off ()' switches all steppers off.
//: No matter of the state, the steppers will be disabled afterwards.
//: States 'MUOS_STEPPER_UNKNOWN' and 'MUOS_STEPPER_DISABLED' will stay, otherwise
//: the new state will be MUOS_STEPPER_OFF.
//:
//: Note: turning the steppers off will loose their position and they must zeroed after
//:       enabling them again.
//:
//: 'muos_stepper_stop (hw)' stops movement of a single stepper, but keeps it energized.
//: For steppers which were running faster than 'MUOS_STEPPER_SLOW' the position will
//: be invalidated and the state becomes 'MUOS_STEPPER_HOLD'. They need to be re-zeroed
//: after a stop. For Steppers running at 'MUOS_STEPPER_SLOW' or slower the position is
//: kept valid and the state becomes 'MUOS_STEPPER_ARMED'.
//: All position matches registered will be cleared.
//:
//: 'muos_stepper_all_stop ()' is the same as 'muos_stepper_stop (hw)' but for all Steppers.
//:
//: Stopping and turning steppers off will never fail nor return an error code.
//: Only the resulting state may differ depending on the initial state. With the exception
//: that trying to stop a stepper with a non existing hardware descriptor will fail with
//: +muos_error_nohw+.
//:
muos_error
muos_stepper_all_on (void);

void
muos_stepper_all_off (void);

muos_error
muos_stepper_stop (uint8_t hw);

void
muos_stepper_all_stop (void);
#endif // MUOS_STEPPER_ENABLE_ALL_HW







// move commands for calibration

//stepper_api:
//: .Basic Calibration Movements
//: ----
//: muos_error
//: muos_stepper_move_raw (uint8_t hw,
//:                        uint8_t prescale,
//:                        uint16_t speed_raw,
//:                        int32_t offset)
//:
//: muos_error
//: muos_stepper_move_zigzag (uint8_t hw,
//:                           uint8_t prescale,
//:                           uint16_t speed_raw,
//:                           int16_t steps,
//:                           uint8_t rep)
//:
//: muos_error
//: muos_stepper_move_zigzagpause (uint8_t hw,
//:                                uint8_t prescale,
//:                                int16_t speed_raw,
//:                                int16_t steps,
//:                                uint8_t rep)
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
//: muos_stepper_move_raw;;
//:   Moves for 'offset' steps at 'speed', negative values give  reverse direction.
//:
//: muos_stepper_move_zigzag;;
//:   Moves 'steps' for 'rep' times forth and back. With instant direction change at then ends.
//:
//: muos_stepper_move_zigzagpause;;
//:   Moves 'steps' for 'rep' times forth and back. Does a short pause before changing direction.
//:
//: WARNING: wrong use of this functions can damage the hardware.
//:
//:
muos_error
muos_stepper_move_raw (uint8_t hw,
                       uint8_t prescale,
                       uint16_t speed_raw,
                       int32_t offset);

#if 0 //PLANNED:
muos_error
muos_stepper_move_zigzag (uint8_t hw,
                          uint8_t prescale,
                          uint16_t speed_raw,
                          int16_t steps,
                          uint8_t rep);

muos_error
muos_stepper_move_zigzagpause (uint8_t hw,
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
//: When the stepper was not 'ARMED' yet (position unknown) it becomes
//: armed with zeroing. Only then fast absolute movements are possible.
//:
muos_error
muos_stepper_set_zero (uint8_t hw, int32_t offset);




//stepper_api:
//: .Actions at Positions
//: ----
//: enum muos_stepper_actions
//: ----
//:
//: Define actions to take when the stepper reaches a position. These are bit values
//: which can be or'ed together.
//:
//: MUOS_STEPPER_ACTION_PERMANENT;;
//:   Keep the registered action. execute it always again on that position.
//: MUOS_STEPPER_ACTION_2ND;;
//:   Exceute the registered action when the stepper hits the position the second time.
//:   Incompatible with MUOS_STEPPER_ACTION_PERMANENT. Used for backlash compensation.
//: MUOS_STEPPER_ACTION_STOP;;
//:   Immediately stop the stepper at the position.
//: MUOS_STEPPER_HPQ_FRONT;;
//:   Use the provided argument as function to push it to the front of the hpq. This gives the
//:   highest possible priority.
//: MUOS_STEPPER_HPQ_FRONT;;
//:   Use the provided argument as function to push it to the back of the hpq.
//:
//:
//PLANNED: mergeable flag, registering actions on the same position might be merged if compatible
enum muos_stepper_actions
  {
   MUOS_STEPPER_ACTION_PERMANENT = (1<<0),
   MUOS_STEPPER_ACTION_2ND = (1<<1), //TODO: implement me
   MUOS_STEPPER_ACTION_STOP = (1<<2),
   //: MUOS_STEPPER_ACTION_OFF;;  //TODO: needed?
   MUOS_STEPPER_HPQ_FRONT = (1<<3),
   MUOS_STEPPER_HPQ_BACK = (1<<4),
  };


//stepper_api:
//: ----
//: muos_error
//: muos_stepper_register_action (uint8_t hw,
//:                               int32_t position,
//:                               uint8_t action,
//:                               uintptr_t arg)
//:
//: muos_error
//: muos_stepper_remove_action (uint8_t hw,
//:                             int32_t position,
//:                             uint8_t action,
//:                             uintptr_t arg)
//: ----
//:
//: +hw+;;
//:   Stepper to address.
//: +position+;;
//:   Position which triggers the action.
//: +action+;;
//:   Action flags or'ed together.
//: +arg+;;
//:   Argument to the action, callback function for the hpq for example.
//:
//: Register or remove and action to be done when the stepper hits a position.
//: Action must be at least one or more of the action flags above or'ed together.
//: Optionally a pointer to a function which gets pushed to the HPQ can be supplied.
//:
//: Actions can only be registered or removed while the stepper is not moving.
//:
//: Removing an action needs the exact same arguments as given registering it. Only one
//: instance gets removed even when the action was registered multiple times.
//:
//: Returning +muos_success+ when everything worked well or one of the following errors:
//: +muos_error_nohw+;;
//:   'hw' out of range.
//: +muos_error_stepper_state+;;
//:   Tried to register/remove an action while the steppers are moving.
//: +muos_error_stepper_noslot+;;
//:   'register_action':: no more slots free to register an action.
//:   'remove_action':: no action found with the given parameters.
//:
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


static inline muos_error
muos_stepper_register_action (uint8_t hw,
                                 int32_t position,
                                 uint8_t action,
                                 uintptr_t arg)
{
  return muos_hw_stepper_register_action (hw,
                                          position,
                                          action,
                                          arg);
}


static inline muos_error
muos_stepper_remove_action (uint8_t hw,
                            int32_t position,
                            uint8_t action,
                            uintptr_t arg)
{
  return muos_hw_stepper_remove_action (hw,
                                        position,
                                        action,
                                        arg);
}

//PLANNED: muos_stepper_remove_all_actions(hw)


/*

  Absolute movements w/ acceleration/deceleration

*/



// speedf is 1/256 factor to min_speed..max_speed (and accel/decel)
muos_error
muos_stepper_move_abs (uint8_t hw, int32_t position, uint8_t speedf);








/*

// READY|RUNNING->ON
void
muos_stepper_invalidate (uint8_t hw);

void
muos_stepper_get_status (uint8_t hw);







// speed is slowest_speed..min_speed/127, negative speed for reverse,
// 0==1 (saves some error handling)
muos_error
muos_stepper_mov_const (uint8_t hw, int8_t speed);


muos_error
muos_stepper_mov_rel (uint8_t hw, int32_t offset, int8_t speed);




// returns time when moving at full speed
uint32_t
muos_stepper_abs_estim (uint8_t hw, int32_t position);

*/


#if 0 // TODO from DESIGN.org
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
