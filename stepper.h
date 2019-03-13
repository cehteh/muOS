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
#include <muos/hpq.h>

#ifndef MUOS_STEPPER_HW
#error need MUOS_STEPPER_HW configuration
#endif


//TODO: startall()
//TODO: sync moves

/*
  configstore:

  prescale       prescaler setting
  cal_speed      slow speed for calibration (zeroing)
                 starting from slow_speed, that could be the speed where it can reverse
                 direction w/o loosing steps with some safety marigin (25%)
  slow_speed     fastest speed which can be accelerated/decelerated from/to zero w/o
                 loosing steps with some safety margin (10%)
  max_speed      maximum speed the steppers run stable w/ loosing steps (after acceleration)
  accel          acceleration factor
  decel          deceleration factor

  conf_valid     bitfield, which values are known good

//PLANNED: axis length/limit switches
  limit_deadband limit switch variations

*/

#ifdef MUOS_HW_STEPPER_H
muos_error
muos_hw_stepper_enable_all (void);

void
muos_hw_stepper_disable_all (void);

muos_error
muos_hw_stepper_set_direction (uint8_t hw, bool dir);

muos_error
muos_hw_stepper_start (uint8_t hw, uint16_t speed_raw, uint8_t prescale);

void
muos_hw_stepper_stop (uint8_t hw);
#endif

//stepper_states:
//: ----
//: enum muos_stepper_state
//: ----
//:
//: MUOS_STEPPER_UNKNOWN;;
//:   - no initialized yet
//: MUOS_STEPPER_OFF;;
//:   - stepper not energized
//:   - position unknown
//: MUOS_STEPPER_ON;;
//:   - stepper energized
//:   - position unknown, only raw movements
//: MUOS_STEPPER_HOLD;;
//:   - stepper energized
//:   - position unknown
//:   - only relative movements
//: MUOS_STEPPER_ARMED;;
//:   - stepper energized
//:   - position known
//:   - all kinds of movements allowed
//: MUOS_STEPPER_WAIT;;
//:   - stepper energized
//:   - position known
//:   - done moving, waiting for sync/continuation
//: MUOS_STEPPER_RAW;;
//:   - stepper moving
//:   - position unknown
//:   - no configuration necessary
//:   - dangerous/invalid moves possible!
//: MUOS_STEPPER_SLOW_CAL;;
//:   - stepper moving slower than slow_speed
//:   - position unknown
//:   - can be stopped instantly without loosing steps
//: MUOS_STEPPER_SLOW_REL;;
//:   - stepper moving slower than slow_speed
//:   - position known
//:   - can be stopped instantly without loosing steps
//: MUOS_STEPPER_SLOW;;
//:   - stepper moving slower than slow_speed
//:   - position known
//:   - can be stopped instantly without loosing steps
//: MUOS_STEPPER_SLOPE;;
//:   - stepper moving, accelerating or decelerating on slope parameters
//:   - position known
//    - must decelerate for stopping w/o loosing steps.
//: MUOS_STEPPER_FAST;;
//:   - stepper moving faster than slow_speed
//:   - position known
//:   - must decelerate for stopping w/o loosing steps.
//: MUOS_STEPPER_STOPPING;;
//:   - stepper moving faster than slow_speed, braking
//:   - position known
//:   - Controlled brake to a slow_speed, then stop
//:
enum muos_stepper_arming_state
  {
   MUOS_STEPPER_UNKNOWN,
   MUOS_STEPPER_OFF,
   MUOS_STEPPER_ON,
   MUOS_STEPPER_HOLD,
   MUOS_STEPPER_ARMED,
   MUOS_STEPPER_WAIT,
   MUOS_STEPPER_RAW,
   MUOS_STEPPER_SLOW_CAL,
   MUOS_STEPPER_SLOW_REL,
   MUOS_STEPPER_SLOW,
   MUOS_STEPPER_SLOPE,
   MUOS_STEPPER_FAST,
   MUOS_STEPPER_STOPPING,  //TODO: unimplemented
  };


typedef void (*muos_stepper_fn)(uint8_t hw);

struct muos_stepper_action
{
  int32_t position;
  uint8_t whattodo;
  uintptr_t arg;
};


struct muos_stepper_slope
{
  int32_t position;
  uint32_t constant;               // constant speed steps at the top of the slope
  uint16_t pos;                    // slope position
  uint16_t end;                    // only slope part
  uint16_t len;                    // only slope part
  uint16_t max_speed;              //PLANNED: calculate correction for maxslope
  uint16_t speed_out;
};

struct muos_stepper_state
{
  volatile enum muos_stepper_arming_state state;
  volatile int32_t position;

  uint16_t speed_flt;               // filtered speed to mitigate rounding errors

  muos_queue_function slope_gen;

