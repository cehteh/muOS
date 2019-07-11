/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2018, 1019                         Christian Thäter <ct@pipapo.org>
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

#ifndef MUOS_WAIT_DEPTH
#error steppers need MUOS_WAIT_DEPTH
#endif

#define MUOS_HW_STEPPER_H
#include <muos/stepper.h>
#include <muos/muos.h>
#include <muos/hw/atmel/avr/atmega328p.h>
#include <muos/rtq.h>
#include <muos/hpq.h>
#include <muos/bgq.h>
#include <muos/configstore.h>

#include <stdlib.h>

extern const struct muos_configstore_data* muos_steppers_config_lock;

/* sync moves */
bool muos_steppers_sync;
volatile uint8_t muos_steppers_pending;

//PLANNED: dynamic prescaler change to increase range and resolution

/*
  generate phase tables and states for unipolar steppers
*/

#ifdef MUOS_STEPPER_TABLES
#define TABLE(name, ...)   static const uint8_t muos_stepper_table_##name[] = {__VA_ARGS__};
MUOS_STEPPER_TABLES;
#undef TABLE
#endif


struct muos_stepper_unipolar_state
{
  uint8_t phase;  // offset to position for correct phase
  int8_t dir_ena; // -1 reverse, 0 disabled, 1 forward
};


#define STEPDIR(...)
#define UNIPOLAR(hw, timer, slope, port, table, mask, wgm)                      \
  static struct muos_stepper_unipolar_state stepper_hw_unipolar_state_##hw;

MUOS_STEPPER_HW;
#undef STEPDIR
#undef UNIPOLAR


/*
  Overflow interrupt per stepper/timer
  Each stepper needs its own 16 bit timer
*/

#define MUOS_STEPPER_TOP(timer, wgm) MUOS_STEPPER_TOP_##timer##_##wgm
#define MUOS_STEPPER_TOP_1_14 ICR1
#define MUOS_STEPPER_TOP_1_15 OCR1A
#define MUOS_STEPPER_TOP_3_14 ICR3
#define MUOS_STEPPER_TOP_3_15 OCR3A
#define MUOS_STEPPER_TOP_4_14 ICR4
#define MUOS_STEPPER_TOP_4_15 OCR4A


static uint8_t tccrb_on[MUOS_STEPPER_NUM];

static bool
muos_hw_stepper_wait_slope (intptr_t hw)
{
  return muos_steppers[hw].ready;
}



//FIXME: only restart when loaded, how do we know a slope is loaded at the end of movement?
void
muos_hw_stepper_cont (void)
{
#define STEPPER_CONT(hw, timer, wgm)                                                                            \
  if (muos_steppers[hw].position != muos_steppers[hw].slope[muos_steppers[hw].active].position)                 \
    { /* restart stepper */                                                                                     \
      MUOS_STEPPER_TOP(timer, wgm) = muos_steppers[hw].slope[muos_steppers[hw].active].speed_in;                \
      TCCR##timer##B = tccrb_on[hw];                                                                            \
    }

#define STEPDIR(hw, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)       \
  STEPPER_CONT(hw, timer, wgm)

#define UNIPOLAR(hw, timer, slope, port, table, mask, wgm)      \
  STEPPER_CONT(hw, timer, wgm)

  MUOS_STEPPER_HW;

#undef STEPDIR
#undef UNIPOLAR
}



static void
movement_end (uint8_t hw);


static void
mock_movement (void)
{
  uint8_t hw = muos_hpq_pop ();
  //FIXME: is is really ok to wait for timeout here? let stepper_disable_all set pending to 0
  if (!muos_steppers[hw].slope_gen || muos_wait (muos_hw_stepper_wait_slope, hw, UINT16_MAX) == muos_success)
    {
      cli ();
      movement_end (hw);
      if (muos_error_pending ())
        muos_hw_stepper_disable_all ();  //FIXME: proper error handling
    }

}


