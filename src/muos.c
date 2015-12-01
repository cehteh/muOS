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

//configuration
#define EXAMPLE


#include <muos/queue.h>
#include <muos/timer.h>
#include <muos/priq.h>
#include <muos/scheduler.h>


uint8_t muos_overflow_count;

#include <util/delay_basic.h>


void blink_led(void)
{
  PINB = _BV(PINB5);
  muos_bgq_pushback (blink_led);
}

void wait_a_bit(intptr_t amount)
{
  for (intptr_t i = 1000; i; --i)
    _delay_loop_2(amount);

  muos_bgq_pushback_arg (wait_a_bit, amount+1);
  MUOS_BGQ_PUSHBACK_ARG (wait_a_bit, amount);
}



int main()
{
  DDRB = _BV(PINB5);

  //muos_bgq_pushback (blink_led);
  //muos_bgq_pushback_arg (wait_a_bit, 0);

  
  //TODO: how to init all muos structures .. #define MUOS_EXPLICIT_INIT
  
  //  MUOS_SCHED_INIT (0, DIV64);

  sei();

  for(;;)
    {
      do
        {
          do
            {
              // while(MUOS_SCHEDQ_SCHEDULE(muos_rtpq));
            }
          while(MUOS_QUEUE_SCHEDULE(muos_rtq));
        }
      while (MUOS_QUEUE_SCHEDULE(muos_bgq));

      //      MUOS_SLEEP;
    }
}