  volatile uint8_t active:1;         // which one is the active from the buffer
  volatile uint8_t ready:1;          // set when the next slope is prepared
  struct muos_stepper_slope slope[2];     // double buffered

  struct muos_stepper_action position_match[MUOS_STEPPER_POSITION_SLOTS];
};


extern struct muos_stepper_state muos_steppers[MUOS_STEPPER_NUM];


void
muos_stepper_50init (void);



//stepper_api:
//: .Switching steppers on and off, stopping
//: ----
//: muos_error
//: muos_stepper_enable_all (void)
//:
//: void
//: muos_stepper_disable_all (void)
//:
//: void
//: muos_stepper_stop (uint8_t hw)
//:
//: void
//: muos_stepper_stop_all (void)
//: ----
//:
//: 'muos_stepper_enable_all ()' tries to switch on all steppers.
//: When successful muos_success is returned.
//: The steppers are energized and in state 'MUOS_STEPPER_ON' which
//: allows only raw movements.
//:
//: .On error following values are returned:
//: +muos_warn_sched_depth+;;
//    Scheduler depth exceeded
//: +muos_error_stepper_state+;;
//:   Steppers are externally disabled, uninitialized or otherwise unavailable.
//:
//: 'muos_stepper_disable_all ()' switches all steppers off.
//: No matter of the state, the steppers will be disabled afterwards.
//: States 'MUOS_STEPPER_UNKNOWN' will stay, otherwise the new state will be MUOS_STEPPER_OFF.
//: Turning the steppers off will loose their position and free the configuration lock.
//:
//: 'muos_stepper_stop (hw)' stops movement of a single stepper, but keeps it energized.
//: For steppers which were running faster than 'MUOS_STEPPER_SLOW' the position will
//: be invalidated and the state becomes 'MUOS_STEPPER_HOLD'. They need to be re-zeroed
//: after a stop. For Steppers running at 'MUOS_STEPPER_SLOW' or slower the position is
//: kept valid and the state becomes 'MUOS_STEPPER_ARMED'.
//:
//: 'muos_stepper_all_stop ()' is the same as 'muos_stepper_stop (hw)' but for all Steppers.
//:
//: Stopping and turning steppers off will never fail nor return an error code.
//: Only the resulting state may differ depending on the initial state.
//:
//PLANNED: api for braking to stop
muos_error
muos_stepper_enable_all (void);

void
muos_stepper_disable_all (void);

void
muos_stepper_stop (uint8_t hw);

void
muos_stepper_stop_all (void);




//stepper_api:
//: .Waiting on a stepper
//: ----
//: muos_error
//: muos_stepper_wait (uint8_t hw,
//:                    enum muos_stepper_arming_state maxstate,
//:                    uint16_t timeout_sec);
//: ----
//:
//: 'muos_stepper_wait ()' waits until a steppers state is equal or less than
//: 'maxstate' or the timeout (in seconds) expires.
//:
//: returns 'muos_success' when the steppers state is reached or 'muos_warn_wait_timeout'
//: when the state stays above 'maxstate' until 'timeout' expires.
//:
muos_error
muos_stepper_wait (uint8_t hw, enum muos_stepper_arming_state maxstate, uint16_t timeout_sec);



//stepper_api:
//: .Locking the config
//: ----
//: muos_error
//: muos_stepper_lock_all (void);
//:
//: void
//: muos_stepper_unlock_all (void)
//: ----
//:
//: All higher level stepper movements need to place a read lock on the configuration. For this
//: The steppers must be first turned into ON state with 'muos_stepper_enable_all()'. Then the
//: 'muos_stepper_lock_all()' will put them in HOLD state. From there calibration movements and
//: zeroing becomes possible.
//:
//: 'muos_stepper_unlock_all()' stops all steppers, releases the configuration lock, and puts them
//: back into ON state.
//:
muos_error
muos_stepper_lock_all (void);

void
muos_stepper_unlock_all (void);


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
//: Only available when the stepper is in HOLD state. Zeroing puts a stepper
//: in ARMED state which allows fast movements.
//:
muos_error
muos_stepper_set_zero (uint8_t hw, int32_t offset);


// zeros the axis relative to the current position
//: .Query position
//: ----
//: int32_t
//: muos_stepper_position (uint8_t hw)
//: ----
//:
//: +hw+;;
//:   Stepper to query.
//:
//: Returns the current position of the given stepper.
//: When 'hw' is out of range, 0 is returned.
//:
int32_t
muos_stepper_position (uint8_t hw);



