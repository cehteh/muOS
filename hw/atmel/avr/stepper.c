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

#ifndef MUOS_SCHED_DEPTH
#error steppers need MUOS_SCHED_DEPTH
#endif

#define MUOS_HW_STEPPER_H
#include <muos/stepper.h>
#include <muos/muos.h>
#include <muos/hw/atmel/avr/atmega328p.h>
#include <muos/hpq.h>
#include <muos/configstore.h>

#include <util/delay_basic.h>
#include <stdlib.h>

extern const struct muos_configstore_data* muos_steppers_config_lock;


#ifdef MUOS_STEPPER_SPEED_BACKOFF
static bool stepper_backoff;                     // speed backoff in effect
static uint8_t stepper_backoff_cnt;              // delayed isr counter
#endif

//PLANNED: dynamic prescaler change to increase range and resolution

// unsigned 32bit/32bit division with 16bit result
__attribute__((always_inline))
static inline uint16_t
udiv_inline (uint32_t num, uint32_t den)
{
  uint32_t mask = 1;
  uint16_t res = 0;

  while (mask && den < num  && !(den & (1L<<31)))
    {
      den <<= 1;
      mask <<= 1;
    }

  while (mask)
    {
      if (num >= den)
	{
	  num -= den;
          res |= mask;
	}
      mask >>=1;
      den >>=1;
    }
  return res;
}





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
#define UNIPOLAR(hw, timer, port, table, mask, wgm)                             \
  static struct muos_stepper_unipolar_state stepper_hw_unipolar_state_##hw;

MUOS_STEPPER_HW;
#undef STEPDIR
#undef UNIPOLAR


/*
  Overflow interrupt per stepper/timer
  Each stepper needs its own 16 bit timer
*/

//boilerplate
#define POSITION_MATCH(hw, timer, wgm)                                                                  \
  for (uint8_t i=0; i<MUOS_STEPPER_POSITION_SLOTS; ++i)                                                 \
    {                                                                                                   \
      if (muos_steppers[hw].position == muos_steppers[hw].position_match[i].position                    \
          && muos_steppers[hw].position_match[i].whattodo)                                              \
        {                                                                                               \
          if (muos_steppers[hw].position_match[i].whattodo                                              \
              & MUOS_STEPPER_ACTION_SLOPE)                                                              \
            {                                                                                           \
              if (!muos_steppers[hw].ready)                                                             \
                {                                                                                       \
                  muos_error_set_isr (muos_error_stepper_slope);                                        \
                  muos_hw_stepper_disable_all ();                                                       \
                  return;                                                                               \
                  }                                                                                     \
                  muos_steppers[hw].active = !muos_steppers[hw].active;                                 \
                  muos_steppers[hw].ready = false;                                                      \
                  muos_steppers[hw].position_match[i].position                                          \
                  = muos_steppers[hw].slope[muos_steppers[hw].active].position;                         \
                  muos_hw_stepper_set_direction (hw,                                                    \
                                                 muos_steppers[hw].position_match[i].position           \
                                                 > muos_steppers[hw].position);                         \
                  muos_error_set_isr (muos_hpq_pushback_isr (muos_steppers[hw].slope_gen, true));       \
                  continue;                                                                             \
                  }                                                                                     \
          if (muos_steppers[hw].position_match[i].whattodo                                              \
              & MUOS_STEPPER_ACTION_STOP)                                                               \
            {                                                                                           \
              TCCR##timer##A = 0;                                                                       \
              TCCR##timer##B = 0;                                                                       \
              TIMSK##timer &= ~_BV(TOIE##timer);                                                        \
              switch (muos_steppers[hw].state)                                                          \
                {                                                                                       \
                case MUOS_STEPPER_RAW:                                                                  \
                  muos_steppers[hw].state = MUOS_STEPPER_ON; break;                                     \
                case MUOS_STEPPER_SLOW_CAL:                                                             \
                  muos_steppers[hw].state = MUOS_STEPPER_HOLD; break;                                   \
                case MUOS_STEPPER_SLOPE:                                                                \
                case MUOS_STEPPER_FAST:                                                                 \
                  if (MUOS_STEPPER_TOP(timer, wgm) < muos_steppers_config_lock->stepper_slowspeed[hw])  \
                    { muos_steppers[hw].state = MUOS_STEPPER_HOLD; break;}                              \
                  /* else fallthrough */                                                                \
                case MUOS_STEPPER_SLOW:                                                                 \
                case MUOS_STEPPER_SLOW_REL:                                                             \
                  muos_steppers[hw].state = MUOS_STEPPER_ARMED; break;                                  \
                default:                                                                                \
                  break;                                                                                \
                }                                                                                       \
            }                                                                                           \
          if (muos_steppers[hw].position_match[i].arg)                                                  \
            {                                                                                           \
              if (muos_steppers[hw].position_match[i].whattodo                                          \
                  & MUOS_STEPPER_HPQ_FRONT)                                                             \
                muos_error_set_isr (muos_hpq_pushfront_isr ((muos_queue_function)                       \
                                                            muos_steppers[hw].position_match[i].arg,    \
                                                            true));                                     \
              else if (muos_steppers[hw].position_match[i].whattodo                                     \
                       & MUOS_STEPPER_HPQ_BACK)                                                         \
                muos_error_set_isr (muos_hpq_pushback_isr ((muos_queue_function)                        \
                                                           muos_steppers[hw].position_match[i].arg,     \
                                                           true));                                      \
            }                                                                                           \
          if (!(muos_steppers[hw].position_match[i].whattodo & MUOS_STEPPER_ACTION_PERMANENT))          \
            {                                                                                           \
              muos_steppers[hw].position_match[i].whattodo = 0;                                         \
            }                                                                                           \
        }                                                                                               \
    }


