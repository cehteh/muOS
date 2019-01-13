/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2018                            Christian Thäter <ct@pipapo.org>
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

#ifdef MUOS_CONFIGSTORE

#include <muos/configstore.h>
#include <muos/eeprom.h>
#include <muos/io.h>

#include <stddef.h>

#ifdef MUOS_EEPROM_CRC16_INCLUDE
#include MUOS_EEPROM_CRC16_INCLUDE
#else
#error MUOS_EEPROM_CRC16_INCLUDE must be configured
#endif

#ifndef MUOS_EEPROM_CRC16_FN
#error MUOS_EEPROM_CRC16_FN must be configured
#endif

static
muos_configstore_status status = CONFIGSTORE_UNKNOWN;

#define ENTRY(type, ary, name, default, verify, min, max, descr) static const char __flash configstore_##name##_str[] = #name;
CONFIGSTORE_DATA_IMPL
#undef ENTRY

const char __flash * const __flash muos_configstore_names[] =
  {
#define ENTRY(type, ary, name, default, verify, min, max, descr) configstore_##name##_str,
   CONFIGSTORE_DATA_IMPL
#undef ENTRY
  };

//PLANNED: do we need an array of type names?


static const uint8_t __flash type_size[] =
  {
#define string char
#define TYPE(type) sizeof(type),
   MUOS_CONFIGSTORE_TYPES
#undef TYPE
#undef string
  };



struct muos_configstore_schema
{
  enum muos_configstore_type type;
  size_t offset;
  uint8_t ary;
};






static const struct muos_configstore_schema __flash schema[] =
  {
#define ARRAY(len) len
#define ENTRY(type, ary, name, default, verify, min, max, descr) \
   {                                                                         \
    MUOS_CONFIGSTORE_TYPE_##type,                                            \
    offsetof(struct muos_configstore_data, name),                            \
    ary                                                                      \
   },
   CONFIGSTORE_DATA_IMPL
#undef ENTRY
#undef ARRAY
  };


#ifdef MUOS_CONFIGSTORE_DEFAULTS
static const struct muos_configstore_data __flash configstore_defaults =
  {

#define ENTRY(type, ary, name, default, verify, min, max, descr) default,
   CONFIGSTORE_DATA_IMPL
#undef ENTRY
  };
#endif


struct muos_configstore_frame
{
  struct muos_configstore_data userdata;
  // uint32_t generation; only used for journaled
  // uint8_t xor;
  uint16_t crc; //PLANNED: on the fly
};


//TODO: calculate type from array capacity
//static uint16_t pos;
static struct muos_configstore_frame the_configstore;


static void*
muos_configstore_value (enum muos_configstore_id id, uint8_t index)
{
  return (void*) &the_configstore + schema[id].offset + index* type_size[schema[id].type];
}



enum muos_configstore_type
muos_configstore_type (enum muos_configstore_id id)
{
  return schema[id].type;
}

uint8_t
muos_configstore_ary (enum muos_configstore_id id)
{
  return schema[id].ary;
}


muos_error
muos_configstore_output_value (enum muos_configstore_id id, uint8_t index)
{
  void* value = muos_configstore_value (id, index);

  switch (schema[id].type)
    {
    case MUOS_CONFIGSTORE_TYPE_int8_t:
      return muos_output_int8 (*(int8_t*) value);

    case MUOS_CONFIGSTORE_TYPE_uint8_t:
      return muos_output_uint8 (*(uint8_t*) value);

    case MUOS_CONFIGSTORE_TYPE_int16_t:
      return muos_output_int16 (*(int16_t*) value);

    case MUOS_CONFIGSTORE_TYPE_uint16_t:
      return muos_output_uint16 (*(uint16_t*) value);

    case MUOS_CONFIGSTORE_TYPE_int32_t:
      return muos_output_int32 (*(int32_t*) value);

    case MUOS_CONFIGSTORE_TYPE_uint32_t:
      return muos_output_uint32 (*(uint32_t*) value);

    case MUOS_CONFIGSTORE_TYPE_size_t:
      return muos_output_uintptr (*(size_t*) value);

    case MUOS_CONFIGSTORE_TYPE_string:
      return muos_output_cstr ((const char*) value);

    default:
      return muos_error_error; /*will never happen*/
    }
}

