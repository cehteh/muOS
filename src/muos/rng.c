/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2016                            Christian Thäter <ct@pipapo.org>
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


#include <stdint.h>

uint8_t quality;
uint8_t pos;
uint8_t len;
uint8_t rnd[8];


void
muos_rng_push (uint8_t value, uint8_t q)
{
  uint8_t index = pos+len;

  if (quality < 127)
    {
      quality += q*q;
    }
  else
    {
      if (len < sizeof(rnd)-1)
        {
          ++index;
          ++len;
          quality = 0;
        }
      else
        {
          index = pos;
        }
    }

  if (index >= sizeof(rnd))
    index -= sizeof(rnd);

  rnd[index] = rnd[index]<<3 ^ rnd[index]>>5 ^ value;
}

uint8_t
muos_rng_available (void)
{
  return len;
}

uint8_t
muos_rng_pop (void)
{
  uint8_t ret = 0;

  if (len)
    {
      ret = rnd[pos];

      if (++pos >= sizeof(rnd))
        pos -= sizeof(rnd);

      --len;
    }

  return ret;
}




#ifdef TEST

// test with:
//  gcc -std=gnu99 -DTEST rng.c && ./a.out | ent

#include <stdio.h>
#include <stdlib.h>


int
main()
{
  FILE* r = fopen("/dev/dsp", "r");

  unsigned long long i = 0;

  do
    {
      muos_rng_push (fgetc(r), 16);

      if (muos_rng_available ())
        {
          ++i;
          putchar (muos_rng_pop ());
        }

    }
  while (i<100000);

  return 0;
}


#endif
