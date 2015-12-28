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

#define MUOS_ERRORS                             \
  MUOS_ERROR(warn_sched_depth)                  \
  MUOS_ERROR(warn_tx_buffer_wait)               \
  MUOS_ERROR(error_clpq_overflow)               \
  MUOS_ERROR(error_bgq_overflow)                \
  MUOS_ERROR(error_rtq_overflow)                \
  MUOS_ERROR(error_rx_buffer_overflow)          \
  MUOS_ERROR(error_rx_frame)                    \
  MUOS_ERROR(error_rx_overrun)                  \
  MUOS_ERROR(error_rx_parity)


enum muos_errorcode
  {
    muos_success,
#define MUOS_ERROR(name) muos_##name,
    MUOS_ERRORS
#undef MUOS_ERROR
    muos_errors_end,
  };

extern volatile uint8_t muos_errors_[(muos_errors_end+7)/8];
extern volatile uint8_t muos_errors_pending_;


static inline uint8_t
muos_error_pending (void)
{
  return muos_errors_pending_;
}

void
muos_error_set_unsafe (enum muos_errorcode err);

void
muos_error_set (enum muos_errorcode err);

static inline bool
muos_error_peek (enum muos_errorcode err)
{
  return muos_errors_[err/8] & 1<<(err%8);
}

bool
muos_error_check (enum muos_errorcode err);

#endif
