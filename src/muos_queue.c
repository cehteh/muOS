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

#include "muos_queue.h"

// queue4

static inline intptr_t
muos_queue4_pop (struct muos_queue4* queue, uint8_t size)
{
  intptr_t ret = queue->queue[queue->start];
  queue->start += 1;
  queue->len -= 1;
  if (queue->start >= size)
    queue->start -= size;

  return ret;
}


bool
muos_queue4_schedule (struct muos_queue4* queue, uint8_t size)
{
  if (queue->len)
    {
      intptr_t fn = muos_queue4_pop (queue, size);

      if (fn<0)
        {
          intptr_t arg = muos_queue4_pop (queue, size);
          ((muos_queue_function_arg)(-fn))(arg);
        }
      else
        {
          ((muos_queue_function)(fn))();
        }
      return true;
    }
  return false;
}


static void
muos_queue4_prep (struct muos_queue4* queue, uint8_t size, uint8_t len)
{
  while (size - queue->len < len)
    {
      MUOS_OVERFLOW;
      muos_queue4_schedule (queue, size);
    }
}


static void
muos_queue4_pushback_intern (struct muos_queue4* queue, uint8_t size, intptr_t value)
{
  uint8_t index = queue->start+queue->len;
  if (index >= size)
    index -= size;
  queue->queue[index] = value;
  queue->len += 1;
}


void
muos_queue4_pushback (struct muos_queue4* queue, uint8_t size, muos_queue_function fn)
{
  muos_queue4_prep (queue, size, 1);
  muos_queue4_pushback_intern (queue, size, (intptr_t) fn);
}


void
muos_queue4_pushback_arg (struct muos_queue4* queue, uint8_t size, muos_queue_function_arg fn, intptr_t arg)
{
  muos_queue4_prep (queue, size, 2);
  muos_queue4_pushback_intern (queue, size, -(intptr_t) fn);
  muos_queue4_pushback_intern (queue, size, arg);
}



static void
muos_queue4_pushfront_intern (struct muos_queue4* queue, uint8_t size, intptr_t value)
{
  queue->start -= 1;
  queue->len += 1;
  if (queue->start >= size)
    queue->start += size;
  queue->queue[queue->start] = value;
}


void
muos_queue4_pushfront (struct muos_queue4* queue, uint8_t size, muos_queue_function fn)
{
  muos_queue4_prep (queue, size ,1);
  muos_queue4_pushfront_intern (queue, size, (intptr_t) fn);
}


void
muos_queue4_pushfront_arg (struct muos_queue4* queue, uint8_t size, muos_queue_function_arg fn, intptr_t arg)
{
  muos_queue4_prep (queue, size, 2);
  muos_queue4_pushfront_intern (queue, size, arg);
  muos_queue4_pushfront_intern (queue, size, -(intptr_t) fn);
}


// queue8

static inline intptr_t
muos_queue8_pop (struct muos_queue8* queue, uint8_t size)
{
  intptr_t ret = queue->queue[queue->start];
  queue->start += 1;
  queue->len -= 1;
  if (queue->start >= size)
    queue->start -= size;

  return ret;
}


bool
muos_queue8_schedule (struct muos_queue8* queue, uint8_t size)
{
  if (queue->len)
    {
      intptr_t fn = muos_queue8_pop (queue, size);

      if (fn<0)
        {
          intptr_t arg = muos_queue8_pop (queue, size);
          ((muos_queue_function_arg)(-fn))(arg);
        }
      else
        {
          ((muos_queue_function)(fn))();
        }
      return true;
    }
  return false;
}


static void
muos_queue8_prep (struct muos_queue8* queue, uint8_t size, uint8_t len)
{
  while (size - queue->len < len)
    {
      MUOS_OVERFLOW;
      muos_queue8_schedule (queue, size);
    }
}


static void
muos_queue8_pushback_intern (struct muos_queue8* queue, uint8_t size, intptr_t value)
{
  uint8_t index = queue->start+queue->len;
  if (index >= size)
    index -= size;
  queue->queue[index] = value;
  queue->len += 1;
}


