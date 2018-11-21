/*
 * add your license here
 */

#include <stdlib.h>
#include <avr/pgmspace.h>


#include <muos/muos.h>
#include <muos/error.h>
#include <muos/hpq.h>
#include <muos/bgq.h>
#include <muos/clpq.h>
#include <muos/clock.h>
#include <muos/serial.h>
#include <muos/io.h>
#include <muos/error.h>

// define events, no main here
#define BLINK MUOS_CLOCK_MILLISECONDS (50)

void
toggle_led_timed (const struct muos_spriq_entry* event)
{
  muos_interrupt_enable ();
  MUOS_DEBUG_C1_TOGGLE;

  muos_clpq_repeat (event, BLINK);
}



void
error (void)
{
  // don't handle errors, should be done in real code here
}


void
lineecho (const char* line)
{
  muos_interrupt_enable ();
  muos_output_cstr ("\r\n<");
  muos_output_cstr (line);
  muos_output_cstr (">\r\n");
}


void
hello_world (void)
{
  muos_interrupt_enable ();
  muos_output_cstr ("Hello World");
  muos_output_nl ();
}


void
init (void)
{
  muos_clpq_at (0, BLINK, toggle_led_timed);
  muos_bgq_pushback (hello_world);
}

