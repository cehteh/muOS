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

extern void
MUOS_SERIAL_RXCALLBACK (void);


#ifdef __AVR_ATmega328P__


void
muos_hw_serial_init (void)
{
  #define BAUD MUOS_SERIAL_BAUD
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= _BV(U2X0);
#else
  UCSR0A &= ~_BV(U2X0);
#endif

  UCSR0C = _BV(UCSZ01)| _BV(UCSZ00);
}



ISR(USART_UDRE_vect)
{
  if (muos_txbuffer.descriptor.len)
    {
      MUOS_SERIAL_TX_REGISTER = MUOS_CBUFFER_POP (muos_txbuffer);
      if (!muos_txbuffer.descriptor.len)
        muos_hw_serial_tx_stop ();
    }
}


ISR(USART_RX_vect)
{
  if (UCSR0A & _BV(FE0))
    muos_error_set_unsafe (muos_error_rx_frame);

  if (UCSR0A & _BV(DOR0))
    muos_error_set_unsafe (muos_error_rx_overrun);

  if (UCSR0A & _BV(UPE0))
    muos_error_set_unsafe (muos_error_rx_parity);

  if (MUOS_BUFFER_FREE(muos_rxbuffer))
    {
      MUOS_BUFFER_PUSH (muos_rxbuffer, MUOS_SERIAL_RX_REGISTER);
      if (!muos_status.serial_rxrtq_pending)
        {
          muos_status.serial_rxrtq_pending = true;
          muos_rtq_pushback_unsafe (MUOS_SERIAL_RXCALLBACK);
        }
    }
  else
    {
      muos_error_set_unsafe (muos_error_rx_buffer_overflow);
    }
}

#endif
