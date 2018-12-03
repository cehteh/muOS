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
#include <muos/configstore.h>

// define events, no main here
#define BLINK MUOS_CLOCK_MILLISECONDS (500)

void
toggle_led_timed (const struct muos_spriq_entry* event)
{
  muos_interrupt_enable ();
  //MUOS_DEBUG_ERROR_TOGGLE;

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

void config_loaded (void)
{
  muos_interrupt_enable ();

  if (muos_error_check(muos_error_configstore_invalid))
    {
      //set defaults, (later save)

    }

  // now play with steppers

  muos_output_cstr_P ("Starting stepper ");
  muos_output_nl ();

  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS (500));

  muos_error_set(muos_stepper_all_on ());
  muos_error_set(muos_stepper_set_zero (0, 100));

  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(250));

  //  muos_stepper_move_abs (0, 0, 255);


  muos_stepper_all_off ();
}



void
stepper_test (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("Starting stepper ");
  muos_output_nl ();

  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS (500));

  //muos_error err =
  muos_error_set(muos_stepper_cal_mov (0,
                                       2,
                                       5000, //0xfff0,
                                       100));

  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(250));

  muos_error_set(muos_stepper_cal_mov (0,
                                       2,
                                       5000, //0xfff0,
                                       -100));

}


void
init (void)
{
  muos_interrupt_enable ();
  muos_output_cstr_P ("Init ");muos_output_nl ();

  //  muos_clpq_at (0, MUOS_CLOCK_MILLISECONDS (1000), toggle_led_timed);

  //  muos_hpq_pushback(stepper_test);

  muos_configstore_load (config_loaded);

}

