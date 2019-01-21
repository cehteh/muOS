/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2018                           Christian Thäter <ct@pipapo.org>
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

#ifndef MUOS_CONFIGSTORE_H
#define MUOS_CONFIGSTORE_H

#ifndef MUOS_CONFIGSTORE_INCLUDE
#error MUOS_CONFIGSTORE_INCLUDE must be configured
#endif
#include MUOS_CONFIGSTORE_INCLUDE
#ifndef CONFIGSTORE_DATA
#error CONFIGSTORE_DATA must be defined in MUOS_CONFIGSTORE_INCLUDE
#endif

#include <muos/muos.h>
#include <muos/io.h>

/*
  Wear leveling and fault tolerant config store.

  the complete user defined configuration is expanded as one in-ram structure, extended by a
  'generation' counter and a checksum.

  In EEPROM it will be mapped as array much times as fit into the given size.

  On loading the whole EEPROM is scanned for the highest generation version with intact checksum.

  On saving the generation counter is incremented and the data is written next to the previous (wrap around)
  position, or when MUOS_CONFIGSTORE_REDUNDANT is set, at the last AND next position. Writes will be verified,
  a failed verification will advance the writing position once more and write again. When all writes (complete
  wraparound) failed then an error is returned.

  loading/saving is asynchronous but access to the configstore will be blocked (error return) while any
  operation is in progress.

//PLANNED: interleaved/wraparound configstore

 |1b1b1b1b|1b1b1b1b|...|
 |.2b2b2b2b|2a2a2a2a|..|
 |3a3a|3b3b3b3b|...3a3a|
*/



//configstore_api:
//: .Configuration Description
//: ----
//: #define CONFIGSTORE_DATA             {__BACKSLASH__}
//FIXME: doc not correct lacks defaults validation
//:   ENTRY(type, ary, name) {__BACKSLASH__}
//:   ...
//: ----
//:
//: +type+::
//:   C type of the data.
//:
//: +ary+::
//:   Size for arrays. 0 for single variables. Otherwise one of
//:   2, 3, 4, 5, 6, 7,8, 16, 32, 64, 128, or ARRAY(n) to define the size
//:   of an array.
//:
//: +name+::
//:   C identifier for the variable.
//:
//: Configuration variables are defined by a C-Preprocessor defined DSL in a single included file.
//: The 'MUOS_CONFIGSTORE_INCLUDE' configuration from the 'Makefile' must point to this file.
//:
//: The user defines 'CONFIGSTORE_DATA' to a sequence of 'ENTRY(type, ary, name)'
//: definitions. MµOS uses this to expand the provided data into various datastructures.
//:


typedef void (*muos_configstore_callback)(void);



typedef enum
  {
   CONFIGSTORE_UNKNOWN,       // status not known yet, did not called load
   CONFIGSTORE_INVALID,       // load did not find a saved config, size mismatch
   CONFIGSTORE_DEAD,          // 'save' could not save+verify data, EEPROM is dead (but ram is ok)
   CONFIGSTORE_VALID,         // successfully loaded config
   CONFIGSTORE_RLOCK,         // readlocks start here, count up
   CONFIGSTORE_RLOCK_MAX=254, // end of readlocks
   CONFIGSTORE_WLOCK=255,     // write lock
  } muos_configstore_status;

muos_configstore_status
muos_configstore_get_status (void);


//bool muosconfig_loaded (intptr_t unused)


#define CONFIGSTORE_ARY_ARRAY(len) [len]
#define CONFIGSTORE_ARY_0
#define CONFIGSTORE_ARY_1 [1]
#define CONFIGSTORE_ARY_2 [2]
#define CONFIGSTORE_ARY_3 [3]
#define CONFIGSTORE_ARY_4 [4]
#define CONFIGSTORE_ARY_5 [5]
#define CONFIGSTORE_ARY_6 [6]
#define CONFIGSTORE_ARY_7 [7]
#define CONFIGSTORE_ARY_8 [8]
#define CONFIGSTORE_ARY_16 [16]
#define CONFIGSTORE_ARY_32 [32]
#define CONFIGSTORE_ARY_64 [64]
#define CONFIGSTORE_ARY_128 [128]

#define CONFIGSTORE_ARY(val) CONFIGSTORE_ARY_##val

//TODO: add callback when config gets changed for reinitializeing dependencies
//TODO: document implicit config_size
#define CONFIGSTORE_DATA_IMPL                           \
  ENTRY(uint16_t, 0, config_size, 0,                    \
        2, 65535,                                       \
        "configuration structure size")                 \
    CONFIGSTORE_DATA

struct muos_configstore_data
{
#define string char
#define ENTRY(type, ary, name, default, min, max, descr) type name CONFIGSTORE_ARY(ary);
  CONFIGSTORE_DATA_IMPL
#undef ENTRY
#undef string
};


enum muos_configstore_id
  {
#define ENTRY(type, ary, name, default, min, max, descr) CONFIGSTORE_ID_##name,
   CONFIGSTORE_DATA_IMPL
#undef ENTRY
   CONFIGSTORE_MAX_ID
  };


