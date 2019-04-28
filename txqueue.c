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

#ifdef MUOS_IO_TXQUEUE

#include <string.h>

#include <muos/muos.h>
#include <muos/serial.h>
#include <muos/txqueue.h>

#include <muos/bgq.h>

#define SERIAL(hw, txqsize)                     \
  MUOS_CBUFFERDEF(muos_txqueue##hw, txqsize);

MUOS_IO_TXQUEUE_SIZE
#undef SERIAL


//TODO: #if MUOS_SERIAL_NUM > 1    optimize table out when there is only one channel
struct muos_cbuffer* const muos_txqueue [] =
  {
#define SERIAL(hw, txqsize)                     \
   &muos_txqueue##hw.descriptor,

   MUOS_IO_TXQUEUE_SIZE
#undef UART
  };
//#endif


static struct fmtconfig_type txq_pfmtconfig[MUOS_SERIAL_NUM];
static struct fmtconfig_type txq_fmtconfig[MUOS_SERIAL_NUM];

void
muos_io_txqueue_20init (void)
{
  for (uint8_t i = 0; i < MUOS_SERIAL_NUM; ++i)
    {
      txq_pfmtconfig[i].base = 10;
      txq_pfmtconfig[i].upcase = 0;
      txq_pfmtconfig[i].eng = 0;
      txq_pfmtconfig[i].flt_int = 15;
      txq_pfmtconfig[i].flt_frac = 15;
      txq_fmtconfig[i].base = 10;
      txq_fmtconfig[i].upcase = 0;
      txq_fmtconfig[i].eng = 0;
      txq_fmtconfig[i].flt_int = 15;
      txq_fmtconfig[i].flt_frac = 15;
    }
}


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

muos_cbuffer_index
muos_txqueue_free MUOS_IO_HWPARAM()
{
  return muos_cbuffer_free (muos_txqueue[MUOS_IO_HWINDEX]);
}


#define TAG(name)  static void txqueue_##name MUOS_IO_HWPARAM();
MUOS_TXQUEUE_TAGS
#undef TAG


void
muos_txqueue_run (void)
{
  //PLANNED: optimize hw out when there is only one serial (no arg on bgq)
  uint8_t hw = muos_bgq_pop ();

  while (muos_cbuffer_used (muos_txqueue[hw]) && muos_cbuffer_free (muos_txbuffer[hw]))
    {
      uint8_t tag = muos_cbuffer_peek (muos_txqueue[hw], 0);
      if (tag < MUOS_TXTAG_NCHARS)
        {
          muos_serial_tx_byte MUOS_IO_HWARG(tag);
          muos_cbuffer_pop (muos_txqueue[hw]);
        }
      else if (tag <= MUOS_TXTAG_NCHARS_END)
        {
          muos_serial_tx_byte MUOS_IO_HWARG(muos_cbuffer_peek (muos_txqueue[hw], 1));
          if (tag > MUOS_TXTAG_NCHARS)
            {
              muos_cbuffer_poke (muos_txqueue[hw], 1, tag-1);
            }
          else
            {
              muos_cbuffer_pop (muos_txqueue[hw]);
            }
          muos_cbuffer_pop (muos_txqueue[hw]);
        }
      else switch (tag)
        {
#define TAG(name)  case MUOS_TXTAG_##name: txqueue_##name MUOS_IO_HWARG(); break;
          MUOS_TXQUEUE_TAGS
#undef TAG
        }
    }

  if (muos_cbuffer_used (muos_txqueue[hw]))
    {
      muos_bgq_push_arg (muos_txqueue_run, (intptr_t)hw);
    }
  else
    {
      muos_serial_status[hw].txqueue_pending = false;
    }
}


void
muos_txqueue_start MUOS_IO_HWPARAM()
{
  if (!muos_serial_status[MUOS_IO_HWINDEX].txqueue_pending)
    {
      muos_serial_status[MUOS_IO_HWINDEX].txqueue_pending = true;
      muos_bgq_push_arg (muos_txqueue_run, (intptr_t)MUOS_IO_HWINDEX);
    }
}


/*
  TAGGED data handling
*/


void txqueue_FSTR MUOS_IO_HWPARAM()
{
  void* str;
  ((uint8_t*)&str)[1] = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 1);
  ((uint8_t*)&str)[0] = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 2);

  uint8_t b = pgm_read_byte (str);
  if (b)
    {
      muos_serial_tx_byte MUOS_IO_HWARG(b);
      ++str;
      muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 1, ((uint8_t*)&str)[1]);
      muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 2, ((uint8_t*)&str)[0]);
    }
  else
    {
      muos_cbuffer_popn (muos_txqueue[MUOS_IO_HWINDEX], 3);
    }
}


