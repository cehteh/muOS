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
#include <muos/hpq.h>
#include <muos/bgq.h>
#include <muos/clpq.h>
#include <muos/serial.h>
#include <muos/cppm.h>

//PLANNED: stash muos_now_ away on recursive mainloops

volatile struct muos_status_flags muos_status;

void
muos_sleep (void)
{
  if (muos_clpq_set_compmatch ())
    {
      //TODO: select sleep mode depending on active hardware (adc, usart)
      muos_hw_sleep_prepare (MUOS_SCHED_SLEEP);
      // muos_hw_sleep () enables interrupts while sleeping
      muos_hw_sleep ();
      muos_hw_sleep_done ();
    }
  else
    {
      // busywait for timespans which are to small for compmatch
      muos_interrupt_enable ();
      while (MUOS_CLOCK_REGISTER < (typeof(MUOS_CLOCK_REGISTER)) muos_clpq.descriptor.spriq[0].when);
      muos_interrupt_disable ();
    }
}

//PLANNED: document and implement
void muos_die (void)
{
  muos_hw_shutdown ();
}

void
muos_start (void)
{
#include <muos/init.inc>
}


static uint8_t sched_depth_;


//muos_api:
//: .Wait for some condition come true
//: ----
//: muos_error muos_wait (muos_wait_fn fn, intptr_t param, muos_shortclock timeout)
//: ----
//: +fn+::
//:   function checking for some condition, must return 'false' while the condition
//:   is not met and finally 'true' on success. Can be NULL, then 'muos_wait()' uses
//:   only the timeout for the wait.
//: +param+::
//:   an optinal intptr_t argument passed to the test function
//: +timeout+::
//:   time limit for the wait
//:
//: Calls a recursive mainloop and with testing for a given condition for some time.
//: Returns 'true' when the condition succeeded and 'false' on timeout or recursion limit.
//:
//: Care must be taken that I/O (and other things) are not anymore in order when the
//: mainloop is called recursively. Often it is better to avoid waiting and divide the
//: work into smaller tasks which are put in order on the work queues.
//:
//: Because of stack limits entering the mainloop recursively is limited. One should always
//: expect that a wait can return instantly with 'false' and the error 'muos_warn_sched_depth'
//: being flaged.
//:
//: .Returns
//:   'muos_success':: the wait condition got met
//:   'muos_warn_sched_depth':: depth limit for recursive mainloops hit
//:   'muos_warn_wait_timeout':: timed out
muos_error
muos_wait (muos_wait_fn fn, intptr_t param, muos_shortclock timeout)
{
  if (sched_depth_ >= MUOS_SCHED_DEPTH)
    {
      return muos_warn_sched_depth;
    }

  muos_clock start = muos_now_ = muos_clock_now ();

  ++sched_depth_;

  muos_interrupt_disable ();

  while (1)
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
                      --sched_depth_;
                      return muos_success;
                    }

                  muos_now_ = muos_clock_now_isr ();

                  if (muos_clock_elapsed (muos_now_, start) > timeout)
                    {
                      muos_interrupt_enable ();
                      --sched_depth_;
                      return muos_warn_wait_timeout;
                    }

                  if (muos_error_pending ())
                    {
                      MUOS_ERRORFN ();
                      muos_interrupt_disable ();
                    }
                }
               while (muos_clpq_schedule (muos_now_));
            }
          while (muos_hpq_schedule ());
        }
      while (muos_bgq_schedule ());

      muos_sleep ();
    }
}


//muos_api:
//: .Enter Mainloop recursively, scheduling other jobs
//: ----
//: muos_error muos_yield (uint8_t count)
//: ----
//: +count+::
//:   number of jobs to schedule, must be less than 254
//:
//: Calls a recursive mainloop executing at most 'count' jobs. A job here is any one
//: thing queued on any of the works queues. Returns if nothing left to do the
//: 'count' limit got reached or the scheduler depth limit got reached. Same precautions
//: as on 'muos_wait()' apply.
//:
//: Yielding id applicable when one has code which loops for some extended time but shall
//: not stall the work to be done *and* this code will never be called recursively.
//:
//: .Returns
//:   'muos_warn_sched_depth':: depth limit for recursive mainloops hit
//:   'muos_success':: yielded at most 'count' times
//PLANNED: add priority, which queues to schedule
muos_error
muos_yield (uint8_t count)
{
  if (sched_depth_ >= MUOS_SCHED_DEPTH)
    {
      return muos_warn_sched_depth;
    }

  ++sched_depth_;
  ++count;
  muos_interrupt_disable ();

  do
    {
      do
        {
          do
            {
              --count;
              muos_now_ = muos_clock_now_isr ();

              if (muos_error_pending ())
                {
                  MUOS_ERRORFN ();
                  muos_interrupt_disable ();
                }
            }
          while (count && muos_clpq_schedule (muos_now_));
        }
      while (count && muos_hpq_schedule ());
    }
  while (count && muos_bgq_schedule ());

  muos_interrupt_enable ();
  --sched_depth_;

  return muos_success;
}



int __attribute__((OS_main))
main()
{
  CLKPR = _BV(CLKPCE);
  CLKPR = 0;

  #if 0
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
#endif

  //TODO: how to init all muos structures .. #define MUOS_EXPLICIT_INIT

#ifdef MUOS_DEBUG
  muos_debug_init ();
#endif

#if MUOS_DEBUG_BUSY ==1
  //TODO: hardware dependent
  //PLANNED: debug driver
  DDRB = _BV(PINB5) | _BV(PINB4);
  PORTB = _BV(PINB5);
#endif

#if MUOS_HPQ_LENGTH >= 2
  muos_hpq_pushback (MUOS_INITFN);
  muos_hpq_pushback (muos_start);
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
                  muos_now_ = muos_clock_now_isr ();

                  if (muos_error_pending ())
                    {
                      MUOS_ERRORFN ();
                      muos_interrupt_disable ();
                    }
                }
              while (muos_clpq_schedule (muos_now_));
            }
          while (muos_hpq_schedule ());
        }
      while (muos_bgq_schedule ());

      muos_sleep ();
    }
}
