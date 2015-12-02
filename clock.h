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
#include "muos_config.h"
#include "muos/muos.h"
#include <muos/hwdef.h>

#define MUOS_CLOCK_COUNTER  MUOS_CONCAT2(muos_clock_,MUOS_CLOCK_HW)
#define MUOS_CLOCK_ISR_OVERFLOW  MUOS_HW_ISR(MUOS_HW_CLOCK_ISRNAME_OVERFLOW(MUOS_CLOCK_HW))
#define MUOS_CLOCK_REGISTER MUOS_HW_CLOCK_REGISTER(MUOS_CLOCK_HW)

MUOS_TYPEDEF(uint, MUOS_CLOCK_TYPE, muos_clock);
MUOS_TYPEDEF(uint, MUOS_CLOCK_SHORT_TYPE, muos_shortclock);
typedef typeof(MUOS_CLOCK_REGISTER) muos_hwclock;
typedef struct {
  muos_clock high;
  muos_hwclock low;
} muos_fullclock;




extern volatile muos_clock MUOS_CLOCK_COUNTER;

#define MUOS_CLOCKDEF(...)                      \
  MUOS_CLOCK_ISR_OVERFLOW                       \
  {                                             \
    ++MUOS_CLOCK_COUNTER;                       \
    (void) __VA_ARGS__;                         \
  }                                             \
  volatile muos_clock MUOS_CLOCK_COUNTER


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
      counter = MUOS_CLOCK_COUNTER;
      hw = MUOS_CLOCK_REGISTER;
    }
  while (counter != MUOS_CLOCK_COUNTER);

  return (counter<<(sizeof(MUOS_CLOCK_REGISTER) * 8)) + hw;
}

static inline muos_shortclock
muos_clock_nowshort (void)
{
  muos_shortclock counter;
  muos_hwclock hw;
  do
    {
      counter = MUOS_CLOCK_COUNTER;
      hw = MUOS_CLOCK_REGISTER;
    }
  while (counter != MUOS_CLOCK_COUNTER);

  return (counter<<(sizeof(MUOS_CLOCK_REGISTER) * 8)) + hw;
}

//TODO: now_long

//static inline void
//muos_clock_stop (void)






#if 0
#define MUOS_CLOCK_ISR_COMPMATCH  MUOS_HW_ISR(MUOS_HW_CLOCK_ISRNAME_COMPMATCH(MUOS_CLOCK_HW, MUOS_CLOCK_HW_COMPAREMATCH))

#define MUOS_CLOCK_COMPMATCHDEF(...)               \
  MUOS_CLOCK_ISR_COMPMATCH \
  {                                                     \
    (void) __VA_ARGS__;                                 \
  }


static inline void
muos_clock_isr_compmatch_enable (muos_clock_hwtype at)
{
  MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE(MUOS_CLOCK_HW, MUOS_CLOCK_HW_COMPAREMATCH, at);
}

static inline void
muos_clock_isr_compmatch_disable (void)
{
  MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE(MUOS_CLOCK_HW, MUOS_CLOCK_HW_COMPAREMATCH);
}


static inline bool
muos_clock_isr_compmatch_enabled (void)
{
  return MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLED(MUOS_CLOCK_HW, MUOS_CLOCK_HW_COMPAREMATCH);
}
#endif


#endif
