// gcc -g -I ../.. -Wall -Wextra -Werror test_spriq.c && ./a.out


#define MUOS_SPRIQ_TYPE uint16_t
#define MUOS_SPRIQ_INDEX uint16_t

#define MUOS_SPRIQ_TESTLEN 16

#define PRINT(var) printf("# %40s: %d\n", #var, var)
#define PRINTF(fmt, ...) printf("# %40s: " fmt "\n", #__VA_ARGS__,  ## __VA_ARGS__)


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <inttypes.h>

#include <muos/lib/spriq.c>



typedef struct
{
  struct muos_spriq descriptor;
  struct muos_spriq_entry spriq[MUOS_SPRIQ_TESTLEN];
} muos_test_type;
muos_test_type spriq_test;




void
muos_spriq_pop (struct muos_spriq* spriq);

bool
muos_spriq_remove (struct muos_spriq* spriq, muos_spriq_priority base, muos_spriq_priority when, muos_spriq_function fn);


#define BASE 32000


/// 10 20 30 32 40 50 60
/// 10 20 30 32 40 50 60
///          32 40 50 60 10 20 30 
///          32 40 50 60 10 20 30 


void
dump (void)
{
  PRINT(spriq_test.descriptor.used);
  for (uint16_t i = 0; i < spriq_test.descriptor.used; ++i)
    {
      //      PRINT(i);
      PRINT(spriq_test.spriq[i].when);
      PRINTF("     %p",spriq_test.spriq[i].fn);
    }
  printf("\n");
}



int
main(int argc, char* argv[])
{
  (void) argc; (void) argv;
  printf("# Test start...\n");

  
  //muos_spriq_push (&spriq_test.descriptor, BASE, 0, (muos_spriq_function)0);
  muos_spriq_push (&spriq_test.descriptor, 0, BASE, (muos_spriq_function)0);
  dump();
  muos_spriq_push (&spriq_test.descriptor, BASE, 10000, (muos_spriq_function)1);
  dump();

  muos_spriq_push (&spriq_test.descriptor, BASE, 20000, (muos_spriq_function)2);
  dump();
  muos_spriq_push (&spriq_test.descriptor, BASE, 30000, (muos_spriq_function)3);
  dump();
  muos_spriq_push (&spriq_test.descriptor, BASE, 40000, (muos_spriq_function)4);
  dump();
#if 1
  muos_spriq_push (&spriq_test.descriptor, BASE, 50000, (muos_spriq_function)5);
  dump();
  muos_spriq_push (&spriq_test.descriptor, BASE, 60000, (muos_spriq_function)6);
  dump();
#endif
#if 1
  muos_spriq_push (&spriq_test.descriptor, BASE, 0, (muos_spriq_function)0x32);
  dump();
  muos_spriq_push (&spriq_test.descriptor, BASE, 65535, (muos_spriq_function)0x319);
  dump();
#endif
  muos_spriq_push (&spriq_test.descriptor, BASE, 1, (muos_spriq_function)0x321);
  dump();
  muos_spriq_push (&spriq_test.descriptor, BASE, 60000, (muos_spriq_function)7);
  dump();


  PRINTF("REMOVE");

  muos_spriq_remove (&spriq_test.descriptor, BASE, 60000, (muos_spriq_function)6);
  dump();



  
  PRINTF("POP");

  while (spriq_test.descriptor.used)
    {
      muos_spriq_pop (&spriq_test.descriptor);
      dump();
    }

  muos_spriq_pop (&spriq_test.descriptor);
  dump();
      
  
  printf("# ... done\n");
  return 0;
}




