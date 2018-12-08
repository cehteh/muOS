// gcc -Wall -Wextra -Werror stepper_accel_decel.c && ./a.out | quickplot -z

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



#define PRINT(var) printf("# %30s: %d\n", #var, var)


uint32_t CLOCK;    // clock ticks
uint16_t OCRA;     // pulse length

int32_t position;
int32_t position_end = 1500;

uint16_t min_speed = 60000;
uint16_t max_speed = 1000;

uint16_t accel = 200;
uint16_t decel = 255;

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

  printf("%u %u %u\n", 0, position, (655360)/(min_speed+1));
  uint32_t speed=OCRA;
  PRINT(speed);
}


bool
overflow_isr(void)
{
  CLOCK += OCRA;
  ++position;

  uint32_t speed = ((65536*256)/((position-start)*accel+256)
                    + (65536*256)/((end-position)*decel+256))
    + max_speed;


  PRINT(speed);
  printf("%u %u %u\n", CLOCK, position, (655360)/(speed+1));

  OCRA = speed<min_speed?speed:min_speed;

  if (position == position_end)
    return false;

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
