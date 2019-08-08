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

muos_hpt_fn muos_hpt_what;

muos_error
muos_hpt_after (muos_hwclock when, muos_hpt_fn what)
{
  MUOS_ASSERT (true, what);

  MUOS_HW_CLOCK_ISR_COMPMATCH_DISABLE (MUOS_HPT_HW);
  if (muos_hpt_what)
    {
      MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE (MUOS_HPT_HW);
      return muos_error_hpt_active;
    }

  muos_hpt_what = what;

  MUOS_HW_CLOCK_ISR_COMPMATCH_CLEAR (MUOS_HPT_HW);
  MUOS_HW_CLOCK_ISR_COMPMATCH_REG (MUOS_HPT_HW) = MUOS_CLOCK_REGISTER + when;
  MUOS_HW_CLOCK_ISR_COMPMATCH_ENABLE (MUOS_HPT_HW);
  return muos_success;
}



#endif
