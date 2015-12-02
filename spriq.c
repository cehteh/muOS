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

#include <muos/spriq.h>

void
muos_spriq_push (struct muos_spriq* spriq, muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function fn)
{
   MUOS_ATOMIC
    {
      muos_spriq_index i = spriq->used;

      for (; i && (when < (muos_spriq_priority)(spriq->spriq[i/2].when - base)); i=i/2)
        {
          spriq->spriq[i] = spriq->spriq[i/2];
        }

      spriq->spriq[i] = (struct muos_spriq_entry){(muos_spriq_priority)(when + base), fn};
      ++spriq->used;
    }
}


void
muos_spriq_pop_unsafe (struct muos_spriq* spriq, struct muos_spriq_entry* event)
{
  muos_spriq_index i = 1;

  if (event)
    *event = spriq->spriq[0];

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


#if MUOS_RTPQ_LENGTH > 0
muos_rtpq_type muos_rtpq;

bool
muos_rtpq_schedule (muos_spriq_priority when)
{
  MUOS_ATOMIC
    {
      if (muos_rtpq.descriptor.used &&
          (muos_spriq_priority)(when - muos_rtpq.descriptor.spriq[0].when) < ((muos_spriq_priority)~0)/2 )
        {
          struct muos_spriq_entry event;
          muos_spriq_pop_unsafe (&muos_rtpq.descriptor, &event);
          MUOS_ATOMIC_RESTORE
            muos_rtpq.descriptor.spriq[0].fn(&event);
          return true;
        }
    }
  return false;
}

#endif