//stepper_api:
//: .Raw Movements
//: ----
//: muos_error
//: muos_stepper_move_raw (uint8_t hw,
//:                        int32_t offset,
//:                        uint16_t speed,
//:                        uint8_t prescale,
//:                        muos_queue_function done)
//: ----
//:
//: +hw+;;
//:   Stepper to control.
//: +offset+;;
//:   Distance and direction to travel.
//: +speed+;;
//;   Direct frequency generator value.
//: +prescale+;;
//;   Hardware specific prescaler selection.
//: +done+;;
//:   Function to be scheduled when done
// : +steps+;;
// :   Steps to zigzag (and pause).
// : +rep+;;
//:   Repetitions.
//:
//: Raw movement bypassing the normal logic and being functional even when there is no
//: configuration available yet.
//:
//: The 'prescale' parameter has the greatest effect on speeds and should be used with
//: uttermost care. The actual values are hardware implementation dependent.
//:
//: WARNING: wrong use of this functions can damage the hardware.
//:
//: Raw movements are used to probe stepper configuration and find maximum speeds, acceleration
//: and deceleration factors.
//:
//: muos_stepper_move_raw;;
//:   Moves for 'offset' steps at 'speed', negative values  reverse the direction.
//:
//TODO: what helper functions are needed foor low level configuration? zigzag etc.
muos_error
muos_stepper_move_raw (uint8_t hw,
                       int32_t offset,
                       uint16_t speed_raw,
                       uint8_t prescale,
                       muos_queue_function done);




//stepper_api:
//: .Calibration Movements
//: ----
//: muos_error
//: muos_stepper_move_cal (uint8_t hw,
//:                        uint16_t speed,
//:                        int32_t offset,
//:                        muos_queue_function done)
//:
//: ----
//:
//: +hw+;;
//:   Stepper to control.
//: +offset+;;
//:   Distance and direction to travel.
//: +speed+;;
//;   Direct frequency generator value.
//: +done+;;
//:   Function to be scheduled when done
//:
//: Calibration movement is needs basic machine configuration and is restricted to 'stepper_calspeed'.
//: This should be used to find end switch positions and zero the axis.
//:
muos_error
muos_stepper_move_cal (uint8_t hw,
                       int32_t offset,
                       uint16_t speed,
                       muos_queue_function done);






//stepper_api:
//: .Relative Movements
//: ----
//: muos_error
//: muos_stepper_move_rel (uint8_t hw,
//:                        unt16_t speed,
//:                        int32_t offset,
//:                        muos_queue_function done)
//:
//: ----
//:
//: +hw+;;
//:   Stepper to control.
//: +offset+;;
//:   Distance and direction to travel.
//: +speed+;;
//;   Direct frequency generator value.
//: +done+;;
//:   Function to be scheduled when done
//:
//PLANNED: use out_steps of slope, no extra state
muos_error
muos_stepper_move_rel (uint8_t hw,
                       int32_t offset,
                       uint16_t speed,
                       muos_queue_function done); //FIXME: done -> plan





/*

  Absolute movements w/ acceleration/deceleration

*/


//stepper_api:
//: .Absolute Movements
//: ----
//: muos_error
//: muos_stepper_move_start (uint8_t hw, muos_queue_function slope_gen)
//: ----
//:
//: +hw+;;
//:   Stepper to control.
//: +slope_gen+;;
//:   Function to be scheduled to generate then next move
//:
//:
muos_error
muos_stepper_move_start (uint8_t hw, muos_queue_function slope_gen);


muos_error
muos_stepper_move_abs (uint8_t hw, int32_t position, uint16_t max_speed);

//TODO: docme
uint32_t
muos_stepper_distance (uint8_t hw, int32_t position);


