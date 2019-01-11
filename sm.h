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


#ifndef MUOS_SM_H
#define MUOS_SM_H

#include MUOS_SM_DEF

#include <muos/error.h>


//sm_api:
//: .States Enumeration
//: ----
//: enum muos_sm_state
//:   {
//:    STATE_NONE,
//:    MUOS_SM_STATES...
//:  }
//: ----
//:
//: A global list of all defined states. The user has to supply
//: a file which defines MUOS_SM_STATES. This gets x-macro expanded
//: for various things. Note that all states get prefixed with 'STATE_'
//: not the usual 'MUOS_*' prefix (these are user application defined states
//: anyway).
//:
//: 'STATE_NONE' gets always defined as the first member implicitly. This
//: is used to flag the state machine when it is not yet initialized or a state
//: transition is in progress.
//:
enum muos_sm_state
  {
   STATE_NONE,
#define STATE(name, ...) STATE_##name,
   MUOS_SM_STATES
#undef STATE
   MUOS_SM_MAXSTATE
  };

//sm_api:
//: .States Transition Functions
//: ----
//: typedef muos_error (*const state_leave_fn)(enum muos_sm_state params[4]);
//: typedef void (*state_enter_fn)(void);
//:
//: typedef muos_error (*const state_leave_fn)(enum muos_sm_state params[4], intptr_t extra);
//: typedef void (*state_enter_fn)(intptr_t extra);
//: ----
//:
//: For each state the user has to define an enter and a leave function. The MµOS
//: state machine driver calls those for changing states.
//:
//: When MUOS_SM_NUM is greater than one, then state transition functions take an
//: 'extra' parameter which passes user defined data and wont be changed by the
//: state machine driver.
//:
//: This functions need to be named 'state_STATE_enter' and 'state_STATE_leave', where 'STATE'
//: is the state as defined above (without the 'STATE_' prefix).
//:
//: The 'leave' function will be called first with the intended state transition
//: in the 'params' field. 'params[0]' is the state to be changed to, 'params[1..3]' are user
//: defined subsequent state parameters. The 'leave' function shall validate the possible
//: state transition and  may return an error when the transition is denied. Otherwise
//: it should clean the current state up and return 'muos_successs'. Note that the 'current' state
//: might not be fully initialized when the 'enter' function did not complete and changes into a new
//: state immediately. When the 'leave' function returns any error the state stays unchanged.
//:
//: The 'enter' function is called to finish the state transition and should set up the new state.
//: Errors on enter shall be handled by changing into another error-handling state.
//:
#if MUOS_SM_NUM > 1
typedef muos_error (*const state_leave_fn)(enum muos_sm_state [4], intptr_t extra);
typedef void (*state_enter_fn)(intptr_t extra);
#else
typedef muos_error (*const state_leave_fn)(enum muos_sm_state [4]);
typedef void (*state_enter_fn)(void);
#endif




