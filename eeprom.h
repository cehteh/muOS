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

#ifndef MUOS_EEPROM_H
#define MUOS_EEPROM_H

#include <stdlib.h>
#include <muos/muos.h>

#include MUOS_HW_HEADER

/*
  asynchronous EEPROM access driver
*/

enum muos_eeprom_mode
  {
   MUOS_EEPROM_IDLE,           // no operation in progress
   MUOS_EEPROM_READ,           // eeprom -> memory
   MUOS_EEPROM_VERIFY,         // eeprom == memory, will set error_eeprom_verify on fail
   MUOS_EEPROM_WRITEERASE,     // eeprom <- memory, erase first
   MUOS_EEPROM_WRITEERASE_CONT,
   MUOS_EEPROM_WRITEVERIFY,    // eeprom <- memory, eeprom == memory, erase first
   MUOS_EEPROM_WRITEVERIFY_CONT,
   MUOS_EEPROM_WRITEONLY,      // eeprom <- memory, no erase
   MUOS_EEPROM_WRITEONLY_CONT,
   //PLANNED: MUOS_EEPROM_WRITE, //SMART,     // eeprom -> memory, smart write, erase/write only when necessary
   //PLANNED: MUOS_EEPROM_REFRESH,          // eeprom <- eeprom, read, erase, write to refresh content
   MUOS_EEPROM_ERASE,          // erase eeprom <- 0xff
   MUOS_EEPROM_ERASE_CONT,
   MUOS_EEPROM_IS_ERASED,      // check that the given range is erased
   //PLANNED: MUOS_EEPROM_ERASESECURE,       // eeprom <- ^eeprom, eeprom <- 0x00, erase, secure erase (equivalent wear)
   MUOS_EEPROM_XOR,            // xor the given range
#ifdef MUOS_EEPROM_CRC16_FN
   MUOS_EEPROM_CRC16,          // crc16 over the given range
#endif
   //PLANNED: MUOS_EEPROM_CRC8,       // crc8 over the given range
  };

/*
  WRITESMART algorithm:

  read first,
  compare if:
    no change -> return
    only bits need to be cleared -> mask, write
    else -> erase, write
  verify
*/


typedef void (*muos_eeprom_callback)(void);

extern enum muos_eeprom_mode muos_hw_eeprom_state (void);
extern muos_error muos_hw_eeprom_access (enum muos_eeprom_mode mode,
                                         void* address,
                                         uintptr_t eeprom,
                                         size_t size,
                                         muos_eeprom_callback complete);


//eeprom_api:
//: .Query State
//: ----
//: enum muos_eeprom_mode muos_eeprom_state (void)
//: ----
//:
//: Polls the state of the driver.
//:
//: Returns 0 (MUOS_EEPROM_IDLE) when no operation is in progress.
//:
inline enum muos_eeprom_mode
muos_eeprom_state (void)
{
  return muos_hw_eeprom_state ();
}

//PLANNED: query current position (ram? eeprom?)
//inline void*
//muos_eeprom_pos (void)
//{
//  return muos_hw_eeprom_pos ();
//}


//eeprom_api:
//: .Parameters and returns
//: The parameters used for accessing the eeprom are orthogonal though all access functions
//:
//: +address+::
//:   start address in ram
//:
//: +eeprom+::
//:   start address in EEPROM
//:
//: +size+::
//:   size in bytes of for the operation
//:
//: +complete+::
//:   callback function called upon completion (also on failure)
//:
//: The access function only return an error when they can not start an asynchronous job.
//: All other errors are set asynchronously and should be handled in the complete callback.
//:
//eeprom_api:
//: .Reading
//: ----
//: muos_error muos_eeprom_read (void* address,
//:                              uintptr_t eeprom,
//:                              size_t size,
//:                              muos_eeprom_callback complete)
//: ----
//:
//: Reads the EEPROM.
//:
static inline muos_error
muos_eeprom_read (void* address,
                  uintptr_t eeprom,
                  size_t size,
                  muos_eeprom_callback complete)
{
  return muos_hw_eeprom_access (MUOS_EEPROM_READ, address, eeprom, size, complete);
}


//eeprom_api:
//: .Verifying
//: ----
//: muos_error muos_eeprom_verify (void* address,
//:                                uintptr_t eeprom,
//:                                size_t size,
//:                                muos_eeprom_callback complete)
//: ----
//:
//: Compares a block of memory with eeprom contents. Aborts on first error, calling 'complete'.
//:
static inline muos_error
muos_eeprom_verify (void* address,
                    uintptr_t eeprom,
                    size_t size,
                    muos_eeprom_callback complete)
{
  return muos_hw_eeprom_access (MUOS_EEPROM_VERIFY, address, eeprom, size, complete);
}