muos_error
muos_configstore_output_name (enum muos_configstore_id id)
{
  return muos_output_fstr (muos_configstore_names[id]);
}


muos_configstore_status
muos_configstore_get_status (void)
{
  return status;
};


const struct muos_configstore_data*
muos_configstore_lock (void)
{
  if (status < CONFIGSTORE_RLOCK_MAX && status >= CONFIGSTORE_VALID)
    {
      ++status;
      return &the_configstore.userdata;
    }
  return NULL;
}

struct muos_configstore_data*
muos_configstore_wlock (void)
{
  if (status == CONFIGSTORE_VALID)
    {
      status = CONFIGSTORE_WLOCK;
      return &the_configstore.userdata;
    }
  return NULL;
}


void
muos_configstore_unlock (const struct muos_configstore_data** lock)
{
  if (*lock)
    {
      if (status == CONFIGSTORE_WLOCK)
        status = CONFIGSTORE_VALID;
      else
        --status;
      *lock = NULL;
    }
}


struct muos_configstore_data*
muos_configstore_initial (void)
{
  if (status == CONFIGSTORE_INVALID)
    {
      status = CONFIGSTORE_WLOCK;
#ifdef MUOS_CONFIGSTORE_DEFAULTS
      memcpy_P (&the_configstore.userdata, &configstore_defaults, sizeof(configstore_defaults));
#endif
      the_configstore.userdata.config_size = sizeof (struct muos_configstore_data);

      return &the_configstore.userdata;
    }
  return NULL;
}


static muos_configstore_callback callback;


//PLANNED: configstore_revert revert to an earlier config
static void
eeprom_read_done (void)
{
  uint16_t crc = MUOS_EEPROM_CRC16_INIT;

  for (size_t i = 0; i < sizeof(struct muos_configstore_frame); ++i)
    {
      crc = MUOS_EEPROM_CRC16_FN (crc, ((uint8_t*)&the_configstore)[i]);
    }

  if (crc == 0 && the_configstore.userdata.config_size == sizeof (struct muos_configstore_data))
    {
      status = CONFIGSTORE_VALID;
    }
  else
    {
     status = CONFIGSTORE_INVALID;
     muos_error_set (muos_error_configstore);
    }

  if (callback)
    callback ();
}

muos_error
muos_configstore_load (muos_configstore_callback cb)
{
  if (status < CONFIGSTORE_RLOCK)
    status = CONFIGSTORE_WLOCK;
  else
    return muos_error_configstore;

  callback = cb;

  //FIXME: simple non redundant version for now
  muos_eeprom_read (&the_configstore,
                    MUOS_CONFIGSTORE_OFFSET,
                    sizeof(struct muos_configstore_frame),
                    eeprom_read_done);


  //PLANNED: later, journaled
  //pos = 0;
  //scan for valid crc
  //remember highest generations position
  //finally load it

  return muos_success;
}



static void
eeprom_write_done (void)
{
  if (muos_error_check (muos_error_eeprom_verify))
    {
      //FIXME: DEAD not as state but error, let states only reflect the state in memory
      status = CONFIGSTORE_DEAD;
      muos_error_set (muos_error_configstore);
    }
  else
    {
      --status;
    }

  if (callback)
    callback ();
}


muos_error
muos_configstore_save (muos_configstore_callback cb)
{
  if (status >= CONFIGSTORE_RLOCK_MAX || status < CONFIGSTORE_VALID)
    return muos_error_configstore;

  //PLANNED: call user defined verification function, checking the whole config

  ++status;

  callback = cb;

  uint16_t crc = MUOS_EEPROM_CRC16_INIT;

  for (size_t i = 0; i < sizeof(struct muos_configstore_frame)-2; ++i)
    {
      crc = MUOS_EEPROM_CRC16_FN (crc, ((uint8_t*)&the_configstore)[i]);
    }

  the_configstore.crc = crc;

  muos_eeprom_write (&the_configstore,
                     MUOS_CONFIGSTORE_OFFSET,
                     sizeof(struct muos_configstore_frame),
                     eeprom_write_done);

  //PLANNED: later, journaled
  return muos_success;
}

#endif

