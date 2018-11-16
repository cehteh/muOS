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


void
toggle_led_timed (const struct muos_spriq_entry* event)
{
  muos_interrupt_enable ();
  MUOS_DEBUG_C1_TOGGLE;

  muos_clpq_repeat (event, MUOS_CLOCK_MILLISECONDS (10));
}



void
error (void)
{
#define MUOS_ERROR(name)                                         \
  muos_error_check (muos_##name);

  //  MUOS_ERRORS;
#undef MUOS_ERROR
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
  muos_clpq_at (0, MUOS_CLOCK_MILLISECONDS (10), toggle_led_timed);
  muos_bgq_pushback (hello_world);
}

