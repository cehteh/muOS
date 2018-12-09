// gcc -Wall -Wextra -Werror stepper_accel_decel.c && ./a.out | quickplot -z

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



#define PRINT(var) printf("# %30s: %d\n", #var, var)


uint32_t CLOCK;    // clock ticks
uint16_t OCRA;     // pulse length

int32_t position;
int32_t position_end = 1024;

const uint16_t min_speed = 65535;
const uint16_t max_speed = 1024;

//TODO: max_slope var, code for long movements
uint16_t max_slope = 65000;

const uint8_t accel = 15;
const uint8_t decel = 15;

int32_t start;
int32_t end;





void
init (void)
{
  OCRA = min_speed;

  start = position;
  end = position_end;

  PRINT(start);
  PRINT(end);

  printf("%u %u %u %u\n", 0, position, (65536*16)/(min_speed+1), min_speed);
  uint32_t speed=OCRA;
  PRINT(speed);
}




uint16_t
usqrt16 (uint16_t x)
{
  uint16_t i;
  for (i=1; i<256 && i*i<x; ++i);
  return i;
}

bool
overflow_isr(void)
{
  CLOCK += OCRA;
  ++position;
  if (position == position_end)
    return false;

  //TODO: init these as constants
  const uint32_t M  = (65535-max_speed)*256;
  //const uint16_t OFFSET = M/(min_speed-max_speed+1)+236;  // end = 4
  //const uint16_t OFFSET = M/(min_speed-max_speed+1)+210;  // end = 8
  //const uint16_t OFFSET = M/(min_speed-max_speed+1)+168;  // end = 16
  //const uint16_t OFFSET = M/(min_speed-max_speed+1)+18;  // end = 255
  // const uint16_t OFFSET = M/(min_speed-max_speed+1)+5;  // end = 1024
  //const uint16_t OFFSET = M/(min_speed-max_speed+1)+1;  // end = 8192
  const uint16_t OFFSET = M/(min_speed-max_speed);  // end = 8192

  uint32_t a = (position-start+1)*accel+OFFSET;
  uint32_t d = (end-position-1)*decel+OFFSET;
  uint32_t speed = M/a + M/d + max_speed;

  PRINT(speed);
  printf("%u %u %u %u\n", CLOCK, position, (uint32_t)((65536*16)/(speed+1)), speed);

  OCRA = speed;


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

  init();

  while (overflow_isr());

  printf("\n#...Test done\n");
  return 0;
}
