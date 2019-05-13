// gcc -g -Wall -Wextra -Werror -I../.. test_spriq.c && ./a.out



#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <inttypes.h>


/*

  Configuration

 */


#define MUOS_CLPQ_LENGTH 16
//#define MUOS_CLOCK_TYPE uint32_t
//#define PD "%d"
#define MUOS_CLOCK_TYPE uint64_t
#define PD "%ld"


#define MUOS_CLPQ_BARRIERS 1  // initial implementation simple barriers


/*

  Mockups

 */

volatile struct muos_status_flags muos_status;

volatile uint8_t fake_hw_clock1;
#define MUOS_CLOCK_HW 1
#define MUOS_HW_CLOCK_REGISTER(...) fake_hw_clock1

void
muos_interrupt_enable (void)
{}

void
muos_interrupt_disable (void)
{}


typedef MUOS_CLOCK_TYPE muos_clock;
typedef uint32_t muos_clock32;
typedef uint16_t muos_clock16;

muos_clock clock_coarse;


muos_clock
muos_clock_now (void)
{
  return clock_coarse + fake_hw_clock1;
}



muos_clock
muos_clock_now_isr (void)
{
  return muos_clock_now ();
}



muos_clock
muos_clock_elapsed (muos_clock now, muos_clock start)
{
  if (now > start)
    return now - start;
  else
    return start - now;
}

muos_clock32
muos_clock32_elapsed (muos_clock32 now, muos_clock32 start)
{
  if (now > start)
    return now - start;
  else
    return start - now;
}

muos_clock16
muos_clock16_elapsed (muos_clock16 now, muos_clock16 start)
{
  if (now > start)
    return now - start;
  else
    return start - now;
}



/*

  Tests here

*/



#include "muos/clpq.h"
#include "muos/clpq.c"

muos_error
muos_error_set_isr (muos_error err)
{
  return err;
}

extern muos_clpq_type muos_clpq;

void
clock_tick (void)
{
  if (!++fake_hw_clock1)
    clock_coarse += 256;

  //printf ("now: "PD"/"PD"\n", muos_clpq.now, muos_clpq.now%65536);
}


void
clpq_dumpp (const char* name);


void
hello (muos_clock16 delayed)
{
  printf("\thello %d\n", delayed);
}

void
world (muos_clock16 delayed)
{
  printf("\tworld %d\n", delayed);
}


#define BLINK 2510
void
blink (muos_clock16 delay)
{
  printf("\tblink %d\n", delay);

  muos_clpq_at (muos_clpq_now () + BLINK - delay, blink);
  clpq_dumpp ("blink rep");
}


#define PING 19990
void
ping (muos_clock16 delay)
{
  printf("\tping %d\n", delay);

  muos_clpq_at (muos_clpq_now () + PING - delay, ping);
  clpq_dumpp ("ping rep");
}


#define LIVE 50
void
live (muos_clock16 delay)
{
  printf("\tlive %d\n", delay);

  muos_clpq_at (muos_clpq_now () + LIVE - delay, live);
  clpq_dumpp ("live rep");
}


const char*
fname(muos_clpq_function fn)
{
  if (fn == NULL) return "NULL";
  if (fn == hello) return "hello";
  if (fn == world) return "world";
  if (fn == blink) return "blink";
  if (fn == ping) return "ping";
  if (fn == live) return "live";
  return "BARRIER";
}

void
clpq_dumpp (const char* name)
{
  printf("DUMP: %s: now "PD"/"PD": used %d:\n", name, muos_clpq.now, muos_clpq.now%65536, muos_clpq.used);
  //for (uint8_t i = 0; i < MUOS_CLPQ_LENGTH; ++i)
  for (uint8_t i = 0; i < muos_clpq.used; ++i)
    {
      printf("  %d: %d %p = %s\n", i, muos_clpq.entries[i].when, muos_clpq.entries[i].what, fname(muos_clpq.entries[i].what));
    }
  printf("---\n\n");
}


#define OK(call) do { printf("\n" #call ": "); if(call) printf("FAILED"); printf("\n\n"); } while (0)






int
main (int argc, char* argv[])
{
  (void) argc; (void) argv;
  printf("# Test start...\n\n");

  clpq_dumpp ("empty");


  //  muos_clpq_at (0, blink);
  muos_clpq_at (10, blink);
  muos_clpq_at (50, ping);
  muos_clpq_at (1150, live);
  clpq_dumpp ("blink");

  printf ("Clock ticking...\n");
  while (muos_clpq_now () < 16553500)
    {
      clock_tick ();
      //printf("s %d\n", muos_clpq_schedule_isr ());
      muos_clpq_schedule_isr ();
#if 0
      while (muos_clpq_schedule_isr ())
        {
          clpq_dumpp ("schedule");
        }
#endif
    }

  printf("\n#...Test done\n");
  return 0;
}










