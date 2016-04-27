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

#ifndef MUOS_LIB_SPRIQ_H
#define MUOS_LIB_SPRIQ_H

#include <stdbool.h>

#include <muos/muos.h>

struct muos_spriq_entry;

//lib_spriq_api:
//: .Types
//: ----
//: struct muos_spriq_entry
//: {
//:   muos_spriq_priority when;
//:   muos_spriq_function fn;
//: }
//:
//: typedef void (*muos_spriq_function)(const struct muos_spriq_entry* event)
//: ----
//:
//: The type for functions stored in a spriq.
//: This function is called with the time it was to be scheduled.
//: The scheduler passes the spriq entry to the function, this makes it simple
//: to schedule repeating jobs, where this 'event->when' just becomes the new
//: 'base' for next push.
//:
typedef void (*muos_spriq_function)(const struct muos_spriq_entry*);


#define MUOS_SPRIQ_SIZE(spq) MUOS_ARRAY_ELEMENTS((spq).spriq)

typedef MUOS_SPRIQ_TYPE muos_spriq_priority;
typedef MUOS_SPRIQ_INDEX muos_spriq_index;


struct muos_spriq_entry
{
  muos_spriq_priority when;
  muos_spriq_function fn;
};

struct muos_spriq
{
  muos_spriq_index used;
  struct muos_spriq_entry spriq[];
};

//lib_spriq_api:
//: .Initialize a spriq
//: ----
//: void muos_spriq_init (struct muos_spriq* spriq)
//: ----
//:
//: +spriq+::
//:   pointer to the spriq
//:
//: Initialization is not necessary at startup, it is only required
//: for to reinitialize and delete an existing queue.
//:
static inline void
muos_spriq_init (struct muos_spriq* spriq)
{
  spriq->used = 0;
}


//lib_spriq_api:
//: .Push a function
//: ----
//: void muos_spriq_push (
//:        struct muos_spriq* spriq,
//:        muos_spriq_priority base,
//:        muos_spriq_priority when,
//:        muos_spriq_function fn)
//: ----
//:
//: +spriq+::
//:   Pointer to the spriq
//: +base+::
//:   Base priority
//: +when+::
//:   Offset to base for the priority
//: +fn+::
//:   Function to push
//:
//: 'base' must be smaller or equal to the smallest (first) element in the queue.
//: For times this is usually 'muos_now()' or the time the event was scheduled.
//: 'when' can cover the full range of the priority data type.
//:
void
muos_spriq_push (struct muos_spriq* spriq, muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function fn);



//lib_spriq_api:
//: .Pop element
//: ----
//: void muos_spriq_pop (struct muos_spriq* spriq)
//: ----
//:
//: +spriq+::
//:   Spriq where to pop from
//:
//: No return, no error checking!
//:
void
muos_spriq_pop (struct muos_spriq* spriq);



#endif
