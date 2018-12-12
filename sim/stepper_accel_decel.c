// gcc -Wall -Wextra -Werror stepper_accel_decel.c && ./a.out | quickplot -z

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


#define PRINT(var) printf("# %30s: %d\n", #var, var)

// configuration

const uint16_t accel = 4500;
const uint16_t decel = 4500;

const uint16_t min_speed = 65000;
uint16_t max_speed = 1000;

uint16_t speed_in = 65000;
uint16_t speed_out=10000;

int32_t position_end = 1000;

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
   MUOS_STEPPER_ACCEL,
   MUOS_STEPPER_FAST,
   MUOS_STEPPER_DECEL,
  };

#define MUOS_STEPPER_POSITION_SLOTS 4

struct stepper_state
{
  enum muos_stepper_arming_state state;
  enum muos_stepper_arming_state before_raw;
  int32_t position;
  int32_t start;
  int32_t end;
  int32_t accel_end;
  int32_t mid;
  int32_t decel_start;
  uint16_t accel_offset; //FIXME: bounds checking against integer overflows
  uint16_t decel_offset;
  uint16_t decel_length;
  uint16_t accel_max;
  uint16_t decel_max;
};

struct stepper_state muos_stepper;



// sim
uint32_t CLOCK;    // clock ticks
uint16_t OCRA;     // pulse length


/*TODO:

      optimize 32 / 16bit division

 */

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

void
init (int32_t dest, uint16_t start_speed, uint16_t max_speed, uint16_t end_speed)
{
  OCRA = speed_in;
  muos_stepper.state = MUOS_STEPPER_ACCEL;

  muos_stepper.start = muos_stepper.position;
  muos_stepper.end = dest;

  uint32_t len;
  if (muos_stepper.end > muos_stepper.start)
    len = muos_stepper.end - muos_stepper.start;
  else
    len = muos_stepper.start - muos_stepper.end;

  (void) len;
  (void) start_speed;
  (void) max_speed;
  (void) end_speed;

  //accel

  // vertex to max_speed
  muos_stepper.accel_max = max_speed - isqrt(16*accel);
  PRINT(muos_stepper.accel_max);

  // find start offset
  muos_stepper.accel_offset =
    -16*accel/(muos_stepper.accel_max - speed_in);
  PRINT(muos_stepper.accel_offset);

  muos_stepper.accel_end =
    isqrt(16*accel) - muos_stepper.accel_offset;
  PRINT(muos_stepper.accel_end);

  //decel
  muos_stepper.decel_max = max_speed - isqrt(16*decel);
  PRINT(muos_stepper.decel_max);

  muos_stepper.decel_offset =
    -16*decel/(muos_stepper.decel_max - speed_out);
  PRINT(muos_stepper.decel_offset);

  muos_stepper.decel_start =
    len - isqrt(16*decel) +  muos_stepper.decel_offset;
  PRINT(muos_stepper.decel_start);

  printf("%u %u %u %u\n", 0, muos_stepper.position, (65536*16)/(speed_in-1), speed_in);
  uint32_t speed=OCRA;
  PRINT(speed);
}


#define CLOCKP (CLOCK/1000)

bool
overflow_isr(void)
{
  CLOCK += OCRA;
  ++muos_stepper.position;
  if (muos_stepper.position == muos_stepper.start+muos_stepper.end)
    {
      OCRA = speed_out;
      printf("%u %u %u %u\n", CLOCKP, muos_stepper.position, (uint32_t)((65536*16)/(OCRA+1)), OCRA);
      return false;
    }

  if (muos_stepper.position == muos_stepper.accel_end)
    {
      printf("%u NAN NAN NAN 0\n", CLOCKP);
      printf("%u NAN NAN NAN 1.0\n", CLOCKP);
      muos_stepper.state= MUOS_STEPPER_FAST;
    }

  if (muos_stepper.position == muos_stepper.decel_start)
    {
      printf("%u NAN NAN NAN 0\n", CLOCKP);
      printf("%u NAN NAN NAN 1.0\n", CLOCKP);
      muos_stepper.state= MUOS_STEPPER_DECEL;
    }

  uint32_t speed;

  switch (muos_stepper.state)
    {
    default:
      return false;
    case MUOS_STEPPER_FAST:
      goto done;
    case MUOS_STEPPER_ACCEL:
      speed = 16*accel/(muos_stepper.position+muos_stepper.accel_offset) + muos_stepper.accel_max;
      
      break;
    case MUOS_STEPPER_DECEL:
      speed = 16*decel/
        ( muos_stepper.end -
        muos_stepper.position + muos_stepper.decel_offset)
        + muos_stepper.decel_max;


      break;
    }


  PRINT(speed);

  OCRA = speed;
 done:
  printf("%u %u %u %u\n", CLOCKP, muos_stepper.position, (uint32_t)((65536*16)/(OCRA+1)), OCRA);

  return true;
}




int
main(int argc, char* argv[])
{
  (void) argc; (void) argv;
  printf("Clock Position Speed\n");
  printf("# Test start...\n");

  muos_stepper.position = 0;

  
  init(position_end, speed_in, max_speed, speed_out);
  while (overflow_isr());

  printf("\n#...Test done\n");
  return 0;
}

