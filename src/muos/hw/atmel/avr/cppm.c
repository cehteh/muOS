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

#if MUOS_CPPM_CHANNELS > 0

#include <muos/clock.h>
#include <muos/hpq.h>
#include <muos/cppm.h>


static volatile muos_clock cppm_time;
static uint8_t cppm_channel = 255;

#ifdef MUOS_CPPM_CAPTURE

void
muos_hw_cppm_init (void)
{
  TCCR1B |= _BV(ICNC1);
  TIMSK1 |= _BV(ICIE1);
}

#ifdef MUOS_CPPM_FRAME_CLOCKSYNC
void
muos_cppm_calibrate (void)
{
  muos_clock_calibrate (cppm_time, MUOS_CPPM_FRAME);
};
#endif

#ifdef MUOS_CPPM_CALLBACK
void MUOS_CPPM_CALLBACK (void);
#endif

ISR(ISRNAME_CAPTURE(MUOS_CPPM_CAPTURE))
{
  MUOS_DEBUG_INTR_ON;
  muos_clock now = ICR1;
  //PLANNED: compile time check that hwclock and icp match
  now += ((TIFR1 & _BV(TOV1)) && (now < (muos_hwclock)~0/2))
    ?(1UL<<(sizeof(muos_hwclock)*8)):0;
  now += muos_clock_count_*(1UL<<(sizeof(muos_hwclock)*8));

  muos_clock elapsed = muos_clock_elapsed (now, cppm_time);
  cppm_time = now;

  if (elapsed < MUOS_CPPM_MIN)
    {
      muos_error_set_isr (muos_error_cppm_frame);
      cppm_channel = 255;
    }
  else if (elapsed > MUOS_CPPM_MAX)
    {
      cppm_channel = 0;
#ifdef MUOS_CPPM_FRAME_CLOCKSYNC
      muos_hpq_pushback_isr (muos_cppm_calibrate);
      /* pushback error here gets silently ignored, it won't harm to miss a calibration */
#endif
    }
  else if (cppm_channel < MUOS_CPPM_CHANNELS)
    {
#ifdef MUOS_CPPM_RAW
      muos_cppm_channel_raw[cppm_channel] = MUOS_CPPM_RAW_FILTER (muos_cppm_channel_raw[cppm_channel], elapsed);
#endif

#ifdef MUOS_CPPM_COOKED
      uint16_t old =
        ((uint16_t)muos_cppm_channel_cooked[cppm_channel] + 125)
        * (MUOS_CPPM_COOKED_MAX - MUOS_CPPM_COOKED_MIN) / 250
        + MUOS_CPPM_COOKED_MIN;

      elapsed = elapsed > (((uint16_t)127 + 125)
                           * (MUOS_CPPM_COOKED_MAX - MUOS_CPPM_COOKED_MIN) / 250
                           + MUOS_CPPM_COOKED_MIN)
        ?(((uint16_t)127 + 125)
          * (MUOS_CPPM_COOKED_MAX - MUOS_CPPM_COOKED_MIN) / 250
          + MUOS_CPPM_COOKED_MIN)
        : elapsed;

      muos_cppm_channel_cooked[cppm_channel] =
        (((int16_t)((old+2*elapsed)/3) - (int16_t)MUOS_CPPM_COOKED_MIN)*250
         / (int16_t)(MUOS_CPPM_COOKED_MAX - MUOS_CPPM_COOKED_MIN))
        - 125;
#endif

      ++cppm_channel;
#ifdef MUOS_CPPM_CALLBACK
      if (cppm_channel == MUOS_CPPM_CHANNELS)
        if (muos_hpq_pushback_isr (MUOS_CPPM_CALLBACK))
          muos_error_set_isr (muos_error_cppm_hpq_callback);
#endif
    }
  MUOS_DEBUG_INTR_OFF;
}




#endif

#if 0
//PLANNED: pcint implementation
//void
//muos_hw_cppm_init (void)
//{
//  GIMSK |= _BV(PCIE);
//  PCMSK |= _BV(PCINT2);
//}
//ISR(ISRNAME_PCINT(MUOS_CPPM_PCINT))
//{
//}

#endif
#endif
