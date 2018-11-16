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

#ifndef MUOS_SERIAL_H
#define MUOS_SERIAL_H

#include <stdint.h>
#include <stdbool.h>
#include <muos/lib/cbuffer.h>
#include <muos/lib/queue.h>
#include <muos/error.h>

//PLANNED: lowwater/highwater generic

void
muos_serial_30init (void);


//uart_api:
//: .Sending data
//: ----
//: muos_error muos_serial_tx_nonblocking_byte (uint8_t b);
//: muos_error muos_serial_tx_blocking_byte (uint8_t b);
//: muos_error muos_serial_tx_byte (uint8_t data)
//: ----
//:
//: +data+::
//:   The byte to send
//:
//: Pushes a single byte on the TX buffer.
//:
//: muos_serial_tx_nonblocking_byte::
//:   Will not block. In case of error one of the following errors gets returned:
//:
//:   muos_error_tx_buffer_overflow:::
//:     Transmission buffer is full
//:
//:   muos_error_tx_blocked:::
//:     There is already a blocking write pending
//:
//: muos_serial_tx_blocking_byte::
//:   Waits until data can be send, entering the scheduler recursively.
//:   May return one of the following errors:
//:
//:   muos_warn_sched_depth:::
//:     Scheduler depth  exceeded.
//:
//:   muos_error_tx_blocked:::
//:     There is already a blocking write pending
//:
//:   muos_warn_wait_timeout:::
//:     TX got stuck. The timeout is calculated internally depending on baudrate so
//:     that some chars should been send. When this error happens something got
//:     seriously wrong and the TX doesn't send any data.
//:
//: muos_serial_tx_byte::
//:   Picks one of the functions above, depending on configuration.
//:
//: All calls return 'muos_success' on success or errors as noted above.
//:
muos_error
muos_serial_tx_nonblocking_byte (uint8_t b);

#ifdef MUOS_SCHED_DEPTH
muos_error
muos_serial_tx_blocking_byte (uint8_t b);
#endif

static inline muos_error
muos_serial_tx_byte (uint8_t b)
{
#ifdef MUOS_SERIAL_TX_BLOCKING
#ifdef MUOS_SCHED_DEPTH
  return muos_serial_tx_blocking_byte (b);
#else
# error MUOS_SERIAL_TX_BLOCKING needs MUOS_SCHED_DEPTH
#endif
#else
  return muos_serial_tx_nonblocking_byte (b);
#endif
}


void
muos_serial_tx_flush (void);


//PLANNED: muos_cbuffer_index muos_serial_tx_avail (void);


//uart_api:
//: .Reading data
//: ----
//: uint16_t muos_serial_rx_nonblocking_byte (void)
//: uint16_t muos_serial_rx_blocking_byte (muos_shortclock timeout)
//: uint16_t muos_serial_rx_byte (void)
//: ----
//:
//: +timeout+::
//:   Time to wait for blocking reads
//:
//: Pops and a byte from the receive buffer. Zero or positive return value is
//: a successful read from the buffer. Negative return indicates an error by the
//: negated error number. Note that the UART driver may flag asynchronous errors too.
//:
//: muos_serial_rx_nonblocking_byte::
//:  Will not block. Following errors can happen:
//:
//:   muos_error_rx_buffer_underflow:::
//:     No data available for reading
//:
//:   muos_error_rx_blocked:::
//:     There is already a blocking read pending
//:
//: muos_serial_rx_blocking_byte::
//:   Waits until data becomes available, entering the scheduler recursively.
//:   A timeout of 0 means infinite waits.
//:
//:   Following errors can happen:
//:
//:   muos_warn_sched_depth:::
//:     Scheduler depth  exceeded.
//:
//:   muos_error_rx_blocked:::
//:     There is already a blocking read pending
//:
//:   muos_warn_wait_timeout:::
//:     No data received within 'timeout'.
//:
//: muos_serial_rx_byte::
//:   Picks one of the functions above, depending on configuration.
//:   The timeout for the blocking case defaults to infinite waits.
//:
//: Return a character value or a negated error as noted above.
//:
int16_t
muos_serial_rx_nonblocking_byte (void);

#ifdef MUOS_SCHED_DEPTH
int16_t
muos_serial_rx_blocking_byte (muos_shortclock timeout);
#endif

static inline int16_t
muos_serial_rx_byte (void)
{
#ifdef MUOS_SERIAL_RX_BLOCKING
#ifdef MUOS_SCHED_DEPTH
  return muos_serial_rx_blocking_byte (0);
#else
# error MUOS_SERIAL_RX_BLOCKING needs MUOS_SCHED_DEPTH
#endif
#else
  return muos_serial_rx_nonblocking_byte ();
#endif
}



//PLANNED: muos_cbuffer_index muos_serial_rx_avail (void);

void
muos_serial_rx_flush (bool desync);


//uart_api:
//: .RX Callback type
//: ----
//: typedef bool (*muos_serial_rxcallback)(void)
//: ----
//:
//: The type for the user-defined 'MUOS_SERIAL_RXCALLBACK' function.
//: This function gets called from the 'hpq' when there is data data on
//: the RX buffer. When it does not consume all data will called again
//: when it returns 'true'. When it returns 'false' it will only
//: be called again when *new* data is available on the buffer.
//:
typedef bool (*muos_serial_rxcallback)(void);

#ifdef MUOS_SERIAL_RXCALLBACK
extern bool MUOS_SERIAL_RXCALLBACK (void);
#endif



#if MUOS_SERIAL_TXBUFFER > 1
typedef MUOS_CBUFFERDEF(MUOS_SERIAL_TXBUFFER) muos_txbuffer_type;
extern muos_txbuffer_type muos_txbuffer;
#endif


#if MUOS_SERIAL_RXBUFFER > 1
typedef MUOS_CBUFFERDEF(MUOS_SERIAL_RXBUFFER) muos_rxbuffer_type;
extern muos_rxbuffer_type muos_rxbuffer;

#ifdef MUOS_SERIAL_RXCALLBACK
void
muos_serial_rxhpq_call (void);
#endif

#endif

#define MUOS_SERIAL_TX_REGISTER MUOS_HW_SERIAL_TX_REGISTER(MUOS_SERIAL_HW)
#define MUOS_SERIAL_RX_REGISTER MUOS_HW_SERIAL_RX_REGISTER(MUOS_SERIAL_HW)

#endif