void txqueue_NL MUOS_IO_HWPARAM()
{
  muos_serial_tx_byte MUOS_IO_HWARG('\r');
  muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 0, '\n');
}

static uint8_t muos_xput_state[MUOS_SERIAL_NUM];

#define Xput(bits)                                                                      \
  static uint##bits##_t                                                                 \
  u##bits##put MUOS_IO_HWPARAM(uint##bits##_t v, uint8_t base, bool upcase)             \
  {                                                                                     \
    if (!muos_xput_state[MUOS_IO_HWINDEX])                                              \
      {                                                                                 \
        for (uint##bits##_t tmp=v; tmp; tmp /= base)                                    \
          ++muos_xput_state[MUOS_IO_HWINDEX];                                           \
      }                                                                                 \
    uint##bits##_t start = 1;                                                           \
    for (uint##bits##_t i = muos_xput_state[MUOS_IO_HWINDEX]-1; i; --i)                 \
      {                                                                                 \
        start *= base;                                                                  \
      }                                                                                 \
    while (start)                                                                       \
      {                                                                                 \
        uint##bits##_t r = v/start;                                                     \
        if (muos_serial_tx_byte MUOS_IO_HWARG((r<10?'0':upcase?'A'-10:'a'-10)+r)        \
            == muos_error_tx_overflow)                                                  \
          return v;                                                                     \
        v -= r*start;                                                                   \
        start /= base;                                                                  \
        --muos_xput_state[MUOS_IO_HWINDEX];                                             \
      }                                                                                 \
    return 0;                                                                           \
  }


Xput(8);
Xput(16);
Xput(32);
//Xput(64); //PLANNED: 64 bit support

#undef Xput

void
txqueue_UINT8 MUOS_IO_HWPARAM()
{
  uint8_t value =
    u8put MUOS_IO_HWARG(muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 1),
                        txq_fmtconfig[MUOS_IO_HWINDEX].base,
                        txq_fmtconfig[MUOS_IO_HWINDEX].upcase);

  if (muos_xput_state[MUOS_IO_HWINDEX])
    {
      muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 1, value);
    }
  else
    {
      muos_cbuffer_popn (muos_txqueue[MUOS_IO_HWINDEX], 2);
      txq_fmtconfig[MUOS_IO_HWINDEX] = txq_pfmtconfig[MUOS_IO_HWINDEX];
    }
}

void
txqueue_UINT16 MUOS_IO_HWPARAM()
{
  uint16_t value;
  ((uint8_t*)&value)[1] = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 1);
  ((uint8_t*)&value)[0] = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 2);

  value =
    u16put MUOS_IO_HWARG(value,
                         txq_fmtconfig[MUOS_IO_HWINDEX].base,
                         txq_fmtconfig[MUOS_IO_HWINDEX].upcase);

  if (muos_xput_state[MUOS_IO_HWINDEX])
    {
      muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 1, ((uint8_t*)&value)[1]);
      muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 2, ((uint8_t*)&value)[0]);
    }
  else
    {
      muos_cbuffer_popn (muos_txqueue[MUOS_IO_HWINDEX], 3);
      txq_fmtconfig[MUOS_IO_HWINDEX] = txq_pfmtconfig[MUOS_IO_HWINDEX];
    }
}

