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

#ifdef MUOS_SERIAL

#if MUOS_SERIAL_TXBUFFER > 1
muos_txbuffer_type muos_txbuffer;
#endif

#if MUOS_SERIAL_RXBUFFER > 1
muos_rxbuffer_type muos_rxbuffer;
#endif

#if MUOS_SERIAL_TXBUFFER > 1 || MUOS_SERIAL_RXBUFFER > 1
void
muos_serial_30init (void)
{
  muos_hw_serial_init ();
}
#endif


#if MUOS_SERIAL_TXBUFFER > 1
muos_error
muos_serial_tx_nonblocking_byte (uint8_t b)
{
  muos_error ret = muos_success;

  if (!muos_status.serial_tx_blocked)
    {
      muos_hw_serial_tx_stop ();

      if (MUOS_CBUFFER_FREE (muos_txbuffer) > 0)
        {
          MUOS_CBUFFER_PUSH (muos_txbuffer, b);
        }
      else
        {
          ret = muos_error_tx_buffer_overflow;
        }

      muos_hw_serial_tx_run ();
    }
  else
    {
      ret = muos_error_tx_blocked;
    }

  return ret;
}



#ifdef MUOS_SCHED_DEPTH
static bool
txtest (intptr_t n)
{
  bool ret;
  muos_hw_serial_tx_stop ();
  ret = MUOS_CBUFFER_FREE (muos_txbuffer) >= (muos_cbuffer_index) n;
  muos_hw_serial_tx_run ();
  return ret;
}

muos_error
muos_serial_tx_blocking_byte (uint8_t b)
{
  muos_error ret = muos_success;

  if (!muos_status.serial_tx_blocked)
    {
      muos_status.serial_tx_blocked = true;
      //TODO: calculate timeout baudrate for txbuffer characters
      ret = muos_wait (txtest, 1, MUOS_CLOCK_MILLISECONDS(10));
      muos_status.serial_tx_blocked = false;

      if (!ret)
        {
          muos_hw_serial_tx_stop ();
          MUOS_CBUFFER_PUSH (muos_txbuffer, b);
          muos_hw_serial_tx_run ();
        }
    }
  else
    {
      ret = muos_error_tx_blocked;
    }

  return ret;
}
#endif


void
muos_serial_tx_flush (void)
{
  muos_hw_serial_tx_stop ();
  muos_cbuffer_init (&muos_txbuffer.descriptor);
  muos_hw_serial_tx_run ();
}


//muos_cbuffer_index
//PLANNED: muos_serial_tx_avail (void)
//{
//  return MUOS_CBUFFER_FREE(muos_txbuffer); 
//}
#endif

#if MUOS_SERIAL_RXBUFFER > 1
int16_t
muos_serial_rx_nonblocking_byte (void)
{
  int16_t ret;

  if (!muos_status.serial_rx_blocked)
    {
      muos_hw_serial_rx_stop ();

      if (MUOS_CBUFFER_USED (muos_rxbuffer))
        {
          ret = MUOS_CBUFFER_POP (muos_rxbuffer);
        }
      else
        {
          ret = -muos_error_rx_buffer_underflow;
        }

      muos_hw_serial_rx_run ();
    }
  else
    {
      ret = -muos_error_rx_blocked;
    }

  return ret;
}


#ifdef MUOS_SCHED_DEPTH
static bool
rxtest (intptr_t n)
{
  bool ret;
  muos_hw_serial_rx_stop ();
  ret = MUOS_CBUFFER_USED (muos_rxbuffer) >= (muos_cbuffer_index) n;
  muos_hw_serial_rx_run ();
  return ret;
}

int16_t
muos_serial_rx_blocking_byte (muos_shortclock timeout)
{
  int16_t ret;

  if (!muos_status.serial_rx_blocked)
    {
      do
        {
          muos_status.serial_rx_blocked = true;
          ret = -muos_wait (rxtest, 1, timeout?timeout:~0U);
          muos_status.serial_rx_blocked = false;

          if (!ret)
            {
              muos_hw_serial_rx_stop ();
              ret = MUOS_CBUFFER_POP (muos_rxbuffer);
              muos_hw_serial_rx_run ();
            }
        }
      while (timeout == 0 && ret == -muos_warn_wait_timeout);
    }
  else
    {
      ret = -muos_error_rx_blocked;
    }

  return ret;
}
#endif


void
muos_serial_rx_flush (bool desync)
{
  muos_hw_serial_rx_stop ();
  muos_cbuffer_init (&muos_rxbuffer.descriptor);
  muos_status.serial_rx_sync = !desync;
  muos_hw_serial_rx_run ();
}

//muos_cbuffer_index
//PLANNED: muos_serial_rx_avail (void)
//{
//  return muos_atomic_get(MUOS_CBUFFER_USED(muos_rxbuffer));
//}


#ifdef MUOS_SERIAL_RXCALLBACK
void
muos_serial_rxhpq_call (void)
{
  bool again = MUOS_SERIAL_RXCALLBACK ();

  muos_interrupt_disable (); // no need for enable, mainloop will disable on return
  if (again && MUOS_CBUFFER_USED (muos_rxbuffer))
    muos_error_set (muos_hpq_pushback_isr (muos_serial_rxhpq_call));
  else
    muos_status.serial_rxhpq_pending = false;
}
#endif

#endif

#endif
