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

//PLANNED: make a 'autoconfig.h' that defines derrived configurations that are dependant on other mixed configs


#include <muos/debug.h>

#include <muos/error.h>
#include <muos/clock.h>

//PLANNED: making workqueues pure queues without push-front, then userland functions can pop arguments without starting with interrupts disabled
//PLANNED: api to purge work queues (state reset)
//PLANNED: for all xxx() which have xxx_isr() make 3 variants:  xxx() does automatic locking xxx_locked() and xxx_unlocked() (needs better names)


extern void
MUOS_INITFN (void);

extern void
MUOS_ERRORFN (void);

#define MUOS_ARRAY_ELEMENTS(array) (sizeof(array)/sizeof(*(array)))


#define MUOS_NOINIT __attribute__ ((section (".noinit")))

typedef bool (*muos_wait_fn)(void*);

void muos_die (void);


//#define MUOS_EXPLICIT_INIT MUOS_NOINIT

extern volatile bool muos_ready;

#ifdef MUOS_WAIT_DEPTH
//muos_api:
//: .Wait for some condition come true
//: ----
//: typedef bool (*muos_wait_fn)(void*)
//:
//: muos_error
//: muos_wait (muos_wait_fn fn, void* param, muos_clock16 timeout)
//:
//: muos_error
//: muos_wait_poll (muos_wait_fn fn, void* param, muos_clock16 timeout, uint32_t rep)
//: ----
//: +fn+::
//:   function checking for some condition, must return 'false' while the condition
//:   is not met and finally 'true' on success. Can be NULL, then 'muos_wait()' uses
//:   only the timeout for the wait.
//: +param+::
//:   an optinal intptr_t argument passed to the test function
//: +timeout+::
//:   time limit for the wait
//: +rep+::
//:   repetitions
//:
//: Calls a recursive mainloop and with testing for a given condition for some time.
//:
//: 'muos_wait()' will put the MCU to sleep when there is nothing to do and wake itself
//: after +timeout+. This makes it suitable for waiting on state changes which are caused
//: by interrupts and application code, but not for polling hardware changes that don't
//: wake the MPU. The time span is limited to 16bit only.
//:
//: 'muos_wait_poll()' puts 'muos_wait()' for at most +rep+ times in a loop which which
//: each sleeps at most for +timeout+. This makes it suitable for polling things that don't
//: wake the MCU and for waiting for long time spans as well.
//:
//: Care must be taken that I/O (and other things) are not anymore in order when the
//: mainloop is called recursively. Often it is better to avoid waiting and divide the
//: work into smaller tasks which are put in order on the work queues.
//:
//: Because of stack limits entering the mainloop recursively is limited. One should always
//: expect that a wait can return instantly with 'muos_warn_sched_depth'.
//:
//: 'muos_wait()' is only available when MUOS_WAIT_DEPTH is defined.
//:
//: .Returns
//:   'muos_success':: the wait condition got met
//:   'muos_warn_sched_depth':: depth limit for recursive mainloops hit
//:   'muos_warn_wait_timeout':: timed out
muos_error
muos_wait (muos_wait_fn fn, void* param, muos_clock16 timeout);

muos_error
muos_wait_poll (muos_wait_fn fn, void* param, muos_clock16 timeout, uint32_t rep);
#endif

#ifdef MUOS_YIELD_DEPTH
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
//: Yielding is applicable when one has code which loops for some extended time but shall
//: not stall the work to be done *and* this code will never be called recursively.
//:
//: 'muos_yield()' is only available when MUOS_YIELD_DEPTH is defined.
//:
//: .Returns
//:   'muos_warn_sched_depth':: depth limit for recursive mainloops hit
//:   'muos_success':: yielded at most 'count' times
//PLANNED: add priority, which queues to schedule
muos_error
muos_yield (uint8_t count);
#endif



#if defined(MUOS_YIELD_DEPTH) || defined(MUOS_WAIT_DEPTH)
// undocumented, will be replaced
uint8_t
muos_sched_depth (void);
#endif


//TODO: needs volatile cast
//#define MUOS_ATOMIC_READ(dest, source) do {dest = source;} while (dest != source)


#endif
