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

#ifndef MUOS_IO_H
#define MUOS_IO_H

#include <stdint.h>
#include <stdbool.h>


struct fmtconfig_type {
  uint8_t base:6;
  uint8_t upcase:1;
  uint8_t eng:1;
  uint8_t flt_int:4;
  uint8_t flt_frac:4;
};



struct ctrlseq_type {
  uint8_t style:2;
  uint8_t fgcolor:3;
  uint8_t bgcolor:3;
};

extern struct fmtconfig_type pfmtconfig;
extern struct fmtconfig_type fmtconfig;




#define MUOS_OUTPUTFNS                          \
  OUTPUTFN(char, char)                          \
  OUTPUTFN(repeat_char, uint8_t, char)          \
  OUTPUTFN(cstr, const char*)                   \
  OUTPUTFN(repeat_cstr, uint8_t, const char*)   \
  OUTPUTFN(mem, const uint8_t*, uint8_t)        \
  OUTPUTFN(nl, void)                            \
  OUTPUTFN(csi_char, const char)                \
  OUTPUTFN(csi_cstr, const char*)               \
  OUTPUTFN(intptr, intptr_t)                    \
  OUTPUTFN(uintptr, uintptr_t)                  \
  OUTPUTFN(int8, int8_t)                        \
  OUTPUTFN(uint8, uint8_t)                      \
  OUTPUTFN(int16, int16_t)                      \
  OUTPUTFN(uint16, uint16_t)                    \
  OUTPUTFN(int32, int32_t)                      \
  OUTPUTFN(uint32, uint32_t)                    \
  OUTPUTFN(int64, int64_t)                      \
  OUTPUTFN(uint64, uint64_t)                    \
  OUTPUTFN(float, float)                        \
  OUTPUTFN(cstr_R)                              \
  OUTPUTFN(mem_R)                               \
  OUTPUTFN(int32_R)                             \
  OUTPUTFN(uint32_R)                            \
  OUTPUTFN(int64_R)                             \
  OUTPUTFN(uint64_R)                            \
  OUTPUTFN(float_R)                             \
  OUTPUTFN(upcase, bool)                        \
  OUTPUTFN(base, uint8_t)                       \
  OUTPUTFN(ifmt, uint8_t, uint8_t)              \
  OUTPUTFN(ffmt, char)                          \
  OUTPUTFN(pupcase, bool)                       \
  OUTPUTFN(pbase, uint8_t)                      \
  OUTPUTFN(pifmt, uint8_t, uint8_t)             \
  OUTPUTFN(pffmt, char)                         \
  OUTPUTFN(ctrl, uint8_t, uint8_t, uint8_t)


/*
ctrl
 style:
  normal
  bold
  italic
  underline

 fgcolor
  Black Red Green Yellow Blue Magenta Cyan White
 bgcolor
  Black Red Green Yellow Blue Magenta Cyan White
*/



//PLANNED: printf compatible sequence parser
//PLANNED: simpler serquencer MUOS_OUTPUT(cstr("foo"),...)
//#define MUOS_OUTPUT(...) MUOS_OUTPUT_(__VA_ARGS__, END)



#if MUOS_SERIAL_TXQUEUE == 0

#define OUTPUTFN(name, ...) void muos_output_##name (__VA_ARGS__);
MUOS_OUTPUTFNS
#undef OUTPUTFN

#else // TXQUEUE enabled

#define OUTPUTFN(name, ...) void muos_txqueue_output_##name (__VA_ARGS__);
MUOS_OUTPUTFNS
#undef OUTPUTFN

static inline void
muos_output_char (char c)
{
  muos_txqueue_output_char (c);
}

static inline void
muos_output_repeat_char (uint8_t rep, char c)
{
  (void) rep;
  (void) c;
}


static inline void
muos_output_cstr (const char* str)
{
  muos_txqueue_output_cstr (str);
}


static inline void
muos_output_repeat_cstr (uint8_t rep, const char* str)
{
  (void) rep;
  (void) str;
}


static inline void
muos_output_mem (const uint8_t* mem, uint8_t len)
{
  (void) mem;
  (void) len;

}


static inline void
muos_output_nl (void)
{
   muos_txqueue_output_nl ();
}


static inline void
muos_output_csi_char (const char c)
{
  (void) c;
}


static inline void
muos_output_csi_cstr (const char* str)
{
  (void) str;
}

static inline void
muos_output_intptr (intptr_t n)
{
  muos_txqueue_output_intptr (n);
}

static inline void
muos_output_uintptr (uintptr_t n)
{
  muos_txqueue_output_uintptr (n);
}

static inline void
muos_output_int8 (int8_t n)
{
  muos_txqueue_output_int8 (n);
}

static inline void
muos_output_uint8 (uint8_t n)
{
  muos_txqueue_output_uint8 (n);
}

static inline void
muos_output_int16 (uint16_t n)
{
  muos_txqueue_output_int16 (n);
}

static inline void
muos_output_uint16 (uint16_t n)
{
  muos_txqueue_output_uint16 (n);
}

static inline void
muos_output_int32 (int32_t n)
{
  muos_txqueue_output_int32 (n);
}

static inline void
muos_output_uint32 (uint32_t n)
{
  muos_txqueue_output_uint32 (n);
}

#if 0 //PLANNED: 64 bit
static inline void
muos_output_int64 (int64_t n)
{
  (void) n;
}

static inline void
muos_output_uint64 (uint64_t n)
{
  (void) n;
}
#endif

#if 0
static inline void
muos_output_float (float)
{
  (void) str;
}


static inline void
muos_output_cstr_R ()
{
  (void) str;
}

static inline void
muos_output_mem_R ()
{
  (void) str;
}

static inline void
muos_output_int16_R ()
{
  (void) str;
}

static inline void
muos_output_uint16_R ()
{
  (void) str;
}

static inline void
muos_output_int32_R ()
{
  (void) str;
}

static inline void
muos_output_uint32_R ()
{
  (void) str;

}

static inline void
muos_output_int64_R ()
{
  (void) str;

}

static inline void
muos_output_uint64_R ()
{
  (void) str;

}

static inline void
muos_output_float_R ()
{
  (void) str;

}
#endif

static inline void
muos_output_upcase (bool upcase)
{
  muos_txqueue_output_upcase (upcase);
}


static inline void
muos_output_base (uint8_t base)
{
  muos_txqueue_output_base (base);
}


static inline void
muos_output_pupcase (bool upcase)
{
  muos_txqueue_output_pupcase (upcase);
}


static inline void
muos_output_pbase (uint8_t base)
{
  muos_txqueue_output_pbase (base);
}








#if 0

static inline void
muos_output_ifmt (uint8_t, uint8_t)
{
  (void) str;

}

static inline void
muos_output_ffmt (char)
{
  (void) str;

}

static inline void
muos_output_pcase (char)
{
  (void) str;

}

oid
muos_output_pbase (uint8_t)
{
  (void) str;

}


static inline void
muos_output_pifmt (uint8_t, uint8_t)
{
  (void) str;

}

static inline void
muos_output_pffmt (char)
{

  (void) str;
}


static inline void
muos_output_ctrl (uint8_t, uint8_t, uint8_t)
{

  (void) str;
}


#endif
#endif

#endif
