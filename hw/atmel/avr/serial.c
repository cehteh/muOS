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

#ifdef MUOS_SERIAL

#include <muos/muos.h>
#include <muos/serial.h>
#include <muos/hpq.h>

#include <stdlib.h>
#include <stdint.h>


//avr_uart_api:
//: .Calculating baudrate
//: ----
//: uint16_t
//: muos_avr_baudcalc (uint32_t baud, uint8_t tol)
//: ----
//:
//: Calculates the value for the UBRR frequency divider.
//: +baud+:: is the baudrate
//: +tol+:: is the acceptable tolerance in %*10
//:
//: *return*;;
//:   - 0xffff when the baudrate can not be reached within tolerance
//:   - the UBRR value OR'ed with 0x8000 when the U2X doublespeed bit should be set
//:
//PLANNED: Hardcode again for minimal builds, return fixed results then
static uint16_t
muos_avr_baudcalc (uint32_t baud, uint8_t tol)
{

#define MUOS_BAUD2UBRR(baud, samples) ((F_CPU+baud*samples/2)/((samples)*(baud))-1)
#define MUOS_UBRR2BAUD(ubrr, samples) (F_CPU/((samples)*((ubrr)+1)))

  uint16_t ubrr = MUOS_BAUD2UBRR (baud, 16);
  uint8_t err = labs (1000L - MUOS_UBRR2BAUD (ubrr, 16) * 1000L / baud);

  if (err <= tol)
    {
      return ubrr;
    }
  else
    {
      ubrr = MUOS_BAUD2UBRR (baud, 8);
      err = labs (1000L - MUOS_UBRR2BAUD (ubrr, 8) * 1000L / baud);
      if (err <= tol)
        {
          return 0x8000|ubrr;
        }
    }
  return 0xffff;
}


