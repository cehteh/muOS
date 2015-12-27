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

extern volatile struct muos_errorflags
  {
    //uint8_t :1;
    //uint8_t :1;
    //uint8_t :1;
    //uint8_t :1;
    //uint8_t :1;
    //uint8_t :1;
    uint8_t sched_depth_warning:1;
    uint8_t clpq_overflow:1;
    uint8_t bgq_overflow:1;
    uint8_t rtq_overflow:1;
    uint8_t tx_buffer_wait:1;
    uint8_t rx_buffer_overflow:1;
    uint8_t rx_frame_error:1;
    uint8_t rx_overrun_error:1;
    uint8_t rx_parity_error:1;
  } muos_errors_;

#define MUOS_ERROR_SET(name) muos_errors_.name = true

#define MUOS_ERROR_PEEK(name) muos_errors.name

#define MUOS_ERROR_CHECK(name)                          \
  ({                                                    \
    muos_interrupt_disable();                           \
    bool r = rmuos_errors.name;                         \
    rmuos_errors.name = false;                          \
    muos_interrupt_enable();                            \
    r;                                                  \
  })


#endif
