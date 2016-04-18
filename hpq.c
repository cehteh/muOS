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

#include <muos/hpq.h>

#if MUOS_HPQ_LENGTH > 0

muos_hpq_type muos_hpq;

muos_error
muos_hpq_pushback_unsafe (muos_queue_function f)
{
  if (!muos_hpq_check (1))
    return muos_error_hpq_overflow;

  MUOS_QUEUE_PUSHBACK(muos_hpq, (f));
  return muos_success;
}

muos_error
muos_hpq_pushback_arg_unsafe (muos_queue_function_arg f, intptr_t a)
{
  if (!muos_hpq_check (2))
    return muos_error_hpq_overflow;

  MUOS_QUEUE_PUSHBACK_ARG(muos_hpq, (f), (a));
  return muos_success;
}

muos_error
muos_hpq_pushfront_unsafe (muos_queue_function f)
{
  if (!muos_hpq_check (1))
    return muos_error_hpq_overflow;

  MUOS_QUEUE_PUSHFRONT(muos_hpq, (f));
  return muos_success;
}

muos_error
muos_hpq_pushfront_arg_unsafe (muos_queue_function_arg f, intptr_t a)
{
  if (!muos_hpq_check (2))
    return muos_error_hpq_overflow;

  MUOS_QUEUE_PUSHFRONT_ARG(muos_hpq, (f), (a));
  return muos_success;
}


muos_error
muos_hpq_pushback (muos_queue_function f)
{
  muos_interrupt_disable ();
  muos_error ret = muos_hpq_pushback_unsafe (f);
  muos_interrupt_enable ();
  return ret;
}

muos_error
muos_hpq_pushback_arg (muos_queue_function_arg f, intptr_t a)
{
  muos_interrupt_disable ();
  muos_error ret = muos_hpq_pushback_arg_unsafe (f, a);
  muos_interrupt_enable ();
  return ret;
}

muos_error
muos_hpq_pushfront (muos_queue_function f)
{
  muos_interrupt_disable ();
  muos_error ret = muos_hpq_pushfront_unsafe (f);
  muos_interrupt_enable ();
  return ret;
}

muos_error
muos_hpq_pushfront_arg (muos_queue_function_arg f, intptr_t a)
{
  muos_interrupt_disable ();
  muos_error ret = muos_hpq_pushfront_arg_unsafe (f, a);
  muos_interrupt_enable ();
  return ret;
}

#endif




