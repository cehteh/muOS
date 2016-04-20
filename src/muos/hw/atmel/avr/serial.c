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

#ifdef MUOS_SERIAL_RXCALLBACK
extern void
MUOS_SERIAL_RXCALLBACK (void);
#endif

#ifdef __AVR_ATmega328P__


#if MUOS_SERIAL_TXBUFFER > 1 || MUOS_SERIAL_RXBUFFER > 1
void
muos_hw_serial_init (void)
{
#define BAUD (MUOS_SERIAL_BAUD * (1000 + MUOS_SERIAL_BAUD_TXBOOST) / 1000)
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
#if MUOS_DEBUG_INTR == 1
  PORTB |= _BV(PINB4);
#endif

  if (muos_txbuffer.descriptor.len)
    {
      MUOS_SERIAL_TX_REGISTER = MUOS_CBUFFER_POP (muos_txbuffer);
      if (!muos_txbuffer.descriptor.len)
        muos_hw_serial_tx_stop ();
    }
#if MUOS_DEBUG_INTR ==1
  PORTB &= ~_BV(PINB4);
#endif
}
#endif


#if MUOS_SERIAL_RXBUFFER > 1
ISR(USART_RX_vect)
{
#if MUOS_DEBUG_INTR == 1
  PORTB |= _BV(PINB4);
#endif
  //TODO: disable error checks depending on config (no parity etc)

  bool err = false;

  if (UCSR0A & _BV(DOR0))
    {
      muos_error_set_unsafe (muos_error_rx_overrun);
      err = true;
    }

  if (UCSR0A & _BV(UPE0))
    {
      muos_error_set_unsafe (muos_error_rx_parity);
      err = true;
    }

  if (UCSR0A & _BV(FE0))
    {
      muos_error_set_unsafe (muos_error_rx_frame);
      err = true;
      muos_status.serial_rx_sync = false;
    }

  if (!MUOS_CBUFFER_FREE(muos_rxbuffer))
    {
      muos_error_set_unsafe (muos_error_rx_buffer_overflow);
      err = true;
    }

  uint8_t data = MUOS_SERIAL_RX_REGISTER;

  //PLANNED: sync when line is idle

  if (!muos_status.serial_rx_sync)
    {
      if (data == MUOS_SERIAL_RXSYNC)
        muos_status.serial_rx_sync = true;
      else
        err = true;
    }

  if (!err)
    {
      MUOS_CBUFFER_PUSH (muos_rxbuffer, data);
      if (!muos_status.serial_rxhpq_pending)
        {
          muos_status.serial_rxhpq_pending = true;
#ifdef MUOS_SERIAL_RXCALLBACK
          muos_error_set (muos_hpq_pushback_unsafe (MUOS_SERIAL_RXCALLBACK));
#endif
        }
    }

#if MUOS_DEBUG_INTR ==1
  PORTB &= ~_BV(PINB4);
#endif
}
#endif

#endif
