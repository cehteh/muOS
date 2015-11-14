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

#ifndef MUOS_PRIQ_H
#define MUOS_PRIQ_H

#include <muos.h>
#include "muos_config.h"

#define MUOS_PRIQ_SIZE(pq) MUOS_ARRAY_ELEMENTS((pq).priq)

MUOS_TYPEDEF(uint, MUOS_PRIQ_TYPE, muos_priq_priority);

typedef void (*muos_priq_function)(void);

//PLANNED: also 8 bit variant
typedef uint16_t muos_priq_size;

struct muos_priq_entry
{
  muos_priq_priority when;
  muos_priq_function fn;
};

struct muos_priq
{
  volatile muos_priq_size used;
  volatile struct muos_priq_entry priq[];
};



#define MUOS_PRIQDEF(size)                      \
struct                                          \
{                                               \
  struct muos_priq  descriptor;                 \
  struct muos_priq_entry priq[size];            \
}


static inline void
muos_priq_init (struct muos_priq* priq)
{
  priq->used = 0;
}


void
muos_priq_push (struct muos_priq* priq, muos_priq_priority base, muos_priq_priority when, muos_priq_function fn);


void
muos_priq_pop (struct muos_priq* priq);

#endif
