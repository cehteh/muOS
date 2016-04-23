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

#include <muos/lib/cbuffer.h>


void
muos_cbuffer_push (struct muos_cbuffer* cbuffer, muos_cbuffer_index size, const uint8_t value)
{
  muos_cbuffer_index index = cbuffer->start + cbuffer->len;
  ++cbuffer->len;
  if (index >= size)
    index -= size;
  cbuffer->cbuffer[index] = value;
}


uint8_t
muos_cbuffer_pop (struct muos_cbuffer* cbuffer, muos_cbuffer_index size)
{
  uint8_t ret = cbuffer->cbuffer[cbuffer->start];
  ++cbuffer->start;
  if (cbuffer->start >= size)
    cbuffer->start -= size;

  --cbuffer->len;
  return ret;
}


void
muos_cbuffer_popn (struct muos_cbuffer* cbuffer, muos_cbuffer_index size, muos_cbuffer_index n)
{
  cbuffer->start += n;
  if (cbuffer->start >= size)
    cbuffer->start -= size;

  cbuffer->len -= n;
}


uint8_t
muos_cbuffer_peek (struct muos_cbuffer* cbuffer, muos_cbuffer_index size, muos_cbuffer_index index)
{
  index = cbuffer->start + index;
  if (index >= size)
    index -= size;

  return cbuffer->cbuffer[index];
}


void
muos_cbuffer_poke (struct muos_cbuffer* cbuffer, muos_cbuffer_index size, muos_cbuffer_index index, const uint8_t value)
{
  index = cbuffer->start + index;
  if (index >= size)
    index -= size;

  cbuffer->cbuffer[index] = value;
}