void
txqueue_UINT32 MUOS_IO_HWPARAM()
{
  uint32_t value;
  ((uint8_t*)&value)[3] = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 1);
  ((uint8_t*)&value)[2] = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 2);
  ((uint8_t*)&value)[1] = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 3);
  ((uint8_t*)&value)[0] = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 4);

  value = u32put MUOS_IO_HWARG(value, txq_fmtconfig[MUOS_IO_HWINDEX].base, txq_fmtconfig[MUOS_IO_HWINDEX].upcase);

  if (muos_xput_state[MUOS_IO_HWINDEX])
    {
      muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 1, ((uint8_t*)&value)[3]);
      muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 2, ((uint8_t*)&value)[2]);
      muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 3, ((uint8_t*)&value)[1]);
      muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 4, ((uint8_t*)&value)[0]);
    }
  else
    {
      muos_cbuffer_popn (muos_txqueue[MUOS_IO_HWINDEX], 5);
      txq_fmtconfig[MUOS_IO_HWINDEX] = txq_pfmtconfig[MUOS_IO_HWINDEX];
    }
}


void txqueue_BASE2 MUOS_IO_HWPARAM()
{
  txq_fmtconfig[MUOS_IO_HWINDEX].base = 2;
  muos_cbuffer_pop (muos_txqueue[MUOS_IO_HWINDEX]);
}

void txqueue_BASE8 MUOS_IO_HWPARAM()
{
  txq_fmtconfig[MUOS_IO_HWINDEX].base = 8;
  muos_cbuffer_pop (muos_txqueue[MUOS_IO_HWINDEX]);
}

void txqueue_BASE10 MUOS_IO_HWPARAM()
{
  txq_fmtconfig[MUOS_IO_HWINDEX].base = 10;
  muos_cbuffer_pop (muos_txqueue[MUOS_IO_HWINDEX]);
}

void txqueue_BASE16 MUOS_IO_HWPARAM()
{
  txq_fmtconfig[MUOS_IO_HWINDEX].base = 16;
  muos_cbuffer_pop (muos_txqueue[MUOS_IO_HWINDEX]);
}

void txqueue_BASEN MUOS_IO_HWPARAM()
{
  txq_fmtconfig[MUOS_IO_HWINDEX].base = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 1);
  muos_cbuffer_popn (muos_txqueue[MUOS_IO_HWINDEX], 2);
}


void txqueue_UPCASE MUOS_IO_HWPARAM()
{
  txq_fmtconfig[MUOS_IO_HWINDEX].upcase = true;
  muos_cbuffer_pop (muos_txqueue[MUOS_IO_HWINDEX]);
}

void txqueue_DOWNCASE MUOS_IO_HWPARAM()
{
  txq_fmtconfig[MUOS_IO_HWINDEX].upcase = false;
  muos_cbuffer_pop (muos_txqueue[MUOS_IO_HWINDEX]);
}


void txqueue_PBASE MUOS_IO_HWPARAM()
{
  txq_fmtconfig[MUOS_IO_HWINDEX].base =
    txq_pfmtconfig[MUOS_IO_HWINDEX].base
    = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 1);
  muos_cbuffer_popn (muos_txqueue[MUOS_IO_HWINDEX], 2);
}


void txqueue_PUPCASE MUOS_IO_HWPARAM()
{
  txq_fmtconfig[MUOS_IO_HWINDEX].upcase =
    txq_pfmtconfig[MUOS_IO_HWINDEX].upcase
    = muos_cbuffer_peek (muos_txqueue[MUOS_IO_HWINDEX], 1);
  muos_cbuffer_popn (muos_txqueue[MUOS_IO_HWINDEX], 2);
}

void txqueue_CSI MUOS_IO_HWPARAM()
{
  muos_serial_tx_byte MUOS_IO_HWARG(0x1b);
  muos_cbuffer_poke (muos_txqueue[MUOS_IO_HWINDEX], 0, '[');
}


