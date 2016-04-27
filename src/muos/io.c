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

#include <muos/muos.h>
#include <muos/serial.h>
#include <muos/io.h>


#include <muos/bgq.h>
#include <muos/txqueue.h>

struct fmtconfig_type pfmtconfig = {10, 0, 0, 15, 15};
struct fmtconfig_type fmtconfig = {10, 0, 0, 15, 15};


#if MUOS_SERIAL_TXQUEUE == 0
//PLANNED: fixed point for integers and mille delimiters

muos_error
muos_output_char (char c)
{
  return muos_serial_tx_byte (c);
}


muos_error
muos_output_cstr (const char* str)
{
  muos_error ret = muos_success;

  while (*str && !ret)
    ret = muos_serial_tx_byte (*str++);

  return ret;
}


muos_error
muos_output_cstrn (const char* str, uint8_t n)
{
  muos_error ret = muos_success;
  while (*str && n-- && !ret)
    ret = muos_serial_tx_byte (*str++);

  return ret;
}


muos_error
muos_output_fstr (muos_flash_cstr str)
{
  muos_error ret = muos_success;

  for (uint8_t b  = pgm_read_byte (str);
       b && !ret;
       b  = pgm_read_byte (++str))
    {
      ret = muos_serial_tx_byte (b);
    }

  return ret;
}


muos_error
muos_output_mem (const uint8_t* mem, uint8_t len)
{
  muos_error ret = muos_success;

  while (len-- && !ret)
    ret = muos_serial_tx_byte (*mem++);

  return ret;
}

#define retcont(expr) ret = ret?ret:(expr)

muos_error
muos_output_nl (void)
{
  muos_error ret = muos_serial_tx_byte ('\r');

  retcont (muos_serial_tx_byte ('\n'));

  return ret;
}


muos_error
muos_output_csi_char (const char c)
{
  muos_error ret = muos_serial_tx_byte (0x1b);
  retcont (muos_serial_tx_byte ('['));
  retcont (muos_serial_tx_byte (c));

  return ret;
}


muos_error
muos_output_csi_cstr (const char* str)
{
  muos_error ret = muos_serial_tx_byte (0x1b);
  retcont (muos_serial_tx_byte ('['));
  retcont (muos_output_cstr (str));

  return ret;
}


muos_error
muos_output_csi_fstr (muos_flash_cstr str)
{
  muos_error ret = muos_serial_tx_byte (0x1b);
  retcont (muos_serial_tx_byte ('['));
  retcont (muos_output_fstr (str));

  return ret;
}


