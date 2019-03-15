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

#include <muos/configstore.h>

#include <stdlib.h>

#define MUOS_HW_STEPPER_H
#include <muos/stepper.h>


//FIXME: 1 step didn't stop

//TODO: API for setting each axis and then start all together

struct muos_stepper_state muos_steppers[MUOS_STEPPER_NUM];

const struct muos_configstore_data* muos_steppers_config_lock;


void
muos_stepper_50init (void)
{
  muos_hw_stepper_init ();

  for(uint8_t i = 0; i < MUOS_STEPPER_NUM; ++i)
    muos_steppers[i].state = MUOS_STEPPER_OFF;
}


/*
  enable/disable
 */

void
muos_stepper_disable_all (void)
{
  muos_hw_stepper_disable_all ();
  muos_configstore_unlock (&muos_steppers_config_lock);

  for (uint8_t i=0; i<MUOS_STEPPER_NUM; ++i)
    {
      if (muos_steppers[i].state > MUOS_STEPPER_OFF)
        muos_steppers[i].state = MUOS_STEPPER_OFF;
    }
}

muos_error
muos_stepper_enable_all (void)
{
  for (uint8_t i=0; i<MUOS_STEPPER_NUM; ++i)
    {
      if (muos_steppers[i].state < MUOS_STEPPER_OFF)
        return muos_error_stepper_state;
    }

  MUOS_OK(muos_hw_stepper_enable_all ());

  for (uint8_t i=0; i<MUOS_STEPPER_NUM; ++i)
    {
      if (muos_steppers[i].state == MUOS_STEPPER_OFF)
        muos_steppers[i].state = MUOS_STEPPER_ON;
    }

  return muos_success;
}


void
muos_stepper_stop (uint8_t hw)
{
  muos_hw_stepper_stop (hw);

#if 0 //PLANNED: ACTION_DONE
  for (uint8_t i=0; i<MUOS_STEPPER_POSITION_SLOTS; ++i)
    {
      if (muos_steppers[hw].position_match[i].whattodo & MUOS_STEPPER_ACTION_DONE
          && muos_steppers[hw].position_match[i].arg)
        {
          if (muos_steppers[hw].position_match[i].whattodo & MUOS_STEPPER_HPQ_FRONT)
            muos_error_set (muos_hpq_pushfront ((muos_queue_function)
                                                muos_steppers[hw].position_match[i].arg));
          else if (muos_steppers[hw].position_match[i].whattodo & MUOS_STEPPER_HPQ_BACK)
            muos_error_set (muos_hpq_pushback ((muos_queue_function)
                                               muos_steppers[hw].position_match[i].arg));

          if (!(muos_steppers[hw].position_match[i].whattodo & MUOS_STEPPER_ACTION_PERMANENT))
            {
              muos_steppers[hw].position_match[i].whattodo = 0;
            }
        }
    }
#endif

  switch  (muos_steppers[hw].state)
    {
    case MUOS_STEPPER_RAW:
      muos_steppers[hw].state = MUOS_STEPPER_ON; break;
    case MUOS_STEPPER_SLOW_CAL:
    case MUOS_STEPPER_SLOPE:
    case MUOS_STEPPER_FAST:
      muos_steppers[hw].state = MUOS_STEPPER_HOLD; break;
    case MUOS_STEPPER_SLOW:
    case MUOS_STEPPER_SLOW_REL:
      muos_steppers[hw].state = MUOS_STEPPER_ARMED; break;
    default:
      break;
    }
}

void
muos_stepper_stop_all (void)
{
  for (uint8_t i=0; i<MUOS_STEPPER_NUM; ++i)
    muos_stepper_stop (i);
}


struct waitdata
{
  uint8_t hw;
  enum muos_stepper_arming_state maxstate;
};


static bool
stepper_wait_pred (intptr_t state)
{
  return muos_steppers[((struct waitdata*)state)->hw].state <= ((struct waitdata*)state)->maxstate;
}

