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

#ifndef MUOS_H
#define MUOS_H

#include <stdint.h>


#include MUOS_HW_HEADER


#define MDEBUG(c) do { muos_serial_tx_byte (c);} while (0)

void
muos_sleep (void);


extern void
MUOS_INITFN (void);

extern void
MUOS_ERRORFN (void);

#define MUOS_ARRAY_ELEMENTS(array) (sizeof(array)/sizeof(*(array)))


#define MUOS_NOINIT __attribute__ ((section (".noinit")))




//#define MUOS_EXPLICIT_INIT MUOS_NOINIT

// stash some status bits together
extern volatile struct muos_status_flags
{
  uint8_t serial_rx_sync:1;
  uint8_t serial_rxrtq_pending:1;
  uint8_t lineedit_pending:1;
  uint8_t lineedit_ovwr:1;
  uint8_t txqueue_pending:1;
} muos_status;


#endif
