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

#include <muos/muos.h>
#include <muos/serial.h>
#include <muos/hpq.h>

#if MUOS_SERIAL_TXBUFFER > 1
muos_txbuffer_type muos_txbuffer;
#endif

#if MUOS_SERIAL_RXBUFFER > 1
muos_rxbuffer_type muos_rxbuffer;
#endif

#if MUOS_SERIAL_TXBUFFER > 1 || MUOS_SERIAL_RXBUFFER > 1
void
muos_serial_init (void)
{
  muos_hw_serial_init ();
}
#endif


#if MUOS_SERIAL_TXBUFFER > 1
void
muos_serial_tx_byte (uint8_t b)
{
  muos_hw_serial_tx_stop ();

  if (MUOS_CBUFFER_FREE (muos_txbuffer) > 0)
    {
      MUOS_CBUFFER_PUSH (muos_txbuffer, b);
    }
  else
    {
      muos_error_set (muos_error_tx_buffer_overflow);
    }

  muos_hw_serial_tx_run ();
}
#endif

#if MUOS_SERIAL_RXBUFFER > 1
uint8_t
muos_serial_rx_byte (void)
{
  uint8_t ret = 0;

  muos_hw_serial_rx_stop ();

  if (MUOS_CBUFFER_USED (muos_rxbuffer))
    {
      ret = MUOS_CBUFFER_POP (muos_rxbuffer);
    }
  else
    {
      muos_error_set (muos_error_rx_buffer_underflow);
    }

  muos_hw_serial_rx_run ();

  return ret;
}


void
muos_serial_rxhpq_again (muos_queue_function f)
{
  muos_interrupt_disable ();

  if (MUOS_CBUFFER_USED (muos_rxbuffer))
    muos_hpq_pushback_unsafe (f);
  else
    muos_status.serial_rxhpq_pending = false;

  muos_interrupt_enable ();
}
#endif
