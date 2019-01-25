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

#include <muos/lib/spriq.h>

/*

Heap data structure zero indexed based

          0
    1           2
 3     4     5     6
7 8   9 ...

keys are wraparound (when-base) thus giving a sliding window with the
smallest key being base.

*/

// parent calculates the parent from a given index,
// 0 has no parent and calling it would be an error
#define parent(index) (((index)+1)/2-1)
#define lchild(index) (((index)+1)*2-1)

//FIXME: cehck that schedule with = 1 works
void
muos_spriq_push (struct muos_spriq* spriq, muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function fn)
{
  muos_spriq_index i = spriq->used;

  if (i && when-base < (spriq->spriq[parent(i)].when - base))
    while (i && when-base < (spriq->spriq[parent(i)].when - base))
      {
        spriq->spriq[i] = spriq->spriq[parent(i)];
        i /= 2;
      }

  spriq->spriq[i] = (struct muos_spriq_entry){when + base, fn};
  ++spriq->used;
}


void
muos_spriq_pop (struct muos_spriq* spriq)
{
  muos_spriq_priority base = spriq->spriq[0].when;
  muos_spriq_index i=0;

  --spriq->used;

  for (i = 1; i <= spriq->used/2; i = lchild(i))
    {
      if ((spriq->spriq[i].when - base) > (spriq->spriq[i+1].when - base))
        ++i;

      if ((spriq->spriq[i].when - base) > (spriq->spriq[spriq->used].when - base))
        break;

      spriq->spriq[parent(i)] = spriq->spriq[i];
    }

  spriq->spriq[parent(i)] = spriq->spriq[spriq->used];
}






void
muos_spriq_remove (struct muos_spriq* spriq, muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function fn)
{
  muos_spriq_index element = 0;

  // find element
  for (; element < spriq->used; ++element)
    {
      if (spriq->spriq[element].fn == fn && spriq->spriq[element].when == base+when)
        break;
    }
  if (element == spriq->used)
    return; // not found

  --spriq->used;

  if (spriq->spriq[spriq->used].when-base > spriq->spriq[parent(element)].when-base)
    {
      //upheap
      if (element && when-base < (spriq->spriq[parent(element)].when - base))
        while (element && when-base < (spriq->spriq[parent(element)].when - base))
          {
            spriq->spriq[element] = spriq->spriq[parent(element)];
            element /= 2;
          }

      spriq->spriq[element] = spriq->spriq[spriq->used];
    }
  else
    {
      //downheap
      for (element = lchild(element); element <= spriq->used/2; element = lchild(element))
        {
          if ((spriq->spriq[element].when - base) > (spriq->spriq[element+1].when - base))
            ++element;

          if ((spriq->spriq[element].when - base) > (spriq->spriq[spriq->used].when - base))
            break;

          spriq->spriq[parent(element)] = spriq->spriq[element];
        }

      spriq->spriq[parent(element)] = spriq->spriq[spriq->used];
    }
}

