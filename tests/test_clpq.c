// gcc -g -Wall -Wextra -Werror -I../.. test_clpq.c && ./a.out



#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <inttypes.h>


/*

  Configuration

 */


#define MUOS_CLPQ_LENGTH 16
#define MUOS_CLOCK_SIZE 8
#define PD "%d"


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

#include <muos/error.h>



#include "muos/lib/clock.h"


muos_clock clock_coarse;



void
muos_clock_now_isr (muos_clock* now)
{
  muos_clock_copy (now, &clock_coarse);
  muos_clock_add8 (now, fake_hw_clock1);
}


muos_error
muos_error_set_isr (muos_error err)
{
  return err;
}


void
clock_tick (void)
{
  if (!++fake_hw_clock1)
    muos_clock_add16 (&clock_coarse, 256);

  muos_clock now;
  muos_clock_now_isr (&now);

  printf ("now: "PD"/"PD"\n",
          muos_clock_clock32 (&now),
          muos_clock_clock16 (&now));
}


void
clpq_dumpp (const char* name);



/*

  Tests here

*/



#include "muos/clpq.c"

void
hello (void)
{
  printf("\thello \n");
}

void
world (void)
{
  printf("\tworld \n");
}


#define BLINK 2510
void
blink (void)
{
  printf("\tblink \n");

  muos_clpq_repeat (BLINK);
  clpq_dumpp ("blink rep");
}


#define PING 19990
void
ping (void)
{
  printf("\tping \n");

  muos_clpq_repeat (PING);
  clpq_dumpp ("ping rep");
}


#define LIVE 50
void
live (void)
{
  printf("\tlive \n");

  muos_clpq_repeat (LIVE);
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
  (void) name;
  printf ("DUMP: %s: used %d:\n", name, muos_clpq.used);
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


  muos_clpq_after (10, blink);
  muos_clpq_after (100, blink);
  muos_clpq_after (50, ping);
  muos_clpq_after (1150, live);
  clpq_dumpp ("initialized");

  printf ("Clock ticking...\n");


  muos_clock now;
  muos_clock_now_isr (&now);

  while (muos_clock_clock32 (&now) < 1655350)
    {
      clock_tick ();

      while (muos_clpq_schedule_isr ())
        {
          clpq_dumpp ("schedule");
        }



      muos_clock_now_isr (&now);
    }

  printf("\n#...Test done\n");
  return 0;
}







#if 0 //ATTIC
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


#endif
