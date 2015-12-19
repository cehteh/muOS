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

#include <muos/clock.h>
#include <muos/rtq.h>
#include <muos/bgq.h>
#include <muos/clpq.h>

extern void
MUOS_INITFN (void);

void muos_error (void)
{
  muos_die ();
}

void muos_die (void)
{
  muos_interrupt_disable ();
  for(;;);
}

static inline void
muos_sleep (void)
{
  muos_hw_sleep_prepare ();
  muos_clpq_set_compmatch ();
  muos_hw_sleep ();
}

void
muos_start (void)
{
  muos_clock_start ();
  muos_interrupt_enable ();
}


static uint8_t sched_depth_;

typedef bool (*muos_wait_fn)(intptr_t);

static bool
muos_sched_enter (void)
{
  if (sched_depth_ >= MUOS_SCHED_DEPTH)
    {
      muos_error_set (MUOS_WARN_SCHED_DEPTH);
      return false;
    }
  return true;
}

bool
muos_wait (muos_wait_fn fn, intptr_t param, muos_shortclock timeout)
{
  if (!muos_sched_enter ())
    return false;

  muos_clock end = muos_now_ + timeout;

  ++sched_depth_;
  while (muos_now_ < end)
    {
      do
        {
          do
            {
               while (muos_clpq_schedule (muos_now_ = muos_clock_now ()));
            }
          while (muos_rtq_schedule ());
        }
      while (muos_bgq_schedule ());

      if (fn)
        {
          if (fn (param))
            {
              --sched_depth_;
              return true;
            }
        }

      muos_sleep ();
    }
  --sched_depth_;
  return false;
}


// count must be <254
void
muos_yield (uint8_t count)
{
  if (!muos_sched_enter ())
    return;

  ++sched_depth_;
  ++count;
  do
    {
      do
        {
          do
            {
              --count;
            }
          while (count && muos_clpq_schedule (muos_now_ = muos_clock_now ()));
        }
      while (count && muos_rtq_schedule ());
    }
  while (count && muos_bgq_schedule ());
  --sched_depth_;
}



int __attribute__((OS_main))
main()
{
  //TODO: how to init all muos structures .. #define MUOS_EXPLICIT_INIT

#if MUOS_RTQ_LENGTH >= 2
  muos_rtq_pushback (MUOS_INITFN);
  muos_rtq_pushback (muos_start);
#elif MUOS_BGQ_LENGTH >= 2
  muos_bgq_pushback (MUOS_INITFN);
  muos_bgq_pushback (muos_start);
#else
  MUOS_INITFN ();
  muos_start ();
#endif

  while (1)
    {
      do
        {
          do
            {
              while (muos_clpq_schedule (muos_now_ = muos_clock_now ()));
            }
          while (muos_rtq_schedule ());
        }
      while (muos_bgq_schedule ());

      muos_sleep ();
    }
}