/*

  io functions

*/


#if MUOS_SERIAL_NUM > 1

bool
muos_tx_wait (intptr_t data)
{
  return muos_txqueue_free (((struct muos_txwait*)data)->hw) > ((struct muos_txwait*)data)->space;
}

#else

bool
muos_tx_wait (intptr_t space)
{
  return muos_serial_tx_free () > space;
}

#endif


muos_error
muos_txqueue_output_char MUOS_IO_HWPARAM(char c)
{
  MUOS_IO_HWCHECK;
  if ((uint8_t)c < 128)
    {
      if (muos_txqueue_free MUOS_IO_HWARG() >= 1)
        {
          muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], c);
        }
      else
        {
          return muos_error_txqueue_overflow;
        }
    }
  else
    {
      if (muos_txqueue_free MUOS_IO_HWARG() >= 2)
        {
          muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_NCHARS);
          muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], c);
        }
      else
        {
          return muos_error_txqueue_overflow;
        }
    }
  muos_txqueue_start MUOS_IO_HWARG();
  return muos_success;
}


muos_error
muos_txqueue_output_cstr MUOS_IO_HWPARAM(const char* str)
{
  MUOS_IO_HWCHECK;
  if (muos_txqueue_free MUOS_IO_HWARG() < strlen (str) + 1)
    {
      return muos_error_txqueue_overflow;
    }

  for(; *str; ++str)
    {
      if ((uint8_t)*str < 128)
        {
          muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], *str);
        }
      else
        {
          //FIXME: strlen may overflow NCHARs range
          muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_NCHARS + strlen (str) - 1);

          for(; *str; ++str)
            {
              muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], *str);
            }
          break;
        }
    }

  muos_txqueue_start MUOS_IO_HWARG();
  return muos_success;
}

muos_error
muos_txqueue_output_cstrn MUOS_IO_HWPARAM(const char* str, uint8_t n)
{
  MUOS_IO_HWCHECK;
  if (muos_txqueue_free MUOS_IO_HWARG() < strlen (str) + 1 || muos_txqueue_free MUOS_IO_HWARG() < n+1)
    {
      return muos_error_txqueue_overflow;
    }

  for(; *str && n; ++str, --n)
    {
      if ((uint8_t)*str < 128)
        {
          muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], *str);
        }
      else
        {
          //FIXME: strlen may overflow NCHARs range
          muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_NCHARS + (n < strlen (str)?n:strlen (str))-1);

          for(; *str && n; ++str, --n)
            {
              muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], *str);
            }
          break;
        }
    }

  muos_txqueue_start MUOS_IO_HWARG();
  return muos_success;
}


muos_error
muos_txqueue_output_fstr MUOS_IO_HWPARAM(muos_flash_cstr str)
{
  MUOS_IO_HWCHECK;
  if (muos_txqueue_free MUOS_IO_HWARG() < 3)
    {
      return muos_error_txqueue_overflow;
    }

  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_FSTR);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], ((uint8_t*)&str)[1]);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], ((uint8_t*)&str)[0]);

  muos_txqueue_start MUOS_IO_HWARG();
  return muos_success;
}


#if 0
muos_error
muos_txqueue_output_mem MUOS_IO_HWPARAM(const uint8_t* mem, uint8_t len)
{
  MUOS_IO_HWCHECK;
  (void) mem;
  (void) len;

  return muos_error_error;
}
#endif

muos_error
muos_txqueue_output_nl MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  if (!muos_txqueue_free MUOS_IO_HWARG())
    {
      return muos_error_txqueue_overflow;
    }

  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_NL);
  muos_txqueue_start MUOS_IO_HWARG();
  return muos_success;
}