#define SLOPE_CALCULATION(hw)                                                                           \
  uint16_t speed=0;                                                                                     \
  struct muos_stepper_slope* slope = &muos_steppers[hw].slope[muos_steppers[hw].active];                \
  switch (muos_steppers[hw].state)                                                                      \
    {                                                                                                   \
      /*TODO:  case MUOS_STEPPER_STOPPING: */                                                           \
    case MUOS_STEPPER_RAW:                                                                              \
    case MUOS_STEPPER_SLOW_CAL:                                                                         \
    case MUOS_STEPPER_SLOW_REL:                                                                         \
      return;                                                                                           \
    case MUOS_STEPPER_SLOW:                                                                             \
    case MUOS_STEPPER_FAST:                                                                             \
      if (slope->constant && !--slope->constant)                                                        \
        {                                                                                               \
          muos_steppers[hw].state = MUOS_STEPPER_SLOPE;                                                 \
        }                                                                                               \
      return;                                                                                           \
    case MUOS_STEPPER_SLOPE:                                                                            \
      speed =                                                                                           \
        udiv_inline (muos_steppers_config_lock->stepper_accel[hw] * MUOS_STEPPER_SLOPE_MULTIPLIER,      \
                     slope->pos)                                                                        \
        +                                                                                               \
        udiv_inline (muos_steppers_config_lock->stepper_decel[hw] * MUOS_STEPPER_SLOPE_MULTIPLIER,      \
                     slope->len - slope->pos)                                                           \
        +                                                                                               \
        slope->max_speed;                                                                               \
      uint16_t old_flt =  muos_steppers[hw].speed_flt;                                                  \
      muos_steppers[hw].speed_flt = (muos_steppers[hw].speed_flt/2 + speed/2)                           \
        + ((uint8_t)muos_steppers[hw].speed_flt & (uint8_t)speed & (uint8_t)1);                         \
      if (slope->constant && muos_steppers[hw].speed_flt > old_flt)                                     \
        {                                                                                               \
          --speed;                                                                                      \
          muos_steppers[hw].state = MUOS_STEPPER_FAST;                                                  \
        }                                                                                               \
      else if (slope->pos == slope->end)                                                                \
        {                                                                                               \
          speed = slope->speed_out;                                                                     \
          muos_steppers[hw].state = MUOS_STEPPER_FAST;                                                  \
        }                                                                                               \
      else                                                                                              \
        ++slope->pos;                                                                                   \
    default:                                                                                            \
      break;                                                                                            \
    }



//PLANNED: see if steppers run smoother when using filtered speed


// per type
#define MUOS_STEPPER_TOP(timer, wgm) MUOS_STEPPER_TOP_##timer##_##wgm
#define MUOS_STEPPER_TOP_1_14 ICR1
#define MUOS_STEPPER_TOP_1_15 OCR1A
#define MUOS_STEPPER_TOP_3_14 ICR3
#define MUOS_STEPPER_TOP_3_15 OCR3A
#define MUOS_STEPPER_TOP_4_14 ICR4
#define MUOS_STEPPER_TOP_4_15 OCR4A


