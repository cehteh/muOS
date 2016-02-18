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
serial_ping (const struct muos_spriq_entry* event)
{
  if (!muos_status.lineedit_pending)
    muos_output_char ('.');

  muos_clpq_repeat (event, MUOS_CLOCK_SECONDS (1));
}

void
toggle_red_timed (const struct muos_spriq_entry* event)
{
  PIND = _BV(PIND2);
  muos_clpq_repeat (event, MUOS_CLOCK_MILLISECONDS (25));
}

void
toggle_yellow_timed (const struct muos_spriq_entry* event)
{
  PINB = _BV(PINB5);
  muos_clpq_repeat (event, MUOS_CLOCK_MILLISECONDS (250));
}

void
toggle_green_timed (const struct muos_spriq_entry* event)
{
  PIND = _BV(PIND3);
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
serial_echo (void)
{
  uint8_t data = muos_serial_rx_byte ();

  if (!muos_error_check (muos_error_rx_buffer_underflow))
    {
      if (data != '\r')
        muos_output_char ((char)data);
      else
        muos_output_nl ();
    }

  muos_serial_rxrtq_again (serial_echo);
}



void
lineecho (const char* line)
{
  muos_output_cstr ("\r\n<");
  muos_output_cstr (line);
  muos_output_cstr (">\r\n");
}





void
mutest (void)
{
  muos_output_cstr_P ("mµOS Ready:");
  muos_output_nl ();

}


  void
init (void)
{
  DDRB = _BV(PINB5) | _BV(PINB4);
  DDRD = _BV(PIND2) | _BV(PIND3);


  //muos_clpq_at (0, 0, toggle_red_timed);
  //  muos_clpq_at (0, 0, toggle_yellow_timed);
  muos_clpq_at (0, MUOS_CLOCK_MILLISECONDS (250), toggle_green_timed);
  //muos_clpq_at (0, 0, serial_ping);
  //muos_clpq_at (0, 0, serial_blinkerr);

  muos_bgq_pushback (mutest);
}


