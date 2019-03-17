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

#ifdef MUOS_IO

#include <muos/muos.h>
#include <muos/serial.h>
#include <muos/io.h>


#include <muos/bgq.h>
#include <muos/txqueue.h>

struct fmtconfig_type pfmtconfig[MUOS_SERIAL_NUM];
struct fmtconfig_type fmtconfig[MUOS_SERIAL_NUM];

void
muos_io_20init (void)
{
  for (uint8_t i = 0; i < MUOS_SERIAL_NUM; ++i)
    {
      pfmtconfig[i].base = 10;
      pfmtconfig[i].upcase = 0;
      pfmtconfig[i].eng = 0;
      pfmtconfig[i].flt_int = 15;
      pfmtconfig[i].flt_frac = 15;
      fmtconfig[i].base = 10;
      fmtconfig[i].upcase = 0;
      fmtconfig[i].eng = 0;
      fmtconfig[i].flt_int = 15;
      fmtconfig[i].flt_frac = 15;
    }
}


/*
  Blocking io
 */
bool
muos_tx_wait (intptr_t data);


#if MUOS_SERIAL_NUM > 1

muos_error
muos_output_wait (uint8_t hw, muos_cbuffer_index space, muos_shortclock timeout)
{
  if (muos_serial_status[hw].serial_tx_blocked)
    return muos_error_serial_status;

  struct muos_txwait waitdata = {hw, space};

  muos_serial_status[hw].serial_tx_blocked = true;
  muos_error ret = muos_wait (muos_tx_wait, (intptr_t)&waitdata, timeout);
  muos_serial_status[hw].serial_tx_blocked = false;

  return ret;
}

#else

muos_error
muos_output_wait (muos_cbuffer_index space, muos_shortclock timeout)
{
  if (muos_serial_status[0].serial_tx_blocked)
    return muos_error_serial_status;

  muos_serial_status[0].serial_tx_blocked = true;
  muos_error ret = muos_wait (muos_tx_wait, space, timeout);
  muos_serial_status[0].serial_tx_blocked = false;

  return ret;
}

#endif



#ifndef MUOS_IO_TXQUEUE
//PLANNED: fixed point for integers and mille delimiters

#if MUOS_SERIAL_NUM > 1
bool
muos_tx_wait (intptr_t data)
{
  return muos_serial_tx_free (((struct muos_txwait*)data)->hw) > ((struct muos_txwait*)data)->space;
}

#else

bool
muos tx_wait (intptr_t space)
{
  return muos_serial_tx_free () > space;
}

#endif

muos_error
muos_output_char MUOS_IO_HWPARAM(char c)
{
  MUOS_IO_HWCHECK;
  return muos_serial_tx_byte MUOS_IO_HWARG(c);
}


muos_error
muos_output_cstr MUOS_IO_HWPARAM(const char* str)
{
  MUOS_IO_HWCHECK;
  muos_error ret = muos_success;

  while (*str && !ret)
    ret = muos_serial_tx_byte MUOS_IO_HWARG(*str++);

  return ret;
}


muos_error
muos_output_cstrn MUOS_IO_HWPARAM(const char* str, uint8_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret = muos_success;
  while (*str && n-- && !ret)
    ret = muos_serial_tx_byte MUOS_IO_HWARG(*str++);

  return ret;
}


muos_error
muos_output_fstr MUOS_IO_HWPARAM(muos_flash_cstr str)
{
  MUOS_IO_HWCHECK;
  muos_error ret = muos_success;

  for (uint8_t b  = pgm_read_byte (str);
       b && !ret;
       b  = pgm_read_byte (++str))
    {
      ret = muos_serial_tx_byte MUOS_IO_HWARG(b);
    }

  return ret;
}


muos_error
muos_output_mem MUOS_IO_HWPARAM(const uint8_t* mem, uint8_t len)
{
  MUOS_IO_HWCHECK;
  muos_error ret = muos_success;

  while (len-- && !ret)
    ret = muos_serial_tx_byte MUOS_IO_HWARG(*mem++);

  return ret;
}

#define retcont(expr) ret = ret?ret:(expr)

