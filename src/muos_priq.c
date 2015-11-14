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

#include "muos_priq.h"

void
muos_priq_push (struct muos_priq* priq, muos_priq_priority base, muos_priq_priority when, muos_priq_function fn)
{
  muos_priq_size i = priq->used;

  for (; i && (when < (muos_priq_priority)(priq->priq[i/2].when - base)); i=i/2)
    {
      priq->priq[i] = priq->priq[i/2];
    }

  priq->priq[i] = (struct muos_priq_entry){(muos_priq_priority)(when + base), fn};
  ++priq->used;
}


void
muos_priq_pop (struct muos_priq* priq)
{
  muos_priq_size i = 1;

  muos_priq_priority base = priq->priq[0].when;

  for (; i < priq->used-1; i=2*i)
    {
      if ((muos_priq_priority)(priq->priq[i].when - base) > (muos_priq_priority)(priq->priq[i+1].when - base))
        ++i;

      if ((muos_priq_priority)(priq->priq[priq->used-1].when - base) < (muos_priq_priority)(priq->priq[i].when - base))
        break;

      priq->priq[i/2] = priq->priq[i];
    }
  priq->priq[i/2] = priq->priq[--priq->used];
}