muos_error
muos_txqueue_output_csi_char MUOS_IO_HWPARAM(const char c)
{
  MUOS_IO_HWCHECK;
  if (muos_txqueue_free MUOS_IO_HWARG() < 2)
    {
      return muos_error_txqueue_overflow;
    }

  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_CSI);
  muos_txqueue_output_char MUOS_IO_HWARG(c);
  muos_txqueue_start MUOS_IO_HWARG();
  return muos_success;
}


muos_error
muos_txqueue_output_csi_cstr MUOS_IO_HWPARAM(const char* str)
{
  MUOS_IO_HWCHECK;
  if (muos_txqueue_free MUOS_IO_HWARG() < strlen (str) + 2)
    {
      return muos_error_txqueue_overflow;
    }

  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_CSI);
  muos_txqueue_output_cstr MUOS_IO_HWARG(str); //TODO: MUOS_OK()? strlen already checked, but NCHARS etc
  muos_txqueue_start MUOS_IO_HWARG();
  return muos_success;
}



muos_error
muos_txqueue_output_csi_fstr MUOS_IO_HWPARAM(muos_flash_cstr str)
{
  MUOS_IO_HWCHECK;
  if (muos_txqueue_free MUOS_IO_HWARG() < 4)
    {
      return muos_error_txqueue_overflow;
    }

  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_CSI);
  muos_txqueue_output_fstr MUOS_IO_HWARG(str); //TODO: MUOS_OK()? strlen already checked, but NCHARS etc
  muos_txqueue_start MUOS_IO_HWARG();
  return muos_success;
}




static muos_error
txqueue_uint8 MUOS_IO_HWPARAM(uint8_t n)
{
  if (muos_txqueue_free MUOS_IO_HWARG() < 2)
    return muos_error_txqueue_overflow;

  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_UINT8);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], n);
  return muos_success;
}

static muos_error
txqueue_uint16 MUOS_IO_HWPARAM(uint16_t n)
{
  if (muos_txqueue_free MUOS_IO_HWARG() < 3)
    return muos_error_txqueue_overflow;

  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_UINT16);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], ((uint8_t*)&n)[1]);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], ((uint8_t*)&n)[0]);
  return muos_success;
}

static muos_error
txqueue_uint32 MUOS_IO_HWPARAM(uint32_t n)
{
  if (muos_txqueue_free MUOS_IO_HWARG() < 5)
    return muos_error_txqueue_overflow;

  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_UINT32);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], ((uint8_t*)&n)[3]);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], ((uint8_t*)&n)[2]);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], ((uint8_t*)&n)[1]);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], ((uint8_t*)&n)[0]);
  return muos_success;
}



muos_error
muos_txqueue_output_intptr MUOS_IO_HWPARAM(intptr_t n)
{
  MUOS_IO_HWCHECK;
  switch (sizeof n)
    {
    case 2:
      return muos_txqueue_output_int16 MUOS_IO_HWARG(n);
    case 4:
      return muos_txqueue_output_int32 MUOS_IO_HWARG(n);
    }
}

muos_error
muos_txqueue_output_uintptr MUOS_IO_HWPARAM(uintptr_t n)
{
  MUOS_IO_HWCHECK;
  switch (sizeof n)
    {
    case 2:
      return muos_txqueue_output_uint16 MUOS_IO_HWARG(n);
      break;
    case 4:
      return muos_txqueue_output_uint32 MUOS_IO_HWARG(n);
    }
}

muos_error
muos_txqueue_output_int8 MUOS_IO_HWPARAM(int8_t n)
{
  MUOS_IO_HWCHECK;

  if (n < 0)
    {
      if (muos_txqueue_free MUOS_IO_HWARG() < 2)
        return muos_error_txqueue_overflow;

      muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], '-');
      return muos_txqueue_output_uint8 MUOS_IO_HWARG(-n);
    }
  else
    {
      return muos_txqueue_output_uint8 MUOS_IO_HWARG(n);
    }
}

