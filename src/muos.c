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
  MUOS_BGQ_PUSHBACK (blink_led);
}

void wait_a_bit(intptr_t amount)
{
  for (intptr_t i = 1000; i; --i)
    _delay_loop_2(amount);

  MUOS_BGQ_PUSHBACK_ARG (wait_a_bit, amount);
}



int main()
{
  DDRB = _BV(PINB5);

  MUOS_BGQ_PUSHBACK (blink_led);
  MUOS_BGQ_PUSHBACK_ARG (wait_a_bit, 10000);
  MUOS_BGQ_PUSHBACK (blink_led);
  MUOS_BGQ_PUSHBACK_ARG (wait_a_bit, 10000);
  MUOS_BGQ_PUSHBACK (blink_led);
  MUOS_BGQ_PUSHBACK_ARG (wait_a_bit, 1000);
  MUOS_BGQ_PUSHBACK (blink_led);
  MUOS_BGQ_PUSHBACK_ARG (wait_a_bit, 1000);

  
  //TODO: how to init all muos structures .. #define MUOS_EXPLICIT_INIT
  
  //  MUOS_SCHED_INIT (0, DIV64);

  sei();

  for(;;)
    {
      do {
        while(MUOS_RTQ_SCHEDULE());
      } while (MUOS_BGQ_SCHEDULE());
      //      for (uint32_t i = 100000; --i;) {}
      //blink_led_tm();
      
      //      MUOS_SLEEP;
    }
}
