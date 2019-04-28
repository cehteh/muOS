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

#include <muos/sm.h>
#include <muos/hpq.h>

#include <stddef.h>

struct muos_statemachine
{
  enum muos_sm_state prev;
  enum muos_sm_state current;
  enum muos_sm_state params[4];
#if MUOS_SM_NUM > 1
  intptr_t extra;
#endif
};

static struct muos_statemachine statemachine[MUOS_SM_NUM];


struct muos_sm_def
{
  state_enter_fn enter;
  state_leave_fn leave;

#ifdef MUOS_SM_NAMES
  const char __flash * const name;
#endif
};

#ifdef MUOS_SM_NAMES
static const char __flash state_name_NONE[] = "NONE";
#define STATE(name) static const char __flash state_name_##name[] = #name;
MUOS_SM_STATES;
#undef STATE
#endif

#define STATE(name) muos_error state_##name##_leave (enum muos_sm_state [4]);
MUOS_SM_STATES;
#undef STATE

#define STATE(name) void state_##name##_enter (void);
MUOS_SM_STATES;
#undef STATE

const struct muos_sm_def __flash state_definitions[] =
  {
#ifdef MUOS_SM_NAMES
   {NULL, NULL, state_name_NONE},
#define STATE(name)                             \
   {                                            \
    state_##name##_enter,                       \
    state_##name##_leave,                       \
    state_name_##name,                          \
   },
   MUOS_SM_STATES
#undef STATE
#else
   {NULL, NULL},
#define STATE(name)                             \
   {                                            \
    state_##name##_enter,                       \
    state_##name##_leave,                       \
   },
   MUOS_SM_STATES
#undef STATE
#endif
};



static void
state_enter (void)
{
  uint8_t sm = muos_hpq_pop ();

  statemachine[sm].current = statemachine[sm].params[0];
  statemachine[sm].params[0] = statemachine[sm].params[1];
  statemachine[sm].params[1] = statemachine[sm].params[2];
  statemachine[sm].params[2] = statemachine[sm].params[3];
  statemachine[sm].params[3] = STATE_NONE;
#if MUOS_SM_NUM > 1
  state_definitions[statemachine[sm].current].enter(statemachine[sm].extra);
#else
  state_definitions[statemachine[sm].current].enter();
#endif
}


enum muos_sm_state
muos_sm_prev (uint8_t sm)
{
  if (sm >= MUOS_SM_NUM)
    return muos_error_nodev;

  return statemachine[sm].prev;
}

enum muos_sm_state
muos_sm_get (uint8_t sm)
{
  if (sm >= MUOS_SM_NUM)
    return muos_error_nodev;

  return statemachine[sm].current;
}

#ifdef MUOS_SM_NAMES
const char __flash*
muos_sm_name_get (enum muos_sm_state state)
{
  if (state >= MUOS_SM_MAXSTATE)
    return NULL;

  return state_definitions[state].name;
}
#endif

enum muos_sm_state*
muos_sm_params_get (uint8_t sm)
{
  if (sm >= MUOS_SM_NUM)
    return NULL;

  return statemachine[sm].params;
}



muos_error
muos_sm_change (uint8_t sm, enum muos_sm_state params[4])
{
  if (sm >= MUOS_SM_NUM)
    return muos_error_nodev;

  if (statemachine[sm].current == STATE_NONE
      || statemachine[sm].current == params[0]
      || params[0] >= MUOS_SM_MAXSTATE)
    return muos_error_sm_state;

  // futile because ISR's may push things to the hpq
  //if (muos_hpq_check (2))
  //  return muos_error_hpq_overflow;
  //PLANNED: intptr_t* muos_queue_reserve(num, init) for reserving elements on a queue (hpq)

  if (state_definitions[statemachine[sm].current].leave)
#if MUOS_SM_NUM > 1
    MUOS_OK(state_definitions[statemachine[sm].current].leave(params), statemachine[sm].extra);
#else
    MUOS_OK(state_definitions[statemachine[sm].current].leave(params));
#endif

  statemachine[sm].prev = statemachine[sm].current;
  statemachine[sm].params[0] = params[0];
  statemachine[sm].params[1] = params[1];
  statemachine[sm].params[2] = params[2];
  statemachine[sm].params[3] = params[3];

  if (state_definitions[params[0]].enter)
    {
      statemachine[sm].current = STATE_NONE;
      //FIXME: wait for 2 elements free, locked wait
      MUOS_OK(muos_hpq_push_arg (state_enter, sm));
    }
  else
    {
      statemachine[sm].current = params[0];
      statemachine[sm].params[0] = params[1];
      statemachine[sm].params[1] = params[2];
      statemachine[sm].params[2] = params[3];
      statemachine[sm].params[3] = STATE_NONE;
    }

  return muos_success;
}

muos_error
muos_sm_next (uint8_t sm)
{
  if (sm >= MUOS_SM_NUM)
    return muos_error_nodev;

  if (statemachine[sm].current == STATE_NONE
      || statemachine[sm].current == statemachine[sm].params[0]
      || !statemachine[sm].params[0])
    return muos_error_sm_state;

  // futile because ISR's may push things to the hpq
  //if (muos_hpq_check (2))
  //  return muos_error_hpq_overflow;
  //PLANNED: intptr_t* muos_queue_reserve(num, init) for reserving elements on a queue (hpq)

  if (state_definitions[statemachine[sm].current].leave)
#if MUOS_SM_NUM > 1
    MUOS_OK(state_definitions[statemachine[sm].current].leave(statemachine[sm].params), statemachine[sm].extra);
#else
    MUOS_OK(state_definitions[statemachine[sm].current].leave(statemachine[sm].params));
#endif

  if (state_definitions[statemachine[sm].params[0]].enter)
    {
      statemachine[sm].current = STATE_NONE;
      //FIXME: wait for 2 elements free, locked wait
      MUOS_OK(muos_hpq_push_arg (state_enter, sm));
    }
  else
    {
      statemachine[sm].current = statemachine[sm].params[0];
      statemachine[sm].params[0] = statemachine[sm].params[1];
      statemachine[sm].params[1] = statemachine[sm].params[2];
      statemachine[sm].params[2] = statemachine[sm].params[3];
      statemachine[sm].params[3] = STATE_NONE;
    }

  return muos_success;
}

#if MUOS_SM_NUM > 1
muos_error
muos_sm_init (uint8_t sm, enum muos_sm_state params[4], intptr_tr extra)
#else
muos_error
muos_sm_init (uint8_t sm, enum muos_sm_state params[4])
#endif
{
  if (sm >= MUOS_SM_NUM)
    return muos_error_nodev;

  if (statemachine[sm].current != STATE_NONE
      || params[0] >= MUOS_SM_MAXSTATE)
    return muos_error_sm_state;

  statemachine[sm].prev = STATE_NONE;
  statemachine[sm].current = params[0];
  statemachine[sm].params[0] = params[1];
  statemachine[sm].params[1] = params[2];
  statemachine[sm].params[2] = params[3];
  statemachine[sm].params[3] = STATE_NONE;

#if MUOS_SM_NUM > 1
  statemachine[sm].extra = extra;
#endif

  if (state_definitions[params[0]].enter)
    {
#if MUOS_SM_NUM > 1
      state_definitions[params[0]].enter(extra);
#else
      state_definitions[params[0]].enter();
#endif
    }

  return muos_success;
}

bool
muos_sm_ready (intptr_t sm)
{
  if (sm >= MUOS_SM_NUM)
    return false;

  return statemachine[sm].current != STATE_NONE;
}

