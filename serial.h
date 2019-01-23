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

#if MUOS_SERIAL_NUM > 1
typedef bool (*muos_serial_rxcallback_type)(uint8_t hw);
#else
typedef bool (*muos_serial_rxcallback_type)(void);
#endif

extern volatile struct muos_serial_flags
{
  uint8_t serial_tx_waiting:1;
  uint8_t serial_rx_waiting:1;
  uint8_t serial_rx_dosync:1;
  uint8_t serial_rx_insync:1;
  uint8_t serial_rxhpq_pending:1;
  uint8_t lineedit_pending:1;
  uint8_t lineedit_ovwr:1;
  uint8_t txqueue_pending:1;
  //Error flags
  uint8_t error_tx_blocked:1;
  uint8_t error_tx_overflow:1;
  uint8_t error_rx_blocked:1;
  uint8_t error_rx_underflow:1;
  uint8_t error_rx_overflow:1;
  uint8_t error_rx_frame:1;
  uint8_t error_rx_overrun:1;
  uint8_t error_rx_parity:1;
} muos_serial_status[MUOS_SERIAL_NUM];

extern char muos_serial_rxsync[MUOS_SERIAL_NUM];
extern muos_serial_rxcallback_type muos_serial_rxcallback[MUOS_SERIAL_NUM];


