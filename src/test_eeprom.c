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
#include <muos/eeprom.h>



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

char mem1[16] = "source";
char mem2[16] = "dest";


void
eeprom_erased (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("erased ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();

  if (!muos_error_check (muos_error_eeprom_verify))
    {
      muos_output_cstr_P ("OK");
      muos_output_nl ();
    }
  else
    {
      muos_output_cstr_P ("FAIL");
      muos_output_nl ();
    }
}


void
eeprom_read (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("read ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();

  if (!muos_error_check (muos_error_eeprom_verify))
    {
      muos_output_cstr_P ("OK");
      muos_output_nl ();
    }
  else
    {
      muos_output_cstr_P ("FAIL");
      muos_output_nl ();
    }

  muos_eeprom_erase (32,
                     16,
                     eeprom_erased);

}





void
eeprom_verifail (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("verfail ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();

  if (muos_error_check (muos_error_eeprom_verify))
    {
      muos_output_cstr_P ("OK");
      muos_output_nl ();
    }
  else
    {
      muos_output_cstr_P ("FAIL");
      muos_output_nl ();
    }

  muos_eeprom_read (mem2,
                    32,
                    16,
                    eeprom_read);
}

void
eeprom_verified (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("verfied ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();

  if (!muos_error_check (muos_error_eeprom_verify))
    {
      muos_output_cstr ("OK");
      muos_output_nl ();
    }
  else
    {
      muos_output_cstr ("FAIL");
      muos_output_nl ();
    }

  muos_eeprom_verify (mem2,
                      32,
                      16,
                      eeprom_verifail);
}

void
eeprom_written (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("written ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();

  muos_eeprom_verify (mem1,
                      32,
                      16,
                      eeprom_verified);
}


void
eeprom_test (const struct muos_spriq_entry* event)
{
  muos_interrupt_enable ();
  (void) event;

  muos_output_cstr_P ("Start ");
  muos_output_uint8 (muos_error_pending ());
  muos_output_nl ();

  muos_eeprom_write (mem1,
                     32,
                     16,
                     eeprom_written);
}




void
init (void)
{
  muos_interrupt_enable ();
  muos_clpq_at (0, MUOS_CLOCK_MILLISECONDS (1), eeprom_test);
}

