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

#include <string.h>

#include <muos/lib/buffer.h>


void
muos_buffer_push (muos_buffer_vptr buffer, const uint8_t value)
{
  buffer->buffer[buffer->len] = value;
  ++buffer->len;
}


void
muos_buffer_pop (muos_buffer_vptr buffer, muos_buffer_index n)
{
  buffer->len -= n;
  memmove ((void*)buffer->buffer, (void*)buffer->buffer+n, buffer->len);
}


