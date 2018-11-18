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



#ifdef __AVR_ATmega328P__


#if MUOS_SERIAL_TXBUFFER > 1 || MUOS_SERIAL_RXBUFFER > 1
void
muos_hw_serial_init (void)
{
#define BAUD (MUOS_SERIAL_BAUD)
#define BAUD_TOL 3
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= _BV(U2X0);
#else
  UCSR0A &= ~_BV(U2X0);
#endif

  UCSR0C = _BV(UCSZ01)| _BV(UCSZ00);
  UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
}
#endif

#if MUOS_SERIAL_TXBUFFER > 1
ISR(USART_UDRE_vect)
{
  MUOS_DEBUG_INTR_ON;

  if (muos_txbuffer.descriptor.len)
    {
      MUOS_SERIAL_TX_REGISTER = MUOS_CBUFFER_POP (muos_txbuffer);
      if (!muos_txbuffer.descriptor.len)
        muos_hw_serial_tx_stop ();
    }

  //  MUOS_DEBUG_INTR_OFF;
}
#endif


#if MUOS_SERIAL_RXBUFFER > 1
ISR(USART_RX_vect)
{
  MUOS_DEBUG_INTR_ON;
  //TODO: disable error checks depending on config (no parity etc)

  bool err = false;

  if (UCSR0A & _BV(DOR0))
    {
      muos_error_set_isr (muos_error_rx_overrun);
      err = true;
    }

  if (UCSR0A & _BV(UPE0))
    {
      muos_error_set_isr (muos_error_rx_parity);
      err = true;
    }

  if (UCSR0A & _BV(FE0))
    {
      muos_error_set_isr (muos_error_rx_frame);
      err = true;
      muos_status.serial_rx_sync = false;
    }

  if (!MUOS_CBUFFER_FREE(muos_rxbuffer))
    {
      muos_error_set_isr (muos_error_rx_buffer_overflow);
      err = true;
    }

  uint8_t data = MUOS_SERIAL_RX_REGISTER;

  //PLANNED: sync when line is idle

  if (!err && !muos_status.serial_rx_sync)
    {
      if (data == MUOS_SERIAL_RXSYNC)
        muos_status.serial_rx_sync = true;
    }

  if (muos_status.serial_rx_sync)
    {
      MUOS_CBUFFER_PUSH (muos_rxbuffer, data);

      if (!muos_status.serial_rxhpq_pending)
        {
          muos_status.serial_rxhpq_pending = true;
#ifdef MUOS_SERIAL_RXCALLBACK
          muos_error_set (muos_hpq_pushback_isr (muos_serial_rxhpq_call));
#endif
        }
    }

  //  MUOS_DEBUG_INTR_OFF;
}
#endif

#endif
