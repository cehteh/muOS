/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2015, 2019                      Christian Thäter <ct@pipapo.org>
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

// autoconfigure MUOS_QUEUE_INDEX
#ifndef MUOS_QUEUE_INDEX

#ifdef MUOS_RTQ_LENGTH
# define MUOS_QUEUE_MAX_LENGTH MUOS_RTQ_LENGTH
#else
# define MUOS_QUEUE_MAX_LENGTH 0
#endif

#ifdef MUOS_HPQ_LENGTH
# if MUOS_HPQ_LENGTH > MUOS_QUEUE_MAX_LENGTH
#  undef MUOS_QUEUE_MAX_LENGTH
#  define MUOS_QUEUE_MAX_LENGTH MUOS_HPQ_LENGTH
# endif
#endif

#ifdef MUOS_BGQ_LENGTH
# if MUOS_BGQ_LENGTH > MUOS_QUEUE_MAX_LENGTH
#  undef MUOS_QUEUE_MAX_LENGTH
#  define MUOS_QUEUE_MAX_LENGTH MUOS_BGQ_LENGTH
# endif
#endif

//TODO: rename MUOS_QUEUE_INDEX to MUOS_QUEUE_WIDTH

#if MUOS_QUEUE_MAX_LENGTH < 16
# define MUOS_QUEUE_INDEX 4
#elif MUOS_QUEUE_MAX_LENGTH < 256
# define MUOS_QUEUE_INDEX 8
#elif MUOS_QUEUE_MAX_LENGTH < 65536
# define MUOS_QUEUE_INDEX 16
#else
# error queue too big
#endif

#endif

#include <stdint.h>
#include <stdbool.h>

typedef void (*muos_queue_function)(void);


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



//lib_queue_api:
//: .Queue Initialization
//: ----
//: void muos_queue_init (struct muos_queue* queue)
//: ----
//:
//: +queue+::
//:   pointer to the queue
//:
//: Initialization is not necessary at startup, it is only required
//: for to reinitialize and delete an existing queue.
//:
static inline void
muos_queue_init (struct muos_queue* queue)
{
  queue->start = 0;
  queue->len = 0;
}

// internal
bool
muos_queue_schedule (struct muos_queue* queue, const muos_queue_size size);

//lib_queue_api:
//: .Queue operations
//: ----
//: void muos_queue_push (struct muos_queue* queue,
//:                       const muos_queue_size size,
//:                       muos_queue_function func)
//:
//: void muos_queue_push_arg (struct muos_queue* queue,
//:                           const muos_queue_size size,
//:                           muos_queue_function func,
//:                           intptr_t arg)
//:
//: intptr_t muos_queue_pop (struct muos_queue* queue,
//:                          const muos_queue_size size)
//: ----
//:
//:  +queue+::
//:    the queue
//:  +size+::
//:    size of the queue
//:  +func+::
//:    function pointer
//:  +arg+::
//:    inptr_t argument
//:
//: --
//: * +muos_queue_push ()+ pushes 'func' onto the back of the queue
//: * +muos_queue_push_arg ()+ pushes 'func' with 'arg' onto the back of the queue
//: * +muos_queue_pop ()+ removes the first argument from the queue
//: --
//:
void
muos_queue_push (struct muos_queue* queue, const muos_queue_size size, muos_queue_function fn);

void
muos_queue_push_arg (struct muos_queue* queue, const muos_queue_size size, muos_queue_function fn, intptr_t arg);

intptr_t
muos_queue_pop (struct muos_queue* queue, const muos_queue_size size);


//: .Queue information
//: ----
//: muos_queue_size muos_queue_free (struct muos_queue* queue, const muos_queue_size size)
//: ----
//:
//:  +queue+::
//:    the queue
//:  +size+::
//:    size of the queue
//:
//: Returns the number of free elements in the queue.
//:
static inline muos_queue_size
muos_queue_free (struct muos_queue* queue, const muos_queue_size size)
{
  return size - queue->len;
}


#endif
