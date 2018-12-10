// gcc -Wall -Wextra -Werror stepper_accel_decel.c && ./a.out | quickplot -z

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



#define PRINT(var) printf("# %30s: %d\n", #var, var)



// configuration

const uint8_t accel = 100;
const uint8_t decel = 150;

const uint16_t min_speed = 65000;
const uint16_t max_speed = 1000;


// state

int32_t position;
//int32_t position_end = 20000;
int32_t accel_end;
int32_t decel_start;
int32_t start;
int32_t end;
uint16_t ad;
uint16_t slope;

enum states
  {
   ACCEL,
   CONSTANT,
   DECEL
  };

enum states state;


// sim
uint32_t CLOCK;    // clock ticks
uint16_t OCRA;     // pulse length


/*TODO:

      optimize 32 / 16bit division

 */


void
init (int32_t togo)
{
  OCRA = min_speed;
  state = ACCEL;

  int32_t len;

  slope = (accel+decel)*64;
  ad = (accel+decel)*2048/slope;

  start = position;
  end = togo;
  len = end-start;


  if (len <= slope)
    {
      accel_end = (len*accel) / (accel+decel);
      decel_start = accel_end;
    }
  else
    {
      accel_end = (slope*accel) / (accel+decel);
      decel_start = accel_end+len-slope;
    }

  PRINT(slope);
  PRINT(start);
  PRINT(end);
  PRINT(accel_end);
  PRINT(decel_start);
  PRINT(ad);

  printf("%u %u %u %u\n", 0, position, (65536*16)/(min_speed-1), min_speed);
  uint32_t speed=OCRA;
  PRINT(speed);
}



bool
overflow_isr(void)
{
  CLOCK += OCRA;
  ++position;
  if (position == start+end)
    return false;

  if (position == accel_end)
    {
      printf("%u NAN NAN NAN 0\n", CLOCK);
      printf("%u NAN NAN NAN 1.0\n", CLOCK);
      state= CONSTANT;
    }

  if (position == decel_start)
    {
      printf("%u NAN NAN NAN 0\n", CLOCK);
      printf("%u NAN NAN NAN 1.0\n", CLOCK);
      state= DECEL;
    }

  uint16_t speed;

  switch (state)
    {
    case CONSTANT:
      goto done;
    case ACCEL:
      speed = (accel*(min_speed-max_speed))/((position-start)*32+accel);
      break;
    case DECEL:
      speed = (decel*(min_speed-max_speed))/((end-position-1)*32+decel);
    }

  speed += max_speed-ad;

  // safety only triggered by rounding errors
  if (speed < max_speed)
    speed = max_speed;

  PRINT(speed);

  OCRA = speed;
 done:
  printf("%u %u %u %u\n", CLOCK, position, (uint32_t)((65536*16)/(OCRA+1)), OCRA);

  return true;
}

#if 0
// linear for test
void
init (void)
{
  OCRA = 8192;
}

bool
overflow_isr(void)
{
  CLOCK += OCRA;
  ++position;

  printf("%u %u %u\n", CLOCK, position, 65536/OCRA);

  if (position == position_end)
    return false;

  return true;
}
#endif


int
main(int argc, char* argv[])
{
  (void) argc; (void) argv;
  printf("Clock Position Speed\n");
  printf("# Test start...\n");

  init(25000);

  while (overflow_isr());

  printf("\n#...Test done\n");
  return 0;
}

