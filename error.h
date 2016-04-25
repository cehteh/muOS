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

#ifndef MUOS_ERROR_H
#define MUOS_ERROR_H

#include <stdint.h>
#include <stdbool.h>

//defined_errors:
#define MUOS_ERRORS                                                                             \
  MUOS_ERROR(error_error) /*: {ERRORDEF} unspecified error */                                   \
  MUOS_ERROR(warn_sched_depth) /*: {ERRORDEF} recursive scheduler calls exceeded */             \
  MUOS_ERROR(warn_wait_timeout) /*: {ERRORDEF} muos_wait() timed out */                         \
  MUOS_ERROR(error_clpq_overflow) /*: {ERRORDEF} clock priority queue full */                   \
  MUOS_ERROR(error_hpq_overflow) /*: {ERRORDEF} high priority queue full */                     \
  MUOS_ERROR(error_bgq_overflow) /*: {ERRORDEF} background priority queue full */               \
  MUOS_ERROR(error_rx_buffer_overflow) /*: {ERRORDEF} dropped received data (user code) */      \
  MUOS_ERROR(error_rx_buffer_underflow) /*: {ERRORDEF} read while no data available */          \
  MUOS_ERROR(error_rx_frame) /*: {ERRORDEF} wrong stop bit timing */                            \
  MUOS_ERROR(error_rx_overrun) /*: {ERRORDEF} dropped received data (uart driver) */            \
  MUOS_ERROR(error_rx_parity) /*: {ERRORDEF} parity error detected */                           \
  MUOS_ERROR(error_tx_buffer_overflow) /*: {ERRORDEF} To much data to send */                   \
  MUOS_ERROR(error_txqueue_overflow) /*: {ERRORDEF} To much data to send (TXQUEUE) */           \
  MUOS_ERROR(error_cppm_frame) /*: {ERRORDEF} received broken cppm frame */                     \
  MUOS_ERROR(error_cppm_hpq_callback) /*: {ERRORDEF} hpq overflow when pushing cppm handler */


//error_api:
//: .The type used for error codes
//: ----
//: typedef enum \{...\} muos_error
//: ----
typedef enum
  {
    muos_success,
#define MUOS_ERROR(name) muos_##name,
    MUOS_ERRORS
#undef MUOS_ERROR
    muos_errors_end,
  } muos_error;

extern volatile uint8_t muos_errors_[(muos_errors_end+7)/8];
extern volatile uint8_t muos_errors_pending_;


//error_api:
//: .Query number of pending errors
//: ----
//: uint8_t muos_error_pending (void)
//: ----
//:
//: Returns the number of errors which are flagged.
//:
static inline uint8_t
muos_error_pending (void)
{
  return muos_errors_pending_;
}


//error_api:
//: .Flagging asynchronous errors
//: ----
//: void muos_error_set (muos_error err)
//: void muos_error_set_isr (muos_error err)
//: ----
//:
//: +err+::
//:   Errorcode to flag
//:
//: When 'err' is 'muos_success' this function just returns. Thus idioms like
//:
//:  muos_error_set (function_which_may_return_an_error ());
//:
//: are possible.
//:
//: When 'err' is already flagged, nothing happens.
//:
//: The '*_isr' function is for contexts where interrupts are disabled.
//:
//PLANNED: do we need a muos_error_again in case a error was already set?
//:
void
muos_error_set_isr (muos_error err);

void
muos_error_set (muos_error err);


//error_api:
//: .Query the status of a error flag
//: ----
//: bool muos_error_peek (muos_error err)
//: ----
//:
//: +err+::
//:   error code to query
//:
//: Returns 'true' when the error is flagged, 'false' otherwise.
//:
bool
muos_error_peek (muos_error err);


//error_api:
//: .Check for errors
//: ----
//: bool muos_error_check (muos_error err)
//: bool muos_error_check_isr (muos_error err)
//: ----
//:
//: +err+::
//:   error code to query
//:
//: When 'err' was flagged then return true *and* clear that flag, otherwise return 'false'.
//:
//: The '*_isr' function is for contexts where interrupts are disabled.
//:
bool
muos_error_check (muos_error err);

bool
muos_error_check_isr (muos_error err);

#endif
