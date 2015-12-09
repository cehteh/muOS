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

//configuration:
//: muos is configure by '#defines' in the muos_config.h file
#ifndef MUOS_CONFIG_H
#define MUOS_CONFIG_H



//:MUOS_INITFN
//: Name of the user-defined initialization function.
//: This function is pushed on the rtq (or, if not available the bgq) when starting up. It is
//: responsible for initializing the system. Interrupts are still diabled and the clock is
//: stopped and will be started after this init function returns.
#define MUOS_INITFN init



//:MUOS_EXPLICIT_INIT
//: when set to 1, then all static initialization of muos internal datastructures is disabled
//: initialization will then be done explicitly in init sections or in main before any of these
//: facilities are used
#define MUOS_EXPLICIT_INIT 0
//#define MUOS_EXPLICIT_INIT 1 //PLANNED: not implemented yet



//:MUOS_QUEUE_INDEX
//: the bits used for indexing queues
//: comes in 3 variants
//:  4 bits allow only small queues for up to (8-16) functions, use only for really small targets
//:  8 bits allow queues for up to 256 functions
//:  16 bits allow huge queues, use only when really required
//#define MUOS_QUEUE_INDEX 4
#define MUOS_QUEUE_INDEX 8
//#define MUOS_QUEUE_INDEX 16



//:MUOS_RTQ_LENGTH
//: How many entries the realtime queue can hold, set to 0 to disable the rtq
#define MUOS_RTQ_LENGTH 32



//:MUOS_BGQ_LENGTH
//: How many entries the background queue can hold, set to 0 to disable the bgq
#define MUOS_BGQ_LENGTH 32



//PLANNED: MUOS_QUEUE_ARGTAG
// : How functions with arguments are tagged in queues, currently only the 'negate is implemened
// : Works with mpu's with up to 64k flash
//#define MUOS_QUEUE_ARGTAG NEGATE
//#endif



//:MUOS_SPRIQ_TYPE
//: Type used for the 'priorities' of the small priority queue.
//: This priority queue uses a 'sliding window'. The rtpq uses this.
//: Scheduler needs to be called at least half of the range provided by this type.
#define MUOS_SPRIQ_TYPE MUOS_CLOCK_SHORT_TYPE
//#define MUOS_SPRIQ_TYPE uint8_t
//#define MUOS_SPRIQ_TYPE uint16_t
//#define MUOS_SPRIQ_TYPE uint32_t



//:MUOS_SPRIQ_INDEX
//: Type to keep track of the size of the spriq.
//: uint8_t for up to 255 entries, uint16_t for up to 65k entries.
//: choosing uint8_t saves only one byte ram, usually not worth it
//#define MUOS_SPRIQ_INDEX uint8_t
#define MUOS_SPRIQ_INDEX uint16_t



//:MUOS_LPRIQ_TYPE
//: type used for the 'priorities' if the priority queue
#define MUOS_LPRIQ_TYPE MUOS_CLOCK_TYPE
//#define MUOS_LPRIQ_TYPE uint8_t
//#define MUOS_LPRIQ_TYPE uint16_t
//#define MUOS_LPRIQ_TYPE uint32_t



//:MUOS_LPRIQ_INDEX
//: Type to keep track of the size of the lpriq.
//#define MUOS_SPRIQ_INDEX uint8_t
#define MUOS_LPRIQ_INDEX uint16_t


//:MUOS_RTPQ_LENGTH
//: How many entriex the scheduling Queue can hold, set to 0 to disable the rtpq
#define MUOS_RTPQ_LENGTH 32



//:MUOS_CLOCK_HW
//: Which Hardware timer to use.
//: This is a hardware dependent config. usually a number or a simple name
//: timer hardware setup is tightly bound to the hardware capabilities and only barely
//: abstracted, check for the respective hardware implementation about possible settings.
#define MUOS_CLOCK_HW 0



//:MUOS_CLOCK_HW_PRESCALER
//: Prescaler from some hardware defined master clock
#define MUOS_CLOCK_PRESCALER 1024


//:MUOS_CLOCK_HW_COMPAREMATCH
//: The Compare/Match unit of the Timer to use
//: This is a hardware dependent config. usually a number or a simple name
#define MUOS_CLOCK_HW_COMPAREMATCH A



//:MUOS_CLOCK_TYPE
//: MµOS clock configuration above the hardware
//: The type used for the overflow counter of the clock.
//: together with the harware register of the clock this gives the full time.
//#define MUOS_CLOCK_TYPE uint16_t
//#define MUOS_CLOCK_TYPE uint32_t
#define MUOS_CLOCK_TYPE uint64_t



//:MUOS_CLOCK_SHORT_TYPE
//: The type used for shorter timespans.
//#define MUOS_CLOCK_SHORT_TYPE uint8_t
#define MUOS_CLOCK_SHORT_TYPE uint16_t
//#define MUOS_CLOCK_SHORT_TYPE uint32_t
//#define MUOS_CLOCK_SHORT_TYPE uint64_t



//:MUOS_NOW
//: enable the muos_now() api, needs additional sizeof(muos_clock) space of ram
//#define MUOS_NOW 0
#define MUOS_NOW 1



//:MUOS_SCHED_MINTIME
//: How long is the shortest time to schedule
//: when scheduling any shorter timespan the hardware timer may be already past
//: this span and thus won't trigger an interrupt, missing a whole cycle.
//: When timer accuracy is not required, biggier values can also be used to batch
//: scheduled events together.
//: Define this as small as possible, the default might be way to big.
//PLANNED: determine this value automatic
#define MUOS_SCHED_MINTIME 127



#endif