//stepper_api:
//: .Slope Calculations
//: ----
//: struct muos_stepper_slope*
//: muos_stepper_slope_get (uint8_t hw)
//:
//: muos_error
//: muos_stepper_slope_prep (uint8_t hw,
//:                          struct muos_stepper_slope* slope,
//:                          uint32_t distance,
//:                          uint16_t speed_in,
//:                          uint16_t max_speed,
//:                          uint16_t speed_out,
//:                          uint16_t out_steps)
//:
//: void
//: muos_stepper_slope_commit (uint8_t hw, int32_t position)
// :
// : muos_error
// : muos_stepper_slope_load (uint8_t hw,
// :                          int32_t position,
// :                          const struct muos_stepper_slope* slope)
//: ----
//:
//: Fast movements require a slope for acceleration and deceleration to be prepared.
//: Such a slope starts at speed_in, accelerates to max_speed (when there is enough
//: distance to travel), keeps the max_speed for some distance, then decelerates to
//: speed_out and finally does some 'out_steps' at constant speed_out.
//:
//: The stepper state itself keeps 2 such slopes internally, one as the active slope for the
//: current movement the other for preparing a slope in the background for the next move.
//: When a move completes it swaps buffers and executes the next move. While scheduling
//: a job to prepare the slope for the next movement.
//:
//: Besides this 2 internal buffers it is also possible for a user to prepare slope for common
//: movements which can be copied in place instead recalculating them for every move.
//:
//TODO: implement slope_load() (doing get, copy, commit)
//:
//: 'muos_stepper_slope_get()' returns a handle to the inactive slope buffer for the next move.
//: This buffer can then be initialized with 'muos_stepper_slope_prep()' or loaded with
//: an already prepared slope. When done, the buffer needs to be committed to tell the driver
//: that the slope is ready to use.
//:
//: 'muos_stepper_slope_get()' will return NULL if either the stepper is not in a state >= ARMED or did
//: not consume the previous slope yet. Zeroing/Arming invalidates the slope buffer and makes it
//: ready to load new slopes (Those error's wont happen with the slope callback).
//: When it is not possible to generate new slopes in a timely manner,the stepper driver will
//: call 'muos_stepper_stop_all()' and error out.
//:
//: 'muos_stepper_slope_prep()' initializes a slope buffer. This calculation is somewhat expensive.
//: Slope calculation is over a distance to travel, while there is a destination position stored
//: within the slope structure this gets only be set when the slope is commited or loaded.
//: +hw+;;
//:   Stepper for which the slope is.
//: +slope+;;
//:   The slope buffer to be initialized.
//: +distance+;;
//:   The absolute amounts of steps to travel (direction doesn't matter).
//: +speed_in+;;
//:   The starting speed, either less than 'stepper_slowspeed' or the speed the stepper is already
//:   moving.
//: +max_speed+;;
//:   Maximum speed for the fastest part of the slope
//: +speed_out+;;
//:   The speed at the end of the slope. must be less than 'stepper_slowspeed' when the slope is not
//:   followed by another movement in the same direction.
//: +out_steps+;;
//:   Steps done at the end at 'speed_out'.
//:
muos_error
muos_stepper_slope_prep (uint8_t hw,
                         struct muos_stepper_slope* slope,
                         uint32_t distance,
                         uint16_t speed_in,
                         uint16_t max_speed,
                         uint16_t speed_out,
                         uint16_t out_steps);


struct muos_stepper_slope*
muos_stepper_slope_get (uint8_t hw);


static inline void
muos_stepper_slope_commit (uint8_t hw, int32_t position)
{
  // no hw check because this must always be called after slope_get() which does the check
  muos_steppers[hw].slope[!muos_steppers[hw].active].position = position;
  muos_steppers[hw].ready = 1;
}

//TODO: slope_load w/ fixing destination
// : muos_error
// : muos_stepper_slope_load (uint8_t hw,
// :                          const struct muos_stepper_slope* slope)






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
//:   Implementation detail: This flag gets deleted on first hit.
//: MUOS_STEPPER_ACTION_STOP;;
//:   Immediately stop the stepper at the position.
//: MUOS_STEPPER_ACTION_SYNC;;
//:   Together with STOP: stops the stepper and puts it in WAIT state
//:   Without STOP: wakes all waiting steppers.
//: MUOS_STEPPER_HPQ_FRONT;;
//:   Use the provided argument as function to push it to the front of the hpq. This gives the
//:   highest possible priority.
//: MUOS_STEPPER_HPQ_BACK;;
//:   Use the provided argument as function to push it to the back of the hpq.
//PLANNED: untested, thus ACTION_DONE is disabled for now
// : MUOS_STEPPER_ACTION_DONE;;
// :   Provided arg is the 'done' function which gets unconditionally (of position) called when the stepper gets stopped.
//:
enum muos_stepper_actions
  {
   MUOS_STEPPER_ACTION_PERMANENT = (1<<0),
   MUOS_STEPPER_ACTION_2ND = (1<<1), //TODO: implement me
   MUOS_STEPPER_ACTION_SLOPE = (1<<2),
   MUOS_STEPPER_ACTION_STOP = (1<<3),
   MUOS_STEPPER_ACTION_SYNC = (1<<4),//TODO: implement me
   MUOS_STEPPER_HPQ_FRONT = (1<<5),
   MUOS_STEPPER_HPQ_BACK = (1<<6),
   //MUOS_STEPPER_ACTION_DONE = (1<<7),

   //PLANNED:   MUOS_STEPPER_MERGE = (1<<7),
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
//PLANNED: muos_stepper_remove_all_actions(hw)
muos_error
muos_stepper_register_action (uint8_t hw,
                              int32_t position,
                              uint8_t action,
                              uintptr_t arg);

muos_error
muos_stepper_remove_action (uint8_t hw,
                            int32_t position,
                            uint8_t action,
                            uintptr_t arg);



// functions suitable for muos_wait()

bool muos_stepper_not_moving (intptr_t hw);










//TODO:  backlash compensation
/*
**** Backlash compensation -> higher level

     Two possible strategies:

     1. speed=0
        Offset is used directly as distance to the current position.
        Can be inaccurate but is fast

     2. speed!=0
        Overshoot by offset steps, then move back offset steps with 'speed'
        Slow but accurate
*/

#endif
