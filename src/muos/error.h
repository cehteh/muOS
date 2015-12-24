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

// insert:
// debug messages before DEBUG
//   only for logging when debugging is on
// warnings before WARN
//   just informal, everything goes on
// errors before ERROR
//   something broken, action needed
// fatal errors before FATAL
//   fuba
#define muos_errordefs                          \
  error(SUCCESS, "no error")                    \
  error(DEBUG, "debug log")                     \
  error(WARN_SCHED_DEPTH, "scheduler depth")    \
  error(WARN, "undefined warning")              \
  error(ERROR, "undefined error")               \
  error(FATAL, "undefined fatal error")


enum muos_errorcode
  {
#define error(symbol, description) MUOS_##symbol,
    muos_errordefs
#undef error
  };


#if MUOS_ERROR_LOG_LENGTH == 0

// setting errors becomes a nop when the error log api is disabled
#define muos_error_set(_) /*nop*/

// whereas muos_error() for querying is undedfind because this needs
// to be handled at a higher level (conditional in users code)

#elif MUOS_ERROR_LOG_LENGTH == 1
extern volatile uint8_t muos_error_;

static inline uint8_t
muos_error (void)
{
  return muos_error_;
}

static inline enum muos_errorcode
muos_error_set (enum muos_errorcode error)
{
  return muos_error_ = error;
}


#elif MUOS_ERROR_LOG_LENGTH > 1
extern volatile struct muos_error_log {
  uint8_t index;
  uint8_t errors[MUOS_ERROR_LOG_LENGTH];
} muos_error_;

static inline uint8_t
muos_error (void)
{
  return muos_error_.errors[muos_error_.index%MUOS_ERROR_LOG_LENGTH];
}

enum muos_errorcode
muos_error_set (enum muos_errorcode error);

void
muos_die (void) __attribute__ ((weak));


#endif


#endif
