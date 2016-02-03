/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2015                            Christian Thäter <ct@pipapo.org>
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

#include <stdint.h>
#include <muos/muos.h>
#include <muos/error.h>

volatile uint8_t muos_errors_pending_;
volatile uint8_t muos_errors_[(muos_errors_end+7)/8];

void
muos_error_set_unsafe (enum muos_errorcode err)
{
  if (!(muos_errors_[err/8] & 1<<(err%8)))
      {
#if MUOS_DEBUG_ERROR ==1
        PORTD |= _BV(PIND2);
#endif
        muos_errors_[err/8] |= 1<<(err%8);
        ++muos_errors_pending_;
      }
}

void
muos_error_set (enum muos_errorcode err)
{
  cli ();
  muos_error_set_unsafe (err);
  sei ();
}


bool
muos_error_check (enum muos_errorcode err)
{
  muos_interrupt_disable();
  bool ret = muos_errors_[err/8] & 1<<(err%8);
  if(ret)
    --muos_errors_pending_;
  muos_errors_[err/8] &= ~(1<<(err%8));

#if MUOS_DEBUG_ERROR ==1
  if (!muos_errors_pending_)
    PORTD &= ~_BV(PIND2);
#endif

  muos_interrupt_enable();
  return ret;
}


