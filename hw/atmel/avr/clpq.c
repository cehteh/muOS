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

#include <muos/clpq.h>

#include <stddef.h>


#ifdef MUOS_SCHED_SLEEP

ISR(ISRNAME_COMPMATCH(MUOS_CLOCK_HW))
{
  // compmatch is one-shot
  MUOS_DEBUG_INTR_ON;
  MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE (MUOS_CLOCK_HW);
  muos_ready = true;
  MUOS_DEBUG_INTR_OFF;
}



static muos_clock16 clpq_compmatch_latency;


// return 'true' to signal to go to sleep and false for busy loop
bool
muos_hw_clpq_wake_isr (void)
{
  // can always sleep
  if (!muos_clpq.used)
    return true;

  if (clpq_barrier (muos_clpq.entries[muos_clpq.used-1].what))
    return true;

  // time from now to next
  muos_clock16 now16 = muos_clock_now16_isr ();
  muos_clock16 next = muos_clpq.entries[muos_clpq.used-1].when - now16;

  if (next <= clpq_compmatch_latency)
    return false;

  if (next >= (muos_hwclock)~0)
    return true;

  MUOS_HW_CLOCK_ISR_COMPMATCH_REG (MUOS_CLOCK_HW) = muos_clpq.entries[muos_clpq.used-1].when;
  MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE (MUOS_CLOCK_HW);

  return true;
}

#endif // MUOS_SCHED_SLEEP


void
muos_clpq_95init (void)
{
#ifdef MUOS_SCHED_SLEEP
  muos_clock16 bench_latency = 0;
  muos_clock now;

  uint8_t i;

  for (i = 0; i < 8 && bench_latency < 32767; ++i)
    {
      muos_interrupt_disable ();
      muos_clock_now_isr (&now);
      bench_latency += muos_clock_since_isr (&now);
      muos_interrupt_enable ();
    }

  bench_latency = (bench_latency+i-1) / i; // round up

  muos_clock16 compmatch_latency = 0;

  muos_clock test;
  muos_clock_now (&test);
  muos_clock_add16 (&test, 172);

  if (muos_error_set (muos_clpq_at (&test, NULL, false)) != muos_success)
    return;

  for (i = 0; i < 8 && compmatch_latency < 32767; ++i)
    {
      muos_interrupt_disable ();
      muos_clock_now_isr (&now);
      muos_hw_clpq_wake_isr ();
      compmatch_latency += muos_clock_since_isr (&now);
      compmatch_latency -= bench_latency;
      muos_interrupt_enable ();
    }

  muos_clpq_remove (&test, NULL);

  clpq_compmatch_latency = (compmatch_latency+i-1)/i; // round up
#endif
}


#endif