muos_error
muos_output_nl MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  muos_error ret = muos_serial_tx_byte MUOS_IO_HWARG('\r');

  retcont (muos_serial_tx_byte MUOS_IO_HWARG('\n'));

  return ret;
}


muos_error
muos_output_csi_char MUOS_IO_HWPARAM(const char c)
{
  MUOS_IO_HWCHECK;
  muos_error ret = muos_serial_tx_byte MUOS_IO_HWARG(0x1b);
  retcont (muos_serial_tx_byte MUOS_IO_HWARG('['));
  retcont (muos_serial_tx_byte MUOS_IO_HWARG(c));

  return ret;
}


muos_error
muos_output_csi_cstr MUOS_IO_HWPARAM(const char* str)
{
  MUOS_IO_HWCHECK;
  muos_error ret = muos_serial_tx_byte MUOS_IO_HWARG(0x1b);
  retcont (muos_serial_tx_byte MUOS_IO_HWARG('['));
  retcont (muos_output_cstr MUOS_IO_HWARG(str));

  return ret;
}


muos_error
muos_output_csi_fstr MUOS_IO_HWPARAM(muos_flash_cstr str)
{
  MUOS_IO_HWCHECK;
  muos_error ret = muos_serial_tx_byte MUOS_IO_HWARG(0x1b);
  retcont (muos_serial_tx_byte MUOS_IO_HWARG('['));
  retcont (muos_output_fstr MUOS_IO_HWARG(str));

  return ret;
}

#define Xput(bits)                                                                      \
  static uint##bits##_t                                                                 \
  u##bits##put MUOS_IO_HWPARAM(uint##bits##_t v, uint8_t base, bool upcase)             \
  {                                                                                     \
    uint8_t digits = 0;                                                                 \
                                                                                        \
    for (uint##bits##_t tmp=v; tmp; tmp /= base)                                        \
      ++digits;                                                                         \
                                                                                        \
    uint##bits##_t start = 1;                                                           \
                                                                                        \
    for (uint##bits##_t i = digits?digits-1:0; i; --i)                                  \
      {                                                                                 \
        start *= base;                                                                  \
      }                                                                                 \
                                                                                        \
    muos_error ret = muos_success;                                                      \
                                                                                        \
    while (start && !ret)                                                               \
      {                                                                                 \
        uint##bits##_t r = v/start;                                                     \
        ret = muos_serial_tx_byte MUOS_IO_HWARG((r<10?'0':upcase?'A'-10:'a'-10)+r);     \
        v -= r*start;                                                                   \
        start /= base;                                                                  \
      }                                                                                 \
    return ret;                                                                         \
  }                                                                                     \
                                                                                        \
  static muos_error                                                                     \
  i##bits##put MUOS_IO_HWPARAM(int##bits##_t v, uint8_t base, uint8_t upcase)           \
  {                                                                                     \
    muos_error ret = muos_success;                                                      \
                                                                                        \
    if (v<0)                                                                            \
      ret = muos_serial_tx_byte MUOS_IO_HWARG('-');                                     \
                                                                                        \
    if (!ret)                                                                           \
      ret = u##bits##put MUOS_IO_HWARG((uint##bits##_t) (v<0?-v:v), base, upcase);      \
                                                                                        \
    return ret;                                                                         \
  }

Xput(8);
Xput(16);
Xput(32);
Xput(64);

#undef Xput

muos_error
muos_output_intptr MUOS_IO_HWPARAM(intptr_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret;

  switch (sizeof(intptr_t))
    {
    case 2:
      ret = i16put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
      break;
    case 4:
      ret = i32put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
      break;
    case 8:
      ret = i64put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
      break;
    }
  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];

  return ret;
}

muos_error
muos_output_uintptr MUOS_IO_HWPARAM(uintptr_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret;

  switch (sizeof(uintptr_t))
    {
    case 2:
      ret = u16put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
      break;
    case 4:
      ret = u32put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
      break;
    case 8:
      ret = u64put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
      break;
    }
  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];

  return ret;
}

muos_error
muos_output_int8 MUOS_IO_HWPARAM(int8_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret = i8put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];
  return ret;
}

