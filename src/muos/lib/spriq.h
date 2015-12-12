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
  volatile muos_spriq_index used;
  volatile struct muos_spriq_entry spriq[];
};


static inline void
muos_spriq_init (struct muos_spriq* spriq)
{
  spriq->used = 0;
}


void
muos_spriq_push (struct muos_spriq* spriq, muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function fn);


void
muos_spriq_pop (struct muos_spriq* spriq, struct muos_spriq_entry* event);



#endif
