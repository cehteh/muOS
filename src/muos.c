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

#include <muos.h>
#include <muos_queue.h>

uint8_t muos_overflow_count;


MUOS_QUEUE8DEF(32)      rtq;
MUOS_QUEUE8DEF(32)      bgq;



#ifdef EXAMPLE
// example code starts here

#include <avr/io.h>
#include <util/delay_basic.h>


void blink_led(void)
{
  PINB = _BV(PINB5);
  muos_queue8_pushback (&bgq.descriptor, 32, blink_led);
}

void wait_a_bit(intptr_t amount)
{
  for (intptr_t i = 1000; i; --i)
    _delay_loop_2(amount);

  muos_queue8_pushback_arg (&bgq.descriptor, 32, wait_a_bit, amount);
}

// example code ends here
#endif

int main()
{
  DDRB = _BV(PINB5);

  //blink_led_n (2000);


  for(;;)
    {
      while(muos_queue8_schedule(&bgq.descriptor, 32)) {}
      
      //      MUOS_SLEEP;
    }
}
