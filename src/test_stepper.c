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
#include <muos/stepper.h>

// define events, no main here
#define BLINK MUOS_CLOCK_MILLISECONDS (500)

void
toggle_led_timed (const struct muos_spriq_entry* event)
{
  muos_interrupt_enable ();
  MUOS_DEBUG_C1_TOGGLE;

  muos_clpq_repeat (event, BLINK);
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
error (void)
{
  // don't handle errors, should be done in real code here
}

#if 0
void config_loaded (void)
{
  muos_interrupt_enable ();

  muos_configstore_status status = muos_configstore_get_status ();

  if (status != CONFIGSTORE_VALID)
    {
      if (status == CONFIGSTORE_INVALID)
        {
          //set defaults, (later save)
        }
      else
        {
          // panic
          for (;;)
            MUOS_DEBUG_ERROR_TOGGLE;
        }
    }

  // now play with steppers
}
#endif



void
stepper_test (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("Starting stepper ");
  muos_output_nl ();

  //muos_error err =
  muos_stepper_cal_mov (0,
                        2,
                        0xfff0,
                        1000);

  muos_output_uint16 (TCNT1);
  muos_output_nl ();

}




void
init (void)
{
  muos_interrupt_enable ();
  muos_output_cstr_P ("Init ");muos_output_nl ();

  //muos_clpq_at (0, MUOS_CLOCK_MILLISECONDS (1000), toggle_led_timed);

  muos_hpq_pushback(stepper_test);
}