static inline void
set_speed (uint8_t hw, uint16_t speed)
{
#define STEPDIR(hw_, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)      \
  case hw_: MUOS_STEPPER_TOP(timer, wgm) = speed; break;

#define UNIPOLAR(hw_, timer, slope, port, table, mask, wgm)                             \
  case hw_: MUOS_STEPPER_TOP(timer, wgm) = speed; break;

  switch (hw)
    {
      MUOS_STEPPER_HW;
    }

#undef STEPDIR
#undef UNIPOLAR
}


void
muos_hw_stepper_speed_set (uint8_t hw, uint16_t speed)
{
  cli ();
  set_speed (hw, speed);
  sei ();
}


static inline uint16_t
get_speed (uint8_t hw)
{
#define STEPDIR(hw_, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)      \
  case hw_: return MUOS_STEPPER_TOP(timer, wgm);

#define UNIPOLAR(hw_, timer, slope, port, table, mask, wgm)                             \
  case hw_: return MUOS_STEPPER_TOP(timer, wgm);

  switch (hw)
    {
    default: /* never happens but saves the return at the end, compiles smaller */
      MUOS_STEPPER_HW;
    }

#undef STEPDIR
#undef UNIPOLAR
}



static inline void
stop_timer (uint8_t hw)
{
#define STEPDIR(hw_, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)      \
  case hw_: TCCR##timer##B = 0; break;

#define UNIPOLAR(hw_, timer, slope, port, table, mask, wgm)     \
  case hw_: TCCR##timer##B = 0; break;

  switch (hw)
    {
      MUOS_STEPPER_HW;
    }
#undef STEPDIR
#undef UNIPOLAR
}


static inline void
disable_timer_interrupts (uint8_t hw)
{
#define STEPDIR(hw_, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)      \
  case hw_: TIMSK##timer &= ~_BV(TOIE##timer); break;

#define UNIPOLAR(hw_, timer, slope, port, table, mask, wgm)     \
  case hw_: TIMSK##timer &= ~_BV(TOIE##timer); break;

  switch (hw)
    {
      MUOS_STEPPER_HW;
    }
#undef STEPDIR
#undef UNIPOLAR
}


static inline uint8_t
get_shift (uint8_t hw)
{
#define STEPDIR(hw_, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)      \
  case hw_: return slope;

#define UNIPOLAR(hw_, timer, slope, port, table, mask, wgm)     \
  case hw_: return slope;

  switch (hw)
    {
    default: /* never happens but saves the return at the end, compiles smaller */
      MUOS_STEPPER_HW;
    }
#undef STEPDIR
#undef UNIPOLAR
}


static void
stepper_sync (uint8_t hw)
{
  if (muos_steppers_sync)
    {
      if (muos_steppers_pending && --muos_steppers_pending)
        {
          if (get_speed (hw) < muos_steppers_config_lock->stepper_safespeed[hw])
            muos_error_set_isr (muos_error_stepper_sync);
          else
            {
              /* stop & wait for sync */
              stop_timer (hw);
            }
        }
      else
        {
          muos_steppers_pending = MUOS_STEPPER_NUM;
#if MUOS_RTQ_LENGTH > 0
          muos_error_set_isr (muos_rtq_push_isr (muos_hw_stepper_cont, true));
#else
          muos_error_set_isr (muos_hpq_push_isr (muos_hw_stepper_cont, true));
#endif
        }
    }
}


static void
stepper_stop (uint8_t hw)
{
  stop_timer (hw);
  disable_timer_interrupts (hw);

  switch (muos_steppers[hw].state)
    {
    case MUOS_STEPPER_RAW:
      muos_steppers[hw].state = MUOS_STEPPER_ON; break;
    case MUOS_STEPPER_SLOW_CAL:
      muos_steppers[hw].state = MUOS_STEPPER_HOLD; break;
    case MUOS_STEPPER_SLOPE:
      if (get_speed(hw) < muos_steppers_config_lock->stepper_safespeed[hw])
        {
          muos_steppers[hw].state = MUOS_STEPPER_HOLD;
          break;
        } /* else fallthrough */
    case MUOS_STEPPER_SLOW:
    case MUOS_STEPPER_SLOW_REL:
      muos_steppers[hw].state = MUOS_STEPPER_ARMED; break;
    default:
      break;
    }
}



