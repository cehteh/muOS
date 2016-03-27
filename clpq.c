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

#include <muos/clpq.h>
#include <muos/clock.h>
#include <muos/error.h>

#if MUOS_CLPQ_LENGTH > 0

muos_clpq_type muos_clpq;


//PLANNED:
//: realtime in interrupt function calls
//: high bit tag
//: sorting before normal functions
//: possibly add a small 'boost' (MUOS_CLOCK_LATENCY/2) for sorting them earlier (sorting only)

bool
muos_clpq_schedule (muos_spriq_priority when)
{
  if (muos_clpq.descriptor.used &&
      (muos_spriq_priority)(when - muos_clpq.descriptor.spriq[0].when) < (muos_spriq_priority)(((muos_spriq_priority)~0)/2))
    {
      if (sizeof(muos_spriq_priority) > sizeof(muos_hwclock)) /* static evaluated */
        {
          // no need for time barrier
          muos_interrupt_enable ();
          muos_clpq.descriptor.spriq[0].fn ((const struct muos_spriq_entry*)&muos_clpq.descriptor.spriq[0]);
          muos_interrupt_disable ();
          muos_spriq_pop (&muos_clpq.descriptor);
        }
      else
        {
          // with time barrier for the sliding window
          if (muos_clpq.descriptor.spriq[0].fn)
            {
              muos_interrupt_enable ();
              muos_clpq.descriptor.spriq[0].fn ((const struct muos_spriq_entry*)&muos_clpq.descriptor.spriq[0]);
              muos_interrupt_disable ();
            }

          if (muos_clpq.descriptor.used > 1)
            {
              muos_spriq_pop (&muos_clpq.descriptor);
            }
          else
            {
              muos_clpq.descriptor.spriq[0].fn = 0;
              muos_clpq.descriptor.spriq[0].when += (muos_spriq_priority)-1/2-1;
            }
        }
      return true;
    }

  return false;
}



void
muos_clpq_at_unsafe (muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function what)
{
  if (muos_clpq.descriptor.used < MUOS_SPRIQ_SIZE(muos_clpq))
    {
      muos_spriq_push (&muos_clpq.descriptor, base, when, what);
    }
  else
    {
      muos_error_set_unsafe (muos_error_clpq_overflow);
    }
}

// This is the time between setting compmatch for wakeup and going to sleep
#define MUOS_CLOCK_LATENCY ((224U+MUOS_CLOCK_PRESCALER/2)/MUOS_CLOCK_PRESCALER)

// The time a busy loop takes
#define MUOS_CLOCK_BUSYLATENCY (16U/MUOS_CLOCK_PRESCALER)

bool
muos_clpq_set_compmatch (void)
{
  muos_spriq_priority at = muos_clpq.descriptor.spriq[0].when -
    (muos_clock_count_ << (sizeof(MUOS_CLOCK_REGISTER) * 8));

  if (at < ((typeof(MUOS_CLOCK_REGISTER)) ~0) - MUOS_CLOCK_BUSYLATENCY)
    {
      if (at > MUOS_CLOCK_LATENCY + MUOS_CLOCK_REGISTER
          && MUOS_CLOCK_REGISTER < ((typeof(MUOS_CLOCK_REGISTER)) ~0) - MUOS_CLOCK_LATENCY)
        {
          MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE (MUOS_CLOCK_HW, at);
        }
      else
        return false;
    }
  else
    MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE (MUOS_CLOCK_HW);

  return true;
}

#endif
