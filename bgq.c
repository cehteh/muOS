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

#include <muos/bgq.h>

#if MUOS_BGQ_LENGTH > 0

muos_bgq_type muos_bgq;


muos_error
muos_bgq_pushback (muos_queue_function f)
{
  muos_interrupt_disable ();
  muos_error ret = muos_bgq_pushback_isr (f);
  muos_interrupt_enable ();
  return ret;
}

muos_error
muos_bgq_pushback_arg (muos_queue_function_arg f, intptr_t a)
{
  muos_interrupt_disable ();
  muos_error ret = muos_bgq_pushback_arg_isr (f, a);
  muos_interrupt_enable ();
  return ret;
}

muos_error
muos_bgq_pushfront (muos_queue_function f)
{
  muos_interrupt_disable ();
  muos_error ret = muos_bgq_pushfront_isr (f);
  muos_interrupt_enable ();
  return ret;
}

muos_error
muos_bgq_pushfront_arg (muos_queue_function_arg f, intptr_t a)
{
  muos_interrupt_disable ();
  muos_error ret = muos_bgq_pushfront_arg_isr (f, a);
  muos_interrupt_enable ();
  return ret;
}

#endif




