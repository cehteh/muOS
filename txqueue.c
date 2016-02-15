/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2015                            Christian Thäter <ct@pipapo.org>
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

#include <string.h>

#include <muos/muos.h>
#include <muos/serial.h>
#include <muos/txqueue.h>

#include <muos/bgq.h>

#include <muos/io.h>

#if MUOS_SERIAL_TXQUEUE > 1

//static struct fmtconfig_type txq_pfmtconfig = {10, 0, 0, 15, 15};
static struct fmtconfig_type txq_fmtconfig = {10, 0, 0, 15, 15};


muos_txqueue_type muos_txqueue;


// Format:
//  The queue is a stream of uint8_t values.
//  0-127 are printed as verbatim ASCII values
//  128-191 the following 1-64 characters are printed verbatim,
//          including values bigger than 127
//  192-255 are tags followed few byte parameters binary data
//	    each tag is associated with a function which interprets
//	    this data. Printing a string from PROGMEM only takes
//          3 bytes on the queue for example.
//

//NOTE: TXQUEUE never runs in interrupts, no locking needed
//PLANNED: store index of the last tag, can be used to patch up the tag and append data

void
muos_txqueue_start (void)
{
  if (!muos_status.txqueue_pending)
    {
      muos_status.txqueue_pending = true;
      muos_bgq_pushback (muos_txqueue_run);
    }
}



muos_cbuffer_index
muos_txqueue_free (void)
{
  return MUOS_CBUFFER_FREE (muos_txqueue);
}

void
muos_txqueue_push (const uint8_t value)
{
  MUOS_CBUFFER_PUSH(muos_txqueue, value);
}

#define TAG(name)  static void txqueue_##name (void);
MUOS_TXQUEUE_TAGS
#undef TAG


void
muos_txqueue_run (void)
{
  while (MUOS_CBUFFER_USED (muos_txqueue) &&  MUOS_CBUFFER_FREE (muos_txbuffer))
    {
      uint8_t tag = MUOS_CBUFFER_PEEK (muos_txqueue, 0);
      if (tag < MUOS_TXTAG_NCHARS)
        {
          muos_serial_tx_byte (tag);
          MUOS_CBUFFER_POPN (muos_txqueue, 1);
        }
      else if (tag <= MUOS_TXTAG_NCHARS_END)
        {
          for (muos_cbuffer_index i = 1; i <= tag-MUOS_TXTAG_NCHARS+1; ++i)
            {
              muos_serial_tx_byte (MUOS_CBUFFER_PEEK (muos_txqueue, i));
              if (muos_error_check (muos_error_tx_buffer_overflow))
                {
                  MUOS_CBUFFER_POKE (muos_txqueue, i-1, tag-i);

                  MUOS_CBUFFER_POPN (muos_txqueue, tag-i-MUOS_TXTAG_NCHARS+2);

                  goto out;
                }
            }
          MUOS_CBUFFER_POPN (muos_txqueue, tag-MUOS_TXTAG_NCHARS+2);
        out:;
        }
      else switch (tag)
        {
#define TAG(name)  case MUOS_TXTAG_##name: txqueue_##name (); break;
          MUOS_TXQUEUE_TAGS
#undef TAG
        }
    }

  if (MUOS_CBUFFER_USED (muos_txqueue))
    {
      muos_bgq_pushback (muos_txqueue_run);
    }
  else
    {
      muos_status.txqueue_pending = false;
    }
}

void txqueue_NL (void)
{
  muos_serial_tx_byte ('\r');
  MUOS_CBUFFER_POKE (muos_txqueue, 0, '\n');
}

static uint8_t muos_xput_state = 0;

