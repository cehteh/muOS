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

#include <muos/clock.h>

volatile muos_clock muos_clock_coarse;

void
muos_clock_90init (void)
{
  //TODO: call clock_hw_init
  MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE(MUOS_CLOCK_HW);
  MUOS_HW_CLOCK_PRESCALE_SET(MUOS_CLOCK_HW, MUOS_CLOCK_PRESCALER);
}



muos_clock
muos_clock_now (void)
{
  muos_clock counter;
  muos_hwclock hw;

  muos_interrupt_disable ();
  hw = MUOS_CLOCK_REGISTER;
  counter = muos_clock_coarse
    + (hw<((muos_hwclock)~0/2)?
       (muos_clock)1<<(sizeof(muos_hwclock)*8)
       :0);
  muos_interrupt_enable ();

  return counter + hw;
}


muos_clock
muos_clock_now_isr (void)
{
  muos_clock counter;
  muos_hwclock hw;

  hw = MUOS_CLOCK_REGISTER;
  counter = muos_clock_coarse
    + (hw<((muos_hwclock)~0/2)?
       (muos_clock)1<<(sizeof(muos_hwclock)*8)
       :0);

  return counter + hw;
}




muos_clock32
muos_clock32_now (void)
{
  muos_clock32 counter;
  muos_hwclock hw;

  muos_interrupt_disable ();
  hw = MUOS_CLOCK_REGISTER;
  counter = muos_clock_coarse
    + (hw<((muos_hwclock)~0/2)?
       (muos_clock)1<<(sizeof(muos_hwclock)*8)
       :0);
  muos_interrupt_enable ();

  return counter + hw;
}


muos_clock32
muos_clock32_now_isr (void)
{
  muos_clock32 counter;
  muos_hwclock hw;

  hw = MUOS_CLOCK_REGISTER;
  counter = muos_clock_coarse
    + (hw<((muos_hwclock)~0/2)?
       (muos_clock)1<<(sizeof(muos_hwclock)*8)
       :0);

  return counter + hw;
}







muos_clock
muos_clock_elapsed (muos_clock now, muos_clock start)
{
  if (now > start)
    return now - start;
  else
    return start - now;
}

muos_clock32
muos_clock32_elapsed (muos_clock32 now, muos_clock32 start)
{
  if (now > start)
    return now - start;
  else
    return start - now;
}

muos_clock16
muos_clock16_elapsed (muos_clock16 now, muos_clock16 start)
{
  if (now > start)
    return now - start;
  else
    return start - now;
}

