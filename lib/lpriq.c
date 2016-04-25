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
#if 0
#include <muos/spriq.h>

void
muos_spriq_push (struct muos_spriq* spriq, muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function fn)
{
  muos_spriq_indexsize i = spriq->used;

  for (; i && (when < (muos_spriq_priority)(spriq->spriq[i/2].when - base)); i=i/2)
    {
      spriq->spriq[i] = spriq->spriq[i/2];
    }

  spriq->spriq[i] = (struct muos_spriq_entry){(muos_spriq_priority)(when + base), fn};
  ++spriq->used;
}


void
muos_spriq_pop_isr (struct muos_spriq* spriq)
{
  muos_spriq_size i = 1;

  muos_spriq_priority base = spriq->spriq[0].when;

  for (; i < spriq->used-1; i=2*i)
    {
      if ((muos_spriq_priority)(spriq->spriq[i].when - base) > (muos_spriq_priority)(spriq->spriq[i+1].when - base))
        ++i;

      if ((muos_spriq_priority)(spriq->spriq[spriq->used-1].when - base) < (muos_spriq_priority)(spriq->spriq[i].when - base))
        break;

      spriq->spriq[i/2] = spriq->spriq[i];
    }
  spriq->spriq[i/2] = spriq->spriq[--spriq->used];
}

#endif
