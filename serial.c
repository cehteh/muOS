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

volatile struct muos_serial_flags muos_serial_status[MUOS_SERIAL_NUM];
char muos_serial_rxsync[MUOS_SERIAL_NUM];
muos_serial_rxcallback_type muos_serial_rxcallback[MUOS_SERIAL_NUM];

/*
  TX/RX buffers
*/

#define UART(hw, txsize, rxsize)                \
  MUOS_CBUFFERDEF(muos_txbuffer##hw, txsize);   \
  MUOS_CBUFFERDEF(muos_rxbuffer##hw, rxsize);
MUOS_SERIAL_HW
#undef UART

// indexed descriptor arrays are only used when there is more than one serial
#if MUOS_SERIAL_NUM > 1
struct muos_cbuffer* const muos_txbuffer[]  =
  {
#define UART(hw, txsize, rxsize)                \
   &muos_txbuffer##hw.descriptor,

   MUOS_SERIAL_HW
#undef UART
  };

struct muos_cbuffer* const muos_rxbuffer[] =
  {
#define UART(hw, txsize, rxsize)                \
   &muos_rxbuffer##hw.descriptor,

   MUOS_SERIAL_HW
#undef UART
  };
#endif


/*
  init/startup
*/

#if MUOS_SERIAL_NUM > 1
muos_error
muos_serial_start (uint8_t hw, uint32_t baud, char config[3], int rxsync, muos_serial_rxcallback_type callback)
{
  if (hw >= MUOS_SERIAL_NUM)
    return muos_error_nodev;

  MUOS_OK (muos_hw_serial_start (hw, baud, config, rxsync));
  muos_serial_rxcallback[hw] = callback;
  return muos_success;
}
#else
muos_error
muos_serial_start (uint32_t baud, char config[3], int rxsync, muos_serial_rxcallback_type callback)
{
  MUOS_OK (muos_hw_serial_start (0, baud, config, rxsync));
  muos_serial_rxcallback[hw] = callback;
  return muos_success;
}
#endif



/*
  Sending
*/

#if MUOS_SERIAL_NUM > 1

muos_error
muos_serial_tx_byte (uint8_t hw, uint8_t b)
{
  if (hw >= MUOS_SERIAL_NUM)
    return muos_error_nodev;

  muos_error ret = muos_success;

  if (!muos_serial_status[hw].serial_tx_waiting)
    {
      muos_serial_tx_stop (hw);

      if (muos_cbuffer_free (muos_txbuffer[hw]) > 0)
        {
          muos_cbuffer_push (muos_txbuffer[hw], b);
        }
      else
        {
          ret = muos_error_tx_overflow;
        }

      muos_serial_tx_run (hw);
    }
  else
    {
      ret = muos_error_tx_blocked;
    }

  return ret;
}

void
muos_serial_tx_flush (uint8_t hw)
{
  muos_serial_tx_stop (hw);
  muos_cbuffer_init (muos_txbuffer[hw]);
  muos_serial_tx_run (hw);
}


#else

muos_error
muos_serial_tx_byte (uint8_t b)
{
  muos_error ret = muos_success;

  if (!muos_serial_status[0].serial_tx_waiting)
    {
      muos_serial_tx_stop ();

      if (muos_cbuffer_free (&muos_txbuffer0) > 0)
        {
          muos_cbuffer_psuh (&muos_txbuffer0, b);
        }
      else
        {
          ret = muos_error_tx_overflow;
        }

      muos_serial_tx_run ();
    }
  else
    {
      ret = muos_error_tx_blocked;
    }

  return ret;
}

void
muos_serial_tx_flush (void)
{
  muos_serial_tx_stop ();
  muos_cbuffer_init (&muos_txbuffer0);
  muos_serial_tx_run ();
}
#endif



/*
  Receiving
*/

#if MUOS_SERIAL_NUM > 1

int16_t
muos_serial_rx_byte (uint8_t hw)
{
  int16_t ret;

  if (!muos_serial_status[hw].serial_rx_waiting)
    {
      muos_serial_rx_stop (hw);

      if (muos_cbuffer_used (muos_rxbuffer[hw]))
        {
          ret = muos_cbuffer_pop (muos_rxbuffer[hw]);
        }
      else
        {
          ret = -muos_error_rx_underflow;
        }

      muos_serial_rx_run (hw);
    }
  else
    {
      ret = -muos_error_rx_blocked;
    }

  return ret;
}


void
muos_serial_rx_flush (uint8_t hw, bool desync)
{
  muos_serial_rx_stop (hw);
  muos_cbuffer_init (muos_rxbuffer[hw]);
  muos_serial_status[hw].serial_rx_insync = !desync;
  muos_serial_rx_run (hw);
}

#else

int16_t
muos_serial_rx_byte (void)
{
  int16_t ret;

  if (!muos_serial_status[0].serial_rx_waiting)
    {
      muos_serial_rx_stop ();

      if (muos_cbuffer_used (&muos_rxbuffer0)
        {
          ret = muos_cbuffer_pop (&muos_rxbuffer0);
        }
      else
        {
          ret = -muos_error_rx_underflow;
        }

      muos_serial_rx_run ();
    }
  else
    {
      ret = -muos_error_rx_blocked;
    }

  return ret;
}


void
muos_serial_rx_flush (bool desync)
{
  muos_serial_rx_stop ();
  muos_cbuffer_init (&muos_rxbuffer0);
  muos_serial_status[0].serial_rx_insync = !desync;
  muos_serial_rx_run ();
}

#endif


/*FIXME: SERIAL_NUM=0*/
void
muos_serial_rxhpq_call (void)
{
  uint8_t hw = muos_hpq_pop_isr ();
  muos_interrupt_enable ();

  bool again = false;

  if (muos_serial_rxcallback[MUOS_SERIAL_NUM])
    again = muos_serial_rxcallback[MUOS_SERIAL_NUM] (hw);

  //FIXME: use  muos_serial_rx_stop (hw);
  muos_interrupt_disable (); // no need for enable, mainloop will disable on return
  if (again && muos_cbuffer_used (muos_rxbuffer[hw]))
    muos_error_set (muos_hpq_pushback_isr (muos_serial_rxhpq_call, true));
  else
    muos_serial_status[hw].serial_rxhpq_pending = false;
}

#endif




//ATTIC
#if 0 //#ifdef MUOS_SCHED_DEPTH  implement later better waiting
static bool
txtest (intptr_t n)
{
  bool ret;
  muos_hw_serial_tx_stop ();
  ret = MUOS_CBUFFER_FREE (muos_txbuffer) >= (muos_cbuffer_index) n;
  muos_hw_serial_tx_run ();
  return ret;
}
//muos_cbuffer_index
//PLANNED: muos_serial_tx_avail (void)
//{
//  return MUOS_CBUFFER_FREE(muos_txbuffer); 
//}

static bool
rxtest (intptr_t n)
{
  bool ret;
  muos_hw_serial_rx_stop ();
  ret = MUOS_CBUFFER_USED (muos_rxbuffer) >= (muos_cbuffer_index) n;
  muos_hw_serial_rx_run ();
  return ret;
}
//muos_cbuffer_index
//PLANNED: muos_serial_rx_avail (void)
//{
//  return muos_atomic_get(MUOS_CBUFFER_USED(muos_rxbuffer));
//}
#endif
