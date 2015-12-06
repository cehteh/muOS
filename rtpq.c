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

#include <muos/rtpq.h>

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
