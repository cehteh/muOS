// gcc -g -Wall -Wextra -Werror stepper_trapez.c && ./a.out | quickplot -z &

/*

Description
-----------


  The different cases for the slope:


  1. Reaches 'max_speed'

           _____
          /     \
         /       \
        /         \
       /           \
      /             \______
     /
    /



  2. Stays below 'max_speed', has a deceleration phase

       /\
      /  \______
     /
    /


     \    /
      \  /
       \/
       /\
      /  \______
     /
    /



  3. Only accelerate to out_speed

         ______
        /


   \          /
    \        /
     \      /
      \    /
       \  /
        \/______
        /



cases:

1. full slope, full speed
2. full slope, reduced speed

3. sort slope

6. accel only slope


8. decel slope



//FIXME: Error when accel to speed_out isnt possible


 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include <inttypes.h>

#define PRINT(var) printf("# %40s: %d\n", #var, var)
#define PRINTF(fmt, ...) printf("# %.40s " fmt ":\n", #__VA_ARGS__,  ## __VA_ARGS__)

// configuration

#define MUOS_STEPPER_SLOPE_SHIFT 2

struct stepper_config
{
  uint16_t max_speed;
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
  };

#define MUOS_STEPPER_POSITION_SLOTS 4

//TOOD: check and document bounds
//TODO: split into prepared/cached and runtime state

struct stepper_slope
{
  int32_t  dest;           // absolute end position

  int32_t decel_start;      // -countdown for starting decel
  uint16_t max_speed;
  uint16_t speed_in;
  uint16_t speed_out;
  uint8_t decel_steps;
};

static uint16_t stepper_slope_soffset; // = config->max_speed>>MUOS_STEPPER_SLOPE_SHIFT;


struct stepper_state
{
  enum muos_stepper_arming_state state;
  enum muos_stepper_arming_state before_raw;
  volatile int32_t position;

  uint16_t max_speed;
  struct stepper_slope slope;  //TODO: doublebuffer
};

struct stepper_state muos_steppers;

struct stepper_slope* slope = &muos_steppers.slope;


// sim
uint32_t CLOCK;    // clock ticks
uint16_t OCRA;     // pulse length



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


static uint8_t
ilogs (uint16_t i)
{
  uint8_t ret = 0;
  for (; i > (1<<MUOS_STEPPER_SLOPE_SHIFT)-1 && ret < 255; ++ret)
    {
      i -= i>>MUOS_STEPPER_SLOPE_SHIFT;
    }
  return ret;
}





bool
init (int32_t dest, uint16_t speed_in, uint16_t max_speed, uint16_t speed_out, uint16_t steps_out)
{
  OCRA = speed_in;
  muos_steppers.state = MUOS_STEPPER_SLOPE;

  slope->dest = dest;

  slope->max_speed = max_speed;
  slope->speed_in = speed_in;
  slope->speed_out = speed_out;

  //PLANNED: slope->steps_out .. add back, compute decel_start based on steps out

  stepper_slope_soffset = config->max_speed>>MUOS_STEPPER_SLOPE_SHIFT;

  uint32_t slope_len = dest - steps_out;

  uint8_t accel_steps = ilogs (speed_in - max_speed);

  slope->decel_steps = ilogs (speed_out - max_speed) + (4 - MUOS_STEPPER_SLOPE_SHIFT);

  slope->decel_start = -(slope_len - slope->decel_steps - 1);

  if (accel_steps + slope->decel_steps > slope_len)
    {
      uint8_t xover = accel_steps + slope->decel_steps - slope_len;
      slope->decel_steps -= (xover)/2+1;
      slope->decel_start -= (xover+1)/2;

      //FIXME: Error when accel to speed_out isnt possible
    }


  return true;
}

#define CLOCKP (CLOCK/1000)


bool
overflow_isr(void)
{
  uint16_t speed = OCRA;
  CLOCK += speed;
  ++muos_steppers.position;

  if (muos_steppers.position == slope->dest)
    {
      OCRA = slope->speed_out;
      printf("%u %u %u %u\n", CLOCKP, muos_steppers.position, 65536*16/(speed+config->max_speed+1), OCRA);
      //printf("%u %u %u %u\n", muos_steppers.position, CLOCKP, (uint32_t)((65536*16)/(OCRA+1)), OCRA);
      return false;
    }


  switch (muos_steppers.state)
    {
    default:
      return false;
      //case MUOS_STEPPER_SLOW:   //TODO: set slow whenever running constant <minspeed
    case MUOS_STEPPER_FAST:
      PRINTF("FAST");
      return true;
    case MUOS_STEPPER_SLOPE:

      if (++slope->decel_start < 0)
        {
          speed -= speed>>MUOS_STEPPER_SLOPE_SHIFT;
          speed += stepper_slope_soffset;

          if (speed < slope->max_speed)
            speed = slope->max_speed;
        }
      else if (slope->decel_start <= slope->decel_steps)
        {
          speed += (speed - config->max_speed + (1<<MUOS_STEPPER_SLOPE_SHIFT))>>MUOS_STEPPER_SLOPE_SHIFT;
          if (speed > slope->speed_out)
            speed = slope->speed_out;
        }
      else
        {
          speed = slope->speed_out;
        }

      break;
    }


  PRINT(speed);

  OCRA = speed;

  printf("%u %u %u %u\n", CLOCKP, muos_steppers.position, 65536*16/(speed+config->max_speed+1), OCRA);
  //printf("%u %u %u %u\n", muos_steppers.position, CLOCKP, 65536*16/(speed+config->max_speed+1), OCRA);

  return true;
}




void
set_config (void)
{

  config->max_speed = 1500;
}



uint16_t speed_in = 64000;
uint16_t speed_out = 8000;
uint16_t out_steps = 3;
int32_t position_end = 70;


int
main(int argc, char* argv[])
{
  (void) argc; (void) argv;
  printf("Clock Position Speed\n");
  printf("# Test start...\n");
  set_config ();
  muos_steppers.max_speed = config->max_speed;  //TODO: initialize on max_slope smallest
  muos_steppers.position = 0;

  if (argc > 1)
    position_end = atoi(argv[1]);

  if (init(position_end, speed_in, config->max_speed, speed_out, out_steps))
    while (overflow_isr());

  printf("\n#...Test done\n");
  return 0;
}




