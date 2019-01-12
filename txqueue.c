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

#ifdef MUOS_SERIAL_TXQUEUE

static struct fmtconfig_type txq_pfmtconfig = {10, 0, 0, 15, 15};
static struct fmtconfig_type txq_fmtconfig = {10, 0, 0, 15, 15};


muos_txqueue_type muos_txqueue;

//PLANNED: transaction support with commit/abort

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
  while (MUOS_CBUFFER_USED (muos_txqueue) && MUOS_CBUFFER_FREE (muos_txbuffer))
    {
      uint8_t tag = MUOS_CBUFFER_PEEK (muos_txqueue, 0);
      if (tag < MUOS_TXTAG_NCHARS)
        {
          muos_serial_tx_byte (tag);
          MUOS_CBUFFER_POP (muos_txqueue);
        }
      else if (tag <= MUOS_TXTAG_NCHARS_END)
        {
          muos_serial_tx_byte (MUOS_CBUFFER_PEEK (muos_txqueue, 1));
          if (tag > MUOS_TXTAG_NCHARS)
            {
              MUOS_CBUFFER_POKE (muos_txqueue, 1, tag-1);
            }
          else
            {
              MUOS_CBUFFER_POP (muos_txqueue);
            }
          MUOS_CBUFFER_POP (muos_txqueue);
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


void txqueue_FSTR (void)
{
  void* str;
  ((uint8_t*)&str)[1] = MUOS_CBUFFER_PEEK (muos_txqueue, 1);
  ((uint8_t*)&str)[0] = MUOS_CBUFFER_PEEK (muos_txqueue, 2);

  uint8_t b = pgm_read_byte (str);
  if (b)
    {
      muos_serial_tx_byte (b);
      ++str;
      MUOS_CBUFFER_POKE (muos_txqueue, 1, ((uint8_t*)&str)[1]);
      MUOS_CBUFFER_POKE (muos_txqueue, 2, ((uint8_t*)&str)[0]);
    }
  else
    {
      MUOS_CBUFFER_POPN (muos_txqueue, 3);
    }
}


void txqueue_NL (void)
{
  muos_serial_tx_byte ('\r');
  MUOS_CBUFFER_POKE (muos_txqueue, 0, '\n');
}

static uint8_t muos_xput_state = 0;

#define Xput(bits)                                                      \
static uint##bits##_t                                                   \
u##bits##put (uint##bits##_t v, uint8_t base, bool upcase)              \
{                                                                       \
  if (!muos_xput_state)                                                 \
    {                                                                   \
      for (uint##bits##_t tmp=v; tmp; tmp /= base)                      \
        ++muos_xput_state;                                              \
    }                                                                   \
                                                                        \
  uint##bits##_t start = 1;                                             \
                                                                        \
  for (uint##bits##_t i = muos_xput_state-1; i; --i)                    \
    {                                                                   \
      start *= base;                                                    \
    }                                                                   \
                                                                        \
  while (start)                                                         \
    {                                                                   \
      uint##bits##_t r = v/start;                                       \
      if (muos_serial_tx_byte ((r<10?'0':upcase?'A'-10:'a'-10)+r)       \
          == muos_error_tx_buffer_overflow)                             \
        return v;                                                       \
      v -= r*start;                                                     \
      start /= base;                                                    \
      --muos_xput_state;                                                \
    }                                                                   \
  return 0;                                                             \
}


Xput(8);
Xput(16);
Xput(32);
//Xput(64); //PLANNED: 64 bit support

#undef Xput

void
txqueue_UINT8 (void)
{
  uint8_t value = u8put (MUOS_CBUFFER_PEEK (muos_txqueue, 1), txq_fmtconfig.base, txq_fmtconfig.upcase);

  if (muos_xput_state)
    {
      MUOS_CBUFFER_POKE (muos_txqueue, 1, value);
    }
  else
    {
      MUOS_CBUFFER_POPN (muos_txqueue, 2);
      txq_fmtconfig = txq_pfmtconfig;
    }
}

void
txqueue_UINT16 (void)
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
      txq_fmtconfig = txq_pfmtconfig;
    }
}

void
txqueue_UINT32 (void)
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
      txq_fmtconfig = txq_pfmtconfig;
    }
}


void txqueue_BASE2 (void)
{
  txq_fmtconfig.base = 2;
  MUOS_CBUFFER_POP (muos_txqueue);
}