static void
movement_end (uint8_t hw)
{
  if (muos_steppers[hw].state == MUOS_STEPPER_SLOPE)
    {
      if (muos_steppers[hw].ready)
        {
          /* load new slope */
          muos_steppers[hw].active = !muos_steppers[hw].active;
          set_speed (hw, muos_steppers[hw].slope[muos_steppers[hw].active].speed_in);

          if (muos_steppers[hw].position
              == muos_steppers[hw].slope[muos_steppers[hw].active].position)
            {
              /* zero movement, stop stepper, start mock movement */
              stop_timer (hw);
              muos_error_set_isr (muos_hpq_push_arg_isr (mock_movement, hw, true));
            }
          else
            {
              muos_hw_stepper_set_direction (hw,
                                             muos_steppers[hw].slope[muos_steppers[hw].active].position
                                             > muos_steppers[hw].position);
            }

          stepper_sync (hw);
        }

      if (muos_steppers[hw].slope_gen)
        {
          if (muos_steppers[hw].ready)
            {
              muos_error_set_isr (muos_hpq_push_isr (muos_steppers[hw].slope_gen, true));
            }
          else
            {
              /* slope_gen missed to generate a slope before next interrupt */
              muos_error_set_isr (muos_error_stepper_slope);
              goto stop; // smaller than stepper_stop (hw);
            }
        }

      if (!muos_steppers[hw].ready && !muos_steppers[hw].slope_gen)
        /* finished */
        goto stop; //smaller than stepper_stop (hw);

    }

  else // !SLOPE
    {
    stop:
      stepper_stop (hw);
    }
  muos_steppers[hw].ready = false;
}



static void
position_match (uint8_t hw)
{
  for (uint8_t i=0; i<MUOS_STEPPER_POSITION_SLOTS; ++i)
    {
      struct muos_stepper_action* position_match = muos_steppers[hw].slope[muos_steppers[hw].active].position_match;
      if (muos_steppers[hw].position == position_match[i].position
          && position_match[i].action)
        {
          switch (position_match[i].action)
            {
            case MUOS_STEPPER_ACTION_CALL:
              position_match[i].callback ();
              break;
#if MUOS_RTQ_LENGTH > 0
            case MUOS_STEPPER_ACTION_RTQ:
              muos_error_set_isr (muos_rtq_push_isr (
                                                     position_match[i].callback,
                                                     true));
              break;
#endif
#if MUOS_HPQ_LENGTH > 0
            case MUOS_STEPPER_ACTION_HPQ:
              muos_error_set_isr (muos_hpq_push_isr (
                                                     position_match[i].callback,
                                                     true));
              break;
#endif
#if MUOS_BGQ_LENGTH > 0
            case MUOS_STEPPER_ACTION_BGQ:
              muos_error_set_isr (muos_bgq_push_isr (
                                                     position_match[i].callback,
                                                     true));
              break;
#endif
            default:
              muos_die();
            }
          position_match[i].action = MUOS_STEPPER_ACTION_NONE;
        }
    }
}



//FIXME: slope calculation should to be exponential to account for slope over time, not steps
//PLANNED: make slope runtime configurable (speed*config/256)

