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

#include <muos/error.h>


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


//TODO: I/O and interrupt safety

#define MUOS_OUTPUTFNS                                                                          \
  OUTPUTFN(char, char) /* a single character */                                                 \
  OUTPUTFN(cstr, const char*) /* a c-string */                                                  \
  OUTPUTFN(cstrn, const char*, uint8_t) /* c-string with given maximal length */                \
  OUTPUTFN(fstr, muos_flash_cstr) /* c-string stored in flash ROM */                            \
  OUTPUTFN(mem, const uint8_t*, uint8_t) /* raw memory with given address and length */         \
  OUTPUTFN(nl, void) /* system dependent newline sequence */                                    \
  OUTPUTFN(csi_char, const char) /* CSI followed by a single character */                       \
  OUTPUTFN(csi_cstr, const char*) /* CSI followed by a c-string */                              \
  OUTPUTFN(csi_fstr, muos_flash_cstr) /* CSI followed by a c-string stored in flash ROM */      \
  OUTPUTFN(intptr, intptr_t) /* signed numeric value of a pointer */                            \
  OUTPUTFN(uintptr, uintptr_t) /* unsigned numeric value of a pointer */                        \
  OUTPUTFN(int8, int8_t) /* signed numeric value */                                             \
  OUTPUTFN(uint8, uint8_t) /* unsigned numeric value */                                         \
  OUTPUTFN(int16, int16_t) /* signed numeric value */                                           \
  OUTPUTFN(uint16, uint16_t) /* unsigned numeric value */                                       \
  OUTPUTFN(int32, int32_t) /* signed numeric value */                                           \
  OUTPUTFN(uint32, uint32_t) /* unsigned numeric value */                                       \
  OUTPUTFN(int64, int64_t) /* signed numeric value */                                           \
  OUTPUTFN(uint64, uint64_t) /* unsigned numeric value */                                       \
  OUTPUTFN(cstr_R, const char*) /* c-string by reference */                                     \
  OUTPUTFN(cstrn_R, const char*, uint8_t) /* c-string by reference with given maximal length */ \
  OUTPUTFN(mem_R) /* raw memory with given address and length by reference */                   \
  OUTPUTFN(int32_R) /* signed numeric value by reference */                                     \
  OUTPUTFN(uint32_R) /* unsigned numeric value by reference */                                  \
  OUTPUTFN(int64_R) /* signed numeric value by reference */                                     \
  OUTPUTFN(uint64_R) /* unsigned numeric value by reference */                                  \
  OUTPUTFN(upcase, bool) /* set digit representation for the next integer conversion */         \
  OUTPUTFN(base, uint8_t) /* set numeric base for the next integer conversion (2-36) */                \
  OUTPUTFN(pupcase, bool) /* set default digit representation */                                \
  OUTPUTFN(pbase, uint8_t) /* set default numeric base (2-36) */                                       \

//PLANNED:  OUTPUTFN(float, float) /* */
//PLANNED:  OUTPUTFN(float_R) /* */
//PLANNED:  OUTPUTFN(ifmt, uint8_t, uint8_t) /* */
//PLANNED:  OUTPUTFN(ffmt, char) /* */
//PLANNED:  OUTPUTFN(pifmt, uint8_t, uint8_t) /* */
//PLANNED:  OUTPUTFN(pffmt, char) /* */
//PLANNED:  OUTPUTFN(ctrl, uint8_t, uint8_t, uint8_t) /* */


//outputfn:cstr_P
//: +muos_output_cstr_P ("literal")+::
//:  puts "literal" into flash section and uses +muos_output_fstr+ for printing it
//outputfn:csi_cstr_P
//: +muos_output_csi_cstr_P ("literal")+::
//:  puts "literal" into flash section and uses +muos_output_csi_fstr+ for printing it
#define muos_output_cstr_P(s) muos_output_fstr (MUOS_PSTR(s))
#define muos_output_csi_cstr_P(s) muos_output_csi_fstr (MUOS_PSTR(s))


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

#define OUTPUTFN(name, ...) muos_error muos_output_##name (__VA_ARGS__);
MUOS_OUTPUTFNS
#undef OUTPUTFN

#else // TXQUEUE enabled

#define OUTPUTFN(name, ...) muos_error muos_txqueue_output_##name (__VA_ARGS__);
MUOS_OUTPUTFNS
#undef OUTPUTFN

static inline void
muos_output_char (char c)
{
  muos_txqueue_output_char (c);
}


static inline void
muos_output_cstr (const char* str)
{
  muos_txqueue_output_cstr (str);
}


static inline void
muos_output_cstrn (const char* str, uint8_t n)
{
  muos_txqueue_output_cstrn (str, n);
}


static inline void
muos_output_fstr (muos_flash_cstr str)
{
  muos_txqueue_output_fstr (str);
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
  muos_txqueue_output_csi_char (c);
}


static inline void
muos_output_csi_cstr (const char* str)
{
  muos_txqueue_output_csi_cstr (str);
}


static inline void
muos_output_csi_fstr (muos_flash_cstr str)
{
  muos_txqueue_output_csi_fstr (str);
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
