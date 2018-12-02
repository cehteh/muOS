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

#ifndef MUOS_HW_STEPPER_PP_H
#define MUOS_HW_STEPPER_PP_H

#include <muos/pp.h>

/*
  only macro definitions for the code generation here
*/


#define GET_DIRECTION_(port, pin, inv) (inv^!(PORT##port & _BV(PORT##port##pin)))
#define GET_DIRECTION(hw) GET_DIRECTION_ hw

/*
  ISR
*/

#define MAKE_OVF_ISR__(index, timer, output, output_mode, wgm)                                                  \
  ISR(TIMER##timer##_OVF_vect)                                                                                  \
  {                                                                                                             \
    muos_steppers[index].position += GET_DIRECTION(MUOS_STEPPER_DIR_HW)?+1:-1;                                  \
    for (uint8_t i=0; i<MUOS_STEPPER_POSITION_SLOTS; ++i)                                                       \
      {                                                                                                         \
        if (muos_steppers[index].position == muos_steppers[index].position_match[i].position                    \
            && muos_steppers[index].position_match[i].whattodo)                                                 \
          {                                                                                                     \
            if (muos_steppers[index].position_match[i].whattodo                                                 \
                & MUOS_STEPPER_ACTION_STOP)                                                                     \
              {                                                                                                 \
                TCCR##timer##A = 0;                                                                             \
                TCCR##timer##B = 0;                                                                             \
                TIMSK##timer &= ~_BV(TOIE##timer);                                                              \
              }                                                                                                 \
            if (muos_steppers[index].position_match[i].arg)                                                     \
              {                                                                                                 \
                if (muos_steppers[index].position_match[i].whattodo                                             \
                    & MUOS_STEPPER_HPQ_FRONT)                                                                   \
                  muos_error_set_isr (muos_hpq_pushfront_isr ((muos_queue_function)                             \
                                                              muos_steppers[index].position_match[i].arg));     \
                else if (muos_steppers[index].position_match[i].whattodo                                        \
                         & MUOS_STEPPER_HPQ_BACK)                                                               \
                  muos_error_set_isr (muos_hpq_pushback_isr ((muos_queue_function)                              \
                                                             muos_steppers[index].position_match[i].arg));      \
              }                                                                                                 \
            if (!(muos_steppers[index].position_match[i].whattodo & MUOS_STEPPER_ACTION_PERMANENT))             \
              {                                                                                                 \
                muos_steppers[index].position_match[i].whattodo = 0;                                            \
              }                                                                                                 \
          }                                                                                                     \
      }                                                                                                         \
   }


#define MAKE_OVF_ISR_(...) MAKE_OVF_ISR__ (__VA_ARGS__)
#define MAKE_OVF_ISR(index, hw) MAKE_OVF_ISR_ (index, MUOS_PP_LIST_EXPAND(hw))


/*
  stepper_init()
*/

// note TCCRxA is better to set when starting and cleared when stopping
#define MUOS_STEPPER_INIT_TCCRB_IMPL(timer, output, output_mode, wgm)   \
  TCCR##timer##B = ((wgm&~0x3)<<(WGM##timer##2 -2));

#define MAKE_STEPPER_INIT_TCCRB(index, hw)                     \
  MUOS_STEPPER_INIT_TCCRB_IMPL hw

// set pin to idle state when not running, note output_modes 1 and 2 are not defined
#define MUOS_STEPPER_PIN_IDLE_2(timer, output)  \
  OC##timer##output##_PORT &=                   \
    ~_BV(OC##timer##output##_BIT);
#define MUOS_STEPPER_PIN_IDLE_3(timer, output)  \
  OC##timer##output##_PORT |=                   \
    _BV(OC##timer##output##_BIT);


#ifdef MUOS_STEPPER_DISABLEALL_INOUT_HW
// first set the drive polarity then set it to output

#define MUOS_STEPPER_DISABLEALL_SET_1(port, pin)  PORT##port |= _BV(PORT##port##pin)
#define MUOS_STEPPER_DISABLEALL_SET_0(port, pin)  PORT##port &= ~_BV(PORT##port##pin)

#define MUOS_STEPPER_DISABLEALL_SET_(port, pin, polarity) \
  MUOS_STEPPER_DISABLEALL_SET_##polarity(port, pin)

#define MUOS_STEPPER_DISABLEALL_SET(hw)      \
  MUOS_STEPPER_DISABLEALL_SET_ hw

#define MUOS_STEPPER_DISABLEALL_DDR_OUTPUT_(port, pin, polarity) \
  DDR##port |= _BV(DD##port##pin)

#define MUOS_STEPPER_DISABLEALL_DDR_OUTPUT(hw)                 \
  MUOS_STEPPER_DISABLEALL_DDR_OUTPUT_ hw
#endif


#define MUOS_STEPPER_DIR_DDR_(port, pin, polarity)      \
  DDR##port |= _BV(DD##port##pin)

#define MUOS_STEPPER_DIR_DDR(hw)                \
  MUOS_STEPPER_DIR_DDR_ hw

#define MUOS_STEPPER_INIT_PIN_IMPL(timer, output, output_mode, wgm)     \
  MUOS_STEPPER_PIN_IDLE_##output_mode(timer, output)

#define MAKE_STEPPER_INIT_PIN(index, hw) MUOS_STEPPER_INIT_PIN_IMPL hw

#define MUOS_STEPPER_PIN_DDR(timer, output, output_mode, wgm)   \
  OC##timer##output##_DDR |=                                    \
    _BV(OC##timer##output##_BIT);

#define MAKE_STEPPER_INIT_DDR(index, hw) MUOS_STEPPER_PIN_DDR hw;

//TODO: implement for other timers (define in hardware header)
#define MUOS_STEPPER_TOP_REGISTER(timer, wgm) MUOS_STEPPER_TOP_##timer##_##wgm
#define MUOS_STEPPER_TOP_1_14 ICR1
#define MUOS_STEPPER_TOP_1_15 OCR1A

//PLANNED: abstract F_CPU for timers running on other clock sources
#define MUOS_STEPPER_START_IMPL_(index, timer, output, output_mode, wgm)                        \
      TCCR##timer##A = (output_mode << COM##timer##output##0) | ((wgm&0x3)<<WGM##timer##0);     \
      OCR##timer##B =                                                                           \
        MUOS_PP_LIST_NTH0(index,MUOS_STEPPER_PULSE_NS)                                          \
        /(F_CPU/4000UL/timerdividers##timer[prescale-1])-1;                                     \
      MUOS_STEPPER_TOP_REGISTER(timer, wgm) = speed_raw;                                        \
      TIFR##timer = _BV(TOV##timer);                                                            \
      TIMSK##timer = _BV(TOIE##timer);                                                          \
      TCCR##timer##B = ((wgm&~0x3)<<(WGM##timer##2 -2)) | prescale;

#define MUOS_STEPPER_START_IMPL(index, exp) MUOS_STEPPER_START_IMPL_ (index, exp)

#define MAKE_STEPPER_START(index, hw)                                   \
      case index:                                                       \
        MUOS_STEPPER_START_IMPL(index, MUOS_PP_LIST_EXPAND(hw));        \
        break;



/*
  direction
*/

#define MUOS_STEPPER_DIR_IMPL_(index, port, pin, polarity)      \
      if (dir^polarity)                                         \
        {                                                       \
          PORT##port &= ~_BV(PORT##port##pin);                  \
        }                                                       \
      else                                                      \
        {                                                       \
          PORT##port |= _BV(PORT##port##pin);                   \
        }

#define MUOS_STEPPER_DIR_IMPL(index, exp) MUOS_STEPPER_DIR_IMPL_ (index, exp)

#define MAKE_STEPPER_SET_DIR(index, hw)                                 \
      case index:                                                       \
        MUOS_STEPPER_DIR_IMPL(index, MUOS_PP_LIST_EXPAND(hw));          \
        break;


/*
  enable / disable
*/

#ifdef MUOS_STEPPER_DISABLEALL_INOUT_HW

#define MUOS_STEPPER_DISABLEALL_DDR_INPUT_(port, pin, polarity) \
  DDR##port &= ~_BV(DD##port##pin);

#define MUOS_STEPPER_DISABLEALL_DDR_INPUT(hw)                 \
  MUOS_STEPPER_DISABLEALL_DDR_INPUT_ hw

  //TODO: when not driven down, then disable pullup
#define MUOS_STEPPER_DISABLEALL_NOPULL_1(port, pin)  error not implemented yet
#define MUOS_STEPPER_DISABLEALL_NOPULL_0(port, pin)
#define MUOS_STEPPER_DISABLEALL_NOPULL_(port, pin, polarity) \
  MUOS_STEPPER_DISABLEALL_NOPULL_##polarity(port, pin)
#define MUOS_STEPPER_DISABLEALL_NOPULL(hw)      \
  MUOS_STEPPER_DISABLEALL_NOPULL_ hw

#define MUOS_STEPPER_DISABLEALL_CHECK_1(port, pin) !(PIN##port & _BV(PIN##port##pin))
#define MUOS_STEPPER_DISABLEALL_CHECK_0(port, pin) (PIN##port & _BV(PIN##port##pin))
#define MUOS_STEPPER_DISABLEALL_CHECK_(port, pin, polarity) \
  MUOS_STEPPER_DISABLEALL_CHECK_##polarity(port, pin)

#define MUOS_STEPPER_DISABLEALL_CHECK(hw) \
  MUOS_STEPPER_DISABLEALL_CHECK_ hw

#endif

#endif