muos_error
muos_stepper_wait (uint8_t hw, enum muos_stepper_arming_state maxstate, uint16_t timeout_sec)
{
  if (hw >= MUOS_STEPPER_NUM)
    return muos_error_nodev;

  struct waitdata wd = {hw, maxstate};

  return muos_wait_poll (stepper_wait_pred, (intptr_t)&wd, MUOS_CLOCK_MILLISECONDS (1), timeout_sec*1000UL);
}


static bool
stepper_waitall_pred (intptr_t state)
{
  for (uint8_t hw = 0; hw < MUOS_STEPPER_NUM; ++hw)
    {
      if (muos_steppers[hw].state > (enum muos_stepper_arming_state)state)
        return false;
    }

  return true;
}


muos_error
muos_stepper_waitall (enum muos_stepper_arming_state maxstate, uint16_t timeout_sec)
{
  return muos_wait_poll (stepper_waitall_pred, (intptr_t)maxstate, MUOS_CLOCK_MILLISECONDS (1), timeout_sec*1000UL);
}





/*
  non moving state transitions
 */


muos_error
muos_stepper_lock_all (void)
{
  if (muos_steppers_config_lock)
    return muos_error_stepper_state;

  for (uint8_t i=0; i<MUOS_STEPPER_NUM; ++i)
    {
      if (muos_steppers[i].state != MUOS_STEPPER_ON)
        return muos_error_stepper_state;
    }

  muos_steppers_config_lock = muos_configstore_lock ();

  if (!muos_steppers_config_lock)
    return muos_error_configstore;

  for (uint8_t i=0; i<MUOS_STEPPER_NUM; ++i)
    {
      if (muos_steppers[i].state == MUOS_STEPPER_ON)
        muos_steppers[i].state = MUOS_STEPPER_HOLD;
    }

  return muos_success;
}


void
muos_stepper_unlock_all (void)
{
  if (muos_steppers_config_lock)
    {
      for (uint8_t i=0; i<MUOS_STEPPER_NUM; ++i)
        {
          if (muos_steppers[i].state > MUOS_STEPPER_HOLD)
            muos_stepper_stop (i);
        }

      muos_configstore_unlock (&muos_steppers_config_lock);

      for (uint8_t i=0; i<MUOS_STEPPER_NUM; ++i)
        {
          if (muos_steppers[i].state > MUOS_STEPPER_ON)
            muos_steppers[i].state = MUOS_STEPPER_ON;
        }
    }
}



muos_error
muos_stepper_set_zero (uint8_t hw, int32_t offset)
{
  if (hw >= MUOS_STEPPER_NUM)
    return muos_error_nodev;

  if (muos_steppers[hw].state != MUOS_STEPPER_HOLD)
    return muos_error_stepper_state;

  if (!muos_steppers_config_lock)
    return muos_error_configstore;  //FIXME: refine configstore errors

  muos_steppers[hw].position -= offset;
  muos_steppers[hw].ready = false;

  for (uint8_t i=0; i<MUOS_STEPPER_POSITION_SLOTS; ++i)
    {
      muos_steppers[hw].position_match[i].position -= offset;
    }

  //FIXME: phase correction for UP steppers

  muos_steppers[hw].state = MUOS_STEPPER_ARMED;

  return muos_success;
}


int32_t
muos_stepper_position (uint8_t hw)
{
  if (hw >= MUOS_STEPPER_NUM)
    return 0;

  muos_interrupt_disable ();
  int32_t pos = muos_steppers[hw].position;
  muos_interrupt_enable ();

  return pos;
}




/*
  raw movements
 */



