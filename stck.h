/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2019                            Christian Thäter <ct@pipapo.org>
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
#ifndef MUOS_STCK_H
#define MUOS_STCK_H

#ifdef MUOS_STCK

#include <muos/muos.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//FIXME: format docs

void
muos_stck_init (void);


static inline char*
muos_stck_brk (void)
{
  extern char* __brkval;
  extern char __heap_start;

  return __brkval?__brkval:&__heap_start;
}



//stck_api:
//: bool muos_stck_ok (char* addr)::
//: checks that addr (and preeceding MUOS_STCK_CONS) bytes are unmodified.
static inline bool
muos_stck_ok (char* addr)
{
  for (char* mem = addr - MUOS_STCK_CONS; mem < addr; ++mem)
    {
      if (*mem != MUOS_STCK_CANARY(mem))
        return false;
    }

  return true;
}


//stck_api:
//: check that reserved space *was* available at any time on the stack
static inline bool
muos_stck_check (size_t reserved)
{
  return muos_stck_ok (muos_stck_brk() + reserved);
}



//stck_api:
//: size_t muos_stck_size (void)
//: returns the stack size (may change with allocations)
static inline size_t
muos_stck_size (void)
{
  return RAMEND - (size_t) muos_stck_brk ();
}


//stck_api:
//: size_t muos_stck_avail (void)
//: returns the stackspace currently available
//:
static inline size_t
muos_stck_avail (void)
{
  return (char*)SP - muos_stck_brk();
}

//stck_api:
//: size_t muos_stck_used (void)
//: returns the stackspace currently used
//:
static inline size_t
muos_stck_used (void)
{
  return RAMEND - SP;
}


//stck_api:
//: size_t muos_stck_minfree (void)
//: returns the minimum space that was available on the stack
//: results may be inaccurate up to MUOS_STCK_CONS bytes.
//:
size_t
muos_stck_minfree (void);



//stck_api:
//: size_t muos_stck_maxused (void)
//: returns the maximum space that was used on the stack
//: results may be inaccurate up to MUOS_STCK_CONS bytes.
//:
static inline size_t
muos_stck_maxused (void)
{
  return muos_stck_size () - muos_stck_minfree ();
}

#endif // MUOS_STCK
#endif /* MUOS_STCK_H */
