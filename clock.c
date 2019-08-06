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

// starting the clock
void
muos_clock_90init (void)
{
  //TODO: call clock_hw_init
  MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE(MUOS_CLOCK_HW);
  MUOS_HW_CLOCK_PRESCALE_SET(MUOS_CLOCK_HW, MUOS_CLOCK_PRESCALER);
}


void
muos_clock_now (muos_clock* now)
{
  muos_hwclock hw;
  uint8_t overflow = 0;

  muos_interrupt_disable ();
  hw = MUOS_CLOCK_REGISTER;
  if (hw<((muos_hwclock)~0/2))
    overflow = MUOS_HW_CLOCK_OVERFLOW (MUOS_CLOCK_HW);

  muos_barray_copy (now->barray, muos_clock_coarse.barray);
  muos_interrupt_enable ();

  muos_barray_add_uint8 (now->barray, overflow, sizeof(MUOS_CLOCK_REGISTER));
  muos_barray_add_uint8 (now->barray, hw, 0);  //FIXME: 16bit hwclock, muos_barray_add_uint16()
}


void
muos_clock_now_isr (muos_clock* now)
{
  uint8_t overflow = 0;
  muos_hwclock hw = MUOS_CLOCK_REGISTER;

  if (hw<((muos_hwclock)~0/2))
    overflow = MUOS_HW_CLOCK_OVERFLOW(MUOS_CLOCK_HW);

  muos_barray_copy (now->barray, muos_clock_coarse.barray);

  muos_barray_add_uint8 (now->barray, overflow, sizeof(MUOS_CLOCK_REGISTER));
  muos_barray_add_uint8 (now->barray, hw, 0);  //FIXME: 16bit hwclock
}


muos_clock16
muos_clock_now16_isr (void)
{
  uint8_t overflow = 0;
  muos_hwclock hw = MUOS_CLOCK_REGISTER;

  if (hw<((muos_hwclock)~0/2))
    overflow = MUOS_HW_CLOCK_OVERFLOW(MUOS_CLOCK_HW);

  return muos_barray_uint16 (((muos_clock)muos_clock_coarse).barray, 0)
    + hw
    + (overflow<<(8*sizeof(MUOS_CLOCK_REGISTER)));
}
