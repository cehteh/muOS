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

#include <muos/lib/cbuffer.h>
#include <muos/error.h>



//PLANNED: #define MUOS_IO .. if undefined then avr-libc stdio, check size requirement if it makes sense

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

extern struct fmtconfig_type pfmtconfig[MUOS_SERIAL_NUM];
extern struct fmtconfig_type fmtconfig[MUOS_SERIAL_NUM];


//TODO: DOCME I/O and interrupt safety

#define MUOS_OUTPUTFNS                                                                                  \
  OUTPUTFN(char, char) /* a single character */                                                         \
    OUTPUTFN(cstr, const char*) /* a c-string */                                                        \
    OUTPUTFN(cstrn, const char*, uint8_t) /* c-string with given maximal length */                      \
    OUTPUTFN(fstr, muos_flash_cstr) /* c-string stored in flash ROM */                                  \
    OUTPUTFN(mem, const uint8_t*, uint8_t) /* raw memory with given address and length */               \
    OUTPUTFN(nl) /* system dependent newline sequence */                                                \
    OUTPUTFN(csi_char, const char) /* CSI followed by a single character */                             \
    OUTPUTFN(csi_cstr, const char*) /* CSI followed by a c-string */                                    \
    OUTPUTFN(csi_fstr, muos_flash_cstr) /* CSI followed by a c-string stored in flash ROM */            \
    OUTPUTFN(intptr, intptr_t) /* signed numeric value of a pointer */                                  \
    OUTPUTFN(uintptr, uintptr_t) /* unsigned numeric value of a pointer */                              \
    OUTPUTFN(int8, int8_t) /* signed numeric value */                                                   \
    OUTPUTFN(uint8, uint8_t) /* unsigned numeric value */                                               \
    OUTPUTFN(int16, int16_t) /* signed numeric value */                                                 \
    OUTPUTFN(uint16, uint16_t) /* unsigned numeric value */                                             \
    OUTPUTFN(int32, int32_t) /* signed numeric value */                                                 \
    OUTPUTFN(uint32, uint32_t) /* unsigned numeric value */                                             \
    OUTPUTFN(int64, int64_t) /* signed numeric value */                                                 \
    OUTPUTFN(uint64, uint64_t) /* unsigned numeric value */                                             \
    OUTPUTFN(cstr_R, const char*) /* c-string by reference */                                           \
    OUTPUTFN(cstrn_R, const char*, uint8_t) /* c-string by reference with given maximal length */       \
    OUTPUTFN(mem_R) /* raw memory with given address and length by reference */                         \
    OUTPUTFN(int32_R) /* signed numeric value by reference */                                           \
    OUTPUTFN(uint32_R) /* unsigned numeric value by reference */                                        \
    OUTPUTFN(int64_R) /* signed numeric value by reference */                                           \
    OUTPUTFN(uint64_R) /* unsigned numeric value by reference */                                        \
    OUTPUTFN(upcase, bool) /* set digit representation for the next integer conversion */               \
    OUTPUTFN(base, uint8_t) /* set numeric base for the next integer conversion (2-36) */               \
    OUTPUTFN(pupcase, bool) /* set default digit representation */                                      \
    OUTPUTFN(pbase, uint8_t) /* set default numeric base (2-36) */
  ;

// for transaction support
//PLANNED:  OUTPUTFN(commit) /* */
//PLANNED:  OUTPUTFN(abort) /* */
//PLANNED:  OUTPUTFN(commitnl) /* */


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

#if MUOS_SERIAL_NUM > 1
#define MUOS_IO_HWPARAM(...) (const uint8_t hw, ## __VA_ARGS__)
#define MUOS_IO_HWARG(...) (hw, ## __VA_ARGS__)
#define MUOS_IO_HWINDEX  hw
#define MUOS_IO_HWCHECK  if (hw >= MUOS_SERIAL_NUM) return muos_error_nodev
//FIXME: insert HWCHECK where approbiate
#else
#define MUOS_IO_HWPARAM(...) (__VA_ARGS__)
#define MUOS_IO_HWARG(...) (__VA_ARGS__)
#define MUOS_IO_HWINDEX  0
#define MUOS_IO_HWCHECK
#endif



