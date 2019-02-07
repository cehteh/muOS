/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2015, 2019                       Christian Thäter <ct@pipapo.org>
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

#include <muos/lib/spriq.h>


void
muos_spriq_push (struct muos_spriq* spriq, muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function fn)
{
  muos_spriq_index i = spriq->used;
  muos_spriq_priority a = when+base;

  while (i)
    {
      muos_spriq_priority b = spriq->spriq[i-1].when;

      if (a >= base && b >= base && a < b)
        break;

      if (a >= base && b < base)
        break;

      if (a < base && b < base && a < b)
        break;

      spriq->spriq[i] = spriq->spriq[i-1];
      --i;
    }

  spriq->spriq[i] = (struct muos_spriq_entry){a, fn};
  ++spriq->used;
}


void
muos_spriq_pop (struct muos_spriq* spriq)
{
  if (spriq->used)
    --spriq->used;
}



bool
muos_spriq_remove (struct muos_spriq* spriq, muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function fn)
{
  muos_spriq_index i;

  muos_spriq_priority rem = when+base;

  for(i = 0; i < spriq->used; ++i)
    {
      if (spriq->spriq[i].when == rem && spriq->spriq[i].fn == fn)
        break;
    }
  if (i != spriq->used)
    {
      --spriq->used;
      for(; i < spriq->used; ++i)
        {
          spriq->spriq[i] = spriq->spriq[i+1];
        }

      return true;
    }
  return false;
}

