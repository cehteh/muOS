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


#include <avr/eeprom.h>

#include <muos/eeprom.h>
#include <muos/hpq.h>
#include <muos/bgq.h>


#ifdef MUOS_EEPROM_CRC16_INCLUDE
#include MUOS_EEPROM_CRC16_INCLUDE
#endif

#ifdef MUOS_EEPROM_DEBUG_WDELAY
#include <util/delay_basic.h>
#endif

#if MUOS_BGQ_LENGTH < 1 && MUOS_HPQ_LENGTH < 1
#error EEPROM needs either BGQ or HPQ
#endif


// local state
static volatile enum muos_eeprom_mode operation;
static uint8_t* memory;
static size_t bytes;
static muos_eeprom_callback callback;  //pushed to bgq/hpq

#ifdef  MUOS_EEPROM_RETRY
static uint8_t retry;
#endif

ISR(ISRNAME_EEPROM_READY)
{
  MUOS_DEBUG_INTR_ON;

  switch (operation)
    {
    case MUOS_EEPROM_WRITE_CONT:
    case MUOS_EEPROM_WRITEVERIFY_CONT:
      EECR |= (1<<EERE);
      if (EEDR != *memory)
        {
          if (--retry)
            goto retry;

          muos_error_set_isr (muos_error_eeprom_verify);
          goto done;
        }

    case MUOS_EEPROM_WRITEERASE_CONT:
    case MUOS_EEPROM_WRITEONLY_CONT:
      ++memory;

    case MUOS_EEPROM_ERASE_CONT:
      ++EEAR;

    default:
      if (!bytes--)
        goto done;
    };

  switch (operation)
    {
    case MUOS_EEPROM_WRITE:
    case MUOS_EEPROM_WRITEVERIFY:
    case MUOS_EEPROM_WRITEERASE:
    case MUOS_EEPROM_WRITEONLY:
    case MUOS_EEPROM_ERASE:
      ++operation; // sets the the *_CONT operation

    default:;
    }

  // smart writing, pass while eeprom == *memory
  if (operation == MUOS_EEPROM_WRITE_CONT)
    {
      while (EECR |= (1<<EERE), EEDR == *memory)
        {
          ++memory;
          ++EEAR;
          if (!--bytes)
            goto done;
          //PLANNED: batching?
        }
    }

  // start ISR writing
#ifdef  MUOS_EEPROM_RETRY
  retry = MUOS_EEPROM_RETRY;
#endif

 retry:
  EEDR = *memory;
  EECR |= (1<<EEMPE);
  EECR |= (1<<EEPE);
  return;

 done:
  EECR = 0;
  operation = MUOS_EEPROM_IDLE;

  if (callback)
    {
#if MUOS_BGQ_LENGTH >= 1
      muos_error_set_isr (muos_bgq_pushback_isr (callback));
#elif MUOS_HPQ_LENGTH >= 1
      muos_error_set_isr (muos_hpq_pushback_isr (callback));
#endif
    }
  return;
}



static void
readbatch (void)
{
  //PLANNED: implement batching

  do
    {
      EECR |= (1<<EERE);

      switch (operation)
        {
#ifdef MUOS_EEPROM_CRC16_FN
        case MUOS_EEPROM_CRC16:
          *(uint16_t*)memory = MUOS_EEPROM_CRC16_FN (*(uint16_t*)memory, EEDR);
          break; // don't increment memory
#endif
        case MUOS_EEPROM_XOR:
          *memory ^= EEDR;
          break; // don't increment memory
        case MUOS_EEPROM_IS_ERASED:
          if (0xff != EEDR)
            {
              muos_error_set (muos_error_eeprom_verify);
              goto done;
            }
          break; // don't increment memory
        case MUOS_EEPROM_VERIFY:
          if (*memory != EEDR)
            {
              muos_error_set (muos_error_eeprom_verify);
              goto done;
            }
        case MUOS_EEPROM_READ:
          *memory = EEDR;
        default:
          ++memory;
        }
      ++EEAR;
    }
  while (--bytes);

 done:
  operation = MUOS_EEPROM_IDLE;
  EECR = 0;
  if (callback)
    {
#if MUOS_BGQ_LENGTH >= 1
      muos_error_set (muos_bgq_pushback (callback));
#elif MUOS_HPQ_LENGTH >= 1
      muos_error_set (muos_hpq_pushback (callback));
#endif
    }
}



enum muos_eeprom_mode
muos_hw_eeprom_state (void)
{
  return operation;
}


muos_error
muos_hw_eeprom_access (enum muos_eeprom_mode mode,
                       void* mem,
                       uintptr_t eeprom,
                       size_t size,
                       muos_eeprom_callback complete)
{
  // check no eeprom operation pending
  if (EECR & (1<<EEPE) && operation == MUOS_EEPROM_IDLE)
    return muos_error_eeprom_busy;

  // set states
  operation = mode;
  memory = mem;
  bytes = size;
  callback = complete;
  EEAR = eeprom;

  // set modes
  switch (operation)
    {
      // write modes need to initialize EECR
    case MUOS_EEPROM_WRITE:
    case MUOS_EEPROM_WRITEERASE:
    case MUOS_EEPROM_WRITEVERIFY:
      EECR = 0;
      break;

    case MUOS_EEPROM_WRITEONLY:
      EECR = 1<<EEPM1;
      break;

    case MUOS_EEPROM_ERASE:
      EECR = 1<<EEPM0;
      break;

      /*
        reading/verifying/scanning modes
      */
    default:
#ifndef DMUOS_EEPROM_RBATCH
      // batching disabled call it directly
      readbatch ();
      return muos_success;
#elif MUOS_BGQ_LENGTH >= 1
      return muos_bgq_pushback (readbatch);
#elif MUOS_HPQ_LENGTH >= 1
      return muos_hpq_pushback (readbatch);
#endif
    }

  /*
    writing modes below
   */
  
#ifdef MUOS_EEPROM_DEBUG_WDELAY
  for (volatile int i = MUOS_EEPROM_DEBUG_WDELAY; i; --i)
    _delay_loop_2 (F_CPU/1000);
#endif

  // start ISR
  EECR |= 1<<EERIE;

  return muos_success;
}