#define UART(hw, txsize, rxsize)                                        \
  MUOS_CBUFFERDEC(muos_txbuffer##hw, txsize);                           \
  MUOS_CBUFFERDEC(muos_rxbuffer##hw, rxsize);

MUOS_SERIAL_HW
#undef UART

#if MUOS_SERIAL_NUM > 1
extern struct muos_cbuffer* const muos_txbuffer[];

static inline muos_cbuffer_index
muos_serial_tx_free (uint8_t hw)
{
  return muos_cbuffer_free (muos_txbuffer[hw]);
}
#else
static inline muos_cbuffer_index
muos_serial_tx_free (void)
{
  return muos_cbuffer_free (&muos_txbuffer0);
}
#endif

//uart_api:
//: .Initializing and Starting
//: ----
//: #if MUOS_SERIAL_NUM > 1
//: muos_error
//: muos_serial_start (uint8_t hw, uint32_t baud, char config[3], int rxsync);
//: #else
//: muos_error
//: muos_serial_start (uint32_t baud, char config[3], int rxsync);
//: #endif
//: ----
//:
//: +hw+::
//:   Index of the serial hardware to use (only when more than one)
//:
//: +baud+::
//:   Baudrate to configure.
//:
//: +config+::
//:   3 character C string in the form "<databits><parity><stopbits>"
//:   defining the Serial configuration. For example "8N1".
//:   /databits/:::
//:     5-8 databits are supported.
//:   /parity/:::
//:     One of 'N'one, 'E'ven or 'Odd'.
//:   /stopbits/:::
//:     1 or 2.
//:
//: +rxsync+::
//:   Character value for a character to synchronize the port with or -1 for
//    disabling synchronization.
//:
//: Initializes and starts a serial port.
//:
//: *return*::
//:   'muos_success' on success or muos_error_serial_config when the baudrate
//:   could not be generated or the configuration contains some unknown
//:   characters. A running port will be stopped and flushed unconditionally
//:   (even in case of error).
//:
#if MUOS_SERIAL_NUM > 1
muos_error
muos_serial_start (uint8_t hw, uint32_t baud, char config[3], int rxsync, muos_serial_rxcallback_type callback);
#else
muos_error
muos_serial_start (uint32_t baud, char config[3], int rxsync, muos_serial_rxcallback_type callback);
#endif



//TODO: DOCME
#if MUOS_SERIAL_NUM > 1
static inline void
muos_serial_tx_run (uint8_t hw)
{
  switch (hw)
    {
#define UART(hw, txsize, rxsize)                \
      case hw:                                  \
        muos_hw_serial_tx##hw##_run ();

      MUOS_SERIAL_HW
#undef UART
    }
}

static inline void
muos_serial_tx_stop (uint8_t hw)
{
  switch (hw)
    {
#define UART(hw, txsize, rxsize)                \
      case hw:                                  \
        muos_hw_serial_tx##hw##_stop ();

      MUOS_SERIAL_HW
#undef UART
    }
}

static inline void
muos_serial_rx_run (uint8_t hw)
{
  switch (hw)
    {
#define UART(hw, txsize, rxsize)                \
      case hw:                                  \
        muos_hw_serial_rx##hw##_run ();

      MUOS_SERIAL_HW
#undef UART
    }
}

static inline void
muos_serial_rx_stop (uint8_t hw)
{
  switch (hw)
    {
#define UART(hw, txsize, rxsize)                \
      case hw:                                  \
        muos_hw_serial_rx##hw##_stop ();

      MUOS_SERIAL_HW
#undef UART
    }
}


#else

static inline void
muos_serial_tx_run (void)
{
  muos_hw_serial_tx0_run ();
}

static inline void
muos_serial_tx_stop (void)
{
  muos_hw_serial_tx0_stop ();
}

static inline void
muos_serial_rx_run (void)
{
  muos_hw_serial_rx0_run ();
}

static inline void
muos_serial_rx_stop (void)
{
  muos_hw_serial_rx0_stop ();
}

#endif



//uart_api:
//: .Sending data
//: ----
//: #if MUOS_SERIAL_NUM > 1
//: muos_error muos_serial_tx_byte (uint8_t hw, uint8_t data)
//: #else
//: muos_error muos_serial_tx_byte (uint8_t data)
//: #endif
//: ----
//:
//: +hw+::
//:   Index of the serial hardware to use (only when more than one)
//:
//: +data+::
//:   The byte to send
//:
//: Pushes a single byte on the TX buffer.
//:
//: *return*::
//:   muos_error_tx_buffer_overflow:::
//:     Transmission buffer is full
//:   muos_error_tx_blocked:::
//:     There is already a blocking write pending
//:   muos_success::
//:     no error.
//:
#if MUOS_SERIAL_NUM > 1
muos_error
muos_serial_tx_byte (uint8_t hw, uint8_t b);
#else
muos_error
muos_serial_tx_byte (uint8_t b);
#endif


//TODO: DOCME
#if MUOS_SERIAL_NUM > 1
void
muos_serial_tx_flush (uint8_t hw);
#else
void
muos_serial_tx_flush (void);
#endif


//PLANNED: muos_cbuffer_index muos_serial_tx_avail (void);


//uart_api:
//: .Reading data
//: ----
//: int16_t muos_serial_rx_byte (uint8_t hw)
//: int16_t muos_serial_rx_byte (void)
//: ----
//:
//: +hw+::
//:   Index of the serial hardware to use (only when more than one)
//:
//: +timeout+::
//:   Time to wait for blocking reads
//:
//: Pops and a byte from the receive buffer. Zero or positive return value is
//: a successful read from the buffer. Negative return indicates an error by the
//: negated error number. Note that the serial driver may flag asynchronous errors too.
//:
//:   -muos_error_rx_buffer_underflow:::
//:     No data available for reading
//:
//: *return*::
//:   a character value or a negated error as noted above.
//:
#if MUOS_SERIAL_NUM > 1
int16_t
muos_serial_rx_byte (uint8_t hw);
#else
int16_t
muos_serial_rx_byte (void);
#endif



//TODO: DOCME
#if MUOS_SERIAL_NUM > 1
void
muos_serial_rx_flush (uint8_t hw, bool desync);
#else
void
muos_serial_rx_flush (bool desync);
#endif







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






#if MUOS_SERIAL_NUM > 1
#else
#endif
void
muos_serial_rxhpq_call (void);



#endif
