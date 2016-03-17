/*
 * add your license here
 */

#include <stdlib.h>
#include <util/delay_basic.h>
#include <avr/pgmspace.h>


#include <muos/muos.h>
#include <muos/error.h>
#include <muos/rtq.h>
#include <muos/bgq.h>
#include <muos/clpq.h>
#include <muos/clock.h>
#include <muos/serial.h>
#include <muos/io.h>
#include <muos/error.h>

// define events, no main here


void
toggle_led_timed (const struct muos_spriq_entry* event)
{
  PINB = _BV(PINB1);
  muos_clpq_repeat (event, MUOS_CLOCK_MILLISECONDS (250));
}



void
error (void)
{
#define MUOS_ERROR(name)                                         \
  muos_error_check (muos_##name);

  MUOS_ERRORS;
#undef MUOS_ERROR
}


void
init (void)
{
  DDRB = _BV(PINB1);
  PORTB |= _BV(PINB1);
  GIMSK |= _BV(PCIE);
  PCMSK |= _BV(PCINT2);

  muos_clpq_at (0, MUOS_CLOCK_MILLISECONDS (250), toggle_led_timed);
}

ISR(PCINT0_vect)
{
  if (PINB & _BV(PINB2))
    muos_clock_calibrate (MUOS_CLOCK_MILLISECONDS (20));
}