muos_error
muos_hw_serial_start (uint8_t hw, uint32_t baud, const char config[3], int rxsync)
{
  // first disable/reset the port and clear buffers when it was already active
  // we want to disable even if the parser later fails with an error
#define UART(hw, txsize, rxsize)                                                \
  case hw:                                                                      \
    UCSR##hw##B &= ~(_BV(TXEN##hw) | _BV(RXEN##hw) | _BV(RXCIE##hw));           \
    muos_cbuffer_init (&muos_txbuffer##hw.descriptor);                          \
    muos_cbuffer_init (&muos_rxbuffer##hw.descriptor);                          \
    if (rxsync >= 0)                                                            \
      {                                                                         \
        muos_serial_status[hw].serial_rx_dosync = true;                         \
        muos_serial_status[hw].serial_rx_insync = false;                        \
        muos_serial_rxsync[hw] = rxsync;                                        \
      }                                                                         \
    else                                                                        \
      {                                                                         \
        muos_serial_status[hw].serial_rx_dosync = false;                        \
        muos_serial_status[hw].serial_rx_insync = true;                         \
      }                                                                         \
    break;

  switch(hw)
    {
      MUOS_SERIAL_HW;
    }
#undef UART

  // parse config
  uint8_t ucsrb = 0;
  uint8_t ucsrc = 0;

  switch (config[0])
    {
    case '5':
      break;
    case '6':
      ucsrc = _BV(UCSZ00);
      break;
    case '7':
      ucsrc = _BV(UCSZ01);
      break;
    case '8':
      ucsrc = _BV(UCSZ01) | _BV(UCSZ00);
      break;
      /*PLANNED: case'9': 9 bit not supported */
      /* UCSR0B |= _BV(UCSZ02); */
      /* UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00); */
    default:
      return muos_fatal_serial_config;
    }

  switch (config[1])
    {
    case 'N':
      break;
    case 'O':
      ucsrc |= _BV(UPM01) | _BV(UPM00);
      break;
    case 'E':
      ucsrc |= _BV(UPM01);
      break;
    default:
      return muos_fatal_serial_config;
    }

  switch (config[2])
    {
    case '1':
      break;
    case '2':
      ucsrc = _BV(USBS0);
      break;
    default:
      return muos_fatal_serial_config;
    }

  uint16_t ubrr = muos_avr_baudcalc (baud, MUOS_SERIAL_BAUDTOL);
  if (ubrr == 0xffff)
    return muos_fatal_serial_config;

#define UART(hw, txsize, rxsize)                                        \
  case hw:                                                              \
    UBRR##hw = ubrr & ~0x8000;                                          \
    if (ubrr & 0x8000)                                                  \
      UCSR##hw##A |= _BV(U2X##hw);                                      \
    else                                                                \
      UCSR##hw##A &= ~_BV(U2X##hw);                                     \
    UCSR##hw##B = ucsrb;                                                \
    UCSR##hw##C = ucsrc;                                                \
    UCSR##hw##B = _BV(TXEN##hw) | _BV(RXEN##hw) | _BV(RXCIE##hw);       \
    break;

  switch(hw)
    {
      MUOS_SERIAL_HW;
    }
#undef UART
  return muos_success;
}



// TX ISR
#ifdef USART_UDRE_vect
#define ISRNAME(hw) USART_UDRE_vect
#else
#define ISRNAME(hw) USART##hw##_UDRE_vect
#endif

#define UART(hw, txsize, rxsize)                                        \
  ISR(ISRNAME(hw))                                                      \
  {                                                                     \
    MUOS_DEBUG_INTR_ON;                                                 \
    if (muos_txbuffer##hw.descriptor.len)                               \
      {                                                                 \
        UDR##hw = muos_cbuffer_pop (&muos_txbuffer##hw.descriptor);     \
        if (!muos_txbuffer##hw.descriptor.len)                          \
          UCSR##hw##B &= ~_BV(UDRIE##hw);                               \
      }                                                                 \
    else                                                                \
      {                                                                 \
        UCSR##hw##B &= ~_BV(UDRIE##hw);                                 \
      }                                                                 \
    MUOS_DEBUG_INTR_OFF;                                                \
}

MUOS_SERIAL_HW;
#undef UART
#undef ISRNAME



// RX ISR
#ifdef USART_RX_vect
#define ISRNAME(hw) USART_RX_vect
#else
#define ISRNAME(hw) USART##hw##_RX_vect
#endif

//PLANNED: sync when line is idle


#define UART(hw, txsize, rxsize)                                                                        \
  ISR(ISRNAME(hw))                                                                                      \
  {                                                                                                     \
    MUOS_DEBUG_INTR_ON;                                                                                 \
    bool err = false;                                                                                   \
    uint8_t data = UDR##hw;                                                                             \
    if (UCSR##hw##A & _BV(DOR##hw))                                                                     \
      {                                                                                                 \
        muos_serial_status[hw].error_rx_overrun;                                                        \
        err = true;                                                                                     \
      }                                                                                                 \
    if (UCSR##hw##A & _BV(UPE##hw))                                                                     \
      {                                                                                                 \
        muos_serial_status[hw].error_rx_parity;                                                         \
        err = true;                                                                                     \
      }                                                                                                 \
    if (UCSR##hw##A & _BV(FE##hw))                                                                      \
      {                                                                                                 \
        muos_serial_status[hw].error_rx_frame;                                                          \
        err = true;                                                                                     \
      }                                                                                                 \
    if (!muos_cbuffer_free (&muos_rxbuffer##hw.descriptor))                                             \
      {                                                                                                 \
        muos_serial_status[hw].error_rx_overflow;                                                       \
        err = true;                                                                                     \
      }                                                                                                 \
    if (err)                                                                                            \
      {                                                                                                 \
        if (muos_serial_status[hw].serial_rx_dosync)                                                    \
          muos_serial_status[hw].serial_rx_insync = false;                                              \
        muos_error_set_isr (muos_error_serial_status);                                                  \
      }                                                                                                 \
    else                                                                                                \
      {                                                                                                 \
        if (muos_serial_status[hw].serial_rx_dosync                                                     \
            && !muos_serial_status[hw].serial_rx_insync                                                 \
            && data == muos_serial_rxsync[hw])                                                          \
          {                                                                                             \
            muos_serial_status[hw].serial_rx_insync = true;                                             \
          }                                                                                             \
        if (muos_serial_status[hw].serial_rx_insync)                                                    \
          {                                                                                             \
            muos_cbuffer_push (&muos_rxbuffer##hw.descriptor, data);                                    \
            if (!muos_serial_status[hw].serial_rxhpq_pending && muos_serial_rxcallback[hw])             \
              {                                                                                         \
                muos_serial_status[hw].serial_rxhpq_pending = true;                                     \
                muos_error_set_isr (muos_hpq_push_arg_isr (muos_serial_rxhpq_call, hw, true));          \
              }                                                                                         \
          }                                                                                             \
      }                                                                                                 \
    MUOS_DEBUG_INTR_OFF;                                                                                \
  }


MUOS_SERIAL_HW;
#undef UART
#undef ISRNAME


#endif
