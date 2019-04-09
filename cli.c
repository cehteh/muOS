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

#include <stdlib.h>
#include <string.h>


#define COMMAND(name, ...) static const char __flash cli_cmd_##name##_str[] = #name;
CLI_COMMANDS
#undef COMMAND

const char __flash * const __flash muos_cli_cmd_names[] =
  {
#define COMMAND(name, ...) cli_cmd_##name##_str,
   CLI_COMMANDS
#undef COMMAND
  };


//PLANNED: prompt
//PLANNED: parameter/prototype description for automatic parsing
//PLANNED: use # as comment line
void
MUOS_CLI_NOCMD (const char*);

void
muos_cli (const char* line)
{
  muos_output_nl (0); //FIXME: linedit should support prompt/newline
  line += strspn_P (line, PSTR(MUOS_CLI_DELIM));

  uint8_t len = strcspn_P (line, PSTR(MUOS_CLI_DELIM));

  const char* cont = line + len;
  cont += strspn_P (cont, PSTR(MUOS_CLI_DELIM));

  if (len)
    {
      enum muos_cli_cmd_id id;
      for (id = 0; id < CLI_CMD_MAX_ID; ++id)
        {
          if (strncmp_P (line, muos_cli_cmd_names[id], len) == 0)
            break;
        }

      if (id == CLI_CMD_MAX_ID)
        {
          MUOS_CLI_NOCMD (line);
          return;
        }

      command_impl[id] (cont);
    }
}



int32_t
muos_cli_arg_int32 (const char** cont)
{
  if (*cont)
    {
      char* end;
      int32_t r = strtol(*cont, &end, 0);
      if (end == *cont)
        {
          *cont = NULL;
        }
      else
        {
          *cont = end;
          *cont += strspn_P (*cont, PSTR(MUOS_CLI_DELIM));
        }
      return r;
    }
  return 0;
}


uint16_t
muos_cli_arg_uint16 (const char** cont)
{
  int32_t r = muos_cli_arg_int32 (cont);
  if (*cont)
    {
      if (r < 0 || r > UINT16_MAX)
        {
          *cont = NULL;
        }
    }
  return r;
}

int16_t
muos_cli_arg_int16 (const char** cont)
{
  int32_t r = muos_cli_arg_int32 (cont);
  if (*cont)
    {
      if (r < INT16_MIN || r > INT16_MAX)
        {
          *cont = NULL;
        }
    }
  return r;
}

uint8_t
muos_cli_arg_uint8 (const char** cont)
{
  int32_t r = muos_cli_arg_int32 (cont);
  if (*cont)
    {
      if (r < 0 || r > UINT8_MAX)
        {
          *cont = NULL;
        }
    }
  return r;
}

int8_t
muos_cli_arg_int8 (const char** cont)
{
  int32_t r = muos_cli_arg_int32 (cont);
  if (*cont)
    {
      if (r < INT8_MIN || r > INT8_MAX)
        {
          *cont = NULL;
        }
    }
  return r;
}


void
muos_cli_arg_strncpy (const char** cont, char* dest, size_t len, const char* delim)
{
  if (*cont)
    {
      //char* begin = *cont;

      size_t slen = strcspn_P (*cont, delim);

      if (slen < len)
        {
          strncpy (dest, *cont, slen);
          dest[slen] = '\0';
          *cont += slen;
          *cont += strspn_P (*cont, delim);
        }
      else
        {
          *dest = '\0';
          *cont = NULL;
        }
    }
}

//size_t
//muos_cli_arg_strlen (const char** cont, const char* delim);
//void
//muos_cli_arg_skip (const char** cont, const char* delim);


#endif