static void
stepper_isr (uint8_t hw)
{
  if (muos_steppers[hw].position
      == muos_steppers[hw].slope[muos_steppers[hw].active].position)
    {
      movement_end (hw);
      if (muos_error_pending())
        {
          muos_hw_stepper_disable_all ();
          return;
        }
    }

  position_match (hw);

  uint16_t speed = get_speed (hw);

  /* slope_calculation */
  //FIXME: already generates the 2nd step SLOPE speed (harmless but inaccurate, might be the end speed overflow when out-steps == 0)
  struct muos_stepper_slope* slope = &muos_steppers[hw].slope[muos_steppers[hw].active];
  switch (muos_steppers[hw].state)
    {
    case MUOS_STEPPER_RAW:
    case MUOS_STEPPER_SLOW_CAL:
    case MUOS_STEPPER_SLOW_REL:
      return;
    case MUOS_STEPPER_SLOW:
      return;
    case MUOS_STEPPER_SLOPE:
      {
        uint8_t shift = get_shift (hw);

        if (++slope->decel_start < 0)
          {
            speed -= speed>>shift;
            speed += muos_steppers[hw].slope_soffset;

            if (speed < slope->max_speed)
              speed = slope->max_speed;
          }
        else if (slope->decel_start <= slope->decel_steps)
          {
            speed += (speed - muos_steppers_config_lock->stepper_maxspeed[hw]
                      + (1<<shift))>>shift;
            if (speed > slope->speed_out)
              speed = slope->speed_out;
          }
        else
          {
            speed = slope->speed_out;
          }
      }
    default:
      break;
    }

  set_speed (hw, speed);
}


// per type

