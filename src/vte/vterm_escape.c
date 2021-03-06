/*
 * Part of the Yaw text editor virtual terminal emulator
 *
 * Based on the no-glib fork of libvte by Ollie Etherington (C) Copyright 2014
 * libvterm Copyright (C) 2009 Bryan Christ
 * libvterm is based on ROTE written by Bruno Takahashi C. de Oliveira
 *
 * As per previous releases, this program is available under the GNU GPL v2
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "vterm.h"
#include "vterm_private.h"
#include "vterm_csi.h"
#include "vterm_render.h"

bool validate_escape_suffix(char c)
{
   if(c >= 'a' && c <= 'z') return TRUE;
   if(c >= 'A' && c <= 'Z') return TRUE;
   if(c == '@') return TRUE;
   if(c == '`') return TRUE;

   return FALSE;
}

void vterm_escape_start(vterm_t *vterm)
{
   vterm->state |= STATE_ESCAPE_MODE;
   vterm->esbuf_len=0;
   vterm->esbuf[0]='\0';

   return;
}

void vterm_escape_cancel(vterm_t *vterm)
{
   vterm->state &= ~STATE_ESCAPE_MODE;
   vterm->esbuf_len=0;
   vterm->esbuf[0]='\0';

   return;
}

void try_interpret_escape_seq(vterm_t *vterm)
{
   char  firstchar=vterm->esbuf[0];
   char  lastchar=vterm->esbuf[vterm->esbuf_len-1];

   /* too early to do anything */
   if(!firstchar) return;

   /* interpret ESC-M as reverse line-feed */
   if(firstchar=='M')
   {
      vterm_scroll_up(vterm);
      vterm_escape_cancel(vterm);
      return;
   }

   if(firstchar != '[' && firstchar != ']')
   {
      vterm_escape_cancel(vterm);
      return;
   }

   /* we have a csi escape sequence: interpret it */
   if(firstchar == '[' && validate_escape_suffix(lastchar))
   {
      vterm_interpret_csi(vterm);
      vterm_escape_cancel(vterm);
   }
   /* we have an xterm escape sequence: interpret it */
   else if(firstchar == ']' && lastchar == '\a')
   {
      vterm_escape_cancel(vterm);
   }

   /* if the escape sequence took up all available space and could
    * not yet be parsed, abort it */
   if(vterm->esbuf_len + 1 >= ESEQ_BUF_SIZE) vterm_escape_cancel(vterm);

   return;
}
