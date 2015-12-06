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

#ifndef MUOS_CONFIG_H
#define MUOS_CONFIG_H

// Template
#ifndef MUOS_
//#define MUOS_
#define MUOS_
#endif

// The defaults choosen here favor the safe but not the exact cases

//FIXME: muos_config.h and muos.h are independent, never include each other

//INIT:

// when set to 1, then all static initialization of muos internal datastructures is disabled
// initialization must then be done explicitly in init sections or in main before any of these
// facilities are used
#ifndef MUOS_EXPLICIT_INIT
#define MUOS_EXPLICIT_INIT 0
//#define MUOS_EXPLICIT_INIT 1
#endif

// Name of the user-define initialization function.
// This function is pushed on the rtq (or if not available bgq) when starting up. It is responsible
// for initializing everything.
#ifndef MUOS_INITFN
#define MUOS_INITFN init
#endif



//QUEUE:

// the bits used for indexing queues
// comes in 3 variants
//  4 bits allow only small queues for up to (8-16) functions, use only for really small targets
//  8 bits allow queues for up to (128-256) functions, this is the default
//  16 bits allow huge queues, use only when really required
#ifndef MUOS_QUEUE_INDEX
//#define MUOS_QUEUE_INDEX 4
#define MUOS_QUEUE_INDEX 8
//#define MUOS_QUEUE_INDEX 16
#endif

// How many entriex the realtime Queue can hold, set to 0 to disable the rtq
#ifndef MUOS_RTQ_LENGTH
#define MUOS_RTQ_LENGTH 32
#endif

// How many entriex the background Queue can hold, set to 0 to disable the bgq
#ifndef MUOS_BGQ_LENGTH
#define MUOS_BGQ_LENGTH 32
#endif


// How functions with arguments are tagged in queues, currently only the negate is implemened
// Works with mpu's with up to 64k flash
#ifndef MUOS_QUEUE_ARGTAG
#define MUOS_QUEUE_ARGTAG NEGATE
#endif



//SPRIQ:

// type used for the 'priorities' if the priority queue
// The priority queue uses a 'sliding window'.
// Scheduler needs to be called at least half of the range provided by this type.
#ifndef MUOS_SPRIQ_TYPE
#define MUOS_SPRIQ_TYPE MUOS_CLOCK_SHORT_TYPE
//#define MUOS_SPRIQ_TYPE 8
//#define MUOS_SPRIQ_TYPE 16
//#define MUOS_SPRIQ_TYPE 32
#endif

// Variable type (size) for holding the size of the spriq
// 8 for up to 255 entries, 16 for up to 65k entries. 8 saves only one bit ram, usually not worth it
#ifndef MUOS_SPRIQ_INDEX
//#define MUOS_SPRIQ_INDEX 8
#define MUOS_SPRIQ_INDEX 16
#endif


//LPRIQ:



// type used for the 'priorities' if the priority queue
// Scheduler needs to be called at least half of the range provided by this type.
#ifndef MUOS_LPRIQ_TYPE
#define MUOS_LPRIQ_TYPE MUOS_CLOCK_TYPE
//#define MUOS_LPRIQ_TYPE 8
//#define MUOS_LPRIQ_TYPE 16
//#define MUOS_LPRIQ_TYPE 32
#endif

// Variable type (size) for holding the size of the priq
// 8 for up to 255 entries, 16 for up to 65k entries. 8 saves only one bit ram, usually not worth it
#ifndef MUOS_LPRIQ_INDEX
//#define MUOS_SPRIQ_INDEX 8
#define MUOS_LPRIQ_INDEX 16
#endif



//RTPQ:

// How many entriex the scheduling Queue can hold, set to 0 to disable the rtpq
#ifndef MUOS_RTPQ_LENGTH
#define MUOS_RTPQ_LENGTH 32
#endif







//CLOCKHW:
// timer hardware setup is tightly bound to the hardware capabilities and only barely
// abstracted, check for the respective hardware implementation about possible
// settings.

// Which Hardware timer to use
// This is a hardware dependent config. usually a number or a simple name
#ifndef MUOS_CLOCK_HW
#define MUOS_CLOCK_HW 0
#endif

// Prescaler from some hardware defined master clock
#ifndef MUOS_CLOCK_HW_PRESCALER
#define MUOS_CLOCK_PRESCALER 256
#endif

// The Compare/Match unit of the Timer to use
// This is a hardware dependent config. usually a number or a simple name
#ifndef MUOS_CLOCK_HW_COMPAREMATCH
#define MUOS_CLOCK_HW_COMPAREMATCH A
#endif


//CLOCK:
// MµOS clock configuration above the hardware

// The type (bitsize) used for the overflow counter of the clock.
// together with the harware register of the clock this gives the full time.
#ifndef MUOS_CLOCK_TYPE
//#define MUOS_CLOCK_TYPE 16
//#define MUOS_CLOCK_TYPE 32
#define MUOS_CLOCK_TYPE 64
#endif

// The type (bitsize) used for shorter unsigned timespans.
#ifndef MUOS_CLOCK_SHORT_TYPE
//#define MUOS_CLOCK_SHORT_TYPE 8
#define MUOS_CLOCK_SHORT_TYPE 16
//#define MUOS_CLOCK_SHORT_TYPE 32
//#define MUOS_CLOCK_SHORT_TYPE 64
#endif

// enable the muos_now() api, needs additional sizeof(muos_clock) space of ram
#ifndef MUOS_NOW
//#define MUOS_NOW 0
#define MUOS_NOW 0
#endif




//SCHED:
// How long is the shortest time to schedule
// when scheduling any shorter timespan the hardware timer may be already past
// this span and thus won't trigger an interrupt, missing a whole cycle.
// When timer accuracy is not required, biggier values can also be used to batch
// scheduled events together.
// Define this as small as possible, the default might be way to big.
//PLANNED: determine this value automatic
#ifndef MUOS_SCHED_MINTIME
//#define MUOS_SCHED_MINTIME 127
#define MUOS_SCHED_MINTIME 1
#endif



// Strategy the scheduler chooses when then next event is in less than
// MUOS_SCHED_MINTIME
//
// SLOPPY::
//      Extend the time to MUOS_SCHED_MINTIME, scheduling the event a bit later.
//
// SPINWAIT::
//      Go into a busy loop, waiting for the exact time.
//      Works only well when MUOS_SCHED_MINTIME is properly adjusted.
//
// SLOPPYSPIN::
//      Go into a busy loop with interrupts enabled, waiting for the exact time or later (if interrupted).
//      Works only well when MUOS_SCHED_MINTIME is properly adjusted.
//
// EARLY::
//      Call the event *now*, thats a bit too early. Saves a bit codespace.
//
// BATCHED::
//      Only schedule events at the timer overflow. This completely eleminates the compare-match interrupt.
//      Saves a lot codespace but most events are considerably late, esp. for 16 bit timers.
//      Works only when the MUOS_TIMER_SHORT_TYPE is biggier than the hardware timer size.
//      MUOS_SCHED_MINTIME is completely ignored this way.
//
#ifndef MUOS_SCHED_STRATEGY
#define MUOS_SCHED_STRATEGY SLOPPY
//#define MUOS_SCHED_STRATEGY SPINWAIT
//#define MUOS_SCHED_STRATEGY SLOPPYSPIN
//#define MUOS_SCHED_STRATEGY EARLY
//#define MUOS_SCHED_STRATEGY BATCHED
#endif




#endif
