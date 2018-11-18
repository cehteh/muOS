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
  MUOS_DEBUG_INTR_ON;
  ++muos_clock_count_;
}

// compmatch interrupt is only used to wake the mainloop
EMPTY_INTERRUPT(ISRNAME_COMPMATCH(MUOS_CLOCK_HW));


#ifdef MUOS_CLOCK_CALIBRATE
static muos_clock calibrate_last;
static int32_t divert;


void
muos_clock_calibrate (const muos_clock now, const muos_clock sync)
{
  muos_clock elapsed = muos_clock_elapsed (now, calibrate_last);
  calibrate_last = now;

  if (sync
#ifdef MUOS_CLOCK_CALIBRATE_MAX_DERIVATION
      && elapsed >= sync - MUOS_CLOCK_CALIBRATE_MAX_DERIVATION(sync)
      && elapsed <= sync + MUOS_CLOCK_CALIBRATE_MAX_DERIVATION(sync)
#endif
      )
    {
      divert = (divert + ((int32_t)sync - (int32_t)elapsed)*3)/4;

      if (divert > MUOS_CLOCK_CALIBRATE_DEADBAND/MUOS_CLOCK_PRESCALER)
        {
          if (OSCCAL != 127)
            ++OSCCAL;
          else
            OSCCAL = MUOS_HW_ATMEL_OSCAL_HIGHSWITCH;
        }
      else if (divert < -MUOS_CLOCK_CALIBRATE_DEADBAND/MUOS_CLOCK_PRESCALER)
        {
          if (OSCCAL != 128)
            --OSCCAL;
          else
            OSCCAL = MUOS_HW_ATMEL_OSCAL_LOWSWITCH;
        }
    }
}
#endif
