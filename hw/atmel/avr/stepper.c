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
#include <muos/muos.h>
#include <muos/hw/atmel/avr/atmega328p.h>
#include <muos/pp.h>
#include <muos/hpq.h>
#include <muos/io.h> /*DEBUG*/

#include <stdlib.h>
/* Signal generation for example drive low polarity

   .......      ____________________________      ........
         |      |                           :     :
         |______|                           :.....:

*/


#define GET_DIRECTION_(port, pin, inv) (((PORT##port & _BV(PORT##port##pin)))?!inv:inv)
#define GET_DIRECTION(hw) GET_DIRECTION_ hw


#define MAKE_OVF_ISR__(index, timer, output, output_mode, wgm)                                                  \
  ISR(TIMER##timer##_OVF_vect)                                                                                  \
  {                                                                                                             \
    MUOS_DEBUG_INTR_ON;                                                                                         \
    muos_steppers[index].position += (GET_DIRECTION(MUOS_STEPPER_DIR_HW))?+1:-1;                                \
    for (uint8_t i=0; i<MUOS_STEPPER_POSITION_SLOTS; ++i)                                                       \
      {                                                                                                         \
        if (muos_steppers[index].position == muos_steppers[index].position_match[i].position)                   \
          {                                                                                                     \
            if (muos_steppers[index].position_match[i].whattodo                                                 \
                && (1<<MUOS_STEPPER_ACTION_STOP))                                                               \
              {                                                                                                 \
                TCCR##timer##B = 0;                                                                             \
                TIMSK##timer &= ~_BV(TOIE##timer);                                                              \
              }                                                                                                 \
            if (muos_steppers[index].position_match[i].whattodo                                                 \
                && muos_steppers[index].position_match[i].arg)                                                  \
              {                                                                                                 \
                if (muos_steppers[index].position_match[i].whattodo                                             \
                    & (1<<MUOS_STEPPER_HPQ_FRONT))                                                              \
                  muos_error_set_isr (muos_hpq_pushfront_isr ((muos_queue_function)                             \
                                                              muos_steppers[index].position_match[i].arg));     \
                else if (muos_steppers[index].position_match[i].whattodo                                        \
                         & (1<<MUOS_STEPPER_HPQ_BACK))                                                          \
                  muos_error_set_isr (muos_hpq_pushback_isr ((muos_queue_function)                              \
                                                              muos_steppers[index].position_match[i].arg));     \
              }                                                                                                 \
            if (!(muos_steppers[index].position_match[i].whattodo & (1<<MUOS_STEPPER_ACTION_PERMANENT)))        \
              {                                                                                                 \
                muos_steppers[index].position_match[i].whattodo = 0;                                            \
              }                                                                                                 \
          }                                                                                                     \
      }                                                                                                         \
   }


#define MAKE_OVF_ISR_(...) MAKE_OVF_ISR__ (__VA_ARGS__)
#define MAKE_OVF_ISR(index, hw) MAKE_OVF_ISR_ (index, MUOS_PP_LIST_EXPAND(hw))

MUOS_PP_CODEGEN(MAKE_OVF_ISR, MUOS_STEPPER_HW);



void
muos_hw_stepper_init (void)
{
  // note TCCRxA is better to set when starting and cleared when stopping
#define MUOS_STEPPER_INIT_TCCRB_IMPL(timer, output, output_mode, wgm)   \
  TCCR##timer##B = ((wgm&~0x3)<<(WGM##timer##2 -2));

#define MAKE_STEPPER_INIT_TCCRB(index, hw)                     \
  MUOS_STEPPER_INIT_TCCRB_IMPL hw

  MUOS_PP_CODEGEN(MAKE_STEPPER_INIT_TCCRB, MUOS_STEPPER_HW); /* generate code */

  // set pin to idle state when not running, note output_modes 1 and 2 are not defined
#define MUOS_STEPPER_PIN_IDLE_2(timer, output)  \
  OC##timer##output##_PORT &=                   \
    ~_BV(OC##timer##output##_BIT);
#define MUOS_STEPPER_PIN_IDLE_3(timer, output)  \
  OC##timer##output##_PORT |=                   \
    _BV(OC##timer##output##_BIT);

#define MUOS_STEPPER_INIT_PIN_IMPL(timer, output, output_mode, wgm) \
  MUOS_STEPPER_PIN_IDLE_##output_mode(timer, output)

#define MAKE_STEPPER_INIT_PIN(index, hw) MUOS_STEPPER_INIT_PIN_IMPL hw

  MUOS_PP_CODEGEN(MAKE_STEPPER_INIT_PIN, MUOS_STEPPER_HW);  /* generate code */

  //TODO: init enable and direction pins

#define MUOS_STEPPER_PIN_DDR(timer, output, output_mode, wgm)   \
  OC##timer##output##_DDR |=                                    \
    _BV(OC##timer##output##_BIT);

#define MAKE_STEPPER_INIT_DDR(index, hw) MUOS_STEPPER_PIN_DDR hw;

  MUOS_PP_CODEGEN(MAKE_STEPPER_INIT_DDR, MUOS_STEPPER_HW);  /* generate code */

  for(uint8_t i = 0; i < MUOS_STEPPER_COUNT; ++i)
    muos_steppers[i].state = MUOS_STEPPER_OFF;
}


//TODO: conditional for different parts
// the values for prescalers are irregular, depending on the actual timer/hardware
// only common is that 0 means off, thus we don't handle that here
static const uint16_t timerdividers0[] = {1,8,64,256,1024};
static const uint16_t* const timerdividers1 = timerdividers0;
static const uint16_t timerdividers2[] = {1,8,32,64,128,256,1024};


//PLANNED: dynamic prescaler change to increase range and resolution
//PLANNED: does const uint8_t hw compile better?

muos_error
muos_hw_stepper_start (uint8_t hw, uint8_t prescale, uint16_t speed_raw)
{
  switch (hw)
    {
      //TODO: implement for other timers (define in hardware header)
#define MUOS_STEPPER_TOP_REGISTER(timer, wgm) MUOS_STEPPER_TOP_##timer##_##wgm
#define MUOS_STEPPER_TOP_1_14 ICR1
#define MUOS_STEPPER_TOP_1_15 OCR1A

      //PLANNED: abstract F_CPU for timers running on other clocks
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

      MUOS_PP_CODEGEN(MAKE_STEPPER_START, MUOS_STEPPER_HW);  /* generate code */

    default:
      return muos_error_nohw;
    }

  return muos_success;
}


static inline bool
muos_hw_stepper_interrupt_disable (uint8_t hw)
{
  bool ret = false;
  switch (hw)
    {
#define MUOS_STEPPER_INTERRUPT_DISABLE_(timer, output, output_mode, wgm)               \
      ret = TIMSK##timer & _BV(TOIE##timer);                                           \
      TIMSK##timer &= ~_BV(TOIE##timer);

#define MAKE_STEPPER_INTERRUPT_DISABLE(index, hw)       \
      case index:                                       \
        MUOS_STEPPER_INTERRUPT_DISABLE_ hw;             \
        break;

      MUOS_PP_CODEGEN(MAKE_STEPPER_INTERRUPT_DISABLE, MUOS_STEPPER_HW);  /* generate code */
    }
  return ret;
}

static inline void
muos_hw_stepper_interrupt_enable (uint8_t hw, bool really)
{
  if (really)
    {
      switch (hw)
        {
#define MUOS_STEPPER_INTERRUPT_ENABLE_(timer, output, output_mode, wgm)        \
          TIMSK##timer |= _BV(TOIE##timer);

#define MAKE_STEPPER_INTERRUPT_ENABLE(index, hw)        \
          case index:                                   \
            MUOS_STEPPER_INTERRUPT_ENABLE_ hw;          \
            break;

          MUOS_PP_CODEGEN(MAKE_STEPPER_INTERRUPT_ENABLE, MUOS_STEPPER_HW);  /* generate code */
        }
    }
}






muos_error
muos_hw_stepper_register_action (uint8_t hw,
                                 int32_t position,
                                 uint8_t action,
                                 uintptr_t arg)
{
  if (muos_steppers[hw].state >= MUOS_STEPPER_SLOW
      || muos_steppers[hw].state == MUOS_STEPPER_CAL)
    {
      return muos_error_stepper_state;
    }

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

//PLANNED: have a on-stop callback?

#if 0 //TODO: needs locked access while runing
int32_t
muos_hw_stepper_position (uint8_t hw)
{
  switch (hw)
    {
#define MUOS_STEPPER_STOP_(timer, output, output_mode, wgm)  \
      TCCR##timer##B = 0;                                    \
      TIMSK##timer &= ~_BV(TOIE##timer);

#define MAKE_STEPPER_STOP(index, hw)            \
      case index:                               \
        MUOS_STEPPER_STOP_ hw;                  \
        break;

      MUOS_PP_CODEGEN(MAKE_STEPPER_STOP, MUOS_STEPPER_HW);  /* generate code */

    default:
      return muos_error_nohw;
    }
}
#endif




#if 0
//TODO: next
//muos_error
//muos_hw_stepper_stop_isr (uint8_t hw);

muos_error
muos_hw_stepper_stop_isr (uint8_t hw)
{
  
}
#endif


#if 0 //TODO: implement
void
muos_hw_stepper_on (uint8_t hw)
{
  
  
}
#endif



#endif

