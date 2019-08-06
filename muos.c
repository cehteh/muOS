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
#include <muos/hpq.h>
#include <muos/bgq.h>
#include <muos/clpq.h>
#include <muos/serial.h>
#include <muos/stepper.h>
#include <muos/cppm.h>
#include <muos/stck.h>

#include <stdlib.h>

//PLANNED: stash muos_now_ away on recursive mainloops
//PLANNED: make ERRORFN optional
//PLANNED: ignoremask for ERRORFN for errors handled elsewhere
//PLANNED: instead disabling interrupts have a mutex and temp buffer for work queues
//PLANNED: finer locking in mainloop instead interrupt disable, per queue
//PLANNED: error_set hook for debugging
//PLANNED: example sections in all API doc
//PLANNED: find some way to get rid of most of the *_isr variants at compiletime

volatile bool muos_schedule;

//PLANNED: document and implement
void muos_die (void)
{
  muos_interrupt_disable ();
  muos_hw_shutdown (); //FIXME: let it fallthrough
  abort ();
}


#if defined(MUOS_YIELD_DEPTH) || defined(MUOS_WAIT_DEPTH)
static uint8_t sched_depth_;
uint8_t
muos_sched_depth (void)
{
  return sched_depth_;
}
#endif


static bool
sched_loop (muos_wait_fn fn, void* param, bool sleep)
{
  MUOS_DEBUG_BUSY_ON;
  muos_interrupt_disable ();
  do
    {
      do
        {
          do
            {
              do
                {
                  if (fn && fn (param))
                    {
                      muos_interrupt_enable ();
                      return true;
                    }

#if defined(MUOS_STCK) && defined(MUOS_STCK_AUTO)
                  if (!muos_stck_check (MUOS_STCK_AUTO))
                    muos_error_set_isr (muos_fatal_stack_overflow);
#endif

#ifdef MUOS_ERRORFN
                  if (muos_error_pending ())
                    {
                      MUOS_ERRORFN ();
                    }
#endif

                 muos_interrupt_disable ();

                 MUOS_DEBUG_SWITCH_TOGGLE;

                 //PLANNED: how to ensure that lower pri queues don't stall?
                }
              while (muos_rtq_schedule ());  //FIXME: rename schedule to scheduler_isr
            }
          while (muos_clpq_schedule_isr ());
        }
      while (muos_hpq_schedule ());
    }
  while (muos_bgq_schedule ());

#ifdef MUOS_SCHED_SLEEP
#error //FIXME: sched sleep implementation is incomplete
  if (sleep && muos_hw_clpq_wake_isr ())
    {
      //TODO: select sleep mode depending on active hardware (adc, usart)
      muos_hw_sleep_prepare (MUOS_SCHED_SLEEP);
      do
        {
          // muos_hw_sleep () enables interrupts while sleeping
          muos_hw_sleep ();
        }
      while (!muos_schedule);
      muos_schedule = false;
      muos_hw_sleep_done ();
    }
#else
  (void) sleep;
#endif // MUOS_SCHED_SLEEP

  muos_interrupt_enable ();
  return false;
}

#ifdef MUOS_YIELD_DEPTH

static bool
yield_countdown (void* count)
{
  return *(uint8_t*)count--;
}


muos_error
muos_yield (uint8_t count)
{
  //TODO: add 'sleep' parameter

  if (sched_depth_ >= MUOS_YIELD_DEPTH)
    {
      return muos_warn_sched_depth;
    }

  ++sched_depth_;
  sched_loop (yield_countdown, &count, true);
  --sched_depth_;

  return muos_success;
}

#endif


#ifdef MUOS_WAIT_DEPTH

struct wait_data
{
  muos_wait_fn fn;
  void* param;
  muos_clock wakeup;
  muos_error ret;
};

static bool
wait_predicate (void* data)
{
  if (((struct wait_data*)data)->fn && ((struct wait_data*)data)->fn (((struct wait_data*)data)->param))
    {
      muos_clpq_remove (&((struct wait_data*)data)->wakeup, NULL);
      return true;
    }

  if (muos_clock_is_expired (&((struct wait_data*)data)->wakeup))
    {
      ((struct wait_data*)data)->ret = muos_warn_wait_timeout;
      return true;
    }

  return false;
}


muos_error
muos_wait (muos_wait_fn fn, void* param, muos_clock16 timeout)
{
  if (sched_depth_ >= MUOS_WAIT_DEPTH)
    {
      return muos_warn_sched_depth;
    }
  ++sched_depth_;

  struct wait_data data;
  data.fn = fn;
  data.param = param;

  muos_clock_now (&data.wakeup);
  muos_clock_add16 (&data.wakeup, timeout);

  while ((data.ret = muos_clpq_at (&data.wakeup, NULL, true)) == muos_error_clpq_nounique)
    muos_clock_add8 (&data.wakeup, 1);

  if (data.ret == muos_success)
    {
      while (!sched_loop (wait_predicate, &data, true));
    }

  --sched_depth_;

  return data.ret;
}

muos_error
muos_wait_poll (muos_wait_fn fn, void* param, muos_clock16 timeout, uint32_t rep)
{
  while (rep--)
    {
      muos_error err = muos_wait (fn, param, timeout);
      if (err != muos_warn_wait_timeout)
        return err;
    }
  return muos_warn_wait_timeout;
}
#endif





//TODO: since we autogenerate declarations here, remove them from header files
#define MUOS_INIT(fn) void fn(void)
#include <muos/init.inc>
#undef MUOS_INIT
void
muos_init (void)
{
#define MUOS_INIT(fn) fn()
#include <muos/init.inc>
#undef MUOS_INIT
}


int __attribute__((OS_main))
main()
{
  MUOS_HW_INIT;

#ifdef MUOS_STCK
  muos_stck_init ();
#endif // MUOS_STCK

  //TODO: how to init all muos structures .. #define MUOS_EXPLICIT_INIT

#if defined(MUOS_CLPQ_LENGTH) && MUOS_CLPQ_LENGTH>0
  //PLANNED: clpq calibration
#endif

#ifdef MUOS_DEBUG
  muos_hw_debug_init ();
#endif

#if MUOS_HPQ_LENGTH >= 2
  muos_hpq_push (MUOS_INITFN);
  muos_hpq_push (muos_init);
#elif MUOS_BGQ_LENGTH >= 2
  muos_bgq_push (MUOS_INITFN);
  muos_bgq_push (muos_init);
#else
  MUOS_INITFN ();
  muos_init ();
#endif

  muos_interrupt_enable ();

  while (1)
    sched_loop (NULL, NULL, true);
}
