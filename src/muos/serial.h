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

void
muos_serial_init (void);

void
muos_serial_tx_byte (uint8_t b);

uint8_t
muos_serial_rx_byte (void);

void
muos_serial_rxhpq_again (muos_queue_function f);

#if MUOS_SERIAL_TXBUFFER > 1
typedef MUOS_CBUFFERDEF(MUOS_SERIAL_TXBUFFER) muos_txbuffer_type;
extern muos_txbuffer_type muos_txbuffer;
#endif


#if MUOS_SERIAL_RXBUFFER > 1
typedef MUOS_CBUFFERDEF(MUOS_SERIAL_RXBUFFER) muos_rxbuffer_type;
extern muos_rxbuffer_type muos_rxbuffer;
#endif

#define MUOS_SERIAL_TX_REGISTER MUOS_HW_SERIAL_TX_REGISTER(MUOS_SERIAL_HW)
#define MUOS_SERIAL_RX_REGISTER MUOS_HW_SERIAL_RX_REGISTER(MUOS_SERIAL_HW)

#endif