void
muos_queue8_pushback (struct muos_queue8* queue, uint8_t size, muos_queue_function fn)
{
  muos_queue8_prep (queue, size, 1);
  muos_queue8_pushback_intern (queue, size, (intptr_t) fn);
}


void
muos_queue8_pushback_arg (struct muos_queue8* queue, uint8_t size, muos_queue_function_arg fn, intptr_t arg)
{
  muos_queue8_prep (queue, size, 2);
  muos_queue8_pushback_intern (queue, size, -(intptr_t) fn);
  muos_queue8_pushback_intern (queue, size, arg);
}



static void
muos_queue8_pushfront_intern (struct muos_queue8* queue, uint8_t size, intptr_t value)
{
  queue->start -= 1;
  queue->len += 1;
  if (queue->start >= size)
    queue->start += size;
  queue->queue[queue->start] = value;
}


void
muos_queue8_pushfront (struct muos_queue8* queue, uint8_t size, muos_queue_function fn)
{
  muos_queue8_prep (queue, size ,1);
  muos_queue8_pushfront_intern (queue, size, (intptr_t) fn);
}


void
muos_queue8_pushfront_arg (struct muos_queue8* queue, uint8_t size, muos_queue_function_arg fn, intptr_t arg)
{
  muos_queue8_prep (queue, size, 2);
  muos_queue8_pushfront_intern (queue, size, arg);
  muos_queue8_pushfront_intern (queue, size, -(intptr_t) fn);
}





// q16

static inline intptr_t
muos_queue16_pop (struct muos_queue16* queue, uint16_t size)
{
  intptr_t ret = queue->queue[queue->start];
  queue->start += 1;
  queue->len -= 1;
  if (queue->start >= size)
    queue->start -= size;

  return ret;
}


bool
muos_queue16_schedule (struct muos_queue16* queue, uint16_t size)
{
  if (queue->len)
    {
      intptr_t fn = muos_queue16_pop (queue, size);

      if (fn<0)
        {
          intptr_t arg = muos_queue16_pop (queue, size);
          ((muos_queue_function_arg)(-fn))(arg);
        }
      else
        {
          ((muos_queue_function)(fn))();
        }
      return true;
    }
  return false;
}


static void
muos_queue16_prep (struct muos_queue16* queue, uint16_t size, uint8_t len)
{
  while (size - queue->len < len)
    {
      MUOS_OVERFLOW;
      muos_queue16_schedule (queue, size);
    }
}


static void
muos_queue16_pushback_intern (struct muos_queue16* queue, uint16_t size, intptr_t value)
{
  uint8_t index = queue->start+queue->len;
  if (index >= size)
    index -= size;
  queue->queue[index] = value;
  queue->len += 1;
}


void
muos_queue16_pushback (struct muos_queue16* queue, uint16_t size, muos_queue_function fn)
{
  muos_queue16_prep (queue, size, 1);
  muos_queue16_pushback_intern (queue, size, (intptr_t) fn);
}


void
muos_queue16_pushback_arg (struct muos_queue16* queue, uint16_t size, muos_queue_function_arg fn, intptr_t arg)
{
  muos_queue16_prep (queue, size, 2);
  muos_queue16_pushback_intern (queue, size, -(intptr_t) fn);
  muos_queue16_pushback_intern (queue, size, arg);
}



static void
muos_queue16_pushfront_intern (struct muos_queue16* queue, uint16_t size, intptr_t value)
{
  queue->start -= 1;
  queue->len += 1;
  if (queue->start >= size)
    queue->start += size;
  queue->queue[queue->start] = value;
}


void
muos_queue16_pushfront (struct muos_queue16* queue, uint16_t size, muos_queue_function fn)
{
  muos_queue16_prep (queue, size ,1);
  muos_queue16_pushfront_intern (queue, size, (intptr_t) fn);
}


void
muos_queue16_pushfront_arg (struct muos_queue16* queue, uint16_t size, muos_queue_function_arg fn, intptr_t arg)
{
  muos_queue16_prep (queue, size, 2);
  muos_queue16_pushfront_intern (queue, size, arg);
  muos_queue16_pushfront_intern (queue, size, -(intptr_t) fn);
}







