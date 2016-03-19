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


#if MUOS_CLOCK_CALIBRATE != 0
static muos_clock calibrate_last;

#ifdef MUOS_CLOCK_CALIBRATE_DRIFT
static muos_clock calibrate_drift_last;
static muos_clock calibrate_drift_sync;
#endif


void
muos_clock_calibrate (muos_clock sync)
{
  muos_clock now = muos_clock_now ();

  if (sync)
    {
      muos_clock elapsed;

      if (now > calibrate_last)
        elapsed = now - calibrate_last;
      else
        elapsed = calibrate_last - now;


#ifndef MUOS_CLOCK_CALIBRATE_DRIFT
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
#else
      if (calibrate_drift_sync + sync > MUOS_CLOCK_CALIBRATE_DRIFT)
        calibrate_drift_last += sync;
      else
        calibrate_drift_sync += sync;

      muos_clock elapsed_drift;

      if (now > calibrate_drift_last)
        elapsed_drift = now - calibrate_drift_last;
      else
        elapsed_drift = calibrate_drift_last - now;

      int8_t divert = (elapsed>sync?1:elapsed<sync?-1:0) +
        (elapsed_drift>calibrate_drift_sync?1:elapsed_drift<calibrate_drift_sync?-1:0);

      if (divert > 0)
        {
          if (OSCCAL != 128)
            --OSCCAL;
          else
            OSCCAL = MUOS_HW_ATMEL_OSCAL_LOWSWITCH;
        }
      else if (divert < 0)
        {
          if (OSCCAL != 127)
            ++OSCCAL;
          else
            OSCCAL = MUOS_HW_ATMEL_OSCAL_HIGHSWITCH;
        }
#endif
    }

#ifdef MUOS_CLOCK_CALIBRATE_DRIFT
  else
    {
      calibrate_drift_sync = 0;
      calibrate_drift_last = now;
    }
#endif

  calibrate_last = now;
}
#endif
