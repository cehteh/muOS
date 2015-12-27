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
#include <muos/rtq.h>

//TODO: errorhandling

#if MUOS_SERIAL_TXBUFFER > 1
muos_txbuffer_type muos_txbuffer;
#endif

#if MUOS_SERIAL_RXBUFFER > 1
muos_rxbuffer_type muos_rxbuffer;
#endif

void
muos_serial_init (void)
{
  muos_hw_serial_init ();

#if MUOS_SERIAL_TXBUFFER > 0
  muos_hw_tx_enable ();
#endif

#if MUOS_SERIAL_RXBUFFER > 0
  muos_hw_rx_enable ();
#endif
}





static
void wait_for_tx (muos_cbuffer_index requested)
{
  while (MUOS_CBUFFER_FREE (muos_txbuffer) < requested)
    {
      //DEBUG: txbuffer_full
      MUOS_ERROR_SET (tx_buffer_wait);
      muos_hw_serial_tx_run ();
      muos_sleep ();
      muos_hw_serial_tx_stop ();
    }
}


void
muos_serial_tx_byte (uint8_t b)
{
  muos_hw_serial_tx_stop ();
  wait_for_tx (1);
  MUOS_CBUFFER_PUSH (muos_txbuffer, b);
  muos_hw_serial_tx_run ();
}



bool
muos_serial_rx_pending (void)
{
  volatile bool ret;
  muos_hw_serial_rx_stop ();
  if (MUOS_BUFFER_USED(muos_rxbuffer))
    {
      ret = true;
    }
  else
    {
      ret = false;
      muos_status.serial_rxrtq_pending = false;
    }
  muos_hw_serial_rx_run ();
  return ret;
}


