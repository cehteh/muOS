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
muos_error_set_unsafe (muos_error err)
{
  if (err && !(muos_errors_[err/8] & 1<<(err%8)))
      {
#if MUOS_DEBUG_ERROR ==1
        PORTD |= _BV(PIND2);
#endif
        muos_errors_[err/8] |= 1<<(err%8);
        ++muos_errors_pending_;
      }
}

void
muos_error_set (muos_error err)
{
  if (err)
    {
      muos_interrupt_disable ();
      muos_error_set_unsafe (err);
      muos_interrupt_enable ();
    }
}

bool
muos_error_peek (muos_error err)
{
  return muos_errors_[err/8] & 1<<(err%8);
}


bool
muos_error_check (muos_error err)
{
  bool ret = false;

  muos_interrupt_disable ();

  if (muos_errors_pending_)
    {
      ret = muos_errors_[err/8] & 1<<(err%8);

      muos_errors_[err/8] &= ~(1<<(err%8));

      if (ret)
        --muos_errors_pending_;

#if MUOS_DEBUG_ERROR ==1
      if (!muos_errors_pending_)
        PORTD &= ~_BV(PIND2);
#endif
    }

  muos_interrupt_enable ();
  return ret;
}