//sm_api:
//: .State Machine Initialization
//: ----
//: #define MUOS_SM_INIT(sm, extra, initialstate, ...)
//:
//: muos_error
//: muos_sm_init (uint8_t sm, enum muos_sm_state params[4], intptr_t extra);
//:
//: #define MUOS_SM_INIT(initialstate, ...)
//:
//: muos_error
//: muos_sm_init (uint8_t sm, enum muos_sm_state params[4]);
//: ----
//:
//: An uninitialized state machine is initially at 'STATE_NONE' and must be
//: initialized to a first state. Again here the API is different depending
//: on the number of state machines to be defined with MUOS_SM_NUM.
//:
//: When there is more than one state machine one must identify the state machine
//: with the 'sm' index and an initial 'extra' member gets passed and initialized.
//: Later this 'extra' member gets passed unaltered to to the state transition functions.
//:
//: 'MUOS_SM_INIT()' shall be used to initialize a state machine. Adds some convenience over
//: calling the 'muos_sm_init()' function directly. 'MUOS_SM_INIT()' takes a number of
//: optional arguments for the parameter array which will be initialized to 'STATE_NONE'
//: if not given. The 'STATE_' prefix must be omitted here as it gets automatically applied.
//:
//: The initialization calls the initial states enter function synchronously, on
//: successful return the state machine is fully initialized and running.
//:
//: Initialization returns an error when 'sm' or 'initialstate' is out of range.
//: The state stays at 'STATE_NONE' then.
//:
#if MUOS_SM_NUM > 1
#define MUOS_SM_INIT(sm, extra, newstate, ...) MUOS_SM_INIT_(sm, extra, newstate, ## __VA_ARGS__, NONE, NONE, NONE)
#define MUOS_SM_INIT_(sm, extra, newstate, p0, p1, p2, ...)                                              \
  muos_sm_init (sm, (enum muos_sm_state [4]) {STATE_##newstate, STATE_##p0,STATE_##p1,STATE_##p2}, extra)

muos_error
muos_sm_init (uint8_t sm, enum muos_sm_state params[4], intptr_t extra);

#else

#define MUOS_SM_INIT(newstate, ...) MUOS_SM_INIT_(newstate, ## __VA_ARGS__, NONE, NONE, NONE)
#define MUOS_SM_INIT_(newstate, p0, p1, p2, ...)                                                    \
  muos_sm_init (0, (enum muos_sm_state [4]) {STATE_##newstate, STATE_##p0,STATE_##p1,STATE_##p2})

muos_error
muos_sm_init (uint8_t sm, enum muos_sm_state params[4]);
#endif



//sm_api:
//: .State Transition
//: ----
//: #define MUOS_SM_CHANGE(sm, newstate, ...)
//:
//: #define MUOS_SM_CHANGE(newstate, ...)
//:
//: muos_error
//: muos_sm_change (uint8_t sm, enum muos_sm_state params[4]);
//:
//: #define MUOS_SM_NEXT(sm)
//: #define MUOS_SM_NEXT
//:
//: muos_error
//: muos_sm_next (uint8_t sm);
//: ----
//:
//: A state transition on a initialized state machine will be initiated by calling
//: the 'MUOS_SM_CHANGE()' macro. Again depending on the MUOS_SM_NUM the API may
//: require the index of the state machine to change.
//: The first 'newstate' parameter is the state one would like to change to. Any
//: optional subsequent parameters will passed in the params array or initialized to
//: 'STATE_NONE' if not given.
//:
//: State changing will validate it's arguments then call the 'leave' function of the
//: current state. When this succeeds the state is temporally set to 'STATE_NONE' to
//: flag that a state transition is in progress and the state enter function for the
//: gets appended to the hpq.
//:
//: On scheduling before the 'enter' function for the new state gets called the
//: current state and params will be set to the new state.
//:
//: 'MUOS_SM_NEXT' transits into the state given by the existing params.
//:
#if MUOS_SM_NUM > 1

#define MUOS_SM_CHANGE(sm, newstate, ...) MUOS_SM_CHANGE_(sm, newstate, ## __VA_ARGS__, NONE, NONE, NONE)
#define MUOS_SM_CHANGE_(sm, newstate, p0, p1, p2, ...)                                                  \
  muos_sm_change (sm, (enum muos_sm_state [4]) {STATE_##newstate, STATE_##p0,STATE_##p1,STATE_##p2})

#define MUOS_SM_NEXT(sm) muos_sm_next (sm)

#else

#define MUOS_SM_CHANGE(newstate, ...) MUOS_SM_CHANGE_(newstate, ## __VA_ARGS__, NONE, NONE, NONE)
#define MUOS_SM_CHANGE_(newstate, p0, p1, p2, ...)                                                    \
  muos_sm_change (0, (enum muos_sm_state [4]) {STATE_##newstate, STATE_##p0,STATE_##p1,STATE_##p2})

#define MUOS_SM_NEXT muos_sm_next (0)

#endif

muos_error
muos_sm_change (uint8_t sm, enum muos_sm_state params[4]);

muos_error
muos_sm_next (uint8_t sm);




//sm_api:
//: .Tool Functions
//: ----
//: enum muos_sm_state*
//: muos_sm_params_get (uint8_t sm);
//:
//: enum muos_sm_state
//: muos_sm_get (uint8_t sm);
//:
//: #ifdef MUOS_SM_NAMES
//: const char __flash *
//: muos_sm_name_get (uint8_t sm);
//: #endif
//:
//: bool
//: muos_sm_ready (intptr_t sm);
//: ----
//:
//: 'muos_sm_params_get()' queries the params array for the state machine.
//:
//: 'muos_sm_get()' returns the current state.
//:
//: 'muos_sm_name_get()' returns a textual representation of the current state.
//:
//: 'muos_sm_ready()' returns true when the state is not 'STATE_NONE'. For use as
//: predicate to 'muos_wait()'
//:
enum muos_sm_state*
muos_sm_params_get (uint8_t sm); //TODO: improper api, needs to be improved

enum muos_sm_state
muos_sm_get (uint8_t sm);

#ifdef MUOS_SM_NAMES
const char __flash *
muos_sm_name_get (uint8_t sm);
#endif

bool
muos_sm_ready (intptr_t sm);

//PLANNED: state change callback


#endif // MUOS_SM_H
