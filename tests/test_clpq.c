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

volatile uint8_t fake_hw_clock1;
#define MUOS_CLOCK_HW 1
#define MUOS_HW_CLOCK_REGISTER(...) fake_hw_clock1

void
muos_interrupt_enable (void)
{}

void
muos_interrupt_disable (void)
{}

#define MUOS_TRACE(flag, fmt,...) do {if (flag) {printf( "TRACE: "fmt"\n", ##__VA_ARGS__);}} while (0)

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


#define BLINK 1000
void
blink (void)
{
  printf("\tblink \n");

  muos_clpq_repeat (BLINK);
  clpq_dumpp ("blink rep");
}


#define PING 10000
void
ping (void)
{
  printf("\tping \n");

  muos_clpq_repeat (PING);
  clpq_dumpp ("ping rep");
}


#define LIVE 100000
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

  muos_clock now;
  muos_clock_now_isr (&now);

  printf ("DUMP: now "PD"/"PD"\n",
          muos_clock_clock32 (&now),
          muos_clock_clock16 (&now));

  uint64_t tstamp = muos_barray_uint32 (now.barray, 2);

  printf ("DUMP: %s: used %d:\n", name, muos_clpq.used);
  for (uint8_t i = muos_clpq.used; i; --i)
    {
      tstamp += clpq_barrier (muos_clpq.entries[i-1].what);
      printf("  %d: \t%ld \t%d %p = %s\n",
             i-1,
             tstamp*65536+muos_clpq.entries[i-1].when,
             muos_clpq.entries[i-1].when,
             muos_clpq.entries[i-1].what,
             fname(muos_clpq.entries[i-1].what));
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
  clpq_dumpp ("initialized");

  muos_clpq_after (100, ping);
  clpq_dumpp ("initialized");

  muos_clpq_after (500000, live);
  clpq_dumpp ("initialized");

  printf ("uniq test\n");

  muos_clock uniq;
  muos_clock_now_isr (&uniq);
  muos_clock_add16 (&uniq, 10000);

  muos_clpq_at (&uniq, NULL, true);
  clpq_dumpp ("uniq 1");

  muos_clpq_at (&uniq, NULL, true);
  clpq_dumpp ("uniq fail");

  muos_error ret;
  while ((ret = muos_clpq_at (&uniq, NULL, true)) == muos_error_clpq_nounique)
    muos_clock_add8 (&uniq, 1);
  clpq_dumpp ("uniq 2");

  printf ("Clock ticking...\n");

  muos_clock now;
  muos_clock_now_isr (&now);

  while (muos_clock_clock32 (&now) < 1655350000UL)
    {
      for (int i = rand()%100 + 1; i; --i)
        clock_tick ();

      while (muos_clpq_schedule_isr ());

      muos_clock_now_isr (&now);
    }

  printf("\n#...Test done\n");
  return 0;
}
