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

#ifdef MUOS_EEPROM_CRC16_INCLUDE
#include MUOS_EEPROM_CRC16_INCLUDE
#else
#error MUOS_EEPROM_CRC16_INCLUDE must be configured
#endif

#ifndef MUOS_EEPROM_CRC16_FN
#error MUOS_EEPROM_CRC16_FN must be configured
#endif

muos_configstore_status status = CONFIGSTORE_UNKNOWN;

struct muos_configstore_frame
{
  struct muos_configstore_data userdata;
  // uint32_t generation; only used for journaled
  // uint8_t xor;
  uint16_t crc; //PLANNED: on the fly
};


//TODO: calculate type from array capacity
//static uint16_t pos;
static struct muos_configstore_frame the_muos_configstore;

static muos_configstore_callback callback;


//PLANNED: configstore_revert revert to an earlier config
static void
eeprom_read_done (void)
{
  uint16_t crc = MUOS_EEPROM_CRC16_INIT;

  for (size_t i = 0; i < sizeof(struct muos_configstore_frame); ++i)
    {
      crc = MUOS_EEPROM_CRC16_FN (crc, ((uint8_t*)&the_muos_configstore)[i]);
    }

  if (crc == 0)
    {
      status = CONFIGSTORE_VALID;
    }
  else
    {
     status = CONFIGSTORE_INVALID;
     muos_error_set (muos_error_configstore_invalid);
    }

  if (callback)
    callback ();
}

muos_error
muos_configstore_load (muos_configstore_callback cb)
{
  if (status != CONFIGSTORE_LOCKED)
    status = CONFIGSTORE_LOCKED;
  else
    return muos_error_set (muos_error_configstore_locked);

  callback = cb;

  //FIXME: simple non redundant version for now
  muos_eeprom_read (&the_muos_configstore,
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
      status = CONFIGSTORE_DEAD;
      muos_error_set (muos_error_configstore_invalid);
    }
  else
    {
      status = CONFIGSTORE_VALID;
    }

  if (callback)
    callback ();
}


muos_error
muos_configstore_save (muos_configstore_callback cb)
{
  if (status != CONFIGSTORE_LOCKED)
    status = CONFIGSTORE_LOCKED;
  else
    return muos_error_set (muos_error_configstore_locked);

  callback = cb;

  uint16_t crc = MUOS_EEPROM_CRC16_INIT;

  for (size_t i = 0; i < sizeof(struct muos_configstore_frame)-2; ++i)
    {
      crc = MUOS_EEPROM_CRC16_FN (crc, ((uint8_t*)&the_muos_configstore)[i]);
    }

  the_muos_configstore.crc = crc;

  muos_eeprom_write (&the_muos_configstore,
                     MUOS_CONFIGSTORE_OFFSET,
                     sizeof(struct muos_configstore_frame),
                     eeprom_write_done);

  //PLANNED: later, journaled
  return muos_success;
}

#endif