void txqueue_BASE8 (void)
{
  txq_fmtconfig.base = 8;
  MUOS_CBUFFER_POP (muos_txqueue);
}

void txqueue_BASE10 (void)
{
  txq_fmtconfig.base = 10;
  MUOS_CBUFFER_POP (muos_txqueue);
}

void txqueue_BASE16 (void)
{
  txq_fmtconfig.base = 16;
  MUOS_CBUFFER_POP (muos_txqueue);
}

void txqueue_BASEN (void)
{
  txq_fmtconfig.base = MUOS_CBUFFER_PEEK (muos_txqueue, 1);
  MUOS_CBUFFER_POPN (muos_txqueue, 2);
}


void txqueue_UPCASE (void)
{
  txq_fmtconfig.upcase = true;
  MUOS_CBUFFER_POP (muos_txqueue);
}

void txqueue_DOWNCASE (void)
{
  txq_fmtconfig.upcase = false;
  MUOS_CBUFFER_POP (muos_txqueue);
}


void txqueue_PBASE (void)
{
  txq_fmtconfig.base = txq_pfmtconfig.base = MUOS_CBUFFER_PEEK (muos_txqueue, 1);
  MUOS_CBUFFER_POPN (muos_txqueue, 2);
}


void txqueue_PUPCASE (void)
{
  txq_fmtconfig.upcase = txq_pfmtconfig.upcase = MUOS_CBUFFER_PEEK (muos_txqueue, 1);
  MUOS_CBUFFER_POPN (muos_txqueue, 2);
}

void txqueue_CSI (void)
{
  muos_serial_tx_byte (0x1b);
  MUOS_CBUFFER_POKE (muos_txqueue, 0, '[');
}


muos_error
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
          return muos_error_txqueue_overflow;
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
          return muos_error_txqueue_overflow;
        }
    }
  muos_txqueue_start ();
  return muos_success;
}


muos_error
muos_txqueue_output_cstr (const char* str)
{
  if (muos_txqueue_free () < strlen (str) + 1)
    {
      return muos_error_txqueue_overflow;
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
          break;
        }
    }

  muos_txqueue_start ();
  return muos_success;
}

muos_error
muos_txqueue_output_cstrn (const char* str, uint8_t n)
{
  if (muos_txqueue_free () < strlen (str) + 1 || muos_txqueue_free () < n+1)
    {
      return muos_error_txqueue_overflow;
    }

  for(; *str && n; ++str, --n)
    {
      if ((uint8_t)*str < 128)
        {
          muos_txqueue_push (*str);
        }
      else
        {
          muos_txqueue_push (MUOS_TXTAG_NCHARS + (n < strlen (str)?n:strlen (str))-1);

          for(; *str && n; ++str, --n)
            {
              muos_txqueue_push (*str);
            }
          break;
        }
    }

  muos_txqueue_start ();
  return muos_success;
}


muos_error
muos_txqueue_output_fstr (muos_flash_cstr str)
{
  if (muos_txqueue_free () < 3)
    {
      return muos_error_txqueue_overflow;
    }

  muos_txqueue_push (MUOS_TXTAG_FSTR);
  muos_txqueue_push (((uint8_t*)&str)[1]);
  muos_txqueue_push (((uint8_t*)&str)[0]);

  muos_txqueue_start ();
  return muos_success;
}



muos_error
muos_txqueue_output_mem (const uint8_t* mem, uint8_t len)
{
  (void) mem;
  (void) len;

  return muos_error_error;
}


muos_error
muos_txqueue_output_nl (void)
{
  if (!muos_txqueue_free ())
    {
      return muos_error_txqueue_overflow;
    }

  muos_txqueue_push (MUOS_TXTAG_NL);
  muos_txqueue_start ();
  return muos_success;
}

muos_error
muos_txqueue_output_csi_char (const char c)
{
  if (muos_txqueue_free () < 2)
    {
      return muos_error_txqueue_overflow;
    }

  muos_txqueue_push (MUOS_TXTAG_CSI);
  muos_txqueue_output_char (c);
  muos_txqueue_start ();
  return muos_success;
}


muos_error
muos_txqueue_output_csi_cstr (const char* str)
{
  if (muos_txqueue_free () < strlen (str) + 2)
    {
      return muos_error_txqueue_overflow;
    }

  muos_txqueue_push (MUOS_TXTAG_CSI);
  muos_txqueue_output_cstr (str);
  muos_txqueue_start ();
  return muos_success;
}