#if MUOS_SERIAL_NUM > 1
#define muos_output_cstr_P(hw, s) muos_output_fstr (hw, MUOS_PSTR(s))

struct muos_txwait
{
  uint8_t hw;
  muos_cbuffer_index space;
};

muos_error
muos_output_wait (uint8_t hw, muos_cbuffer_index space, muos_clock16 timeout);

muos_error
muos_output_lock (uint8_t hw);

muos_error
muos_output_unlock (uint8_t hw);

#else
#define muos_output_cstr_P(s) muos_output_fstr (MUOS_PSTR(s))

muos_error
muos_output_wait (muos_cbuffer_index space, muos_clock16 timeout);

muos_error
muos_output_lock (void);

muos_error
muos_output_unlock (void);

#endif

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
//PLANNED: simpler sequencer MUOS_OUTPUT(cstr("foo"),...)
//#define MUOS_OUTPUT(...) MUOS_OUTPUT_(__VA_ARGS__, END)


#ifndef MUOS_IO_TXQUEUE

#define OUTPUTFN(name, ...) muos_error muos_output_##name MUOS_IO_HWPARAM(__VA_ARGS__);
MUOS_OUTPUTFNS
#undef OUTPUTFN

#else /*  OR  */

#define OUTPUTFN(name, ...) muos_error muos_txqueue_output_##name MUOS_IO_HWPARAM(__VA_ARGS__);
MUOS_OUTPUTFNS
#undef OUTPUTFN


static inline muos_error
muos_output_char MUOS_IO_HWPARAM(char c)
{
  return muos_txqueue_output_char MUOS_IO_HWARG(c);
}


static inline muos_error
muos_output_cstr MUOS_IO_HWPARAM(const char* str)
{
  return muos_txqueue_output_cstr MUOS_IO_HWARG(str);
}


static inline muos_error
muos_output_cstrn MUOS_IO_HWPARAM(const char* str, uint8_t n)
{
  return muos_txqueue_output_cstrn MUOS_IO_HWARG(str, n);
}


static inline muos_error
muos_output_fstr MUOS_IO_HWPARAM(muos_flash_cstr str)
{
  return muos_txqueue_output_fstr MUOS_IO_HWARG(str);
}


#if 0
static inline muos_error
muos_output_mem MUOS_IO_HWPARAM(const uint8_t* mem, uint8_t len)
{
  //PLANNED: not implemented
  (void) mem;
  (void) len;
  return muos_error_error;
}
#endif

static inline muos_error
muos_output_nl MUOS_IO_HWPARAM()
{
   return muos_txqueue_output_nl MUOS_IO_HWARG();
}


static inline muos_error
muos_output_csi_char MUOS_IO_HWPARAM(const char c)
{
  return muos_txqueue_output_csi_char MUOS_IO_HWARG(c);
}


static inline muos_error
muos_output_csi_cstr MUOS_IO_HWPARAM(const char* str)
{
  return muos_txqueue_output_csi_cstr MUOS_IO_HWARG(str);
}


static inline muos_error
muos_output_csi_fstr MUOS_IO_HWPARAM(muos_flash_cstr str)
{
  return muos_txqueue_output_csi_fstr MUOS_IO_HWARG(str);
}


static inline muos_error
muos_output_intptr MUOS_IO_HWPARAM(intptr_t n)
{
  return muos_txqueue_output_intptr MUOS_IO_HWARG(n);
}

static inline muos_error
muos_output_uintptr MUOS_IO_HWPARAM(uintptr_t n)
{
  return muos_txqueue_output_uintptr MUOS_IO_HWARG(n);
}

static inline muos_error
muos_output_int8 MUOS_IO_HWPARAM(int8_t n)
{
  return muos_txqueue_output_int8 MUOS_IO_HWARG(n);
}