#define Xput(bits)                                                      \
  static uint##bits##_t                                                 \
  u##bits##put (uint##bits##_t v, uint8_t base, bool upcase)            \
  {                                                                     \
    uint8_t digits = 0;                                                 \
                                                                        \
    for (uint##bits##_t tmp=v; tmp; tmp /= base)                        \
      ++digits;                                                         \
                                                                        \
    uint##bits##_t start = 1;                                           \
                                                                        \
    for (uint##bits##_t i = digits-1; i; --i)                           \
      {                                                                 \
        start *= base;                                                  \
      }                                                                 \
                                                                        \
    muos_error ret = muos_success;                                      \
                                                                        \
    while (start && !ret)                                               \
      {                                                                 \
        uint##bits##_t r = v/start;                                     \
        ret = muos_serial_tx_byte ((r<10?'0':upcase?'A'-10:'a'-10)+r);  \
        v -= r*start;                                                   \
        start /= base;                                                  \
      }                                                                 \
    return ret;                                                         \
    }                                                                   \
                                                                        \
     static muos_error                                                  \
  i##bits##put (int##bits##_t v, uint8_t base, uint8_t upcase)          \
  {                                                                     \
    muos_error ret = muos_success;                                      \
                                                                        \
    if (v<0)                                                            \
      ret =  muos_serial_tx_byte ('-');                                 \
                                                                        \
    if (ret)                                                            \
      ret = u##bits##put ((uint##bits##_t) (v<0?-v:v), base, upcase);   \
                                                                        \
    return ret;                                                         \
  }




Xput(8);
Xput(16);
Xput(32);
Xput(64);

#undef Xput

muos_error
muos_output_intptr (intptr_t n)
{
  muos_error ret;

  switch (sizeof(intptr_t))
    {
    case 2:
      ret = i16put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    case 4:
      ret = i32put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    case 8:
      ret = i64put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    }
  fmtconfig = pfmtconfig;

  return ret;
}

muos_error
muos_output_uintptr (uintptr_t n)
{
  muos_error ret;

  switch (sizeof(uintptr_t))
    {
    case 2:
      ret = u16put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    case 4:
      ret = u32put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    case 8:
      ret = u64put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    }
  fmtconfig = pfmtconfig;

  return ret;
}

muos_error
muos_output_int8 (int8_t n)
{
  muos_error ret = i8put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
  return ret;
}

muos_error
muos_output_uint8 (uint8_t n)
{
  muos_error ret = u8put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
  return ret;
}

muos_error
muos_output_int16 (int16_t n)
{
  muos_error ret = i16put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
  return ret;
}

muos_error
muos_output_uint16 (uint16_t n)
{
  muos_error ret = u16put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
  return ret;
}

muos_error
muos_output_int32 (int32_t n)
{
  muos_error ret = i32put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
  return ret;
}

muos_error
muos_output_uint32 (uint32_t n)
{
  muos_error ret = u32put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
  return ret;
}

muos_error
muos_output_int64 (int64_t n)
{
  muos_error ret = i64put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
  return ret;
}

muos_error
muos_output_uint64 (uint64_t n)
{
  muos_error ret = u64put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
  return ret;
}

#if 0
muos_error
muos_output_float (float)
{
  muos_output_cstr ("PLANNED");
}


muos_error
muos_output_cstr_R ()
{
  return muos_error_error;
}

muos_error
muos_output_mem_R ()
{
  return muos_error_error;
}

muos_error
muos_output_int16_R ()
{

  return muos_error_error;
}

muos_error
muos_output_uint16_R ()
{

  return muos_error_error;
}

muos_error
muos_output_int32_R ()
{

  return muos_error_error;
}

muos_error
muos_output_uint32_R ()
{

  return muos_error_error;
}

muos_error
muos_output_int64_R ()
{

  return muos_error_error;
}

muos_error
muos_output_uint64_R ()
{

  return muos_error_error;
}

muos_error
muos_output_float_R ()
{

  return muos_error_error;
}
#endif

muos_error
muos_output_upcase (bool upcase)
{
  fmtconfig.upcase = upcase;
  return muos_success;
}


muos_error
muos_output_base (uint8_t base)
{
  fmtconfig.base = base;
  return muos_success;
}


muos_error
muos_output_pupcase (bool upcase)
{
  pfmtconfig.upcase = fmtconfig.upcase = upcase;
  return muos_success;
}


muos_error
muos_output_pbase (uint8_t base)
{
  pfmtconfig.base = fmtconfig.base = base;
  return muos_success;
}








#if 0

muos_error
muos_output_ifmt (uint8_t, uint8_t)
{

  return muos_error_error;
}

muos_error
muos_output_ffmt (char)
{

  return muos_error_error;
}

muos_error
muos_output_pcase (char)
{

  return muos_error_error;
}

oid
muos_output_pbase (uint8_t)
{

  return muos_error_error;
}


muos_error
muos_output_pifmt (uint8_t, uint8_t)
{

  return muos_error_error;
}

muos_error
muos_output_pffmt (char)
{

  return muos_error_error;
}


muos_error
muos_output_ctrl (uint8_t, uint8_t, uint8_t)
{

  return muos_error_error;
}


#endif

#endif