// simple attributes/access control, only affects output and set commands
//TODO: implement me
typedef enum
  {
   CONFIGSTORE_PUBLIC = 0,  // all granted
   CONFIGSTORE_HIDDEN = 1,  // hide from listing
   CONFIGSTORE_WPROT  = 2,  // needs auth for set
   CONFIGSTORE_RPROT  = 4,  // needs auth for output
  } muos_configstore_access;

// the types supported by the configstore
#define MUOS_CONFIGSTORE_TYPES                  \
  TYPE(int8_t)                                  \
    TYPE(uint8_t)                               \
    TYPE(int16_t)                               \
    TYPE(uint16_t)                              \
    TYPE(int32_t)                               \
    TYPE(string) /* string as char array */

// no  TYPE(uint32_t), so we can use strtol when setting



enum muos_configstore_type
  {
#define TYPE(type) MUOS_CONFIGSTORE_TYPE_##type,
   MUOS_CONFIGSTORE_TYPES
#undef TYPE
   MUOS_CONFIGSTORE_MAX_TYPE
  };



enum muos_configstore_type
muos_configstore_type (enum muos_configstore_id id);


uint8_t
muos_configstore_ary (enum muos_configstore_id id);

muos_error
muos_configstore_output_name MUOS_IO_HWPARAM(enum muos_configstore_id id);

muos_error
muos_configstore_output_value MUOS_IO_HWPARAM(enum muos_configstore_id id, uint8_t index);

muos_error
muos_configstore_set (char* var, uint8_t index, char* val);


// API

// loading/saving

//configstore_api:
//: .Loading and Saving
//: ----
//: typedef void (*muos_configstore_callback)(void)
//:
//: muos_error
//: muos_configstore_load (muos_configstore_callback callback)
//:
//: muos_error
//: muos_configstore_save (muos_configstore_callback callback)
//: ----
//:
//: +callback+::
//:   function called on completion.
//:
//: Both functions return 'muos_error_configstore_locked' in case
//: of an error. Other errors should be handled in 'callback'.
muos_error
muos_configstore_load (muos_configstore_callback callback);


muos_error
muos_configstore_save (muos_configstore_callback callback);


//configstore_api:
//: .Access
//: ----
//: const struct muos_configstore_data*
//: muos_configstore_lock (void)
//:
//: struct muos_configstore_data*
//: muos_configstore_wlock (void)
//:
//: void
//: muos_configstore_unlock (const struct muos_configstore_data** lock)
//:
//: void
//: muos_configstore_unwlock (struct muos_configstore_data** lock)
//:
//: struct muos_configstore_data*
//: muos_configstore_initial (void)
//: ----
//:
//: The configuration data implements a simple locking scheme with multiple
//: readers or a single writer. Each successful lock must be paired with a
//: unlock, no further consistency checks are made!
//:
//: Locking works only on valid data.
//:
//: There are approx. 250 read locks available. Exceeding this number makes the
//: lock fail.
//:
//: 'muos_configstore_lock ()';;
//:   Checks for availability of the configstore.
//:   On success it places a read lock on the configstore and returns a pointer
//:   to a const 'muos_configstore_data' data structure which holds all the defined
//:   elements. On failure +NULL+ is returned and one may inspect the configstore status.
//:   No mutations must be made to the data.
//:
//: 'muos_configstore_wlock ()';;
//:   Checks for availability of the configstore.
//:   On success it places a write lock on the configstore and returns a pointer
//:   to a 'muos_configstore_data' data structure which holds all the defined elements.
//:   On failure +NULL+ is returned and one may inspect the configstore status.
//:   The write lock blocks all other access to the configstore and may modify its contents.
//:
//: 'muos_configstore_initial ()';;
//:   Works only when the configstore is 'invalid'. Places a write locks on the data which
//:   must be unlocked afterwards. This is used when the configstore is uninitialized/prime
//:   or damaged to populate it with defaults.
//:
//: 'muos_configstore_unlock ()' 'muos_configstore_unwlock ()';;
//:   Frees the lock obtained by 'muos_configstore_lock()', 'muos_configstore_wlock()' or
//:  'muos_configstore_initial ()'. Care must be taken that lock is matched by a unlock.
//:
//:
const struct muos_configstore_data*
muos_configstore_lock (void);

void
muos_configstore_unlock (const struct muos_configstore_data** lock);

struct muos_configstore_data*
muos_configstore_wlock (void);

struct muos_configstore_data*
muos_configstore_initial (void);

static inline void
muos_configstore_unwlock (struct muos_configstore_data** lock)
{
  muos_configstore_unlock ((const struct muos_configstore_data**) lock);
}

//TODO: docme
#ifdef MUOS_CONFIGSTORE_DEFAULTS
muos_error
muos_configstore_defaults (void);
#endif

// generic api

#if 0
enum muos_configstore_id
muos_configstore_lookup_id (const char* name);

size_t
muos_configstore_index (enum muos_configstore_id id);
#endif

// in ram API



// virtual  API




#endif