//eeprom_api:
//: .Xoring
//: ----
//: muos_error muos_eeprom_xor (uint8_t* address,
//:                             uintptr_t eeprom,
//:                             size_t size,
//:                             muos_eeprom_callback complete)
//: ----
//:
//: XOR'es the given range with address pointing to a single uint8_t.
//:
static inline muos_error
muos_eeprom_xor (uint8_t* address,
                 uintptr_t eeprom,
                 size_t size,
                 muos_eeprom_callback complete)
{
  return muos_hw_eeprom_access (MUOS_EEPROM_XOR, address, eeprom, size, complete);
}



//eeprom_api:
//: .Cyclic Redundancy Check
//: ----
//: muos_error muos_eeprom_crc16 (uint16_t* address,
//:                               uintptr_t eeprom,
//:                               size_t size,
//:                               muos_eeprom_callback complete)
//: ----
//:
//: Caclculates the CRC16 (configured by MUOS_EEPROM_CRC16_FN) of the given range and stores it at address.
//: NOTE: the initial value of '*address' must be set by the user.
//:
//: Only available when MUOS_EEPROM_CRC16_FN is configured.
//:
#ifdef MUOS_EEPROM_CRC16_FN
static inline muos_error
muos_eeprom_crc16 (uint16_t* address,
                   uintptr_t eeprom,
                   size_t size,
                   muos_eeprom_callback complete)
{
  return muos_hw_eeprom_access (MUOS_EEPROM_CRC16, address, eeprom, size, complete);
}
#endif


//eeprom_api:
//: .Writing
//: ----
//: muos_error muos_eeprom_writeerase (void* address,
//:                                    uintptr_t eeprom,
//:                                    size_t size,
//:                                    muos_eeprom_callback complete)
//:
//: muos_error muos_eeprom_writeverify (void* address,
//:                                     uintptr_t eeprom,
//:                                     size_t size,
//:                                     muos_eeprom_callback complete)
//:
//: muos_error muos_eeprom_writeonly (void* address,
//:                                   uintptr_t eeprom,
//:                                   size_t size,
//:                                   muos_eeprom_callback complete)
//: ----
//:
//: Transfers data from memory to eeprom.
//:
//: muos_eeprom_writeerase::
//:   Erases data before writing. Faster than erasing the block first.
//:
//: muos_eeprom_writeverify::
//:   Erases data before writing. Verifies after write.
//:
//: muos_eeprom_writeonly::
//:   Only writes (clears bits) without erasing.
//:
static inline muos_error
muos_eeprom_writeerase (void* address,
                        uintptr_t eeprom,
                        size_t size,
                        muos_eeprom_callback complete)
{
  return muos_hw_eeprom_access (MUOS_EEPROM_WRITEERASE, address, eeprom, size, complete);
}


static inline muos_error
muos_eeprom_writeverify (void* address,
                         uintptr_t eeprom,
                         size_t size,
                         muos_eeprom_callback complete)
{
  return muos_hw_eeprom_access (MUOS_EEPROM_WRITEVERIFY, address, eeprom, size, complete);
}


static inline muos_error
muos_eeprom_writeonly (void* address,
                       uintptr_t eeprom,
                       size_t size,
                       muos_eeprom_callback complete)
{
  return muos_hw_eeprom_access (MUOS_EEPROM_WRITEONLY, address, eeprom, size, complete);
}


//eeprom_api:
//: .Erasing
//: ----
//: muos_error muos_eeprom_erase (uintptr_t eeprom,
//:                               size_t size,
//:                               muos_eeprom_callback complete)
//: ----
//:
//: Erases the given range.
//:
static inline muos_error
muos_eeprom_erase (uintptr_t eeprom,
                   size_t size,
                   muos_eeprom_callback complete)
{
  return muos_hw_eeprom_access (MUOS_EEPROM_ERASE, NULL, eeprom, size, complete);
}


//eeprom_api:
//: .Check for erased
//: ----
//: muos_error muos_eeprom_is_erased (uintptr_t eeprom,
//:                                   size_t size,
//:                                   muos_eeprom_callback complete)
//: ----
//:
//: Checks if the given range is erased.
//:
static inline muos_error
muos_eeprom_is_erased (uintptr_t eeprom,
                       size_t size,
                       muos_eeprom_callback complete)
{
  return muos_hw_eeprom_access (MUOS_EEPROM_IS_ERASED, NULL, eeprom, size, complete);
}


#endif