static inline muos_error
muos_output_uint8 MUOS_IO_HWPARAM(uint8_t n)
{
  return muos_txqueue_output_uint8 MUOS_IO_HWARG(n);
}

static inline muos_error
muos_output_int16 MUOS_IO_HWPARAM(uint16_t n)
{
  return muos_txqueue_output_int16 MUOS_IO_HWARG(n);
}

static inline muos_error
muos_output_uint16 MUOS_IO_HWPARAM(uint16_t n)
{
  return muos_txqueue_output_uint16 MUOS_IO_HWARG(n);
}

static inline muos_error
muos_output_int32 MUOS_IO_HWPARAM(int32_t n)
{
  return muos_txqueue_output_int32 MUOS_IO_HWARG(n);
}

static inline muos_error
muos_output_uint32 MUOS_IO_HWPARAM(uint32_t n)
{
  return muos_txqueue_output_uint32 MUOS_IO_HWARG(n);
}

static inline muos_error
muos_output_upcase MUOS_IO_HWPARAM(bool upcase)
{
  return muos_txqueue_output_upcase MUOS_IO_HWARG(upcase);
}


static inline muos_error
muos_output_base MUOS_IO_HWPARAM(uint8_t base)
{
  return muos_txqueue_output_base MUOS_IO_HWARG(base);
}


static inline muos_error
muos_output_pupcase MUOS_IO_HWPARAM(bool upcase)
{
  return muos_txqueue_output_pupcase MUOS_IO_HWARG(upcase);
}


static inline muos_error
muos_output_pbase MUOS_IO_HWPARAM(uint8_t base)
{
  return muos_txqueue_output_pbase MUOS_IO_HWARG(base);
}


#endif






#if 0 //PLANNED: 64 bit
static inline muos_error
muos_output_int64 MUOS_IO_HWPARAM(int64_t n)
{
  (void) n;
}

static inline muos_error
muos_output_uint64 MUOS_IO_HWPARAM(uint64_t n)
{
  (void) n;
}
#endif

#if 0
static inline muos_error
muos_output_float MUOS_IO_HWPARAM(float)
{
  (void) str;
}


static inline muos_error
muos_output_cstr_R MUOS_IO_HWPARAM()
{
  (void) str;
}

static inline muos_error
muos_output_mem_R MUOS_IO_HWPARAM()
{
  (void) str;
}

static inline muos_error
muos_output_int16_R MUOS_IO_HWPARAM()
{
  (void) str;
}

static inline muos_error
muos_output_uint16_R MUOS_IO_HWPARAM()
{
  (void) str;
}

static inline muos_error
muos_output_int32_R MUOS_IO_HWPARAM()
{
  (void) str;
}

static inline muos_error
muos_output_uint32_R MUOS_IO_HWPARAM()
{
  (void) str;

}

static inline muos_error
muos_output_int64_R MUOS_IO_HWPARAM()
{
  (void) str;

}

static inline muos_error
muos_output_uint64_R MUOS_IO_HWPARAM()
{
  (void) str;

}

static inline muos_error
muos_output_float_R MUOS_IO_HWPARAM()
{
  (void) str;

}
#endif


#if 0 //PLANNED:

static inline muos_error
muos_output_ifmt MUOS_IO_HWPARAM(uint8_t, uint8_t)
{
  (void) str;

}

static inline muos_error
muos_output_ffmt MUOS_IO_HWPARAM(char)
{
  (void) str;

}

static inline muos_error
muos_output_pcase MUOS_IO_HWPARAM(char)
{
  (void) str;

}

oid
muos_output_pbase MUOS_IO_HWPARAM(uint8_t)
{
  (void) str;

}


static inline muos_error
muos_output_pifmt MUOS_IO_HWPARAM(uint8_t, uint8_t)
{
  (void) str;

}

static inline muos_error
muos_output_pffmt MUOS_IO_HWPARAM(char)
{

  (void) str;
}


static inline muos_error
muos_output_ctrl MUOS_IO_HWPARAM(uint8_t, uint8_t, uint8_t)
{

  (void) str;
}


#endif





#endif
