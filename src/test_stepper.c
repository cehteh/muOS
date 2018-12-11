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
      struct muos_configstore_data* config = muos_configstore_initial ();
      if (!config)
        {
          MUOS_DEBUG_ERROR_ON;
          muos_output_cstr_P ("Config error ");
          muos_output_nl ();
          return;
        }

      muos_output_cstr_P ("Config defaults ");
      muos_output_nl ();

      config->stepper_prescale[0] = 2;
      config->stepper_minspeed[0] = 65000;
      config->stepper_maxspeed[0] = 256;
      config->stepper_accel[0] = 100;
      config->stepper_decel[0] = 150;

      muos_configstore_unlock ();
    }

  // now play with steppers

  muos_output_cstr_P ("Starting stepper ");
  muos_output_nl ();

  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS (250));

  muos_error_set(muos_stepper_all_on ());
  muos_error_set(muos_stepper_set_zero (0, 0));

  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(250));

  muos_stepper_move_abs (0, 10000, 255);

  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(2000));
  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(2000));
  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(2000));
  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(2000));
  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(2000));
  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(2000));

  muos_stepper_all_off ();

  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(250));
  muos_output_cstr_P ("Done ");
  muos_output_nl ();
}



#if 0
void
stepper_test (void)
{
  muos_interrupt_enable ();

  muos_output_cstr_P ("Starting stepper ");
  muos_output_nl ();

  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS (500));

  //muos_error err =
  muos_error_set(muos_stepper_move_raw (0,
                                       2,
                                       5000, //0xfff0,
                                       100));

  muos_wait (0, 0, MUOS_CLOCK_MILLISECONDS(250));

  muos_error_set(muos_stepper_move_raw (0,
                                        2,
                                        5000, //0xfff0,
                                        -100));

}
#endif

void
app_start (void)
{
  muos_interrupt_enable ();
  muos_configstore_load (config_loaded);
}

void
init (void)
{
  muos_interrupt_enable ();
  muos_output_cstr_P ("Init ");muos_output_nl ();

  //  muos_clpq_at (0, MUOS_CLOCK_MILLISECONDS (1000), toggle_led_timed);

  muos_hpq_pushback (app_start);
}

