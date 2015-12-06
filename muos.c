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

#include "muos_config.h"
#include <muos/queue.h>
#include <muos/clock.h>
#include <muos/spriq.h>

extern void
MUOS_INITFN (void);

void muos_error (void)
{
  muos_die ();
}

void muos_die (void)
{
  cli();
  for(;;);
}


void
muos_start (void)
{
  muos_clock_start ();
  //TODO: muos_intr_enable()
  sei();
}

int  __attribute__((OS_main))
main()
{
  //TODO: how to init all muos structures .. #define MUOS_EXPLICIT_INIT
  //TODO: bool muos_wait (fn, param, timeout)
  //TODO: void muos_yield (count)

#if MUOS_RTQ_LENGTH >= 2
  muos_rtq_pushback (MUOS_INITFN);
  muos_rtq_pushback (muos_start);
#elif MUOS_BGQ_LENGTH >= 2
  muos_bgq_pushback (MUOS_INITFN);
  muos_bgq_pushback (muos_start);
#else
  MUOS_INITFN();
  muos_start();
#endif

  do
    {
      do
        {
          do
            {
              while(muos_rtpq_schedule(muos_clock_now()));
            }
          while(muos_rtq_schedule ());
        }
      while (muos_bgq_schedule ());

      //      MUOS_SLEEP; schedule
    }
  while (1);
}
