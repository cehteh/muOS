/*
 *      mµOS            - my micro OS
 *
 * Copyright (C)
 *      2019                           Christian Thäter <ct@pipapo.org>
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

#ifndef MUOS_CLI_H
#define MUOS_CLI_H

#ifndef MUOS_CLI_INCLUDE
#error MUOS_CLI_INCLUDE must be configured
#endif
#include MUOS_CLI_INCLUDE
#ifndef CLI_COMMANDS
#error CLI_COMMANDS must be defined in MUOS_CLI_INCLUDE
#endif

#include <muos/muos.h>


// exapand command names
enum muos_cli_cmd_id
  {
#define COMMAND(name, ...) CLI_CMD_##name,
   CLI_COMMANDS
#undef COMMAND
   CLI_CMD_MAX_ID,
  };



#define COMMAND(name, ...) void cli_cmd_##name(char** tokstate);
   CLI_COMMANDS
#undef COMMAND

#define COMMAND(name, ...) static const char __flash cli_cmd_##name##_str[] = #name;
CLI_COMMANDS
#undef COMMAND

static
const char __flash * const __flash cli_cmd_names[] =
  {
#define COMMAND(name, ...) cli_cmd_##name##_str,
   CLI_COMMANDS
#undef COMMAND
  };


typedef void (*muos_cli_cmd)(char** tokstate);
  
static
const muos_cli_cmd __flash command_impl[] =
  {
#define COMMAND(name, ...) cli_cmd_##name,
   CLI_COMMANDS
#undef COMMAND
  };


void
muos_cli (char* line);

#endif
