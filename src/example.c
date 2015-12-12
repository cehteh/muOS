/*
 * add your license here
 */

#include <util/delay_basic.h>


#include <muos/muos.h>
#include <muos/rtq.h>
#include <muos/bgq.h>
#include <muos/clpq.h>

// define events, no main here

void
blink_led_time (const struct muos_spriq_entry* event)
{
  PINB = _BV(PINB5);
  (void) event;
  //muos_repeat (event, 1000);
}



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
}


void
init (void)
{
  DDRB = _BV(PINB5);

  muos_bgq_pushback (blink_led);
  muos_bgq_pushback_arg (wait_a_bit, 10);
  //muos_at (1000, blink_led_time);
}