muos_error
muos_txqueue_output_csi_fstr (muos_flash_cstr str)
{
  if (muos_txqueue_free () < 4)
    {
      return muos_error_txqueue_overflow;
    }

  muos_txqueue_push (MUOS_TXTAG_CSI);
  muos_txqueue_output_fstr (str);
  muos_txqueue_start ();
  return muos_success;
}




static muos_error
txqueue_uint8 (uint8_t n)
{
  if (muos_txqueue_free () < 2)
    return muos_error_txqueue_overflow;

  muos_txqueue_push (MUOS_TXTAG_UINT8);
  muos_txqueue_push (n);
  return muos_success;
}

static muos_error
txqueue_uint16 (uint16_t n)
{
  if (muos_txqueue_free () < 3)
    return muos_error_txqueue_overflow;

  muos_txqueue_push (MUOS_TXTAG_UINT16);
  muos_txqueue_push (((uint8_t*)&n)[1]);
  muos_txqueue_push (((uint8_t*)&n)[0]);
  return muos_success;
}

static muos_error
txqueue_uint32 (uint32_t n)
{
  if (muos_txqueue_free () < 5)
    return muos_error_txqueue_overflow;

  muos_txqueue_push (MUOS_TXTAG_UINT32);
  muos_txqueue_push (((uint8_t*)&n)[3]);
  muos_txqueue_push (((uint8_t*)&n)[2]);
  muos_txqueue_push (((uint8_t*)&n)[1]);
  muos_txqueue_push (((uint8_t*)&n)[0]);
  return muos_success;
}



muos_error
muos_txqueue_output_intptr (intptr_t n)
{
  switch (sizeof n)
    {
    case 2:
      return muos_txqueue_output_int16 (n);
    case 4:
      return muos_txqueue_output_int32 (n);
    }
}

muos_error
muos_txqueue_output_uintptr (uintptr_t n)
{
  switch (sizeof n)
    {
    case 2:
      return muos_txqueue_output_uint16 (n);
      break;
    case 4:
      return muos_txqueue_output_uint32 (n);
    }
}

muos_error
muos_txqueue_output_int8 (int8_t n)
{

  if (n < 0)
    {
      if (muos_txqueue_free () < 2)
        return muos_error_txqueue_overflow;

      muos_txqueue_push ('-');
      return muos_txqueue_output_uint8 (-n);
    }
  else
    {
      return muos_txqueue_output_uint8 (n);
    }
}

muos_error
muos_txqueue_output_uint8 (uint8_t n)
{
  muos_error ret;

  if (n < fmtconfig.base)
    {
      ret = muos_txqueue_output_char ((((char)n<10?'0':fmtconfig.upcase?'A'-10:'a'-10))+(char)n);
    }
  else
    {
      ret = txqueue_uint8 (n);
    }

  fmtconfig = pfmtconfig;

  if (!ret)
    muos_txqueue_start ();

  return ret;
}

muos_error
muos_txqueue_output_int16 (int16_t n)
{
  if (n < 0)
    {
      if (muos_txqueue_free () < 3)
        return muos_error_txqueue_overflow;

      muos_txqueue_push ('-');
      return muos_txqueue_output_uint16 (-n);
    }
  else
    {
      return muos_txqueue_output_uint16 (n);
    }
}

muos_error
muos_txqueue_output_uint16 (uint16_t n)
{
  muos_error ret;

  if (n < fmtconfig.base)
    {
      ret = muos_txqueue_output_char ((((char)n<10?'0':fmtconfig.upcase?'A'-10:'a'-10))+(char)n);
    }
  else if (n <= 0xff)
    {
      ret = txqueue_uint8 (n);
    }
  else
    {
      ret = txqueue_uint16 (n);
    }

  fmtconfig = pfmtconfig;

  if (!ret)
    muos_txqueue_start ();

  return ret;
}

muos_error
muos_txqueue_output_int32 (int32_t n)
{
  if (n < 0)
    {
      if (muos_txqueue_free () < 5) //PLANNED: fix overprovisioning again MUOS_CBUFFER_RPOP (muos_txqueue);
        return muos_error_txqueue_overflow;

      muos_txqueue_push ('-');
      return muos_txqueue_output_uint32 (-n);
    }
  else
    {
      return muos_txqueue_output_uint32 (n);
    }
  return muos_success;
}


