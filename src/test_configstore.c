/*
 * add your license here
 */

#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#include <util/delay_basic.h>

#include <muos/muos.h>
#include <muos/error.h>
#include <muos/hpq.h>
#include <muos/bgq.h>
#include <muos/clpq.h>
#include <muos/clock.h>
#include <muos/serial.h>
#include <muos/io.h>
#include <muos/error.h>


#include <muos/configstore.h>


// config layout must be defined before including configstore.h


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
configstore_test (const struct muos_spriq_entry* event)
{
  muos_interrupt_enable ();
  (void) event;

  muos_output_cstr_P ("finally ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();

}


void
configstore_loaded (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("loaded ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();
}



void
configstore_saved (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("saved ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();

  muos_configstore_load (configstore_loaded);
}

void
configstore_load (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("Loading config ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();

  muos_configstore_load (configstore_loaded);
}

void
configstore_save (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("Saving config ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();

  muos_configstore_save (configstore_saved);
}


void
init (void)
{
  muos_interrupt_enable ();
  muos_bgq_pushback (configstore_load);
  
  muos_clpq_at (0, MUOS_CLOCK_MILLISECONDS (1000), configstore_test);
}

