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
//:   CONFIGSTORE_ENTRY(type, ary, name) {__BACKSLASH__}
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
//: The user defines 'CONFIGSTORE_DATA' to a sequence of 'CONFIGSTORE_ENTRY(type, ary, name)'
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

//TODO: document implicit config_size
#define CONFIGSTORE_DATA_IMPL                           \
  CONFIGSTORE_ENTRY  (size_t, 0, config_size,           \
                      "configuration structure size")   \
    CONFIGSTORE_DATA

struct muos_configstore_data
{
#define CONFIGSTORE_ENTRY(type, ary, name, descr) type name CONFIGSTORE_ARY(ary);
  CONFIGSTORE_DATA_IMPL
#undef CONFIGSTORE_ENTRY
};


#if 0
enum muos_configstore_id
  {
#define CONFIGSTORE_ENTRY(type, ary, name, descr) MUOS_CONFIGSTORE_ID_##name,
   CONFIGSTORE_DATA
#undef CONFIGSTORE_ENTRY
  };
#endif



// API

// loading/saving

//configstore_api:
//: .Loading and Saving
//: ----
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
//: muos_configstore_unlock (void)
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
//: 'muos_configstore_unlock ()';;
//:   Frees the lock obtained by 'muos_configstore_lock()' or 'muos_configstore_wlock()'.
//:   Care must be taken that every *successful* lock is matched by a unlock.
//:
//: 'muos_configstore_initial ()';;
//:   Works only when the configstore is 'invalid'. Places a write locks on the data which
//:   must be unlocked afterwards. This is used when the configstore is uninitialized/prime
//:   or damaged to populate it with defaults.
//:
const struct muos_configstore_data*
muos_configstore_lock (void);

struct muos_configstore_data*
muos_configstore_wlock (void);

void
muos_configstore_unlock (void);

struct muos_configstore_data*
muos_configstore_initial (void);


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