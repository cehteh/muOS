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

#include <muos/error.h>

//PLANNED: generate error description table in progmem

#if MUOS_ERROR_LOG_LENGTH == 1
uint8_t muos_error_;

#elif MUOS_ERROR_LOG_LENGTH > 1
struct muos_error_log muos_error_;

void
muos_error_set (enum muos_errorcode error)
{
  if (muos_error_.errors[muos_error_.index%MUOS_ERROR_LOG_LENGTH] == error)
    return;

  if (muos_error_.errors[muos_error_.index%MUOS_ERROR_LOG_LENGTH])
    ++muos_error_.index;

  muos_error_.errors[muos_error_.index%MUOS_ERROR_LOG_LENGTH] = error;
}

#endif
