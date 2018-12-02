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
#include <muos/hw/atmel/avr/stepper_pp.h>
#include <muos/hpq.h>
#include <muos/io.h> /*DEBUG*/

#include <stdlib.h>
/* Signal generation for example drive low polarity

   .......      ____________________________      ........
         |      |                           :     :
         |______|                           :.....:

*/



MUOS_PP_CODEGEN(MAKE_OVF_ISR, MUOS_STEPPER_HW);




//TODO: refactor all the codegen #defines into a stepper_meta.h

void
muos_hw_stepper_init (void)
{
  MUOS_PP_CODEGEN(MAKE_STEPPER_INIT_TCCRB, MUOS_STEPPER_HW);

#ifdef MUOS_STEPPER_DISABLEALL_INOUT_HW
  MUOS_STEPPER_DISABLEALL_DDR_OUTPUT(MUOS_STEPPER_DISABLEALL_INOUT_HW);
#endif

  MUOS_STEPPER_DIR_DDR(MUOS_STEPPER_DIR_HW);

  MUOS_PP_CODEGEN(MAKE_STEPPER_INIT_PIN, MUOS_STEPPER_HW);

  MUOS_PP_CODEGEN(MAKE_STEPPER_INIT_DDR, MUOS_STEPPER_HW);

  for(uint8_t i = 0; i < MUOS_STEPPER_COUNT; ++i)
    muos_steppers[i].state = MUOS_STEPPER_OFF;
}


//TODO: conditional for different parts
// the values for prescalers are irregular, depending on the actual timer/hardware
// only common is that 0 means off, thus we don't handle that here [prescale-1] below
#ifdef MUOS_HW_ATMEL_ATMEGA328P_H
static const uint16_t timerdividers0[] = {1,8,64,256,1024};
static const uint16_t* const timerdividers1 = timerdividers0;
static const uint16_t timerdividers2[] = {1,8,32,64,128,256,1024};
#endif

//PLANNED: dynamic prescaler change to increase range and resolution
//PLANNED: does const uint8_t hw compile better?

muos_error
muos_hw_stepper_start (uint8_t hw, uint8_t prescale, uint16_t speed_raw)
{
  switch (hw)
    {
      //TODO: implement for other timers (define in hardware header)
      MUOS_PP_CODEGEN(MAKE_STEPPER_START, MUOS_STEPPER_HW);

    default:
      return muos_error_nohw;
    }

  return muos_success;
}



muos_error
muos_hw_stepper_set_direction (uint8_t hw, bool dir)
{
  switch (hw)
    {
      //TODO: exec and wait only when direction changes
      MUOS_PP_CODEGEN(MAKE_STEPPER_SET_DIR, MUOS_STEPPER_DIR_HW);

    default:
      return muos_error_nohw;
    }

  // wait dir ns
  //TODO: if (muos_warn_wait_timeout != muos_wait (0, 0, MUOS_CLOCK_NANOSECONDS (MUOS_STEPPER_ENABLE_NS)))
  return muos_success;
}




muos_error
muos_hw_stepper_register_action (uint8_t hw,
                                 int32_t position,
                                 uint8_t action,
                                 uintptr_t arg)
{
  if (hw >= MUOS_STEPPER_COUNT)
    return muos_error_nohw;

  if (!muos_stepper_mutable_state(hw))
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


muos_error
muos_hw_stepper_remove_action (uint8_t hw,
                               int32_t position,
                               uint8_t action,
                               uintptr_t arg)
{
  if (hw >= MUOS_STEPPER_COUNT)
    return muos_error_nohw;

  if (!muos_stepper_mutable_state(hw))
    {
      return muos_error_stepper_state;
    }

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



#ifdef MUOS_STEPPER_DISABLEALL_INOUT_HW

muos_error
muos_hw_stepper_enableall (void)
{
  MUOS_STEPPER_DISABLEALL_DDR_INPUT (MUOS_STEPPER_DISABLEALL_INOUT_HW);

  MUOS_STEPPER_DISABLEALL_NOPULL (MUOS_STEPPER_DISABLEALL_INOUT_HW);

  if (muos_warn_wait_timeout != muos_wait (0, 0, MUOS_CLOCK_NANOSECONDS (MUOS_STEPPER_ENABLE_NS)))
    {
      MUOS_STEPPER_DISABLEALL_DDR_OUTPUT (MUOS_STEPPER_DISABLEALL_INOUT_HW);
      return muos_warn_sched_depth;
    }

  if (MUOS_STEPPER_DISABLEALL_CHECK (MUOS_STEPPER_DISABLEALL_INOUT_HW))
    {
      return muos_success;
    }

  MUOS_STEPPER_DISABLEALL_DDR_OUTPUT (MUOS_STEPPER_DISABLEALL_INOUT_HW);
  return muos_error_stepper_state;
}



void
muos_hw_stepper_disableall (void)
{
  MUOS_STEPPER_DISABLEALL_SET (MUOS_STEPPER_DISABLEALL_INOUT_HW);
  MUOS_STEPPER_DISABLEALL_DDR_OUTPUT(MUOS_STEPPER_DISABLEALL_INOUT_HW);
}

#endif
















#if 0 //TODO: needs locked access while running, why does one want the position when running?
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