muos_error
muos_stepper_move_raw (uint8_t hw,
                       int32_t offset,
                       uint16_t speed_raw,
                       uint8_t prescale,
                       muos_queue_function done)
{
  if (hw >= MUOS_STEPPER_NUM)
    return muos_error_nodev;

  if (muos_steppers[hw].state != MUOS_STEPPER_ON)
    return muos_error_stepper_state;

  if (offset)
    {
      muos_hw_stepper_set_direction (hw, offset>0?1:0);
      muos_stepper_register_action (hw,
                                    muos_steppers[hw].position + offset,
                                    MUOS_STEPPER_ACTION_STOP|(done?(MUOS_STEPPER_HPQ_BACK/*|MUOS_STEPPER_ACTION_DONE*/):0),
                                    (uintptr_t)done);

      muos_steppers[hw].state = MUOS_STEPPER_RAW;

      muos_hw_stepper_start (hw, speed_raw, prescale);
    }
  else
    {
      muos_error_set (muos_hpq_pushback (done));
    }

  return muos_success;
}



/*
  cal movements
 */

muos_error
muos_stepper_move_cal (uint8_t hw,
                       int32_t offset,
                       uint16_t speed,
                       muos_queue_function done)
{
  if (hw >= MUOS_STEPPER_NUM)
    return muos_error_nodev;

  if (muos_steppers[hw].state != MUOS_STEPPER_HOLD
      || !muos_steppers_config_lock)
    return muos_error_stepper_state;

  if (speed < muos_steppers_config_lock->stepper_calspeed[hw])
    return muos_error_stepper_range;

  if (offset)
    {
      muos_hw_stepper_set_direction (hw, offset>0?1:0);
      muos_stepper_register_action (hw,
                                    muos_steppers[hw].position + offset,
                                    MUOS_STEPPER_ACTION_STOP|(done?(MUOS_STEPPER_HPQ_BACK/*|MUOS_STEPPER_ACTION_DONE*/):0),
                                    (uintptr_t)done);

      muos_steppers[hw].state = MUOS_STEPPER_SLOW_CAL;

      muos_hw_stepper_start (hw, speed, muos_steppers_config_lock->stepper_prescale[hw]);
    }
  else
    {
      muos_error_set (muos_hpq_pushback (done));
    }

  return muos_success;
}



/*
  relative movements
 */


muos_error
muos_stepper_move_rel (uint8_t hw,
                       int32_t offset,
                       uint16_t speed,
                       muos_queue_function done)
{
  if (hw >= MUOS_STEPPER_NUM)
    return muos_error_nodev;

  if (muos_steppers[hw].state != MUOS_STEPPER_ARMED
      || !muos_steppers_config_lock)
    return muos_error_stepper_state;

  if (speed < muos_steppers_config_lock->stepper_slowspeed[hw])
    speed = muos_steppers_config_lock->stepper_slowspeed[hw];

  if (offset)
    {
      muos_hw_stepper_set_direction (hw, offset>0?1:0);
      muos_stepper_register_action (hw,
                                    muos_steppers[hw].position + offset,
                                    MUOS_STEPPER_ACTION_STOP|(done?(MUOS_STEPPER_HPQ_BACK/*|MUOS_STEPPER_ACTION_DONE*/):0),
                                    (uintptr_t)done);

      muos_steppers[hw].state = MUOS_STEPPER_SLOW_REL;

      muos_hw_stepper_start (hw, speed, muos_steppers_config_lock->stepper_prescale[hw]);
    }
  else
    {
      muos_error_set (muos_hpq_pushback (done));
    }

  return muos_success;
}



/*
  absolute movement
  Slope calculations
*/

static uint32_t
slope_speed (uint16_t position,
             uint16_t slope_len,
             uint16_t accel,
             uint16_t decel)
{
  return
    (accel * MUOS_STEPPER_SLOPE_MULTIPLIER / position)
    +
    (decel * MUOS_STEPPER_SLOPE_MULTIPLIER / (slope_len-position));
}

