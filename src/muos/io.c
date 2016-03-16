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
//DOCME: bare io just fails when buffers are full
//PLANNED: fixed point for integers and mille delimiters

void
muos_output_char (char c)
{
  muos_serial_tx_byte (c);
}

void
muos_output_repeat_char (uint8_t rep, char c)
{
  while (rep--)
    muos_serial_tx_byte (c);
}


void
muos_output_cstr (const char* str)
{
  while (*str)
    muos_serial_tx_byte (*str++);
}


void
muos_output_cstrn (const char* str, uint8_t n)
{
  while (*str && n--)
    muos_serial_tx_byte (*str++);
}


void
muos_output_fstr (muos_flash_cstr str)
{
  for (uint8_t b  = pgm_read_byte (str);
       b;
       b  = pgm_read_byte (++str))
    {
      muos_serial_tx_byte (b);
    }
}


void
muos_output_repeat_cstr (uint8_t rep, const char* str)
{
  while (rep--)
    {
      const char* p = str;
      while (*p)
        muos_serial_tx_byte (*p++);
    }
}


void
muos_output_mem (const uint8_t* mem, uint8_t len)
{
  while (len--)
    muos_serial_tx_byte (*mem++);
}


void
muos_output_nl (void)
{
  muos_serial_tx_byte ('\r');
  muos_serial_tx_byte ('\n');
}


void
muos_output_csi_char (const char c)
{
  muos_serial_tx_byte (0x1b);
  muos_serial_tx_byte ('[');
  muos_serial_tx_byte (c);
}


void
muos_output_csi_cstr (const char* str)
{
  muos_serial_tx_byte (0x1b);
  muos_serial_tx_byte ('[');

  if (str)
    while (*str)
      muos_serial_tx_byte (*str++);
}


void
muos_output_csi_fstr (muos_flash_cstr str)
{
  muos_serial_tx_byte (0x1b);
  muos_serial_tx_byte ('[');

  muos_output_fstr (str);
}


//FIXME: broken, port back from txqueue
#define Xput(bits)                                                                      \
  static void                                                                           \
  u##bits##put (uint##bits##_t v, uint8_t base, uint8_t upcase)                         \
  {                                                                                     \
    if(!v)                                                                              \
      {                                                                                 \
        muos_serial_tx_byte ('0');                                                      \
        return;                                                                         \
      }                                                                                 \
                                                                                        \
    uint##bits##_t start = 1;                                                           \
                                                                                        \
    for (uint##bits##_t i = base; i && i <= (uint##bits##_t)-1/2+1 && i<=v; i = i*base) \
      start = i;                                                                        \
                                                                                        \
    while (start)                                                                       \
      {                                                                                 \
        uint##bits##_t r = v/start;                                                     \
        if(r || upcase&2)                                                               \
          {                                                                             \
            muos_serial_tx_byte ((r<10?'0':(upcase&1?'A'-10:'a'-10))+r);                \
            upcase|=2;                                                                  \
          }                                                                             \
        v-=r*start;                                                                     \
        start /= base;                                                                  \
      }                                                                                 \
  }                                                                                     \
                                                                                        \
  static void                                                                           \
  i##bits##put (int##bits##_t v, uint8_t base, uint8_t upcase)                          \
  {                                                                                     \
    if (v<0)                                                                            \
      muos_serial_tx_byte ('-');                                                        \
                                                                                        \
    u##bits##put ((uint##bits##_t) (v<0?-v:v), base, upcase);                           \
  }

Xput(8);
Xput(16);
Xput(32);
Xput(64);

#undef Xput

void
muos_output_intptr (intptr_t n)
{
  switch (sizeof(intptr_t))
    {
    case 2:
      i16put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    case 4:
      i32put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    case 8:
      i64put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    }
  fmtconfig = pfmtconfig;
}

void
muos_output_uintptr (uintptr_t n)
{
  switch (sizeof(uintptr_t))
    {
    case 2:
      u16put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    case 4:
      u32put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    case 8:
      u64put (n, fmtconfig.base, fmtconfig.upcase);
      break;
    }
  fmtconfig = pfmtconfig;
}

void
muos_output_int8 (int8_t n)
{
  i8put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
}

void
muos_output_uint8 (uint8_t n)
{
  u8put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
}

void
muos_output_int16 (int16_t n)
{
  i16put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
}

void
muos_output_uint16 (uint16_t n)
{
  u16put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
}

void
muos_output_int32 (int32_t n)
{
  i32put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
}

void
muos_output_uint32 (uint32_t n)
{
  u32put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
}

void
muos_output_int64 (int64_t n)
{
  i64put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
}

void
muos_output_uint64 (uint64_t n)
{
  u64put (n, fmtconfig.base, fmtconfig.upcase);
  fmtconfig = pfmtconfig;
}

#if 0
void
muos_output_float (float)
{
  muos_output_cstr ("PLANNED");
}


void
muos_output_cstr_R ()
{

}

void
muos_output_mem_R ()
{

}

void
muos_output_int16_R ()
{

}

void
muos_output_uint16_R ()
{

}

void
muos_output_int32_R ()
{

}

void
muos_output_uint32_R ()
{

}

void
muos_output_int64_R ()
{

}

void
muos_output_uint64_R ()
{

}

void
muos_output_float_R ()
{

}
#endif

void
muos_output_upcase (bool upcase)
{
  fmtconfig.upcase = upcase;
}


void
muos_output_base (uint8_t base)
{
  fmtconfig.base = base;
}


void
muos_output_pupcase (bool upcase)
{
  pfmtconfig.upcase = fmtconfig.upcase = upcase;
}


void
muos_output_pbase (uint8_t base)
{
  pfmtconfig.base = fmtconfig.base = base;
}








#if 0

void
muos_output_ifmt (uint8_t, uint8_t)
{

}

void
muos_output_ffmt (char)
{

}

void
muos_output_pcase (char)
{

}

oid
muos_output_pbase (uint8_t)
{

}


void
muos_output_pifmt (uint8_t, uint8_t)
{

}

void
muos_output_pffmt (char)
{

}


void
muos_output_ctrl (uint8_t, uint8_t, uint8_t)
{

}


#endif

#endif
