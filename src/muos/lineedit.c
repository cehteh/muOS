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

#include <muos/serial.h>
#include <muos/io.h>
#include <muos/error.h>

#include <string.h>

extern void MUOS_LINEEDIT_CALLBACK (const char*);

static uint8_t cursor;
static uint8_t used;
static char buffer[MUOS_LINEEDIT_BUFFER];

static uint8_t pending;

enum
  {
    ESCAPE = 1,
    CSI,
    DEL,
    PGUP,
    PGDOWN,
    POS1,
    END,
    OVWR,
  };

void
muos_lineedit (void)
{
  uint8_t data = muos_serial_rx_byte ();

  if (!muos_error_check (muos_error_rx_buffer_underflow))
    {
      if (used < MUOS_LINEEDIT_BUFFER-1)
        {
          switch (pending<<8 | data)
            {
            case 0x1b:
              // escape
              pending = ESCAPE;
              break;

            case ESCAPE<<8 | 0x5b:
              pending = CSI;
              break;

            case CSI<<8 | 0x41:
            case 0x0b:
              // up
#if 0 //PLANNED: history
#endif
              pending = 0;
              break;

            case CSI<<8 | 0x42:
            case 0x0a:
              // down
#if 0 //PLANNED: history
#endif
              pending = 0;
              break;

            case CSI<<8 | 0x43:
            case 0x0c:
              // right
              if (cursor<used)
                {
                  ++cursor;
                  muos_output_csi_char ('C');
                }
              pending = 0;
              break;

            case CSI<<8 | 0x44:
            case 0x08:
              // left
              if (cursor)
                {
                  --cursor;
                  muos_output_csi_char ('D');
                }
              pending = 0;
              break;

            case CSI<<8 | 0x33:
              // del start
              pending = DEL;
              break;

            case DEL<<8 | 0x7e:
              // del
              if (used && cursor < used)
                {
                  --used;
                  memmove (buffer+cursor, buffer+cursor+1, used-cursor+1);

                  muos_output_csi_char ('s');
                  muos_output_csi_cstr ("?25l");
                  muos_output_cstr (buffer+cursor);
                  muos_output_csi_char ('K');
                  muos_output_csi_char ('u');
                  muos_output_csi_cstr ("?25h");
                }
              pending = 0;
              break;

            case CSI<<8 | 0x35:
              // pgup start
              pending = PGUP;
              break;

            case PGUP<<8 | 0x7e:
              // pgup
#if 0 //PLANNED: completion
#endif
              pending = 0;
              break;

            case CSI<<8 | 0x36:
              // pgdown start
              pending = PGDOWN;
              break;

            case PGDOWN<<8 | 0x7e:
              // pgdown
#if 0 //PLANNED: completion
#endif
              pending = 0;
              break;

            case CSI<<8 | 0x31:
              // pos1 start
              pending = POS1;
              break;

            case POS1<<8 | 0x7e:
              // pos1
              pending = 0;
              cursor = 0;
              muos_output_char ('\r');
              break;

            case CSI<<8 | 0x34:
              // end start
              pending = END;
              break;

            case END<<8 | 0x7e:
              // end
              pending = 0;
              cursor = used;
              muos_output_csi_cstr (NULL);
              muos_output_uint16 (cursor+1);
              muos_output_char ('G');
              break;

            case CSI<<8 | 0x32:
              // ovwr start
              pending = OVWR;
              break;

            case OVWR<<8 | 0x7e:
              // ovwr
              muos_status.lineedit_ovwr ^= 1;
              pending = 0;
              break;

            case 0x09:
              // tab
#if 0 //PLANNED: completion
#endif
              break;

            case 0x0d:
              // return
              MUOS_LINEEDIT_CALLBACK (buffer);
              used = 0;
              cursor = 0;
              *buffer = 0;
              pending = 0;
              break;

            case 0x7f:
              // backspace
              if (cursor > 0)
                {
                  --used;
                  --cursor;
                  memmove (buffer+cursor, buffer+cursor+1, used-cursor+1);

                  muos_output_csi_char ('D');
                  muos_output_csi_char ('s');
                  muos_output_csi_cstr ("?25l");
                  muos_output_cstr (buffer+cursor);
                  muos_output_csi_char ('K');
                  muos_output_csi_char ('u');
                  muos_output_csi_cstr ("?25h");
                }
              break;

            default:
              pending = 0;
              if (cursor < used && muos_status.lineedit_ovwr)
                {
                  buffer[cursor] = data;
                  muos_output_char (buffer[cursor]);
                  ++cursor;
                }
              else if (used < MUOS_LINEEDIT_BUFFER-2)
                {
                  memmove (buffer+cursor+1, buffer+cursor, used-cursor+1);
                  buffer[cursor] = data;
                  muos_output_cstr (buffer+cursor);
                  ++used;
                  ++cursor;
                  if (cursor != used)
                    {
                      muos_output_csi_cstr (0);
                      muos_output_uint8 (used-cursor);
                      muos_output_char ('D');
                    }
                }
            }
        }

      if (used)
        {
          muos_status.lineedit_pending = true;
        }
      else
        {
          muos_status.lineedit_pending = false;
        }
    }

  muos_serial_rxrtq_again (muos_lineedit);
}

