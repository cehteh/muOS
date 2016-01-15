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

#include <muos/muos.h>
#include <muos/error.h>
#include <muos/clock.h>
#include <muos/rtq.h>
#include <muos/bgq.h>
#include <muos/clpq.h>


volatile struct muos_status_flags muos_status;

void
muos_sleep (void)
{
  //TODO: select sleed mode depending on active hardware (adc, usart)
  muos_hw_sleep_prepare (MUOS_SCHED_SLEEP);
  muos_clpq_set_compmatch ();
#if MUOS_DEBUG_IDLE ==1
  PORTB &= ~_BV(PINB5);
#endif
  muos_hw_sleep ();
  muos_hw_sleep_done ();
#if MUOS_DEBUG_IDLE ==1
  PORTB |= _BV(PINB5);
#endif
}

void muos_die (void)
{
  muos_interrupt_disable ();
  muos_hw_shutdown ();
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
      muos_error_set (muos_warn_sched_depth);
      return false;
    }
  return true;
}

bool
muos_wait (muos_wait_fn fn, intptr_t param, muos_shortclock timeout)
{
  if (fn && fn (param))
    {
      return true;
    }

  if (!muos_sched_enter ())
    return false;

  bool overflow = false;
  muos_clock end = muos_now_ + timeout;

  if (end < muos_now_)
    overflow = true;

  ++sched_depth_;
  while (overflow || muos_now_<end)
    {
      if (overflow && muos_now_<end)
        {
          overflow = false;
        }

      do
        {
          do
            {
              do
                {
                  muos_now_ = muos_clock_now ();

                  if (muos_error_pending ())
                    {
                      muos_interrupt_disable ();
                      MUOS_ERRORFN ();
                      muos_interrupt_disable ();
                    }
                }
               while (muos_clpq_schedule (muos_now_));
            }
          while (muos_rtq_schedule ());
        }
      while (muos_bgq_schedule ());

      if (fn && fn (param))
        {
          --sched_depth_;
          return true;
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
              muos_now_ = muos_clock_now ();

              if (muos_error_pending ())
                {
                  muos_interrupt_disable ();
                  MUOS_ERRORFN ();
                  muos_interrupt_disable ();
                }
            }
          while (count && muos_clpq_schedule (muos_now_));
        }
      while (count && muos_rtq_schedule ());
    }
  while (count && muos_bgq_schedule ());
  --sched_depth_;
}



int __attribute__((OS_main))
main()
{

#if F_CPU != 16000000UL
  CLKPR = _BV(CLKPCE);
#if F_CPU == 62500UL
  CLKPR = _BV(CLKPS3);
#endif
#if F_CPU == 125000UL
  CLKPR = _BV(CLKPS2) | _BV(CLKPS1)| _BV(CLKPS0);
#endif
#if F_CPU == 250000UL
  CLKPR = _BV(CLKPS2) | _BV(CLKPS1);
#endif
#if F_CPU == 500000UL
  CLKPR = _BV(CLKPS2) | _BV(CLKPS0);
#endif
#if F_CPU == 1000000UL
  CLKPR = _BV(CLKPS2);
#endif
#if F_CPU == 2000000UL
  CLKPR = _BV(CLKPS1) | _BV(CLKPS0);
#endif
#if F_CPU == 4000000UL
  CLKPR = _BV(CLKPS1);
#endif
#if F_CPU == 8000000UL
  CLKPR = _BV(CLKPS0);
#endif
#endif

  //TODO: how to init all muos structures .. #define MUOS_EXPLICIT_INIT

#if MUOS_DEBUG_IDLE ==1
  //TODO: hardware dependent
  DDRB = _BV(PINB5);
  PORTB |= _BV(PINB5);
#endif

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
              do
                {
                  muos_now_ = muos_clock_now ();

                  if (muos_error_pending ())
                    {
                      muos_interrupt_disable ();
                      MUOS_ERRORFN ();
                      muos_interrupt_disable ();
                    }
                }
              while (muos_clpq_schedule (muos_now_));
            }
          while (muos_rtq_schedule ());
        }
      while (muos_bgq_schedule ());

      muos_sleep ();
    }
}
