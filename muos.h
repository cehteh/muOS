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

#ifndef MUOS_H
#define MUOS_H

#include <stdint.h>
#include <stdbool.h>

#include <muos/debug.h>

//TODO: fixup, documentation




#include <muos/error.h>
#include <muos/clock.h>


void
muos_sleep (void);


extern void
MUOS_INITFN (void);

extern void
MUOS_ERRORFN (void);

#define MUOS_ARRAY_ELEMENTS(array) (sizeof(array)/sizeof(*(array)))


#define MUOS_NOINIT __attribute__ ((section (".noinit")))


typedef bool (*muos_wait_fn)(intptr_t);



//#define MUOS_EXPLICIT_INIT MUOS_NOINIT

// stash some status bits together
extern volatile struct muos_status_flags
{
  uint8_t serial_tx_blocked:1;
  uint8_t serial_rx_blocked:1;
  uint8_t serial_rx_sync:1;
  uint8_t serial_rxhpq_pending:1;
  uint8_t lineedit_pending:1;
  uint8_t lineedit_ovwr:1;
  uint8_t txqueue_pending:1;
} muos_status;



#ifdef MUOS_SCHED_DEPTH
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
muos_wait (muos_wait_fn fn, intptr_t param, muos_shortclock timeout);



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
muos_yield (uint8_t count);
#endif // MUOS_SCHED_DEPTH


#endif