muos_error
muos_output_uint8 MUOS_IO_HWPARAM(uint8_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret = u8put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];
  return ret;
}

muos_error
muos_output_int16 MUOS_IO_HWPARAM(int16_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret = i16put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];
  return ret;
}

muos_error
muos_output_uint16 MUOS_IO_HWPARAM(uint16_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret = u16put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];
  return ret;
}

muos_error
muos_output_int32 MUOS_IO_HWPARAM(int32_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret = i32put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];
  return ret;
}

muos_error
muos_output_uint32 MUOS_IO_HWPARAM(uint32_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret = u32put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];
  return ret;
}

muos_error
muos_output_int64 MUOS_IO_HWPARAM(int64_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret = i64put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];
  return ret;
}

muos_error
muos_output_uint64 MUOS_IO_HWPARAM(uint64_t n)
{
  MUOS_IO_HWCHECK;
  muos_error ret = u64put MUOS_IO_HWARG(n, fmtconfig[MUOS_IO_HWINDEX].base, fmtconfig[MUOS_IO_HWINDEX].upcase);
  fmtconfig[MUOS_IO_HWINDEX] = pfmtconfig[MUOS_IO_HWINDEX];
  return ret;
}

#if 0
muos_error
muos_output_float MUOS_IO_HWPARAM(float)
{
  MUOS_IO_HWCHECK;
  muos_output_cstr MUOS_IO_HWARG("PLANNED");
}


muos_error
muos_output_cstr_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  return muos_error_error;
}

muos_error
muos_output_mem_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;
  return muos_error_error;
}

muos_error
muos_output_int16_R MUOS_IO_HWPARAM()
{

  MUOS_IO_HWCHECK;
  return muos_error_error;
}

muos_error
muos_output_uint16_R MUOS_IO_HWPARAM()
{

  MUOS_IO_HWCHECK;
  return muos_error_error;
}

muos_error
muos_output_int32_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}

muos_error
muos_output_uint32_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}

muos_error
muos_output_int64_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}

muos_error
muos_output_uint64_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}

muos_error
muos_output_float_R MUOS_IO_HWPARAM()
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}
#endif

muos_error
muos_output_upcase MUOS_IO_HWPARAM(bool upcase)
{
  MUOS_IO_HWCHECK;
  fmtconfig[MUOS_IO_HWINDEX].upcase = upcase;
  return muos_success;
}


muos_error
muos_output_base MUOS_IO_HWPARAM(uint8_t base)
{
  MUOS_IO_HWCHECK;
  fmtconfig[MUOS_IO_HWINDEX].base = base;
  return muos_success;
}


muos_error
muos_output_pupcase MUOS_IO_HWPARAM(bool upcase)
{
  MUOS_IO_HWCHECK;
  pfmtconfig[MUOS_IO_HWINDEX].upcase = fmtconfig[MUOS_IO_HWINDEX].upcase = upcase;
  return muos_success;
}


muos_error
muos_output_pbase MUOS_IO_HWPARAM(uint8_t base)
{
  MUOS_IO_HWCHECK;
  pfmtconfig[MUOS_IO_HWINDEX].base = fmtconfig[MUOS_IO_HWINDEX].base = base;
  return muos_success;
}








#if 0

muos_error
muos_output_ifmt MUOS_IO_HWPARAM(uint8_t, uint8_t)
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}

muos_error
muos_output_ffmt MUOS_IO_HWPARAM(char)
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}

muos_error
muos_output_pcase MUOS_IO_HWPARAM(char)
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}

muos_error
muos_output_pbase MUOS_IO_HWPARAM(uint8_t)
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}


muos_error
muos_output_pifmt MUOS_IO_HWPARAM(uint8_t, uint8_t)
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}

muos_error
muos_output_pffmt MUOS_IO_HWPARAM(char)
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}


muos_error
muos_output_ctrl MUOS_IO_HWPARAM(uint8_t, uint8_t, uint8_t)
{
  MUOS_IO_HWCHECK;

  return muos_error_error;
}


#endif

#endif


#endif
