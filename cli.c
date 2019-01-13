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

#ifdef MUOS_CLI

#include <muos/cli.h>
#include <muos/io.h>

//PLANNED: prompt

void
MUOS_CLI_NOCMD (const char*);


void
muos_cli (char* line)
{
  muos_output_nl ();
  line += strspn_P (line, PSTR(MUOS_CLI_DELIM));
  char* tokstate;
  const char* tok = strtok_rP (line, PSTR(MUOS_CLI_DELIM), &tokstate);

  if (tok)
    {
      enum muos_cli_cmd_id id;
      for (id = 0; id < CLI_CMD_MAX_ID; ++id)
        {
          if (strcmp_P (tok, cli_cmd_names[id]) == 0)
            break;
        }

      if (id == CLI_CMD_MAX_ID)
        {
          MUOS_CLI_NOCMD(tok);
          return;
        }

      command_impl[id](&tokstate);
    }
}

#endif
