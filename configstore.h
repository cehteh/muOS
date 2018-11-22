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
   CONFIGSTORE_UNKNOWN,   // status not known yet, did not called load
   CONFIGSTORE_INVALID,   // load did not find a saved config
   CONFIGSTORE_VALID,     // successfully loaded config
   CONFIGSTORE_LOCKED,    // some async operation in progress
   CONFIGSTORE_DEAD,      // 'save' could not save+verify data, EEPROM is dead
} muos_configstore_status;

extern muos_configstore_status status;

#define CONFIGSTORE_ARY_ARRAY(len) [len]
#define CONFIGSTORE_ARY_0
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

struct muos_configstore_data
{
#define CONFIGSTORE_ENTRY(type, ary, name) type name CONFIGSTORE_ARY(ary);
  CONFIGSTORE_DATA
#undef CONFIGSTORE_ENTRY
};


#if 0
enum muos_configstore_id
  {
#define CONFIGSTORE_ENTRY(type, ary, name) MUOS_CONFIGSTORE_ID_##name,
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


// generic api

#if 0
enum muos_configstore_id
muos_configstore_lookup_id (const char* name);

size_t
muos_configstore_index (enum muos_configstore_id id);
#endif

// in ram API

//TODO: static inline
#if 0
struct muos_configstore_data*
muos_configstore (void); // inmemory pointer or NULL
#endif


// virtual  API




#endif