muos_error
muos_txqueue_output_uint32 (uint32_t n)
{
  muos_error ret;

  if (n < fmtconfig.base)
    {
      ret = muos_txqueue_output_char ((((char)n<10?'0':fmtconfig.upcase?'A'-10:'a'-10))+(char)n);
    }
  else if (n <= 0xff)
    {
      ret = txqueue_uint8 (n);
    }
  else if (n <= 0xffff)
    {
      ret = txqueue_uint16 (n);
    }
  else
    {
      ret = txqueue_uint32 (n);
    }

  fmtconfig = pfmtconfig;
  
  if (!ret)
    muos_txqueue_start ();

  return ret;
}

#if 0
muos_error
muos_txqueue_output_int64 (int64_t n)
{
  (void) n;
}

muos_error
muos_txqueue_output_uint64 (uint64_t n)
{
  (void) n;
}

muos_error
muos_txqueue_output_float (float)
{
  (void) str;
}


muos_error
muos_txqueue_output_cstr_R ()
{
  (void) str;
}

muos_error
muos_txqueue_output_mem_R ()
{
  (void) str;
}

muos_error
muos_txqueue_output_int16_R ()
{
  (void) str;
}

muos_error
muos_txqueue_output_uint16_R ()
{
  (void) str;
}

muos_error
muos_txqueue_output_int32_R ()
{
  (void) str;
}

muos_error
muos_txqueue_output_uint32_R ()
{
  (void) str;

}

muos_error
muos_txqueue_output_int64_R ()
{
  (void) str;

}

muos_error
muos_txqueue_output_uint64_R ()
{
  (void) str;

}

muos_error
muos_txqueue_output_float_R ()
{
  (void) str;

}
#endif

muos_error
muos_txqueue_output_upcase (bool upcase)
{
  if (muos_txqueue_free () < 1)
    return muos_error_txqueue_overflow;

  fmtconfig.upcase = upcase;
  if (upcase)
    muos_txqueue_push (MUOS_TXTAG_UPCASE);
  else
    muos_txqueue_push (MUOS_TXTAG_DOWNCASE);

  return muos_success;
}


muos_error
muos_txqueue_output_base (uint8_t base)
{
  if (muos_txqueue_free () < 1)
    return muos_error_txqueue_overflow;

  fmtconfig.base = base;
  switch (base)
    {
    case 2:
      muos_txqueue_push (MUOS_TXTAG_BASE2);
      break;
    case 8:
      muos_txqueue_push (MUOS_TXTAG_BASE8);
      break;
    case 10:
      muos_txqueue_push (MUOS_TXTAG_BASE10);
      break;
    case 16:
      muos_txqueue_push (MUOS_TXTAG_BASE16);
      break;
    default:
      if (muos_txqueue_free () >= 2)
        {
          muos_txqueue_push (MUOS_TXTAG_BASEN);
          muos_txqueue_push (base);
        }
      else
        return muos_error_txqueue_overflow;
    }
  return muos_success;
}


muos_error
muos_txqueue_output_pupcase (bool upcase)
{
  if (muos_txqueue_free () < 2)
    return muos_error_txqueue_overflow;

  fmtconfig.upcase = pfmtconfig.upcase = upcase;
  muos_txqueue_push (MUOS_TXTAG_PUPCASE);
  muos_txqueue_push (upcase);
  return muos_success;
}


muos_error
muos_txqueue_output_pbase (uint8_t base)
{
  if (muos_txqueue_free () < 2)
    return muos_error_txqueue_overflow;

  fmtconfig.base = pfmtconfig.base = base;
  muos_txqueue_push (MUOS_TXTAG_PBASE);
  muos_txqueue_push (base);
  return muos_success;
}








#if 0

muos_error
muos_txqueue_output_ifmt (uint8_t, uint8_t)
{
  (void) str;

}

muos_error
muos_txqueue_output_ffmt (char)
{
  (void) str;

}

muos_error
muos_txqueue_output_pcase (char)
{
  (void) str;

}

oid
muos_txqueue_output_pbase (uint8_t)
{
  (void) str;

}


muos_error
muos_txqueue_output_pifmt (uint8_t, uint8_t)
{
  (void) str;

}

muos_error
muos_txqueue_output_pffmt (char)
{

  (void) str;
}


muos_error
muos_txqueue_output_ctrl (uint8_t, uint8_t, uint8_t)
{

  (void) str;
}


#endif








#endif