/* finds the position which is just equal or faster than speed */
static uint16_t
slope_bisect (uint16_t speed, uint16_t slope_len, uint16_t fact_a, uint16_t fact_b)
{
  uint16_t mid;
  uint32_t tmp_speed;
  uint16_t low;
  uint16_t high;

  for (high = 1; high < slope_len; high = high < slope_len/2? high*2:slope_len)
    {
      tmp_speed = slope_speed (high, slope_len,fact_a, fact_b);
      if (tmp_speed < speed)
        break;

#if defined(MUOS_SCHED_DEPTH)               \
  && defined(MUOS_STEPPER_SLOPE_YIELD_ITER) \
  && MUOS_STEPPER_SLOPE_YIELDS > 0
      muos_yield (MUOS_STEPPER_SLOPE_YIELDS);
#endif
    }

#if defined(MUOS_SCHED_DEPTH)                   \
  && !defined(MUOS_STEPPER_SLOPE_YIELD_ITER)    \
  && MUOS_STEPPER_SLOPE_YIELDS > 0
  muos_yield (MUOS_STEPPER_SLOPE_YIELDS);
#endif

  low = high/2;

  while (high > low+1)
    {
      mid = (high-low)/2+low;
      tmp_speed = slope_speed (mid, slope_len, fact_a, fact_b);

      if (tmp_speed < speed)
        {
          high = mid;
        }
      else if (tmp_speed > speed)
        {
          low = mid;
        }
      else
        break;

#if defined(MUOS_SCHED_DEPTH)               \
  && defined(MUOS_STEPPER_SLOPE_YIELD_ITER) \
  && MUOS_STEPPER_SLOPE_YIELDS > 0
      muos_yield (MUOS_STEPPER_SLOPE_YIELDS);
#endif
    }
#if defined(MUOS_SCHED_DEPTH)                   \
  && !defined(MUOS_STEPPER_SLOPE_YIELD_ITER)    \
  && MUOS_STEPPER_SLOPE_YIELDS > 0
  muos_yield (MUOS_STEPPER_SLOPE_YIELDS);
#endif

  return high;
}


//FIXME: off by one error at end of slope, last step is too fast
muos_error
muos_stepper_slope_prep (uint8_t hw,
                         struct muos_stepper_slope* slope,
                         uint32_t distance,
                         uint16_t speed_in,
                         uint16_t max_speed,
                         uint16_t speed_out,
                         uint16_t out_steps)
{
  if (hw >= MUOS_STEPPER_NUM)
    return muos_error_nodev;

  if (!slope)
    return muos_error_stepper_state;

  if (!muos_steppers_config_lock)
    return muos_error_configstore;  //FIXME: refine configstore errors

  if (max_speed < muos_steppers_config_lock->stepper_maxspeed[hw])
    max_speed = muos_steppers_config_lock->stepper_maxspeed[hw];

  if (out_steps > distance)
    out_steps = distance;  //TODO: fast return only out_steps w/o fast or slope

  slope->len =
    (distance - out_steps) < muos_steppers_config_lock->stepper_maxslope[hw]
    ? distance-out_steps
    : muos_steppers_config_lock->stepper_maxslope[hw];

  // find start and end offset, that is where speed_in/out is on the slope_len
  slope->pos = slope_bisect (speed_in - max_speed,
                             slope->len,
                             muos_steppers_config_lock->stepper_accel[hw],
                             muos_steppers_config_lock->stepper_decel[hw]);

  slope->end = slope->len - (slope_bisect (speed_out - max_speed,
                                           slope->len,
                                           muos_steppers_config_lock->stepper_decel[hw],
                                           muos_steppers_config_lock->stepper_accel[hw]));

  slope->speed_out = speed_out;
  slope->constant = distance - (slope->end - slope->pos) - out_steps;
  slope->max_speed = max_speed;

  return muos_success;
}


struct muos_stepper_slope*
muos_stepper_slope_get (uint8_t hw)
{
  if (hw >= MUOS_STEPPER_NUM
      || muos_steppers[hw].ready
      || muos_steppers[hw].state < MUOS_STEPPER_ARMED)
    return NULL;

  return &muos_steppers[hw].slope[!muos_steppers[hw].active];
}



/*
  absolute movements
 */

