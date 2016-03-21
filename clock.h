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

#ifndef MUOS_CLOCK_H
#define MUOS_CLOCK_H

#include <stdbool.h>

#include <muos/muos.h>


#define MUOS_CLOCK_SECONDS(s) (((uint64_t)s)*F_CPU/MUOS_CLOCK_PRESCALER)
#define MUOS_CLOCK_MILLISECONDS(s) (MUOS_CLOCK_SECONDS(s)/1000)
#define MUOS_CLOCK_MICROSECONDS(s) (MUOS_CLOCK_MILLISECONDS(s)/1000)

#define MUOS_CLOCK_REGISTER MUOS_HW_CLOCK_REGISTER(MUOS_CLOCK_HW)
typedef typeof(MUOS_CLOCK_REGISTER) muos_hwclock;

typedef MUOS_CLOCK_TYPE muos_clock;
typedef MUOS_CLOCK_SHORT_TYPE muos_shortclock;

typedef struct {
  muos_clock coarse;
  muos_hwclock fine;
} muos_fullclock;


extern volatile muos_clock muos_clock_count_;

extern muos_clock muos_now_;

static inline muos_clock
muos_now ()
{
  return muos_now_;
}

static inline void
muos_clock_start (void)
{
  MUOS_HW_CLOCK_ISR_OVERFLOW_ENABLE(MUOS_CLOCK_HW);
  MUOS_HW_CLOCK_PRESCALE_SET(MUOS_CLOCK_HW, MUOS_CLOCK_PRESCALER);
}


static inline muos_clock
muos_clock_now (void)
{
  muos_clock counter;
  muos_hwclock hw;
  do
    {
      counter = muos_clock_count_;
      hw = MUOS_CLOCK_REGISTER;
    }
  while ((uint8_t)counter != (uint8_t)muos_clock_count_);

  return (counter<<(sizeof(MUOS_CLOCK_REGISTER) * 8)) + hw;
}

static inline muos_shortclock
muos_clock_shortnow (void)
{
  muos_shortclock counter;
  muos_hwclock hw;
  do
    {
      counter = (muos_shortclock)muos_clock_count_;
      hw = MUOS_CLOCK_REGISTER;
    }
  while ((uint8_t)counter != (uint8_t)muos_clock_count_);

  return (counter<<(sizeof(MUOS_CLOCK_REGISTER) * 8)) + hw;
}


static inline muos_fullclock
muos_clock_fullnow (void)
{
  muos_fullclock clock;
  do
    {
      clock.coarse = muos_clock_count_;
      clock.fine = MUOS_CLOCK_REGISTER;
    }
  while ((uint8_t)clock.coarse != (uint8_t)muos_clock_count_);

  return clock;
}

muos_clock
muos_clock_elapsed (muos_clock now, muos_clock start);


#if MUOS_CLOCK_CALIBRATE != 0
void
muos_clock_calibrate (const muos_clock now, const muos_clock sync);
#endif

#endif