#define STEPDIR(hw, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)       \
  ISR(TIMER##timer##_OVF_vect)                                                          \
  {                                                                                     \
    MUOS_DEBUG_INTR_ON;                                                                 \
    muos_steppers[hw].position +=                                                       \
      (dirpol^!(PORT##dirport & _BV(PORT##dirport##dirpin)))?+1:-1;                     \
    stepper_isr (hw);                                                                   \
  }


#define UNIPOLAR(hw, timer, slope, port, table, mask, wgm)                                      \
  ISR(TIMER##timer##_OVF_vect)                                                                  \
  {                                                                                             \
    MUOS_DEBUG_INTR_ON;                                                                         \
    muos_steppers[hw].position += stepper_hw_unipolar_state_##hw.dir_ena;                       \
    uint8_t value = muos_stepper_table_##table[((uint8_t)muos_steppers[hw].position             \
                                                + stepper_hw_unipolar_state_##hw.phase)         \
                                               %sizeof(muos_stepper_table_##table)] & mask;     \
    PORT##port = (PORT##port & ~mask) | value;                                                  \
    stepper_isr (hw);                                                                           \
  }


MUOS_STEPPER_HW

#undef MOVEMENT_END_MATCH
#undef SLOPE_CALCULATION
#undef STEPDIR
#undef UNIPOLAR



/*
  Initialize all steppers
*/

#define MUOS_STEPPER_PIN_IDLE_2(timer, output)  \
  OC##timer##output##_PORT &=                   \
    ~_BV(OC##timer##output##_BIT);

#define MUOS_STEPPER_PIN_IDLE_3(timer, output)  \
  OC##timer##output##_PORT |=                   \
    _BV(OC##timer##output##_BIT);


void
muos_hw_stepper_init (void)
{
#ifdef MUOS_STEPPER_ENABLEALL_INOUT_HW
#define ENABLEALL(port, pin)                    \
  PORT##port &= ~_BV(PORT##port##pin);          \
  DDR##port |= _BV(DD##port##pin)

  MUOS_STEPPER_ENABLEALL_INOUT_HW;
#undef ENABLEALL
#endif


#define STEPDIR(hw, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)       \
  TCCR##timer##B = ((wgm&~0x3)<<(WGM##timer##2 -2));                                    \
  DDR##dirport |= _BV(DD##dirport##dirpin);                                             \
  MUOS_STEPPER_PIN_IDLE_##out_mode(timer, output);                                      \
  OC##timer##output##_DDR |=                                                            \
    _BV(OC##timer##output##_BIT);

#define UNIPOLAR(hw, timer, slope,port, table, mask, wgm)       \
  TCCR##timer##B = ((wgm&~0x3)<<(WGM##timer##2 -2));            \
  PORT##port &= ~mask;                                          \
  DDR##port |= mask;

  MUOS_STEPPER_HW;
#undef STEPDIR
#undef UNIPOLAR
}







/*
  enabling / disabling
 */


void
muos_hw_stepper_disable_all (void)
{
#ifdef MUOS_STEPPER_ENABLEALL_INOUT_HW
  // port to output, pulled down
#define ENABLEALL(port, pin)                    \
  DDR##port |= _BV(DD##port##pin);

  MUOS_STEPPER_ENABLEALL_INOUT_HW;
#undef ENABLEALL
#endif

  //TODO: set pin to defined state
#define DISABLE_TIMER(timer)                    \
  TCCR##timer##A = 0;                           \
  TCCR##timer##B = 0;                           \
  TIMSK##timer &= ~_BV(TOIE##timer);

#define STEPDIR(hw, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)       \
  DISABLE_TIMER(timer);

#define UNIPOLAR(hw, timer, slope, port, table, mask, wgm)      \
  DISABLE_TIMER(timer);                                         \
  PORT##port = (PORT##port & ~mask);

  MUOS_STEPPER_HW;
#undef DISABLE_TIMER
#undef STEPDIR
#undef UNIPOLAR
  muos_steppers_pending = 0;
}



muos_error
muos_hw_stepper_enable_all (void)
{
#ifdef MUOS_STEPPER_ENABLEALL_INOUT_HW
  bool must_wait; /* only wait when we really switch the steppers on */

#define ENABLEALL(port, pin)                                                                            \
  must_wait = (DDR##port & _BV(DD##port##pin));                                                         \
  DDR##port &= ~_BV(DD##port##pin);                                                                     \
  if (must_wait                                                                                         \
      && muos_warn_wait_timeout != muos_wait (0, 0, MUOS_CLOCK_NANOSECONDS (MUOS_STEPPER_ENABLE_NS)))   \
    {                                                                                                   \
      DDR##port |= _BV(DD##port##pin);                                                                  \
      return muos_warn_sched_depth;                                                                     \
    }                                                                                                   \
  if ((PIN##port & _BV(P##port##pin)) == 0)                                                             \
    {                                                                                                   \
      muos_stepper_disable_all ();                                                                      \
      DDR##port |= _BV(DD##port##pin);                                                                  \
      return muos_error_stepper_state;                                                                  \
    }

  MUOS_STEPPER_ENABLEALL_INOUT_HW;
#undef ENABLEALL
#endif

  // enable unipolar motors
  uint8_t value; (void) value;

#define STEPDIR(hw, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)

#define UNIPOLAR(hw, timer, slope, port, table, mask, wgm)                                      \
  value = muos_stepper_table_##table[((uint8_t)muos_steppers[hw].position                       \
                                      + stepper_hw_unipolar_state_##hw.phase)                   \
                                     %sizeof(muos_stepper_table_##table)] & mask;               \
  PORT##port = (PORT##port & ~mask) | value;

  MUOS_STEPPER_HW;
#undef STEPDIR
#undef UNIPOLAR

  return muos_success;
}





/*
 start/stop/direction
*/


muos_error
muos_hw_stepper_set_direction (uint8_t hw, bool dir)
{
  switch (hw)
    {
#define STEPDIR(hw, timer, slope, output, out_mode, wgm, dirport, dirpin, dirpol)       \
      case hw:                                                                          \
        if (dir^dirpol)                                                                 \
          PORT##dirport &= ~_BV(P##dirport##dirpin);                                    \
        else                                                                            \
          PORT##dirport |= _BV(P##dirport##dirpin);                                     \
        break;

      //TODO: dirpol for unipolar
#define UNIPOLAR(hw, timer, slope,port, table, mask, wgm)       \
      case hw:                                                  \
        if (dir)                                                \
          stepper_hw_unipolar_state_##hw.dir_ena = 1;           \
        else                                                    \
          stepper_hw_unipolar_state_##hw.dir_ena = -1;          \
        break;

      MUOS_STEPPER_HW;
#undef STEPDIR
#undef UNIPOLAR

    default:
      return muos_fatal_nodev;
    }

  //PLANNED: busy loop for very short delays, eventually we need muos function for this
  //PLANNED: only delay when direction was changed
  //FIXME: measure and adjust timing
  //FIXME: not needed, or caller needs to delay _delay_loop_2 (((MUOS_STEPPER_DIR_NS - 5250) * 70000) / F_CPU);
  return muos_success;
}







//TODO: conditional for different parts
// the values for prescalers are irregular, depending on the actual timer/hardware
// only common is that 0 means off, thus we don't handle that here [prescale-1] below
#ifdef MUOS_HW_ATMEL_ATMEGA328P_H
static const uint16_t __flash timerdividers0[] = {1,8,64,256,1024};
static const uint16_t __flash* __flash const timerdividers1 = timerdividers0;
static const uint16_t __flash* __flash const timerdividers3 = timerdividers0;
static const uint16_t __flash* __flash const timerdividers4 = timerdividers0;
static const uint16_t __flash timerdividers2[] = {1,8,32,64,128,256,1024};
#else
#error "unknown hardware"
#endif

//PLANNED: abstract F_CPU for timers running on other clock sources

muos_error
muos_hw_stepper_start (uint8_t hw, uint16_t speed_in, uint8_t prescale, bool run)
{
  switch (hw)
    {
#define TIMER_START(hw,timer, output, output_mode, wgm)                                         \
        TCCR##timer##A = (output_mode << COM##timer##output##0) | ((wgm&0x3)<<WGM##timer##0);   \
        tccrb_on[hw] = ((wgm&~0x3)<<(WGM##timer##2 -2));                                        \
        MUOS_STEPPER_TOP(timer, wgm) = speed_in;                                                \
        TCNT##timer = 0;                                                                        \
        TIFR##timer = _BV(TOV##timer);                                                          \
        TIMSK##timer = _BV(TOIE##timer);                                                        \
        tccrb_on[hw] |= prescale;                                                               \
        if (run)                                                                                \
          TCCR##timer##B = tccrb_on[hw]

#define STEPDIR(hw, timer, slope, output, output_mode, wgm, dirport, dirpin, dirpol)            \
      case hw:                                                                                  \
        OCR##timer##B =                                                                         \
          (F_CPU / (1000UL * timerdividers##timer[prescale - 1]))                               \
          * (uint32_t)MUOS_STEPPER_STEP_NS / 1000000L ;                                         \
        TIMER_START(hw, timer, output, output_mode, wgm);                                       \
        break;

#define UNIPOLAR(hw, timer, slope, port, table, mask, wgm)      \
      case hw:                                                  \
        TIMER_START(hw, timer, B, 0, wgm);                      \
        break;

      MUOS_STEPPER_HW;

#undef TIMER_START
#undef STEPDIR
#undef UNIPOLAR

    default:
      return muos_fatal_nodev;
    }

  return muos_success;
}



void
muos_hw_stepper_stop (uint8_t hw)
{
#define STEPDIR(hw, timer, slope,output, out_mode, wgm, dirport, dirpin, dirpol)        \
  case hw:                                                                              \
    TCCR##timer##A = 0;                                                                 \
    TCCR##timer##B = 0;                                                                 \
    TIMSK##timer &= ~_BV(TOIE##timer);                                                  \
  break;


#define UNIPOLAR(hw, timer, slope, port, table, mask, wgm)       \
  case hw:                                                       \
    TCCR##timer##A = 0;                                          \
    TCCR##timer##B = 0;                                          \
    TIMSK##timer &= ~_BV(TOIE##timer);                           \
    break;

  switch (hw)
    {
      MUOS_STEPPER_HW;
    }

#undef STEPDIR
#undef UNIPOLAR
}


#endif
