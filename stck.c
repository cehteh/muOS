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
#ifdef MUOS_STCK

#include <muos/stck.h>

void
muos_stck_init (void)
{
  for (char* mem = muos_stck_brk(); mem < (char*)SP; ++mem)
    *mem = MUOS_STCK_CANARY(mem);
}


size_t
muos_stck_minfree (void)
{
  //PLANNED: implement binary search

  char* mem;
  for (mem = muos_stck_brk() + MUOS_STCK_CONS;
       mem < (char*)SP;
       mem += MUOS_STCK_CONS)
    {
      if (*mem != MUOS_STCK_CANARY(mem))
        break;
    }

  return mem - muos_stck_brk();
}


#endif
