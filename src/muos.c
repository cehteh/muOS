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
#include <muos/clock.h>
#include <muos/spriq.h>


uint8_t muos_overflow_count;

#include <util/delay_basic.h>


void muos_error (void)
{
  muos_die ();
}

void muos_die (void)
{
  cli();
  for(;;);
}


void blink_led(void)
{
  PINB = _BV(PINB5);
  muos_bgq_pushback (blink_led);
}

void blink_led_time(void)
{
  PINB = _BV(PINB5);
  //muos_rtpq_repeat (1000);
  //muos_ltpq_repeat (1000);
  //muos_schedule_at (1000);
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

  muos_clock_start ();
  sei(); //TODO: muos_intr_enable() muos_start () (timer reset, and go)

  for(;;)
    {
      do
        {
          do
            {
              while(muos_rtpq_schedule(muos_clock_now()));
            }
          while(muos_rtq_schedule ());
        }
      while (muos_bgq_schedule ());

      //      MUOS_SLEEP;
    }
}
