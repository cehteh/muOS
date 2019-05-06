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
muos_clpq_schedule (void)
{
  muos_clpq.now = muos_clock_now ();

#if 0 //FIXME: new clpq
  if (muos_clpq.descriptor.used)
    {
      if ((when - muos_clpq.descriptor.spriq[muos_clpq.descriptor.used - 1].when) < ((muos_spriq_priority)~0/2))
        {

          const struct muos_spriq_entry tmp = muos_clpq.descriptor.spriq[muos_clpq.descriptor.used - 1];

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
                  muos_clpq.descriptor.spriq[0].fn = 0;
                  muos_clpq.descriptor.spriq[0].when += (muos_spriq_priority)-1/2-1;
                }
              else
                muos_spriq_pop (&muos_clpq.descriptor);
            }

          if (tmp.fn)
            {
              muos_interrupt_enable ();
              tmp.fn (&tmp);
              muos_interrupt_disable ();
            }
          return true;  //PLANNED: may shortcut 'return false' when nothing to be done in 'near' future (within hwclock reach)
        }
    }
#endif

  return false;
}


//PLANNED: delay relative to clpq.now
//muos_error
//muos_clpq16_in (muos_clock16 when, muos_clpq_function what)
//muos_error
//muos_clpq32_in (muos_clock32 when, muos_clpq_function what)


muos_error
muos_clpq_at_isr (muos_clock when, muos_clpq_function what)
{
  (void) when; (void) what; //FIXME:
#if 0 //FIXME: new clpq
  if (muos_clpq.descriptor.used == MUOS_SPRIQ_SIZE(muos_clpq))
    return muos_error_clpq_overflow;

  muos_spriq_push (&muos_clpq.descriptor, base, when, what);
#endif
  return muos_success;
}

#endif
