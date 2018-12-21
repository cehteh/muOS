// gcc -g -Wall -Wextra -Werror stepper_accel_decel.c && ./a.out | quickplot -z &

/*

Description
-----------


Preparation
~~~~~~~~~~~

we have the formula for generating a slope shape:

         64*accel          64*decel
 speed = -------- + ---------------------- + max_speed
         position   (slope_len - position)

where
   accel and decel are factors for the curve shape

   slope_len is equal or smaller than max_slope

   speed and max_speed are the mcu timer counter values (the smaller the faster)

   position is the x axis/position on the slope

This generates a concave function which comes from infinity at position=0
goes close to max_speed and then increases again to infinity at slope_len.

We then find the position for the starting and ending speeds speed_in and speed_out,
by bisect (or sectant method) approximation.



Slope Generation
~~~~~~~~~~~~~~~~

For the full distance we start the slope at accel_offset (speed_in). Then run until the
speeds start to increase again (around half of the slope). Then a constant speed stretch is
inserted to make up the accel and decel offset distance any any difference between the
slope_len and the full distance. When this constant speed stretch is done slope operation
continues decelerating to speed_out (slope_len-decel_offset).

advantages:
 - always has a smooth
 - We can pre calculate the max speed reached for max_slope and use it as offset later. Far moves
   will then always run exactly at max_speed on the constant speed stretch.

 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <inttypes.h>

#define PRINT(var) printf("# %40s: %d\n", #var, var)
#define PRINTF(fmt, ...) printf("# %.40s " fmt ":\n", #__VA_ARGS__,  ## __VA_ARGS__)

// configuration

#define MUOS_STEPPER_SLOPE_MULTIPLIER 64UL

struct stepper_config
{
  uint16_t accel;
  uint16_t decel;
  uint16_t min_speed;
  uint16_t max_speed;
  uint16_t max_slope;
}the_config;


struct stepper_config* config = &the_config;



// state

enum muos_stepper_arming_state
  {
   MUOS_STEPPER_UNKNOWN,
   MUOS_STEPPER_DISABLED,
   MUOS_STEPPER_RAW,
   MUOS_STEPPER_OFF,
   MUOS_STEPPER_HOLD,
   MUOS_STEPPER_ARMED,
   MUOS_STEPPER_STOPPED,
   MUOS_STEPPER_SLOW,
   MUOS_STEPPER_SLOPE,
   MUOS_STEPPER_FAST,
   MUOS_STEPPER_FASTOUT,
  };

#define MUOS_STEPPER_POSITION_SLOTS 4

//TOOD: check and document bounds
//TODO: split into prepared/cached and runtime state

struct stepper_slope
{
  int32_t  dest;           // absolute end position
  uint32_t fast;           // fast steps at the top of the slope
  uint16_t pos;            // slope position
  uint16_t end;            // only slope part
  uint16_t len;            // only slope part
  uint16_t speed_out;
};



struct stepper_state
{
  enum muos_stepper_arming_state state;
  enum muos_stepper_arming_state before_raw;
  volatile int32_t position;

  uint16_t speed_flt;
  uint16_t max_speed;
  struct stepper_slope slope;  //TODO: doublebuffer
};

struct stepper_state muos_steppers;

struct stepper_slope* slope = &muos_steppers.slope;


// sim
uint32_t CLOCK;    // clock ticks
uint16_t OCRA;     // pulse length


__attribute__((always_inline))
static inline uint16_t
udiv_inline (uint32_t num, uint32_t den)
{
  uint32_t mask = 1;
  uint16_t res = 0;

  while (mask && den < num  && !(den & (1L<<31)))
    {
      den <<= 1;
      mask <<= 1;
    }

  while (mask)
    {
      if (num >= den)
	{
	  num -= den;
          res |= mask;
	}
      mask >>=1;
      den >>=1;
    }
  return res;
}

#if 0
uint16_t
isqrt (uint32_t n)
{
  uint32_t c = 0x8000;
  uint32_t g = 0x8000;

  for(;;)
    {
      if(g*g > n)
        g ^= c;
      c >>= 1;
      if(c == 0)
        return g;
      g |= c;
    }
}
#endif


/* simple version ignoring maxspeed for bisecting and 32bit */
uint32_t
slope_speed (uint16_t position,
             uint16_t slope_len,
             uint16_t accel,
             uint16_t decel)
{
  return
    (accel * MUOS_STEPPER_SLOPE_MULTIPLIER / position)
    +
    (decel * MUOS_STEPPER_SLOPE_MULTIPLIER / (slope_len-position));
}




/* finds the position which is just equal or faster than speed */
uint16_t
slope_bisect (uint16_t speed, uint16_t slope_len, uint16_t fact_a, uint16_t fact_b)
{
  uint16_t mid;
  uint32_t tmp_speed;
  uint16_t low;
  uint16_t high;

  for (high = 1; high < slope_len; high = high < slope_len/2? high*2:slope_len)
    {
      tmp_speed = slope_speed (high, slope_len,fact_a, fact_b);
      if (tmp_speed < speed)
        break;
    }

  low = high/2;

  while (high > low+1)
    {
      mid = (high-low)/2+low;
      tmp_speed = slope_speed (mid, slope_len, fact_a, fact_b);
      PRINT(tmp_speed);

      if (tmp_speed < speed)
        {
          high = mid;
        }
      else if (tmp_speed > speed)
        {
          low = mid;
        }
      else
        break;
    }

  return high;
}


