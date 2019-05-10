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

#ifdef MUOS_ERROR_DEF
#include MUOS_ERROR_DEF
#endif

#ifndef MUOS_APP_ERRORS
#define MUOS_APP_ERRORS
#endif

//PLANNED: error log

//error_api:
//: .Error Check Macro
//: ----
//: MUOS_OK(fn)
//: ----
//: Wraps fn which must be a function call returning a 'muos_error' an a check for success
//: or returning the error.
#define MUOS_OK(fn) do { muos_error err = fn; if (err) return err; } while(0)

//TODO: conditional error generation depending on what drivers are enabled
//defined_errors:

#include "error_codes.h"

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

#ifdef MUOS_HW_HEADER
#include MUOS_HW_HEADER
#endif

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
//: muos_error muos_error_set (muos_error err)
//: muos_error muos_error_set_isr (muos_error err)
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
//: Returns 'err'.
//:
//PLANNED: do we need a muos_error_again in case a error was already set?
//:
muos_error
muos_error_set_isr (muos_error err);

static inline muos_error
muos_error_set (muos_error err)
{
  if (err)
    {
      muos_interrupt_disable ();
      muos_error_set_isr (err);
      muos_interrupt_enable ();
    }
  return err;
}



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
static inline bool
muos_error_peek (muos_error err)
{
  return muos_errors_[err/8] & 1<<(err%8);
}



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
muos_error_check_isr (muos_error err);

static inline bool
muos_error_check (muos_error err)
{
  bool ret = false;

  muos_interrupt_disable ();
  ret = muos_error_check_isr (err);
  muos_interrupt_enable ();

  return ret;
}

//error_api:
//: .Clear all pending errors
//: ----
//: void muos_error_clearall (void)
//: void muos_error_clearall_isr (void)
//: ----
//:
//: Clears all pending async errors.
//:
//: The '*_isr' function is for contexts where interrupts are disabled.
//:
void
muos_error_clearall_isr (void);

static inline void
muos_error_clearall (void)
{
  muos_interrupt_disable ();
  muos_error_clearall_isr ();
  muos_interrupt_enable ();
}


//TODO: docme
#ifdef MUOS_ERROR_STR
const char __flash*
muos_error_str (muos_error err);
#endif

#endif
