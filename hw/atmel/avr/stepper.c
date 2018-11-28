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
#include <muos/io.h> /*DEBUG*/

/* Signal generation for example drive low polarity

   .......      ____________________________      ........
         |      |                           :     :
         |______|                           :.....:

*/

//TODO: ISR action position match: stop, reverse, schedule function, pop next position/parameters
//      callbacks

#define MAKE_OVF_ISR(index, hw)                                 \
  ISR(MUOS_PP_CAT3(TIMER, MUOS_PP_LIST_NTH0(0,hw), _OVF_vect))  \
  {                                                             \
    MUOS_DEBUG_INTR_ON;                                         \
    muos_steppers[index].position += 1/*FIXME: direction*/;     \
    /*TODO: when to stop etc */                                 \
  }

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

  // set pin to idle state when not running
#define MUOS_STEPPER_PIN_IDLE_0(timer, output)
#define MUOS_STEPPER_PIN_IDLE_1(timer, output)
#define MUOS_STEPPER_PIN_IDLE_2(timer, output)  \
  OC##timer##output##_PORT &=                   \
    ~_BV(OC##timer##output##_BIT);
#define MUOS_STEPPER_PIN_IDLE_3(timer, output)  \
  OC##timer##output##_PORT |=                   \
    _BV(OC##timer##output##_BIT);

#define MUOS_STEPPER_INIT_PIN_IMPL(timer, output, output_mode, wgm) \
  MUOS_STEPPER_PIN_IDLE_##output_mode(timer, output)

#define MAKE_STEPPER_INIT_PIN(index, hw)        \
  MUOS_STEPPER_INIT_PIN_IMPL hw

  MUOS_PP_CODEGEN(MAKE_STEPPER_INIT_PIN, MUOS_STEPPER_HW);  /* generate code */


#define MUOS_STEPPER_PIN_DDR(timer, output, output_mode, wgm)   \
  OC##timer##output##_DDR |=                                    \
    _BV(OC##timer##output##_BIT);

#define MAKE_STEPPER_INIT_DDR(index, hw)           \
  MUOS_STEPPER_PIN_DDR hw;

  MUOS_PP_CODEGEN(MAKE_STEPPER_INIT_DDR, MUOS_STEPPER_HW);  /* generate code */

  for(uint8_t i = 0; i < MUOS_STEPPER_COUNT; ++i)
    muos_steppers[i].state = MUOS_STEPPER_OFF;
}


//TODO: conditional for different parts
// the values for prescalers are irregular, depending on the actual timer/hardware
// only common is that 0 means official, thus we don't handle that here
static const uint16_t timerdividers0[] = {1,8,64,256,1024};
static const uint16_t* const timerdividers1 = timerdividers0;
static const uint16_t timerdividers2[] = {1,8,32,64,128,256,1024};





//PLANNED: dynamic prescaler change to increase range and resolution

muos_error
muos_hw_stepper_start (uint8_t hw, uint8_t prescale, uint16_t speed_raw)
{
  //FIXME: prescaler range
  switch (hw)
    {

      //TODO: check state
#define MUOS_STEPPER_TOP_REGISTER(timer, wgm) MUOS_STEPPER_TOP_##wgm (timer)
#define MUOS_STEPPER_TOP_14(timer) ICR##timer
#define MUOS_STEPPER_TOP_15(timer) OCR##timer##A

      //PLANNED: abstract F_CPU for timers running on other clocks
#define MUOS_STEPPER_START_IMPL_(index, timer, output, output_mode, wgm)                        \
      TCCR##timer##A = (output_mode << COM##timer##output##0) | ((wgm&0x3)<<WGM##timer##0);     \
      OCR##timer##B =                                                                           \
        MUOS_PP_LIST_NTH0(index,MUOS_STEPPER_PULSE_NS)                                          \
        /(F_CPU/4000UL/timerdividers##timer[prescale-1])-1;                                     \
      MUOS_STEPPER_TOP_REGISTER(timer, wgm) = speed_raw;                                        \
      TIFR1 = _BV(TOV1);                                                                        \
      TIMSK1 = _BV(TOIE1);                                                                      \
      TCCR##timer##B = ((wgm&~0x3)<<(WGM##timer##2 -2)) | prescale;

#define MUOS_STEPPER_START_IMPL(index, exp) \
       MUOS_STEPPER_START_IMPL_ (index, exp)

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


#if 0
//TODO: next
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

