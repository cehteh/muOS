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


extern volatile struct muos_errorflags
  {
#define MUOS_ERROR(name) uint8_t name:1;
    MUOS_ERRORS
#undef MUOS_ERROR
  } muos_errors_;

#define MUOS_ERROR_SET(name) muos_errors_.name = true

#define MUOS_ERROR_PEEK(name) muos_errors.name

#define MUOS_ERROR_CHECK(name)                          \
  ({                                                    \
    muos_interrupt_disable();                           \
    bool r = muos_errors_.name;                         \
    muos_errors_.name = false;                          \
    muos_interrupt_enable();                            \
    r;                                                  \
  })


#endif
