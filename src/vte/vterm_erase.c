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

#include "vterm.h"
#include "vterm_private.h"
#include "vterm_render.h"

void vterm_erase(vterm_t *vterm)
{
   int   cell_count;
   int   x,y;
   int   i;

   if(vterm == NULL) return;

   cell_count=vterm->rows*vterm->cols;

   for(i=0;i < cell_count;i++)
   {
      x=i%vterm->cols;
      y=(int)(i/vterm->cols);
      vterm->cells[y][x].ch=0x20;
      vterm->cells[y][x].attr=COLOR_PAIR(vterm->colors);
   }

   return;
}

void vterm_erase_row(vterm_t *vterm,int row)
{
   int  i;

   if(vterm == NULL) return;

   if(row == -1) row=vterm->crow;

   for(i=0;i < vterm->cols;i++)
   {
      vterm->cells[row][i].ch=0x20;
      vterm->cells[row][i].attr=COLOR_PAIR(vterm->colors);
   }

   return;
}

void vterm_erase_rows(vterm_t *vterm,int start_row)
{
   if(vterm == NULL) return;
   if(start_row < 0) return;

   while(start_row < vterm->rows)
   {
      vterm_erase_row(vterm,start_row);
      start_row++;
   }

   return;
}

void vterm_erase_col(vterm_t *vterm,int col)
{
   int  i;

   if(vterm==NULL) return;

   if(col==-1) col=vterm->ccol;

   for(i=0;i < vterm->rows;i++)
   {
      vterm->cells[i][col].ch=0x20;
      vterm->cells[i][col].attr=COLOR_PAIR(vterm->colors);
   }

   return;
}

void vterm_erase_cols(vterm_t *vterm,int start_col)
{
   if(vterm == NULL) return;
   if(start_col < 0) return;

   while(start_col < vterm->cols)
   {
      vterm_erase_col(vterm,start_col);
      start_col++;
   }

   return;
}