muos_error
muos_txqueue_output_uint8 MUOS_IO_HWPARAM(uint8_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret;

  if (n < fmtconfig[MUOS_IO_HWINDEX].base)
    {
      ret = muos_txqueue_output_char MUOS_IO_HWARG((((char)n<10?'0':fmtconfig[MUOS_IO_HWINDEX].upcase?'A'-10:'a'-10))+(char)n);
    }
  else
    {
      ret = txqueue_uint8 MUOS_IO_HWARG(n);
    }

  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];

  if (!ret)
    muos_txqueue_start MUOS_IO_HWARG();

  return ret;
}

muos_error
muos_txqueue_output_int16 MUOS_IO_HWPARAM(int16_t n)
{
  MUOS_IO_HWCHECK;
  if (n < 0)
    {
      if (muos_txqueue_free MUOS_IO_HWARG() < 3)
        return muos_error_txqueue_overflow;

      muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], '-');
      return muos_txqueue_output_uint16 MUOS_IO_HWARG(-n);
    }
  else
    {
      return muos_txqueue_output_uint16 MUOS_IO_HWARG(n);
    }
}

muos_error
muos_txqueue_output_uint16 MUOS_IO_HWPARAM(uint16_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret;

  if (n < fmtconfig[MUOS_IO_HWINDEX].base)
    {
      ret = muos_txqueue_output_char MUOS_IO_HWARG((((char)n<10?'0':fmtconfig[MUOS_IO_HWINDEX].upcase?'A'-10:'a'-10))+(char)n);
    }
  else if (n <= 0xff)
    {
      ret = txqueue_uint8 MUOS_IO_HWARG(n);
    }
  else
    {
      ret = txqueue_uint16 MUOS_IO_HWARG(n);
    }

  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];

  if (!ret)
    muos_txqueue_start MUOS_IO_HWARG();

  return ret;
}

muos_error
muos_txqueue_output_int32 MUOS_IO_HWPARAM(int32_t n)
{
  MUOS_IO_HWCHECK;
  if (n < 0)
    {
      if (muos_txqueue_free MUOS_IO_HWARG() < 5) //PLANNED: fix overprovisioning again MUOS_CBUFFER_RPOP (muos_txqueue);
        return muos_error_txqueue_overflow;

      muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], '-');
      return muos_txqueue_output_uint32 MUOS_IO_HWARG(-n);
    }
  else
    {
      return muos_txqueue_output_uint32 MUOS_IO_HWARG(n);
    }
  return muos_success;
}


muos_error
muos_txqueue_output_uint32 MUOS_IO_HWPARAM(uint32_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret;

  if (n < fmtconfig[MUOS_IO_HWINDEX].base)
    {
      ret = muos_txqueue_output_char MUOS_IO_HWARG((((char)n<10?'0':fmtconfig[MUOS_IO_HWINDEX].upcase?'A'-10:'a'-10))+(char)n);
    }
  else if (n <= 0xff)
    {
      ret = txqueue_uint8 MUOS_IO_HWARG(n);
    }
  else if (n <= 0xffff)
    {
      ret = txqueue_uint16 MUOS_IO_HWARG(n);
    }
  else
    {
      ret = txqueue_uint32 MUOS_IO_HWARG(n);
    }

  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];
  
  if (!ret)
    muos_txqueue_start MUOS_IO_HWARG();

  return ret;
}

#if 0
muos_error
muos_txqueue_output_int64 MUOS_IO_HWPARAM(int64_t n)
{
  MUOS_IO_HWCHECK;
  (void) n;
}

muos_error
muos_txqueue_output_uint64 MUOS_IO_HWPARAM(uint64_t n)
{
  MUOS_IO_HWCHECK;
  (void) n;
}

muos_error
muos_txqueue_output_float MUOS_IO_HWPARAM(float)
{
  MUOS_IO_HWCHECK;
  (void) str;
}


muos_error
muos_txqueue_output_cstr_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  (void) str;
}

muos_error
muos_txqueue_output_mem_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  (void) str;
}

muos_error
muos_txqueue_output_int16_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  (void) str;
}

muos_error
muos_txqueue_output_uint16_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  (void) str;
}

