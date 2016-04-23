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

#ifndef MUOS_LIB_QUEUE_H
#define MUOS_LIB_QUEUE_H

#include <muos/muos.h>

#include <stdint.h>
#include <stdbool.h>

typedef void (*muos_queue_function)(void);
typedef void (*muos_queue_function_arg)(intptr_t);


#if MUOS_QUEUE_INDEX == 4

typedef uint8_t muos_queue_size;

struct muos_queue
{
  uint8_t      start:4;
  uint8_t      len:4;
  intptr_t     queue[];
};

#elif MUOS_QUEUE_INDEX == 8

typedef uint8_t muos_queue_size;

struct muos_queue
{
  uint8_t      start;
  uint8_t      len;
  intptr_t     queue[];
};

#elif MUOS_QUEUE_INDEX == 16

typedef uint16_t muos_queue_size;

struct muos_queue
{
  uint16_t     start;
  uint16_t     len;
  intptr_t     queue[];
};


#else
#error "illegal MUOS_QUEUE_INDEX"
#endif


//lib_queue_api:
//: .Queue definition
//: ----
//: MUOS_QUEUEDEF(size)
//: ----
//:
//: +size+::
//:   number of elements
//:
//: Macro defining the type of a queue for the given size. To instantiate a queue
//: use +MUOS_QUEUEDEF(16) myqueue;+ for example.
//:
#define MUOS_QUEUEDEF(size)                     \
struct                                          \
{                                               \
  struct muos_queue descriptor;                 \
  intptr_t      queue[size];                    \
}



static inline void
muos_queue_init (struct muos_queue* queue)
{
  queue->start = 0;
  queue->len = 0;
}

bool
muos_queue_schedule (struct muos_queue* queue, const muos_queue_size size);

void
muos_queue_pushback (struct muos_queue* queue, const muos_queue_size size, muos_queue_function fn);

void
muos_queue_pushback_arg (struct muos_queue* queue, const muos_queue_size size, muos_queue_function_arg fn, intptr_t arg);

void
muos_queue_pushfront (struct muos_queue* queue, const muos_queue_size size, muos_queue_function fn);

void
muos_queue_pushfront_arg (struct muos_queue* queue, const muos_queue_size size, muos_queue_function_arg fn, intptr_t arg);



//PLANNED: stowing more than one arg

//lib_queue_api:
//: .Queue API Macros
//: ----
//: MUOS_QUEUE_SIZE(queue)
//: MUOS_QUEUE_FREE(queue)
//: MUOS_QUEUE_PUSHBACK(queue, func)
//: MUOS_QUEUE_PUSHBACK_ARG(queue, func, arg)
//: MUOS_QUEUE_PUSHFRONT(queue, func)
//: MUOS_QUEUE_PUSHFRONT_ARG(queue, func, arg)
//: ----
//:
//:  +queue+::
//:    the queue as defined with +MUOS_QUEUEDEF()+
//:  +func+::
//:    function pointer
//:  +arg+::
//:    inptr_t argument
//:
//: --
//: * +MUOS_QUEUE_SIZE(queue)+ returns the number of elements with what the queue was defined
//: * +MUOS_QUEUE_FREE(queue)+ returns the number of elements which are free in the queue
//: * +MUOS_QUEUE_PUSHBACK(queue, func)+ pushes 'func' onto the back of the queue
//: * +MUOS_QUEUE_PUSHBACK_ARG(queue, func, arg)+ pushes 'func' and 'arg' onto the back of the queue
//: * +MUOS_QUEUE_PUSHFRONT(queue, func)+ pushes 'func' onto front of the queue
//: * +MUOS_QUEUE_PUSHFRONT_ARG(queue, func, arg)+ pushes 'func' and 'arg' onto the front of the queue
//: --
//:
//: NOTE: there are no 'pop' functions yet, only a internal 'schedule' function. Functions for poping
//:       elements will be added on demand.
//:
#define MUOS_QUEUE_SIZE(q)  MUOS_ARRAY_ELEMENTS((q).queue)
#define MUOS_QUEUE_FREE(q) MUOS_QUEUE_SIZE(q)-(q).descriptor.len
#define MUOS_QUEUE_SCHEDULE(q) muos_queue_schedule (&(q).descriptor, MUOS_QUEUE_SIZE(q))
#define MUOS_QUEUE_PUSHBACK(q, f) muos_queue_pushback (&(q).descriptor, MUOS_QUEUE_SIZE(q), (f))
#define MUOS_QUEUE_PUSHBACK_ARG(q, f, a) muos_queue_pushback_arg (&(q).descriptor, MUOS_QUEUE_SIZE(q), (f), (a))
#define MUOS_QUEUE_PUSHFRONT(q, f) muos_queue_pushfront (&(q).descriptor, MUOS_QUEUE_SIZE(q), (f))
#define MUOS_QUEUE_PUSHFRONT_ARG(q, f, a) muos_queue_pushfront_arg (&(q).descriptor, MUOS_QUEUE_SIZE(q), (f), (a))


#endif
