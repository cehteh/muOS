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

ISR(ISRNAME_OVERFLOW(MUOS_CLOCK_HW))
{
#if MUOS_DEBUG_INTR == 1
  //FIXME: PORTB |= _BV(PINB4);
#endif
  ++muos_clock_count_;
#if MUOS_DEBUG_INTR ==1
  //FIXME: PORTB &= ~_BV(PINB4);
#endif

}

// compmatch interrupt is only used to wake the mainloop
EMPTY_INTERRUPT(ISRNAME_COMPMATCH(MUOS_CLOCK_HW, MUOS_CLOCK_HW_COMPAREMATCH));


#if MUOS_CLOCK_CALIBRATE == 1
static muos_clock muos_clock_calibrate_last;

//PLANNED: add long time calibration to keep frequency more stable

void
muos_clock_calibrate (muos_clock sync)
{
  muos_clock now = muos_clock_now ();

  if (sync)
    {
      muos_clock elapsed;

      if (now > muos_clock_calibrate_last)
        elapsed = now - muos_clock_calibrate_last;
      else
        elapsed = muos_clock_calibrate_last - now;

      if (elapsed > sync)
        {
          if (OSCCAL != 128)
            --OSCCAL;
          else
            OSCCAL = MUOS_HW_ATMEL_OSCAL_LOWSWITCH;
        }
      else if (elapsed < sync)
        {
          if (OSCCAL != 127)
            ++OSCCAL;
          else
            OSCCAL = MUOS_HW_ATMEL_OSCAL_HIGHSWITCH;
        }
    }

  muos_clock_calibrate_last = now;
}
#endif