#if 0 //PLANNED: find offset for max_speed for full length slope
/* finds the max speed for the whole slope */
uint16_t
slope_trisect (uint16_t slope_len, uint16_t fact_a, uint16_t fact_b)
{
  uint32_t speed;
  uint32_t tmp_speed;
  uint16_t low = 1;
  uint16_t high = slope_len - 1;
  uint16_t mid;

  while (high > low+1)
    {
      mid = (high-low)/2+low;
      speed = slope_speed (mid, slope_len, fact_a, fact_b);

      //TODO:
  return high;
}
#endif



void
init (int32_t dest, uint16_t speed_in, uint16_t max_speed, uint16_t speed_out, uint16_t out_steps)
{
  OCRA = speed_in;
  muos_steppers.state = MUOS_STEPPER_SLOPE;

  slope->dest = dest;

#define MUOS_ATOMIC_READ(dest, source) do {dest = source;} while (dest != source)

  int32_t position;
  MUOS_ATOMIC_READ(position, muos_steppers.position);

  uint32_t len;
  if (dest > position)
    len = dest - position;
  else
    len = position - dest;

  PRINT(len);

  if (out_steps > len)
    out_steps = len;  //TODO: fast return only out_steps w/o fast or slope

  slope->len = len-out_steps < config->max_slope? len-out_steps : config->max_slope;

  PRINT(slope->len);

  // find start offset, that is where speed_in is
  slope->pos = slope_bisect (speed_in - max_speed,
                             slope->len,
                             config->accel,
                             config->decel);

  slope->end = slope->len - (slope_bisect (speed_out - max_speed,
                                         slope->len,
                                         config->decel,
                                          config->accel));

  PRINT(slope->pos);
  PRINT(slope->end);

  slope->speed_out = speed_out;

  slope->fast = len - (slope->end - slope->pos) - out_steps - 2;

  PRINT(slope->fast);

  muos_steppers.speed_flt = 65535;

  printf("%u %u %u %u\n", 0, muos_steppers.position, (65536*16)/(speed_in-1), speed_in);
  PRINT(OCRA);
}


#define CLOCKP (CLOCK/1000)


bool
overflow_isr(void)
{
  CLOCK += OCRA;
  ++muos_steppers.position;

  if (muos_steppers.position == slope->dest)
    {
      OCRA = slope->speed_out;
      printf("%u %u %u %u\n", CLOCKP, muos_steppers.position, (uint32_t)((65536*16)/(OCRA+1)), OCRA);
      return false;
    }

  uint16_t speed=0;

  switch (muos_steppers.state)
    {
    default:
      return false;
    case MUOS_STEPPER_FAST:
      PRINTF("FAST");
      if (!--slope->fast)
        muos_steppers.state = MUOS_STEPPER_SLOPE;

      printf("%u %u %u %u\n", CLOCKP, muos_steppers.position,
             65536*16/(OCRA+config->max_speed), OCRA);

      return true;
      break;
    case MUOS_STEPPER_FASTOUT:
      PRINTF("FASTOUT");

      printf("%u %u %u %u\n", CLOCKP, muos_steppers.position,
             65536*16/(OCRA+config->max_speed), OCRA);
      return true;
    case MUOS_STEPPER_SLOPE:
      speed =
        udiv_inline (config->accel * MUOS_STEPPER_SLOPE_MULTIPLIER,
                     slope->pos)
        +
        udiv_inline (config->decel * MUOS_STEPPER_SLOPE_MULTIPLIER,
                     slope->len - slope->pos)
        +
        config->max_speed;

      uint16_t old_flt =  muos_steppers.speed_flt;
      muos_steppers.speed_flt = (muos_steppers.speed_flt + speed)/2;
      if (slope->fast && muos_steppers.speed_flt > old_flt)
        {
          PRINTF("ENTER FAST");
          --speed;
          muos_steppers.state = MUOS_STEPPER_FAST;
        }
      else if (slope->pos == slope->end)
        {
          PRINTF("ENTER FASTOUT");
          speed = slope->speed_out;
          muos_steppers.state = MUOS_STEPPER_FASTOUT;
        }
      else
        ++slope->pos;

      break;
    }


  PRINT(speed);

  OCRA = speed;

  printf("%u %u %u %u\n", CLOCKP, muos_steppers.position,
         65536*16/(speed+config->max_speed), OCRA);

  return true;
}




void
set_config (void)
{
  config->accel = 4800;
  config->decel =  600;
  config->min_speed = 65000;
  config->max_speed = 1000;
  //config->max_slope = 32768;  //TODO: adjust max_speed by smallest point on max_slope
  config->max_slope = 1800;  //TODO: adjust max_speed by smallest point on max_slope
}



uint16_t speed_in = 65000;
uint16_t speed_out = 1500;
uint16_t out_steps = 1000;
int32_t position_end = 20000;


int
main(int argc, char* argv[])
{
  (void) argc; (void) argv;
  printf("Clock Position Speed\n");
  printf("# Test start...\n");
  set_config ();
  muos_steppers.max_speed = config->max_speed;  //TODO: initialize on max_slope smallest
  muos_steppers.position = 0;


  init(position_end, speed_in, config->max_speed, speed_out, out_steps);
  while (overflow_isr());

  printf("\n#...Test done\n");
  return 0;
}




