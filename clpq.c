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


//PLANNED: realtime in interrupt function calls
//PLANNED: sorting before normal functions
//PLANNED: possibly add a small 'boost' (MUOS_CLOCK_LATENCY/2) for sorting them earlier (sorting only)


//PLANNNED: wrapper for recurring jobs

bool
muos_clpq_schedule (muos_spriq_priority when)
{

  if (muos_clpq.descriptor.used)
    {
      muos_spriq_priority last = muos_clpq.descriptor.used - 1;
      if ((when - muos_clpq.descriptor.spriq[last].when) < ((muos_spriq_priority)~0/2))
        {
          const struct muos_spriq_entry tmp = muos_clpq.descriptor.spriq[last];

          if (sizeof(muos_spriq_priority) > sizeof(muos_hwclock)) /* statically evaluated */
            {
              // no need for time barrier
              muos_spriq_pop (&muos_clpq.descriptor);
            }
          else
            {
              // with time barrier for the sliding window
              if (muos_clpq.descriptor.used == 1)
                {
                  muos_clpq.descriptor.spriq[last].fn = 0;
                  muos_clpq.descriptor.spriq[last].when += (muos_spriq_priority)-1/2-1;
                }
              else
                muos_spriq_pop (&muos_clpq.descriptor);
            }

          if (muos_clpq.descriptor.spriq[last].fn)
            {
              muos_clpq.descriptor.spriq[last].fn (&tmp);
              muos_interrupt_disable ();
            }

          return true;  //PLANNED: may shortcut 'return false' when nothing to be done in 'near' future (within hwclock reach)
        }
    }

  return false;
}



//TODO: return error instead async
muos_error
muos_clpq_at_isr (muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function what)
{
  if (muos_clpq.descriptor.used == MUOS_SPRIQ_SIZE(muos_clpq))
    return muos_error_clpq_overflow;

  muos_spriq_push (&muos_clpq.descriptor, base, when, what);
  return muos_success;
}

// This is the time between setting compmatch for wakeup and going to sleep
#define MUOS_CLOCK_LATENCY (32U/MUOS_CLOCK_PRESCALER)


// The time a schedule loop takes
#define MUOS_CLOCK_BUSYLATENCY (64U/MUOS_CLOCK_PRESCALER)


//TODO: return when already compmatch or disable compmatch when time is too short
/*
//TODO: new implementation

    - must be in current hw_count range
    - not too soon (time to set compmatch)
    - wakeup earlier
    - compmatch can wrap!
    - dont forget to reschedule when a new event was added before the first one

    also
    when - now
        < hw range
           else no compmatch
        > compset time
     -> compmatch
   */

bool
muos_clpq_set_compmatch (void)
{
  if (!muos_clpq.descriptor.used)
    return true;

  muos_spriq_priority at = muos_clpq.descriptor.spriq[muos_clpq.descriptor.used - 1].when -
    (muos_clock_count_ << (sizeof(MUOS_CLOCK_REGISTER) * 8));

  muos_spriq_priority now = MUOS_CLOCK_REGISTER;

  if (at < ((typeof(MUOS_CLOCK_REGISTER)) ~0) - MUOS_CLOCK_BUSYLATENCY)
    {
      if (at > MUOS_CLOCK_LATENCY + now
          && now < ((typeof(MUOS_CLOCK_REGISTER)) ~0) - MUOS_CLOCK_LATENCY)
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
