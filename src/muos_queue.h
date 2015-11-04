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

#ifndef MUOS_QUEUE_H
#define MUOS_QUEUE_H

#include <muos.h>

#include <stdint.h>
#include <stdbool.h>

typedef void (*muos_queue_function)(void);
typedef void (*muos_queue_function_arg)(intptr_t);


struct muos_queue4
{
  volatile uint8_t      start:4;
  volatile uint8_t      len:4;
  volatile intptr_t     queue[];
};

static inline void
muos_queue4_init (struct muos_queue4* queue)
{
  queue->start = 0;
  queue->len = 0;
}

bool
muos_queue4_schedule (struct muos_queue4* queue, uint8_t size);

void
muos_queue4_pushback (struct muos_queue4* queue, uint8_t size, muos_queue_function fn);

void
muos_queue4_pushback_arg (struct muos_queue4* queue, uint8_t size, muos_queue_function_arg fn, intptr_t arg);

void
muos_queue4_pushfront (struct muos_queue4* queue, uint8_t size, muos_queue_function fn);

void
muos_queue4_pushfront_arg (struct muos_queue4* queue, uint8_t size, muos_queue_function_arg fn, intptr_t arg);


struct muos_queue8
{
  volatile uint8_t      start;
  volatile uint8_t      len;
  volatile intptr_t     queue[];
};


static inline void
muos_queue8_init (struct muos_queue8* queue)
{
  queue->start = 0;
  queue->len = 0;
}

bool
muos_queue8_schedule (struct muos_queue8* queue, uint8_t size);

void
muos_queue8_pushback (struct muos_queue8* queue, uint8_t size, muos_queue_function fn);

void
muos_queue8_pushback_arg (struct muos_queue8* queue, uint8_t size, muos_queue_function_arg fn, intptr_t arg);

void
muos_queue8_pushfront (struct muos_queue8* queue, uint8_t size, muos_queue_function fn);

void
muos_queue8_pushfront_arg (struct muos_queue8* queue, uint8_t size, muos_queue_function_arg fn, intptr_t arg);


struct muos_queue16
{
  volatile uint16_t     start;
  volatile uint16_t     len;
  volatile intptr_t     queue[];
};


static inline void
muos_queue16_init (struct muos_queue16* queue)
{
  queue->start = 0;
  queue->len = 0;
}

bool
muos_queue16_schedule (struct muos_queue16* queue, uint16_t size);

void
muos_queue16_pushback (struct muos_queue16* queue, uint16_t size, muos_queue_function fn);

void
muos_queue16_pushback_arg (struct muos_queue16* queue, uint16_t size, muos_queue_function_arg fn, intptr_t arg);

void
muos_queue16_pushfront (struct muos_queue16* queue, uint16_t size, muos_queue_function fn);

void
muos_queue16_pushfront_arg (struct muos_queue16* queue, uint16_t size, muos_queue_function_arg fn, intptr_t arg);

#define MUOS_QUEUE4DEF(size)                    \
struct                                          \
{                                               \
  struct muos_queue4  descriptor;               \
  intptr_t      queue[size];                    \
}


#define MUOS_QUEUE8DEF(size)                    \
struct                                          \
{                                               \
  struct muos_queue8 descriptor;                \
  intptr_t      queue[size];                    \
}


#define MUOS_QUEUE16DEF(size)                   \
struct                                          \
{                                               \
  struct muos_queue16 descriptor;               \
  intptr_t      queue[size];                    \
}


#endif