#ifdef MUOS_STEPPER_SPEED_BACKOFF
static inline bool
isr_load_check (void)
{
  uint8_t steppers_active = 0;
#define STEPDIR(hw, timer, output, out_mode, wgm, dirport, dirpin, dirpol)      \
  if (stepper_backoff_cnt < UINT8_MAX)                                          \
    stepper_backoff_cnt += !!(TIFR##timer & TOV##timer);                        \
  if (muos_steppers[hw].state >= MUOS_STEPPER_RAW)                              \
    ++steppers_active;

#define UNIPOLAR(hw, timer, port, table, mask, wgm)                             \
  if (stepper_backoff_cnt < UINT8_MAX)                                          \
    stepper_backoff_cnt += !!(TIFR##timer & TOV##timer);                        \
  if (muos_steppers[hw].state >= MUOS_STEPPER_RAW)                              \
    ++steppers_active;

  MUOS_STEPPER_HW;

#undef STEPDIR
#undef UNIPOLAR

  return stepper_backoff_cnt >= steppers_active;
}


// set speed backoff when same or more then STEPPER_NUM interrupts are pending
// clear backoff when speed falls again
#define SET_SPEED(timer, wgm)                                                   \
  if (stepper_backoff)                                                          \
    {                                                                           \
      if (speed > MUOS_STEPPER_TOP(timer, wgm))                                 \
        {                                                                       \
          muos_error_check_isr (muos_warn_stepper_backoff);                     \
          stepper_backoff = false;                                              \
        }                                                                       \
    }                                                                           \
  else                                                                          \
    {                                                                           \
      if (isr_load_check ())                                                    \
        {                                                                       \
          muos_error_set_isr (muos_warn_stepper_backoff);                       \
          stepper_backoff = true;                                               \
        }                                                                       \
    }                                                                           \
  if (!stepper_backoff || speed >= slope->speed_out)                            \
    {                                                                           \
      MUOS_STEPPER_TOP(timer, wgm) = speed;                                     \
    }                                                                           \
  if (stepper_backoff_cnt)                                                      \
    --stepper_backoff_cnt

#else

#define SET_SPEED(timer, wgm)                                                   \
  MUOS_STEPPER_TOP(timer, wgm) = speed

#endif




#define STEPDIR(hw, timer, output, out_mode, wgm, dirport, dirpin, dirpol)      \
  ISR(TIMER##timer##_OVF_vect)                                                  \
  {                                                                             \
    MUOS_DEBUG_INTR_ON;                                                         \
    muos_steppers[hw].position +=                                               \
      (dirpol^!(PORT##dirport & _BV(PORT##dirport##dirpin)))?+1:-1;             \
    POSITION_MATCH(hw, timer, wgm);                                             \
    SLOPE_CALCULATION(hw);                                                      \
    SET_SPEED(timer, wgm);                                                      \
  }


#define UNIPOLAR(hw, timer, port, table, mask, wgm)                                             \
  ISR(TIMER##timer##_OVF_vect)                                                                  \
  {                                                                                             \
    MUOS_DEBUG_INTR_ON;                                                                         \
    muos_steppers[hw].position += stepper_hw_unipolar_state_##hw.dir_ena;                       \
    uint8_t value = muos_stepper_table_##table[((uint8_t)muos_steppers[hw].position             \
                                                + stepper_hw_unipolar_state_##hw.phase)         \
                                               %sizeof(muos_stepper_table_##table)] & mask;     \
    PORT##port = (PORT##port & ~mask) | value;                                                  \
    POSITION_MATCH(hw, timer, wgm);                                                             \
    SLOPE_CALCULATION(hw);                                                                      \
    SET_SPEED(timer, wgm);                                                                      \
  }


MUOS_STEPPER_HW

#undef POSITION_MATCH
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


#define STEPDIR(hw, timer, output, out_mode, wgm, dirport, dirpin, dirpol)      \
  TCCR##timer##B = ((wgm&~0x3)<<(WGM##timer##2 -2));                            \
  DDR##dirport |= _BV(DD##dirport##dirpin);                                     \
  MUOS_STEPPER_PIN_IDLE_##out_mode(timer, output);                              \
  OC##timer##output##_DDR |=                                                    \
    _BV(OC##timer##output##_BIT);

#define UNIPOLAR(hw, timer, port, table, mask, wgm)     \
  TCCR##timer##B = ((wgm&~0x3)<<(WGM##timer##2 -2));    \
  PORT##port &= ~mask;                                  \
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

#define STEPDIR(hw, timer, output, out_mode, wgm, dirport, dirpin, dirpol) \
  DISABLE_TIMER(timer);

#define UNIPOLAR(hw, timer, port, table, mask, wgm)     \
  DISABLE_TIMER(timer);                                 \
  PORT##port = (PORT##port & ~mask);

  MUOS_STEPPER_HW;
#undef DISABLE_TIMER
#undef STEPDIR
#undef UNIPOLAR

  /* clear backkoff warning */
  muos_error_check (muos_warn_stepper_backoff);
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

#define STEPDIR(hw, timer, output, out_mode, wgm, dirport, dirpin, dirpol)

#define UNIPOLAR(hw, timer, port, table, mask, wgm)                                             \
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
#define STEPDIR(hw, timer, output, out_mode, wgm, dirport, dirpin, dirpol)      \
      case hw:                                                                  \
        if (dir^dirpol)                                                         \
          PORT##dirport &= ~_BV(P##dirport##dirpin);                            \
        else                                                                    \
          PORT##dirport |= _BV(P##dirport##dirpin);                             \
        break;

      //TODO: dirpol for unipolar
#define UNIPOLAR(hw, timer, port, table, mask, wgm)     \
      case hw:                                          \
        if (dir)                                        \
           stepper_hw_unipolar_state_##hw.dir_ena = 1;  \
        else                                            \
          stepper_hw_unipolar_state_##hw.dir_ena = -1;  \
        break;

      MUOS_STEPPER_HW;
#undef STEPDIR
#undef UNIPOLAR

    default:
      return muos_error_nodev;
    }

  //PLANNED: busy loop for very short delays, eventually we need muos function for this
  //PLANNED: only delay when direction was changed
  //FIXME: measure and adjust timing
  _delay_loop_2 (((MUOS_STEPPER_DIR_NS - 5250) * 70000) / F_CPU);
  return muos_success;
}







//TODO: conditional for different parts
//TODO: progmem
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
muos_hw_stepper_start (uint8_t hw, uint16_t speed_in, uint8_t prescale)
{
  switch (hw)
    {
#define TIMER_START(timer, output, output_mode, wgm)                                            \
        TCCR##timer##A = (output_mode << COM##timer##output##0) | ((wgm&0x3)<<WGM##timer##0);   \
        TCCR##timer##B = ((wgm&~0x3)<<(WGM##timer##2 -2));                                      \
        MUOS_STEPPER_TOP(timer, wgm) = speed_in;                                                \
        TCNT##timer = 0;                                                                        \
        TIFR##timer = _BV(TOV##timer);                                                          \
        TIMSK##timer = _BV(TOIE##timer);                                                        \
        TCCR##timer##B |= prescale                                                              \

#define STEPDIR(hw, timer, output, output_mode, wgm, dirport, dirpin, dirpol)                   \
      case hw:                                                                                  \
        OCR##timer##B =                                                                         \
          (F_CPU / (1000UL * timerdividers##timer[prescale - 1]))                               \
          * (uint32_t)MUOS_STEPPER_STEP_NS / 1000000L ;                                         \
        muos_steppers[hw].speed_flt = 65535;                                                    \
        TIMER_START(timer, output, output_mode, wgm);                                           \
        break;

#define UNIPOLAR(hw, timer, port, table, mask, wgm)     \
      case hw:                                          \
        muos_steppers[hw].speed_flt = 65535;            \
        TIMER_START(timer, B, 0, wgm);                  \
        break;

      MUOS_STEPPER_HW;

#undef TIMER_START
#undef STEPDIR
#undef UNIPOLAR

    default:
      return muos_error_nodev;
    }

  return muos_success;
}



void
muos_hw_stepper_stop (uint8_t hw)
{
#define STEPDIR(hw, timer, output, out_mode, wgm, dirport, dirpin, dirpol)      \
  case hw:                                                                      \
  TCCR##timer##A = 0;                                                           \
  TCCR##timer##B = 0;                                                           \
  TIMSK##timer &= ~_BV(TOIE##timer);                                            \
  break;


#define UNIPOLAR(hw, timer, port, table, mask, wgm)     \
  case hw:                                              \
  TCCR##timer##A = 0;                                   \
  TCCR##timer##B = 0;                                   \
  TIMSK##timer &= ~_BV(TOIE##timer);                    \
  break;

  switch (hw)
    {
      MUOS_STEPPER_HW;
    }

#undef STEPDIR
#undef UNIPOLAR
}


#endif

