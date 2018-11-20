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

#ifdef MUOS_HW_EEPROM

#ifdef DMUOS_EEPROM_DEBUG_WDELAY
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


ISR(ISRNAME_EEPROM_READY)
{
  MUOS_DEBUG_INTR_ON;

  if (!--bytes)
    {
      EECR &= ~(1<<EERIE);

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

  switch (operation)
    {
    case MUOS_EEPROM_WRITEVERIFY:
      EECR |= (1<<EERE);
      if (EEDR != *memory)
        {
          EECR &= ~(1<<EERIE);
          muos_error_set_isr (muos_error_eeprom_verify);
          operation = MUOS_EEPROM_IDLE;
          return;
          //PLANNED: retry
        }

    case MUOS_EEPROM_WRITE:
    case MUOS_EEPROM_WRITEONLY:
      EEDR = *++memory;

    case MUOS_EEPROM_ERASE:
      ++EEAR;
    }

  // start ISR writing
  EECR |= (1<<EEMPE);
  EECR |= (1<<EEPE);
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
        case MUOS_EEPROM_READ:
          *memory = EEDR;
          break;
        case MUOS_EEPROM_VERIFY:
          if (*memory != EEDR)
            {
              muos_error_set (muos_error_eeprom_verify);
              goto done;
            }
        }
      ++EEAR;
      ++memory;
    }
  while (--bytes);

 done:
  operation = MUOS_EEPROM_IDLE;
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
                       void* eeprom,
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
    case MUOS_EEPROM_WRITE:
    case MUOS_EEPROM_WRITEVERIFY:
      break;

    case MUOS_EEPROM_WRITEONLY:
      EECR = 1<<EEPM1;
      break;

    case MUOS_EEPROM_ERASE:
      EECR = 1<<EEPM0;
      break;

    default:
      // reading modes
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

#ifdef DMUOS_EEPROM_DEBUG_WDELAY
  for (int i=DMUOS_EEPROM_DEBUG_WDELAY; i; --i)
    _delay_loop_2 (F_CPU/1000);
#endif

  // start ISR writing
  EECR |= 1<<EERIE;
  EEDR = *memory;

  cli ();
  EECR |= (1<<EEMPE);
  EECR |= (1<<EEPE);
  sei ();

  return muos_success;
}

#endif

