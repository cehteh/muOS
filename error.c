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

#include <stdint.h>
#include <muos/muos.h>
#include <muos/error.h>

#include <stddef.h>

volatile uint8_t muos_errors_pending_;
volatile MUOS_BARRAY(muos_errors_, muos_errors_end);

#ifdef MUOS_ERROR_CTX
static uint16_t ctx_line;
static const char __flash* ctx_fileid;

uint16_t
muos_error_ctx_line (void)
{
  return ctx_line;
}

const char __flash*
muos_error_ctx_file (void)
{
  return ctx_fileid;
}
#endif

#ifdef MUOS_ERROR_STR
#define MUOS_ERROR(name, ...) static const char __flash muos_error_##name##_str[] = #name;
static const char __flash muos_success_str[] = "muos_success";
MUOS_ERRORS
#undef MUOS_ERROR

static
const char __flash * const __flash muos_error_names[] =
  {
   muos_success_str,
#define MUOS_ERROR(name, ...) muos_error_##name##_str,
   MUOS_ERRORS
#undef MUOS_ERROR
  };


const char __flash*
muos_error_str (muos_error err)
{
  if (err >= muos_errors_end)
    return muos_error_error_error_str;

  return muos_error_names[err];
}
#endif

#ifdef MUOS_ERROR_CTX
muos_error
muos_error_set_isr_ (muos_error err, uint16_t line, const char __flash* fileid)
{
  if (err && !(muos_errors_[err/8] & 1<<(err%8)))
      {
        MUOS_DEBUG_ERROR_ON;
        muos_barray_setbit (muos_errors_, err);
        ++muos_errors_pending_;
        ctx_line = line;
        ctx_fileid = fileid;
      }
  return err;
}

#else
muos_error
muos_error_set_isr_ (muos_error err)
{
  if (err && !(muos_errors_[err/8] & 1<<(err%8)))
      {
        MUOS_DEBUG_ERROR_ON;
        muos_barray_setbit (muos_errors_, err);
        ++muos_errors_pending_;
      }
  return err;
}
#endif


bool
muos_error_check_isr (muos_error err)
{
  bool ret = false;

  if (muos_errors_pending_)
    {
      ret = muos_error_peek (err);

      if (ret)
        {
          muos_barray_clearbit (muos_errors_, err);
          --muos_errors_pending_;
        }

#ifdef MUOS_DEBUG_ERROR
      if (!muos_errors_pending_)
        {
          MUOS_DEBUG_ERROR_OFF;
        }
#endif
    }

  return ret;
}


void
muos_error_clearall_isr (void)
{
  muos_barray_clear(muos_errors_);

  muos_errors_pending_ = 0;
  MUOS_DEBUG_ERROR_OFF;
}
