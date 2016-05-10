/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2016                            Christian Thäter <ct@pipapo.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

enum qerror
  {
    NO_ERROR,
    DIVISON_BY_ZERO,
  };


typedef int32_t rational_base_t;
typedef uint32_t urational_base_t;

typedef struct
{
  rational_base_t p;
  rational_base_t q;
}rational;

// global flag gets set when an operation becomes inexact
// constructors are never inexact
bool inexact;


// count trailing zero bits
static uint8_t
zbits (uint16_t a)
{
  if (a & 1)
    return 0;

  uint8_t r = 1;

  if (!(a & 0xff))
    {
      a >>= 8;
      r += 8;
    }

  if (!(a & 0xf))
    {
      a >>= 4;
      r += 4;
    }

  if (!(a & 0x3))
    {
      a >>= 2;
      r += 2;
    }

  return r - (a & 0x1);
}


// steins algorithm
static urational_base_t
ugcd (urational_base_t a, urational_base_t b)
{
  if (!a)
    return b;

  uint8_t k = zbits (a | b);

  a >>= zbits (a);

  while (b)
    {
      b >>= zbits (b);

      if (a > b)
        {
          urational_base_t t = a;
          a = b;
          b = t;
        }

      b -= a;
    };

  return a << k;
}


//TODO: test alternative
//#define QABS(q) (((q) + ((q) >> (sizeof(q)-1))) ^ ((q) >> (sizeof(q)-1)))
#define QABS(q) ((q)<0?-(q):(q))

// greatest common divisor
static rational_base_t
gcd (rational_base_t a, rational_base_t b)
{
  return ugcd (QABS(a), QABS(b));
}



// normalize a value
rational
qnorm (rational q)
{
  rational_base_t d = gcd (q.p, q.q);
  return (rational){q.p/d,q.q/d};
}



// equivalent form
static rational
qequiv (rational q)
{
  return ((rational){-q.p, -q.q});
}

#define QMAX 126

// optimize, partial normalization, only whats necessary don't loose exactness
rational
qopt (rational q)
{
  if (q.q < 0)
    q = qequiv (q);

  if (QABS (q.p) > QMAX || QABS(q.q) > QMAX)
    return qnorm (q);
  else
    return q;
}


// constructor
rational
Q (rational_base_t p, rational_base_t q)
{
  return qopt(((rational){p, q}));
}


// error constructor
rational
Qerror (enum qerror e)
{
  return (rational){e, 0};
}


rational
Qfloat (float f)
{
  float x = (float)INT16_MAX/f;

  if (x<INT16_MAX)
    return qopt(((rational){truncf(x)*f, truncf(x)}));
  else
    return qopt(((rational){1, truncf(x)/ (float)INT16_MAX}));
}

rational
Qint (rational_base_t i)
{
  return (rational){i, 1};
}


rational
qreduce (rational q, rational_base_t lim)
{
  rational_base_t m = q.p/lim > q.q/lim ? q.p/lim : q.q/lim;
  if (m>1)
    {
      inexact = true;
      q.p /= m;
      q.q /= m;
    }
  return qnorm (q);
}



// convert to fixed point integer
rational_base_t
qfixedpoint (rational v, rational_base_t q)
{
  return v.p*q/v.q;
}

// check and extract error
enum qerror
qerror (rational q)
{
  if (!q.q)
    return q.p;
  else
    return 0;
}


// return integer part (0 on error)
rational_base_t
qint (rational q)
{
  return q.q?q.p/q.q:0;
}


// convert to float
float
qfloat (rational q)
{
  return (float)q.p/(float)q.q;
}


// check for equality
bool
qequal (rational a, rational b)
{
  
  return a.p * b.q == b.p * a.q;
}


// compare two rationals
int8_t
qcmp (rational a, rational b)
{
  rational_base_t r = a.p * b.q - b.p * a.q;
  return r<0?-1:r>0?1:0;
}

// a+b
rational
qadd (rational a, rational b)
{
  return Q (a.p*b.q + a.q*b.p, a.q*b.q);
}

// a-b
rational
qsub (rational a, rational b)
{
  return Q (a.p*b.q - a.q*b.p, a.q*b.q);
}

// a*b
rational
qmul (rational a, rational b)
{
  return Q (a.p*b.p, a.q*b.q);
}

// a/b
rational
qdiv (rational a, rational b)
{
  return Q (a.p*b.q, a.q*b.p);
}

// -a
rational
qneg (rational a)
{
  return Q (-a.p, a.q);
}

// 1/a
rational
qinv (rational a)
{
  if (!a.p)
    return Qerror (DIVISON_BY_ZERO);

  return Q (a.q, a.p);
}





#ifdef TEST

// test with:
//  gcc -DTEST rationals.c && ./a.out

#include <stdio.h>

int
main()
{
  printf ("rationals lib test\n");
  printf ("BEGIN\n");

  rational pi = Q (355, 113);  // approx

  printf ("fp    pi = %d\n", qfixedpoint (pi, 10000));
  printf ("float pi = %f\n", qfloat (pi));
  printf ("int   pi = %d \n", qint (pi));

  printf ("float   pi = %f \n", qfloat (Q(1110,1000)));

  printf ("float   pi = %f \n", qfloat (Q(1111,1000)));
  printf ("float   pi = %f \n", qfloat (Q(11,10)));


  rational qq = Qfloat(sqrt(2));

  printf ("float   pi = %f \n", qfloat (qmul(qq,qq)));
  printf (" p = %d\n", qq.p);
  printf (" q = %d\n", qq.q);

  //qq = qreduce (qq, 200, 200);
  qq = qreduce (qq, 16385);
  printf ("float   pi = %f \n", qfloat (qq));
  printf (" p = %d\n", qq.p);
  printf (" q = %d\n", qq.q);

  printf ("%f \n", qfloat(qmul(qq,qq)));
  
  printf ("DONE\n");
  return 0;
}


#endif
