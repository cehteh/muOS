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


#if MUOS_CLPQ_LENGTH > 0

//#include <muos/muos.h>
#include <muos/clpq.h>

//FIXME: probe the the latencies

// This is the time between setting compmatch for wakeup and going to sleep
#define MUOS_CLOCK_LATENCY (32U/MUOS_CLOCK_PRESCALER)


// The time a schedule loop takes
#define MUOS_CLOCK_BUSYLATENCY (128U/MUOS_CLOCK_PRESCALER)





//static uint16_t clock_latency;
//static uint16_t busy_latency;




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
muos_hw_clpq_set_compmatch (void)
{
 
#if 0 //FIXME: new impl
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
    {
      MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE (MUOS_CLOCK_HW);
    }
#endif
  return true;
}

#endif
