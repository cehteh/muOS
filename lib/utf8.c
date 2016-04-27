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

#include <muos/lib/utf8.h>

size_t
muos_utf8len (const char* str)
{
  size_t len = 0;
  for (; *str; ++str)
    if (!muos_utf8cont (*str))
      ++len;
   return len;
}


uint8_t
muos_utf8size (const char* start)
{
  while (muos_utf8cont (*start))
    --start;

  uint8_t size = 1;
  if (muos_utf8start (*start))
    {
      size = 2;
      for (uint8_t s = *start<<2; s&128; s<<=1)
        {
          ++size;
        }
    }

  return size;
}



