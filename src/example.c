/*
 * add your license here
 */

#include <stdlib.h>
#include <util/delay_basic.h>
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
#include <muos/cppm.h>

// define events, no main here


void
toggle_led_timed (const struct muos_spriq_entry* event)
{
  //PINA = _BV(PINA6);
  muos_clpq_repeat (event, MUOS_CLOCK_MILLISECONDS (100));
}



void
toggle_led_timed2 (const struct muos_spriq_entry* event)
{
  //PINA = _BV(PINA0) | _BV(PINA1) | _BV(PINA2) | _BV(PINA3) | _BV(PINA4) | _BV(PINA5) | _BV(PINA6);
  muos_clpq_repeat (event, MUOS_CLOCK_MICROSECONDS (2000));
}


void
toggle_led (void)
{
  //PINA = _BV(PINA5);
  //PINA = _BV(PINA5);
  muos_bgq_pushback (toggle_led);
  //PINA = _BV(PINA5);
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
lineecho (const char* line)
{
  muos_output_cstr ("\r\n<");
  muos_output_cstr (line);
  muos_output_cstr (">\r\n");
#if 0 //hexdump
  muos_output_cstr ("<");
  while (*line)
    {
      muos_output_base (16);
      muos_output_uint8 (*line);
      muos_output_char (' ');
      ++line;
    }
  muos_output_cstr (">\r\n");
#endif
}

void
cppm_output (void)
{
#if 0
  //PINA = _BV(PINA2);

  //if (muos_cppm_channel_cooked[0] > 0)
  //  PORTA |= _BV(PINA0);
  //else
  //  PORTA &= ~_BV(PINA0);

  //if (muos_cppm_channel_cooked[1] >= 0)
  //  PORTA |= _BV(PINA1);
  //else
  //  PORTA &= ~_BV(PINA1);

#if 0
  if (muos_cppm_channel[2] > MUOS_CLOCK_MICROSECONDS(1500))
    PORTA |= _BV(PINA2);
  else
    PORTA &= ~_BV(PINA2);

  if (muos_cppm_channel[3] > MUOS_CLOCK_MICROSECONDS(1500))
    PORTA |= _BV(PINA3);
  else
    PORTA &= ~_BV(PINA3);

  if (muos_cppm_channel[4] > MUOS_CLOCK_MICROSECONDS(1500))
    PORTA |= _BV(PINA4);
  else
    PORTA &= ~_BV(PINA4);
  if (muos_cppm_channel[5] > MUOS_CLOCK_MICROSECONDS(1500))
    PORTA |= _BV(PINA5);
  else
    PORTA &= ~_BV(PINA5);

  if (muos_cppm_channel[6] > MUOS_CLOCK_MICROSECONDS(1500))
    PORTA |= _BV(PINA6);
  else
    PORTA &= ~_BV(PINA6);
#endif
#endif
}

void
hello_world (void)
{
  muos_output_cstr ("Hello World");
  muos_output_nl ();
}


void
init (void)
{
  //DDRA = _BV(PINA0) | _BV(PINA1) | _BV(PINA2) | _BV(PINA3) | _BV(PINA4) | _BV(PINA5) | _BV(PINA6);
  //  PINA = _BV(PINA0) | _BV(PINA2) | _BV(PINA4) | _BV(PINA6);

  // PINA = _BV(PINA1);
  //  GIMSK |= _BV(PCIE);
  //PCMSK |= _BV(PCINT2);

  //muos_clpq_at (0, MUOS_CLOCK_MILLISECONDS (100), toggle_led_timed);
  //muos_clpq_at (0, MUOS_CLOCK_MILLISECONDS (1), toggle_led_timed2);
  muos_bgq_pushback (hello_world);
}