muos_error
muos_txqueue_output_int32_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  (void) str;
}

muos_error
muos_txqueue_output_uint32_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  (void) str;

}

muos_error
muos_txqueue_output_int64_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  (void) str;

}

muos_error
muos_txqueue_output_uint64_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  (void) str;

}

muos_error
muos_txqueue_output_float_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  (void) str;

}
#endif

muos_error
muos_txqueue_output_upcase MUOS_IO_HWPARAM(bool upcase)
{
  MUOS_IO_HWCHECK;
  if (muos_txqueue_free MUOS_IO_HWARG() < 1)
    return muos_error_txqueue_overflow;

  fmtconfig[MUOS_IO_HWINDEX].upcase = upcase;
  if (upcase)
    muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_UPCASE);
  else
    muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_DOWNCASE);

  return muos_success;
}


muos_error
muos_txqueue_output_base MUOS_IO_HWPARAM(uint8_t base)
{
  MUOS_IO_HWCHECK;
  if (muos_txqueue_free MUOS_IO_HWARG() < 1)
    return muos_error_txqueue_overflow;

  fmtconfig[MUOS_IO_HWINDEX].base = base;
  switch (base)
    {
    case 2:
      muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_BASE2);
      break;
    case 8:
      muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_BASE8);
      break;
    case 10:
      muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_BASE10);
      break;
    case 16:
      muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_BASE16);
      break;
    default:
      if (muos_txqueue_free MUOS_IO_HWARG() >= 2)
        {
          muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_BASEN);
          muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], base);
        }
      else
        return muos_error_txqueue_overflow;
    }
  return muos_success;
}


muos_error
muos_txqueue_output_pupcase MUOS_IO_HWPARAM(bool upcase)
{
  MUOS_IO_HWCHECK;
  if (muos_txqueue_free MUOS_IO_HWARG() < 2)
    return muos_error_txqueue_overflow;

  fmtconfig[MUOS_IO_HWINDEX].upcase = pfmtconfig[MUOS_IO_HWINDEX].upcase = upcase;
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_PUPCASE);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], upcase);
  return muos_success;
}


muos_error
muos_txqueue_output_pbase MUOS_IO_HWPARAM(uint8_t base)
{
  MUOS_IO_HWCHECK;
  if (muos_txqueue_free MUOS_IO_HWARG() < 2)
    return muos_error_txqueue_overflow;

  fmtconfig[MUOS_IO_HWINDEX].base = pfmtconfig[MUOS_IO_HWINDEX].base = base;
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], MUOS_TXTAG_PBASE);
  muos_cbuffer_push (muos_txqueue[MUOS_IO_HWINDEX], base);
  return muos_success;
}








#if 0

muos_error
muos_txqueue_output_ifmt MUOS_IO_HWPARAM(uint8_t, uint8_t)
{
  MUOS_IO_HWCHECK;
  (void) str;

}

muos_error
muos_txqueue_output_ffmt MUOS_IO_HWPARAM(char)
{
  MUOS_IO_HWCHECK;
  (void) str;

}

muos_error
muos_txqueue_output_pcase MUOS_IO_HWPARAM(char)
{
  MUOS_IO_HWCHECK;
  (void) str;

}

muos_error
muos_txqueue_output_pbase MUOS_IO_HWPARAM(uint8_t)
{
  MUOS_IO_HWCHECK;
  (void) str;

}


muos_error
muos_txqueue_output_pifmt MUOS_IO_HWPARAM(uint8_t, uint8_t)
{
  MUOS_IO_HWCHECK;
  (void) str;

}

muos_error
muos_txqueue_output_pffmt MUOS_IO_HWPARAM(char)
{
  MUOS_IO_HWCHECK;

  (void) str;
}


muos_error
muos_txqueue_output_ctrl MUOS_IO_HWPARAM(uint8_t, uint8_t, uint8_t)
{
  MUOS_IO_HWCHECK;

  (void) str;
}


#endif








#endif




