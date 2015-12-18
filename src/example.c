/*
 * add your license here
 */

#include <stdlib.h>
#include <util/delay_basic.h>


#include <muos/muos.h>
#include <muos/rtq.h>
#include <muos/bgq.h>
#include <muos/clpq.h>
#include <muos/clock.h>

// define events, no main here


void
toggle_red_timed (const struct muos_spriq_entry* event)
{
  PIND = _BV(PIND2);
  muos_clpq_repeat (event, MUOS_CLOCK_MILLISECONDS (500));
}

void
toggle_yellow_timed (const struct muos_spriq_entry* event)
{
  PINB = _BV(PINB5);
  muos_clpq_repeat (event, MUOS_CLOCK_MILLISECONDS (125));
}

void
toggle_green_timed (const struct muos_spriq_entry* event)
{
  PIND = _BV(PIND3);
  muos_clpq_repeat (event, MUOS_CLOCK_MILLISECONDS (250));
}


void
init (void)
{
  DDRB = _BV(PINB5) | _BV(PINB4);
  DDRD = _BV(PIND2) | _BV(PIND3);

  muos_clpq_at (0, 0, toggle_green_timed);
  muos_clpq_at (0, 0, toggle_red_timed);
  muos_clpq_at (0, 0, toggle_yellow_timed);
}


