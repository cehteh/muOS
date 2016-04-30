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
#include <muos/lib/utf8.h>

#include <string.h>

extern void MUOS_LINEEDIT_CALLBACK (const char*);

static uint8_t cursor;
static uint8_t used;
static char buffer[MUOS_LINEEDIT_BUFFER];

#if MUOS_LINEEDIT_RECALL == 1
static char recall;
#endif

//PLANNED: strategy on input errors, desync?
//FIXME: full line overflow, because lineedit buffer > txqueue

static uint8_t pending;

enum
  {
    UTF8_1 = 1,
    UTF8_2,
    UTF8_3,
    UTF8_4,
    UTF8DROP,
    ESCAPE,
    CSI,
    DEL,
    PGUP,
    PGDOWN,
    POS1,
    END,
    OVWR,
  };


#ifdef MUOS_LINEEDIT_UTF8
static void
utf8del (void)
{
  uint8_t len = muos_utf8size (buffer+cursor);
  used -= len;
  memmove (buffer+cursor, buffer+cursor+len, used-cursor+len);
}

static void
utf8line_redraw (void)
{
  muos_output_csi_cstr_P ("?25l\r");
  muos_output_cstrn (buffer, cursor);
  muos_output_csi_cstr_P ("s");
  muos_output_cstr (buffer+cursor);
  muos_output_csi_cstr_P ("K\x1b[u\x1b[?25h");
}
#endif



bool
muos_lineedit (void)
{
  uint8_t data = muos_serial_rx_byte ();

  if (!muos_error_check (muos_error_rx_buffer_underflow))
    {
#ifdef MUOS_LINEEDIT_UTF8
      if (pending == UTF8DROP)
        {
          if (muos_utf8cont (data))
            {
              return true;
            }
          else
            pending = 0;
        }
#else
      if (data > 127)
        {
          muos_output_char (7);
          return true;
        }
#endif

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
#if MUOS_LINEEDIT_RECALL == 1
          if (recall)
            {
              *buffer = recall;
              recall = 0;
              cursor = used = strlen (buffer);
              muos_output_cstr (buffer);
            }
#endif

#if 0 //PLANNED: history
#endif
          pending = 0;
          break;

        case CSI<<8 | 0x42:
        case 0x0a:
          // down
#if MUOS_LINEEDIT_RECALL == 1
          if (!recall)
            {
              recall = *buffer;
              used = 0;
              cursor = 0;
              *buffer = 0;
              muos_output_cstr_P ("\r\x1b[K");
            }
#endif

#if 0 //PLANNED: history
#endif
          pending = 0;
          break;

        case CSI<<8 | 0x43:
        case 0x0c:
          // right
          if (cursor<used)
            {
#ifdef MUOS_LINEEDIT_UTF8
              cursor += muos_utf8size (buffer+cursor);
              utf8line_redraw ();
#else
              ++cursor;
              muos_output_csi_char ('C');
#endif
            }
          else
            muos_output_char (7);

          pending = 0;
          break;

        case CSI<<8 | 0x44:
        case 0x08:
          // left
          if (cursor)
            {
#ifdef MUOS_LINEEDIT_UTF8
              cursor -= muos_utf8size (buffer+cursor-1);
              utf8line_redraw ();
#else
              --cursor;
              muos_output_csi_char ('D');
#endif
            }
          else
            muos_output_char (7);

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
#ifdef MUOS_LINEEDIT_UTF8
              utf8del ();
              utf8line_redraw ();
#else
              --used;
              memmove (buffer+cursor, buffer+cursor+1, used-cursor+1);
              muos_output_csi_cstr_P ("s\x1b[?25l");
              muos_output_cstr (buffer+cursor);
              muos_output_csi_cstr_P ("K\x1b[u\x1b[?25h");
#endif

            }
          else
            {
              muos_output_char (7);
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

#ifdef MUOS_LINEEDIT_UTF8
          utf8line_redraw ();
#else
          muos_output_csi_cstr (NULL);
          muos_output_uint16 (cursor+1);
          muos_output_char ('G');
#endif
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
          //TODO: suspend lineedit until the callback is finished (w/ wraper)
          //TODO: push callback on bgq
          MUOS_LINEEDIT_CALLBACK (buffer);
#if MUOS_LINEEDIT_RECALL == 1
          if (*buffer)
            recall = *buffer;
#endif
          used = 0;
          cursor = 0;
          *buffer = 0;
          pending = 0;
          break;

        case 0x7f:
          // backspace
          if (cursor > 0)
            {
#ifdef MUOS_LINEEDIT_UTF8
              uint8_t len = muos_utf8size (buffer+cursor-1);

              used -= len;
              cursor -= len;
              memmove (buffer+cursor, buffer+cursor+len, used-cursor+1);
              utf8line_redraw ();
#else
              --used;
              --cursor;
              memmove (buffer+cursor, buffer+cursor+1, used-cursor+1);
              muos_output_csi_cstr_P ("D\x1b[s\x1b[?25l");
              muos_output_cstr (buffer+cursor);
              muos_output_csi_cstr_P ("K\x1b[u\x1b[?25h");
#endif
            }
          else
            {
              muos_output_char (7);
            }
          break;

        default:

          //nonprintable
          if (data < 32)
            {
              muos_output_char (7);
              pending = 0;
              return true;
            }


#ifdef MUOS_LINEEDIT_UTF8

          if (muos_utf8start (data))
            {
              pending = muos_utf8size ((const char*)&data);
            }
          else if (muos_utf8ascii (data))
            {
              pending = UTF8_1;
            }

          uint8_t ovwr_len = cursor < used && muos_status.lineedit_ovwr?muos_utf8size (buffer+cursor):0;

          if (used+pending - ovwr_len >= MUOS_LINEEDIT_BUFFER)
            {
              if (pending > 1)
                pending = UTF8DROP;
              else
                pending = 0;

              muos_output_char (7);
              break;
            }

          if (ovwr_len && !muos_utf8cont (data))
            utf8del ();

#if MUOS_LINEEDIT_RECALL == 1
          recall = 0;
#endif
          memmove (buffer+cursor+1, buffer+cursor, used-cursor+1);
          buffer[cursor] = data;

          ++used;
          ++cursor;
          if (pending == UTF8_1)
            utf8line_redraw ();
          --pending;

#else
          pending = 0;
          if (cursor < used && muos_status.lineedit_ovwr)
            {
              buffer[cursor] = data;
              muos_output_char (buffer[cursor]);
              ++cursor;
            }
          else if (used < MUOS_LINEEDIT_BUFFER-1)
            {
              memmove (buffer+cursor+1, buffer+cursor, used-cursor+1);
              buffer[cursor] = data;
              muos_output_cstr (buffer+cursor);
              ++used;
              ++cursor;
              if (cursor != used)
                {
                  muos_output_csi_char ('0');
                  muos_output_uint8 (used-cursor);
                  muos_output_char ('D');
                }
            }
#endif
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

  return true;
}

