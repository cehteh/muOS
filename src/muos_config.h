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



// How functions with arguments are tagged in queues, currently only the negate is implemened
// Works with mpu's with up to 64k flash
#ifndef MUOS_QUEUE_ARGTAG
#define MUOS_QUEUE_ARGTAG NEGATE
#endif



// The type (bitsize) used for the overflow counter of the timer.
// together with the harware register of the timer this gives the full time.
#ifndef MUOS_TIMER_TYPE
//#define MUOS_TIMER_TYPE 16
//#define MUOS_TIMER_TYPE 32
#define MUOS_TIMER_TYPE 64
#endif

// The type (bitsize) used for shorter unsigned timespans.
#ifndef MUOS_TIMER_SHORT_TYPE
//#define MUOS_TIMER_SHORT_TYPE 8
#define MUOS_TIMER_SHORT_TYPE 16
//#define MUOS_TIMER_SHORT_TYPE 32
//#define MUOS_TIMER_SHORT_TYPE 64
#endif

// How many entriex the realtime Queue can hold, set to 0 to disable the rtq
#ifndef MUOS_RTQ_LENGTH
#define MUOS_RTQ_LENGTH 32
#endif

// How many entriex the background Queue can hold, set to 0 to disable the bgq
#ifndef MUOS_BGQ_LENGTH
#define MUOS_BGQ_LENGTH 32
#endif


// type used for the 'priorities' if the priority queue
// The priority queue uses a 'sliding window'.
// Scheduler needs to be called at least half of the range provided by this type.
#ifndef MUOS_PRIQ_TYPE
#define MUOS_PRIQ_TYPE MUOS_TIMER_SHORT_TYPE
//#define MUOS_PRIQ_TYPE 8
//#define MUOS_PRIQ_TYPE 16
//#define MUOS_PRIQ_TYPE 32
#endif



// How long is the shortest time to schedule
// when scheduling any shorter timespan the hardware timer may be already past
// this span and thus won't trigger an interrupt, missing a whole cycle.
// When timer accuracy is not required, biggier values can also be used to batch
// scheduled events together.
// Define this as small as possible. PLANNED: determine this value automatic
#ifndef MUOS_SCHED_MINTIME
#define MUOS_SCHED_MINTIME 127
#endif


#endif
