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

//PLANNED: stash muos_now_ away on recursive mainloops
//PLANNED: make ERRORFN optional
//PLANNED: ignoremask for ERRORFN for errors handled elsewhere
//PLANNED: instead disabling interrupts have a mutex and temp buffer for work queues
//PLANNED: finer locking in mainloop instead interrupt disable, per queue
//PLANNED: error_set hook for debugging
//PLANNED: example sections in all API doc
//PLANNED: find some way to get rid of most of the *_isr variants at compiletime


volatile struct muos_status_flags muos_status;


void
muos_sleep (void)
{
  if (muos_hw_clpq_set_compmatch ())
    {
      //TODO: select sleep mode depending on active hardware (adc, usart)
      muos_hw_sleep_prepare (MUOS_SCHED_SLEEP);
      // muos_hw_sleep () enables interrupts while sleeping
      do
          muos_hw_sleep ();
      while (!muos_status.schedule);
      muos_status.schedule = false;
      muos_hw_sleep_done ();
    }
  else
    {
      // busywait for timespans which are to small for compmatch
      muos_interrupt_enable ();
      //FIXME: new clpq while (MUOS_CLOCK_REGISTER < (typeof(MUOS_CLOCK_REGISTER)) muos_clpq.descriptor.spriq[0].when);
      muos_interrupt_disable ();
    }
}

//PLANNED: document and implement
void muos_die (void)
{
  muos_interrupt_disable ();
  muos_hw_shutdown ();
}


static bool
yield_loop (uint8_t count)
{
  muos_interrupt_disable ();
  do
    {
      do
        {
          do
            {
              do
                {
#ifdef MUOS_ERRORFN
                  if (muos_error_pending ())
                    {
                      MUOS_ERRORFN ();
                    }
#endif

                 muos_interrupt_disable ();

                 MUOS_DEBUG_SWITCH_TOGGLE;

                  if (!count--)
                    {
                      muos_interrupt_enable ();
                      return true;
                    }
                }
              while (muos_rtq_schedule ());  //FIXME: rename schedule to scheduler_isr
            }
          while (muos_clpq_schedule_isr ());
        }
      while (muos_hpq_schedule ());
    }
  while (muos_bgq_schedule ());

  muos_interrupt_enable ();
  return false;
}


#if defined(MUOS_YIELD_DEPTH) || defined(MUOS_WAIT_DEPTH)
static uint8_t sched_depth_;
uint8_t
muos_sched_depth (void)
{
  return sched_depth_;
}
#endif


#ifdef MUOS_YIELD_DEPTH
muos_error
muos_yield (uint8_t count)
{
  if (sched_depth_ >= MUOS_YIELD_DEPTH)
    {
      return muos_warn_sched_depth;
    }

  ++sched_depth_;
  yield_loop (count);
  --sched_depth_;

  return muos_success;
}

#endif


#include <muos/io.h>
void
muos_clpq_dump (uint8_t what);



#ifdef MUOS_WAIT_DEPTH

muos_error
muos_wait (muos_wait_fn fn, intptr_t param, muos_clock16 timeout)
{
  if (sched_depth_ >= MUOS_WAIT_DEPTH)
    {
      return muos_warn_sched_depth;
    }
  ++sched_depth_;

  muos_clock wakeup;
  muos_clock_now (&wakeup);
  muos_clock_add16 (&wakeup, timeout);

  muos_error ret = muos_clpq_at (&wakeup, NULL);

  if (ret == muos_success)
    while (1)
      {
        if (fn && fn (param))
          {
            muos_clpq_remove (&wakeup, NULL);  //FIXME: there is a race when the clpq_schedule removed the wakeup
            break;
          }

        if (muos_clock_is_expired (&wakeup))
          {
            ret = muos_warn_wait_timeout;
            break;
          }

        yield_loop (1);
      }


  --sched_depth_;

  return ret;
}

muos_error
muos_wait_poll (muos_wait_fn fn, intptr_t param, muos_clock16 timeout, uint32_t rep)
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
    {
      while (yield_loop (255));
      muos_sleep ();
    }
}
