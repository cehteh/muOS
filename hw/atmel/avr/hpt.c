/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2019                            Christian Thäter <ct@pipapo.org>
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

#ifdef MUOS_HPT

#include <muos/hpt.h>

#include <stddef.h>

extern muos_hpt_fn muos_hpt_what;

ISR(ISRNAME_COMPMATCH(MUOS_HPT_HW))
{
  MUOS_DEBUG_INTR_ON;

  if (muos_hpt_what)
    {
      muos_hwclock again = muos_hpt_what ();

      if (again)
        {
          MUOS_HW_CLOCK_ISR_COMPMATCH_REG (MUOS_HPT_HW) =
            MUOS_HW_CLOCK_ISR_COMPMATCH_REG (MUOS_HPT_HW)
            + again;
        }
      else
        {
          muos_hpt_what = NULL;
          MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE (MUOS_HPT_HW);
        }
    }
  else
    {
      MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE (MUOS_HPT_HW);
    }

  MUOS_DEBUG_INTR_OFF;
}


#endif