muos_error
muos_stepper_move_start (uint8_t hw, muos_queue_function slope_gen)
{
  if (hw >= MUOS_STEPPER_NUM)
    return muos_error_nodev;

  if (muos_steppers[hw].state != MUOS_STEPPER_ARMED
      || !muos_steppers_config_lock
      || !muos_steppers[hw].ready)
    return muos_error_stepper_state;


  // load buffer
  muos_steppers[hw].active = !muos_steppers[hw].active;
  muos_steppers[hw].ready = false;
  muos_steppers[hw].slope_gen = slope_gen;

  int32_t position = muos_steppers[hw].slope[muos_steppers[hw].active].position;

  if (position != muos_steppers[hw].position)
    {
      muos_hw_stepper_set_direction (hw, position>muos_steppers[hw].position?1:0);

      muos_stepper_register_action (hw,
                                    position,
                                    slope_gen?MUOS_STEPPER_ACTION_SLOPE:MUOS_STEPPER_ACTION_STOP,
                                    0);

      muos_steppers[hw].state = MUOS_STEPPER_SLOPE;

      MUOS_OK (muos_hw_stepper_start (hw,
                                      muos_steppers_config_lock->stepper_slowspeed[hw],
                                      muos_steppers_config_lock->stepper_prescale[hw]));
    }

  if (slope_gen)
    MUOS_OK (muos_hpq_pushback (slope_gen));

  return muos_success;
}


uint32_t
muos_stepper_distance (uint8_t hw, int32_t position)
{
  if (hw >= MUOS_STEPPER_NUM)
    return 0;

  return (position > muos_steppers[hw].position)
    ? position - muos_steppers[hw].position
    : muos_steppers[hw].position - position;
}


muos_error
muos_stepper_move_abs (uint8_t hw, int32_t position, uint16_t max_speed)
{
  if (hw >= MUOS_STEPPER_NUM)
    return muos_error_nodev;

  struct muos_stepper_slope* slope = muos_stepper_slope_get (hw);

  if (!slope)
    return muos_error_stepper_slope;

  uint32_t distance = muos_stepper_distance(hw, position);

  MUOS_OK (muos_stepper_slope_prep (hw,
                                    slope,
                                    distance,
                                    muos_steppers_config_lock->stepper_slowspeed[hw],
                                    max_speed,
                                    muos_steppers_config_lock->stepper_slowspeed[hw],
                                    0));

  muos_stepper_slope_commit (hw, position);
  MUOS_OK (muos_stepper_move_start (hw, NULL));

  return muos_success;
}



/*
  action registry
 */

muos_error
muos_stepper_register_action (uint8_t hw,
                              int32_t position,
                              uint8_t action,
                              uintptr_t arg)
{
  if (hw >= MUOS_STEPPER_NUM)
    return muos_error_nodev;

  for (uint8_t i=0; i<MUOS_STEPPER_POSITION_SLOTS; ++i)
    {
      if (!muos_steppers[hw].position_match[i].whattodo)
        {
          muos_steppers[hw].position_match[i].position = position;
          muos_steppers[hw].position_match[i].whattodo = action;
          muos_steppers[hw].position_match[i].arg = arg;
          return muos_success;
        }
    }

  return muos_error_stepper_noslot;
}


muos_error
muos_stepper_remove_action (uint8_t hw,
                            int32_t position,
                            uint8_t action,
                            uintptr_t arg)
{
  if (hw >= MUOS_STEPPER_NUM)
    return muos_error_nodev;

  for (uint8_t i=0; i<MUOS_STEPPER_POSITION_SLOTS; ++i)
    {
      if (muos_steppers[hw].position_match[i].whattodo == action
          && muos_steppers[hw].position_match[i].position == position
          && muos_steppers[hw].position_match[i].arg == arg)
        {
          muos_steppers[hw].position_match[i].whattodo = 0;
          return muos_success;
        }
    }

  return muos_error_stepper_noslot;
}





bool
muos_stepper_not_moving (intptr_t hw)
{
  if (hw >= MUOS_STEPPER_NUM)
    return false;

  return muos_steppers[hw].state < MUOS_STEPPER_RAW;
}



#endif