#define Xput(bits)                                              \
static uint##bits##_t                                           \
u##bits##put (uint##bits##_t v, uint8_t base, bool upcase)      \
{                                                               \
  if (!muos_xput_state)                                         \
    {                                                           \
      for (uint##bits##_t tmp=v; tmp; tmp /= base)              \
        ++muos_xput_state;                                      \
    }                                                           \
                                                                \
  uint##bits##_t start = 1;                                     \
                                                                \
  for (uint##bits##_t i = muos_xput_state-1; i; --i)            \
    {                                                           \
      start *= base;                                            \
    }                                                           \
                                                                \
  while (start)                                                 \
    {                                                           \
      uint##bits##_t r = v/start;                               \
      muos_serial_tx_byte ((r<10?'0':upcase?'A'-10:'a'-10)+r);  \
      if (muos_error_check (muos_error_tx_buffer_overflow))     \
        return v;                                               \
      v -= r*start;                                             \
      start /= base;                                            \
      --muos_xput_state;                                        \
    }                                                           \
  return 0;                                                     \
}


Xput(8);
Xput(16);
Xput(32);
//Xput(64); //PLANNED: 64 bit support

#undef Xput

void txqueue_UINT8 (void)
{
  uint8_t value = u8put (MUOS_CBUFFER_PEEK (muos_txqueue, 1), txq_fmtconfig.base, txq_fmtconfig.upcase);

  if (muos_xput_state)
    {
      MUOS_CBUFFER_POKE (muos_txqueue, 1, value);
    }
  else
    {
      MUOS_CBUFFER_POPN (muos_txqueue, 2);
    }
}

void txqueue_UINT16 (void)
{
  uint16_t value;
  ((uint8_t*)&value)[1] = MUOS_CBUFFER_PEEK (muos_txqueue, 1);
  ((uint8_t*)&value)[0] = MUOS_CBUFFER_PEEK (muos_txqueue, 2);

  value = u16put (value, txq_fmtconfig.base, txq_fmtconfig.upcase);

  if (muos_xput_state)
    {
      MUOS_CBUFFER_POKE (muos_txqueue, 1, ((uint8_t*)&value)[1]);
      MUOS_CBUFFER_POKE (muos_txqueue, 2, ((uint8_t*)&value)[0]);
    }
  else
    {
      MUOS_CBUFFER_POPN (muos_txqueue, 3);
      //TODO: reset fmtconfig
    }
}

void txqueue_UINT32 (void)
{
  uint32_t value;
  ((uint8_t*)&value)[3] = MUOS_CBUFFER_PEEK (muos_txqueue, 1);
  ((uint8_t*)&value)[2] = MUOS_CBUFFER_PEEK (muos_txqueue, 2);
  ((uint8_t*)&value)[1] = MUOS_CBUFFER_PEEK (muos_txqueue, 3);
  ((uint8_t*)&value)[0] = MUOS_CBUFFER_PEEK (muos_txqueue, 4);

  value = u32put (value, txq_fmtconfig.base, txq_fmtconfig.upcase);

  if (muos_xput_state)
    {
      MUOS_CBUFFER_POKE (muos_txqueue, 1, ((uint8_t*)&value)[3]);
      MUOS_CBUFFER_POKE (muos_txqueue, 2, ((uint8_t*)&value)[2]);
      MUOS_CBUFFER_POKE (muos_txqueue, 3, ((uint8_t*)&value)[1]);
      MUOS_CBUFFER_POKE (muos_txqueue, 4, ((uint8_t*)&value)[0]);
    }
  else
    {
      MUOS_CBUFFER_POPN (muos_txqueue, 5);
      //TODO: reset fmtconfig
    }
}




void
muos_txqueue_output_char (char c)
{
  if ((uint8_t)c < 128)
    {
      if (muos_txqueue_free () >= 1)
        {
          muos_txqueue_push (c);
        }
      else
        {
          muos_error_set (muos_error_txqueue_overflow);
          return;
        }
    }
  else
    {
      if (muos_txqueue_free () >= 2)
        {
          muos_txqueue_push (MUOS_TXTAG_NCHARS);
          muos_txqueue_push (c);
        }
      else
        {
          muos_error_set (muos_error_txqueue_overflow);
          return;
        }
    }
  muos_txqueue_start ();
}

void
muos_txqueue_output_repeat_char (uint8_t rep, char c)
{
  (void) rep;
  (void) c;
}


void
muos_txqueue_output_cstr (const char* str)
{
  //FIXME: check error generation, something fishy here with small tx buffer
  if (muos_txqueue_free () < strlen (str) + 1)
    {
      muos_error_set (muos_error_txqueue_overflow);
      return;
    }

  for(; *str; ++str)
    {
      if ((uint8_t)*str < 128)
        {
          muos_txqueue_push (*str);
        }
      else
        {
          muos_txqueue_push (MUOS_TXTAG_NCHARS + strlen (str) - 1);

          for(; *str; ++str)
            {
              muos_txqueue_push (*str);
            }
        }
    }

  muos_txqueue_start ();
}


void
muos_txqueue_output_repeat_cstr (uint8_t rep, const char* str)
{
  (void) rep;
  (void) str;
}


void
muos_txqueue_output_mem (const uint8_t* mem, uint8_t len)
{
  (void) mem;
  (void) len;

}


void
muos_txqueue_output_nl (void)
{
  if (!muos_txqueue_free ())
    {
      muos_error_set (muos_error_txqueue_overflow);
      return;
    }

  muos_txqueue_push (MUOS_TXTAG_NL);
  muos_txqueue_start ();
}

void
muos_txqueue_output_csi_char (const char c)
{
  (void) c;
}


void
muos_txqueue_output_csi_cstr (const char* str)
{
  (void) str;
}




static void
txqueue_uint8 (uint8_t n)
{
  if (muos_txqueue_free () >= 2)
    {
      muos_txqueue_push (MUOS_TXTAG_UINT8);
      muos_txqueue_push (n);
    }
  else
    {
      muos_error_set (muos_error_txqueue_overflow);
    }
}

static void
txqueue_uint16 (uint16_t n)
{
  if (muos_txqueue_free () >= 3)
    {
      muos_txqueue_push (MUOS_TXTAG_UINT16);
      muos_txqueue_push (((uint8_t*)&n)[1]);
      muos_txqueue_push (((uint8_t*)&n)[0]);
    }
  else
    {
      muos_error_set (muos_error_txqueue_overflow);
    }
}

static void
txqueue_uint32 (uint32_t n)
{
  if (muos_txqueue_free () >= 5)
    {
      muos_txqueue_push (MUOS_TXTAG_UINT32);
      muos_txqueue_push (((uint8_t*)&n)[3]);
      muos_txqueue_push (((uint8_t*)&n)[2]);
      muos_txqueue_push (((uint8_t*)&n)[1]);
      muos_txqueue_push (((uint8_t*)&n)[0]);
    }
  else
    {
      muos_error_set (muos_error_txqueue_overflow);
    }
}





void
muos_txqueue_output_intptr (intptr_t n)
{
  (void) n;
}

void
muos_txqueue_output_uintptr (uintptr_t n)
{
  (void) n;
}

void
muos_txqueue_output_int8 (int8_t n)
{
  (void) n;
}

void
muos_txqueue_output_uint8 (uint8_t n)
{
  if (n <= fmtconfig.base)
    {
      muos_txqueue_output_char ((((char)n<10?'0':fmtconfig.upcase?'A'-10:'a'-10))+(char)n);
    }
  else
    {
      txqueue_uint8 (n);
    }
}

void
muos_txqueue_output_int16 (uint16_t n)
{
  (void) n;
}

void
muos_txqueue_output_uint16 (uint16_t n)
{
  if (n <= fmtconfig.base)
    {
      muos_txqueue_output_char ((((char)n<10?'0':fmtconfig.upcase?'A'-10:'a'-10))+(char)n);
    }
  else if (n <= 0xff)
    {
      txqueue_uint8 (n);
    }
  else
    {
      txqueue_uint16 (n);
    }

  muos_txqueue_start ();
}

void
muos_txqueue_output_int32 (int32_t n)
{
  (void) n;
}


void
muos_txqueue_output_uint32 (uint32_t n)
{
  if (n <= fmtconfig.base)
    {
      muos_txqueue_output_char ((((char)n<10?'0':fmtconfig.upcase?'A'-10:'a'-10))+(char)n);
    }
  else if (n <= 0xff)
    {
      txqueue_uint8 (n);
    }
  else if (n <= 0xffff)
    {
      txqueue_uint16 (n);
    }
  else
    {
      txqueue_uint32 (n);
    }

  muos_txqueue_start ();
}

#if 0
void
muos_txqueue_output_int64 (int64_t n)
{
  (void) n;
}

void
muos_txqueue_output_uint64 (uint64_t n)
{
  (void) n;
}

void
muos_txqueue_output_float (float)
{
  (void) str;
}


void
muos_txqueue_output_cstr_R ()
{
  (void) str;
}

void
muos_txqueue_output_mem_R ()
{
  (void) str;
}

void
muos_txqueue_output_int16_R ()
{
  (void) str;
}

void
muos_txqueue_output_uint16_R ()
{
  (void) str;
}

void
muos_txqueue_output_int32_R ()
{
  (void) str;
}

void
muos_txqueue_output_uint32_R ()
{
  (void) str;

}

void
muos_txqueue_output_int64_R ()
{
  (void) str;

}

void
muos_txqueue_output_uint64_R ()
{
  (void) str;

}

void
muos_txqueue_output_float_R ()
{
  (void) str;

}
#endif

void
muos_txqueue_output_upcase (bool upcase)
{
  (void) upcase;
}


void
muos_txqueue_output_base (uint8_t base)
{
  (void) base;
}


void
muos_txqueue_output_pupcase (bool upcase)
{
  (void) upcase;
}


void
muos_txqueue_output_pbase (uint8_t base)
{
  (void) base;
}








#if 0

void
muos_txqueue_output_ifmt (uint8_t, uint8_t)
{
  (void) str;

}

void
muos_txqueue_output_ffmt (char)
{
  (void) str;

}

void
muos_txqueue_output_pcase (char)
{
  (void) str;

}

oid
muos_txqueue_output_pbase (uint8_t)
{
  (void) str;

}


void
muos_txqueue_output_pifmt (uint8_t, uint8_t)
{
  (void) str;

}

void
muos_txqueue_output_pffmt (char)
{

  (void) str;
}


void
muos_txqueue_output_ctrl (uint8_t, uint8_t, uint8_t)
{

  (void) str;
}


#endif








#endif




