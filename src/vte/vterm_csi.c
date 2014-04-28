/*
 * Modifed to remove glib2.0 dependency by Ollie Etherington (C) Copyright 2014
 *
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

#include "vterm.h"
#include "vterm_private.h"
#include "vterm_csi.h"
#include "vterm_colors.h"
#include "vterm_misc.h"

void vterm_interpret_csi(vterm_t *vterm)
{
   static int  csiparam[MAX_CSI_ES_PARAMS];
   int         param_count = 0;
   const char  *p;
   char        verb;

   p=vterm->esbuf+1;
   verb=vterm->esbuf[vterm->esbuf_len-1];

   /* parse numeric parameters */
   while ((*p >= '0' && *p <= '9') || *p == ';' || *p == '?')
   {
      if(*p == '?')
      {
         p++;
         continue;
      }

      if(*p == ';')
      {
         if(param_count >= MAX_CSI_ES_PARAMS) return; /* too long! */
         csiparam[param_count++]=0;
      }
      else
      {
         if(param_count==0) csiparam[param_count++] = 0;

         csiparam[param_count-1] *= 10;
         csiparam[param_count-1] += *p - '0';
      }

      p++;
   }

   /* delegate handling depending on command character (verb) */
   switch (verb)
   {
      case 'm':
      {
         interpret_csi_SGR(vterm,csiparam,param_count);
			break;
      }

      case 'l':
      {
         interpret_dec_RM(vterm,csiparam,param_count);
         break;
      }

      case 'h':
      {
         interpret_dec_SM(vterm,csiparam,param_count);
         break;
      }

      case 'J':
      {
         interpret_csi_ED(vterm,csiparam,param_count);
         break;
      }
      case 'H':
      case 'f':
      {
         interpret_csi_CUP(vterm,csiparam,param_count);
         break;
      }
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'e':
      case 'a':
      case 'd':
      case '`':
      {
         interpret_csi_CUx(vterm,verb,csiparam,param_count);
         break;
      }
      case 'K':
      {
         interpret_csi_EL(vterm,csiparam,param_count);
         break;
      }
      case '@':
      {
         interpret_csi_ICH(vterm,csiparam,param_count);
         break;
      }
      case 'P':
      {
         interpret_csi_DCH(vterm,csiparam,param_count);
         break;
      }
      case 'L':
      {
         interpret_csi_IL(vterm,csiparam,param_count);
         break;
      }
      case 'M':
      {
         interpret_csi_DL(vterm,csiparam,param_count);
         break;
      }
      case 'X':
      {
         interpret_csi_ECH(vterm,csiparam,param_count);
         break;
      }
      case 'r':
      {
         interpret_csi_DECSTBM(vterm,csiparam,param_count);
         break;
      }
      case 's':
      {
         interpret_csi_SAVECUR(vterm,csiparam,param_count);
         break;
      }
      case 'u':
      {
         interpret_csi_RESTORECUR(vterm,csiparam,param_count);
         break;
      }
#ifdef DEBUG
      default:
         fprintf(stderr, "Unrecogized CSI: <%s>\n", rt->pd->esbuf); break;
#endif
   }
}

/* Interpret the DEC RM (reset mode) */
void interpret_dec_RM(vterm_t *vterm,int param[],int pcount)
{
   int i;

   if(pcount==0) return;

   for(i=0;i < pcount;i++)
   {
      /* civis is actually the "normal" vibility for rxvt   */
      if(param[i]==25) vterm->state |= STATE_CURSOR_INVIS;
   }
}

/* Interpret DEC SM (set mode) */
void interpret_dec_SM(vterm_t *vterm,int param[],int pcount)
{
   int i;

   if(pcount==0) return;

   for(i=0;i < pcount;i++)
   {
      /* civis is actually "normal" for rxvt */
      if(param[i]==25) vterm->state &= ~STATE_CURSOR_INVIS;
   }
}

/* interprets a 'move cursor' (CUP) escape sequence */
void interpret_csi_CUP(vterm_t *vterm, int param[], int pcount)
{
   if (pcount == 0)
   {
      /* special case */
      vterm->crow=0;
      vterm->ccol=0;
      return;
   }
   else if (pcount < 2) return;  // malformed

   vterm->crow=param[0]-1;       // convert from 1-based to 0-based.
   vterm->ccol=param[1]-1;       // convert from 1-based to 0-based.

   // vterm->state |= STATE_DIRTY_CURSOR;

   clamp_cursor_to_bounds(vterm);
}

/* Interpret the 'relative mode' sequences: CUU, CUD, CUF, CUB, CNL,
 * CPL, CHA, HPR, VPA, VPR, HPA */
void interpret_csi_CUx(vterm_t *vterm,char verb,int param[],int pcount)
{
   int n=1;

   if(pcount && param[0]>0) n=param[0];

   switch (verb)
   {
      case 'A':         vterm->crow -= n;          break;
      case 'B':
      case 'e':         vterm->crow += n;          break;
      case 'C':
      case 'a':         vterm->ccol += n;          break;
      case 'D':         vterm->ccol -= n;          break;
      case 'E':
      {
         vterm->crow += n;
         vterm->ccol = 0;
         break;
      }
      case 'F':
      {
         vterm->crow -= n;
         vterm->ccol = 0;
         break;
      }
      case 'G':
      case '`':         vterm->ccol=param[0]-1;    break;
      case 'd':         vterm->crow=param[0]-1;    break;
   }

   // vterm->state |= STATE_DIRTY_CURSOR;
   clamp_cursor_to_bounds(vterm);

   return;
}

/* Interpret the 'delete chars' sequence (DCH) */
void interpret_csi_DCH(vterm_t *vterm, int param[], int pcount)
{
   int i;
   int n=1;

   if(pcount && param[0] > 0) n=param[0];

   for(i=vterm->ccol;i < vterm->cols;i++)
   {
      if(i+n < vterm->cols)
      {
         vterm->cells[vterm->crow][i]=vterm->cells[vterm->crow][i+n];
      }
      else
      {
         vterm->cells[vterm->crow][i].ch=0x20;
         vterm->cells[vterm->crow][i].attr=vterm->curattr;
      }
   }
}

/* Interpret a 'set scrolling region' (DECSTBM) sequence */
void interpret_csi_DECSTBM(vterm_t *vterm,int param[],int pcount)
{
   int newtop, newbottom;

   if(!pcount)
   {
      newtop=0;
      newbottom=vterm->rows-1;
   }
   else if(pcount < 2) return; /* malformed */
   else
   {
      newtop=param[0]-1;
      newbottom=param[1]-1;
   }

   /* clamp to bounds */
   if(newtop < 0) newtop=0;
   if(newtop >= vterm->rows) newtop=vterm->rows-1;
   if(newbottom < 0) newbottom=0;
   if(newbottom >= vterm->rows) newbottom=vterm->rows-1;

   /* check for range validity */
   if(newtop > newbottom) return;
   vterm->scroll_min=newtop;
   vterm->scroll_max=newbottom;

   if(vterm->scroll_min != 0)
      vterm->state |= STATE_SCROLL_SHORT;

   if(vterm->scroll_max != vterm->rows-1)
      vterm->state |= STATE_SCROLL_SHORT;

   if((vterm->scroll_min == 0) && (vterm->scroll_max == vterm->rows-1))
      vterm->state &= ~STATE_SCROLL_SHORT;

   return;
}

/* Interpret a 'delete line' sequence (DL) */
void interpret_csi_DL(vterm_t *vterm,int param[],int pcount)
{
   int i, j;
   int n=1;

   if(pcount && param[0] > 0) n=param[0];

   for(i=vterm->crow;i <= vterm->scroll_max; i++)
   {
      if(i+n <= vterm->scroll_max)
      {
         memcpy(vterm->cells[i],vterm->cells[i+n],
            sizeof(vterm_cell_t)*vterm->cols);
      }
      else
      {
         for(j=0;j < vterm->cols;j++)
         {
            vterm->cells[i][j].ch=0x20;
            vterm->cells[i][j].attr=vterm->curattr;
         }
      }
   }

   return;
}

/* Interpret an 'erase characters' (ECH) sequence */
void interpret_csi_ECH(vterm_t *vterm,int param[],int pcount)
{
   int i;
   int n=1;

   if(pcount && param[0] > 0) n=param[0];

   for(i=vterm->ccol;i < vterm->ccol+n; i++)
   {
      if(i >= vterm->cols) break;

      vterm->cells[vterm->crow][i].ch=0x20;
      vterm->cells[vterm->crow][i].attr=vterm->curattr;
   }

   return;
}

/* interprets an 'erase display' (ED) escape sequence */
void interpret_csi_ED(vterm_t *vterm, int param[], int pcount)
{
   int r, c;
   int start_row, start_col, end_row, end_col;

   /* decide range */
   if(pcount && param[0]==2)
   {
      start_row=0;
      start_col=0;
      end_row=vterm->rows-1;
      end_col=vterm->cols-1;
   }
   else if(pcount && param[0]==1)
   {
      start_row=0;
      start_col=0;
      end_row=vterm->crow;
      end_col=vterm->ccol;
   }
   else
   {
      start_row=vterm->crow;
      start_col=vterm->ccol;
      end_row=vterm->rows-1;
      end_col=vterm->cols-1;
   }

   /* clean range */
   for(r=start_row;r <= end_row;r++)
   {
      for(c=start_col;c <= end_col;c++)
      {
         vterm->cells[r][c].ch=0x20;               // erase with blanks.
         vterm->cells[r][c].attr=vterm->curattr;   // set to current attributes.
      }
   }
}

/* Interpret the 'erase line' escape sequence */
void interpret_csi_EL(vterm_t *vterm, int param[], int pcount)
{
   int erase_start, erase_end, i;
   int cmd=0;

   if(pcount>0) cmd=param[0];

   switch(cmd)
   {
      case 1:
      {
         erase_start=0;
         erase_end=vterm->ccol;
         break;
      }
      case 2:
      {
         erase_start=0;
         erase_end=vterm->cols-1;
         break;
      }
      default:
      {
         erase_start=vterm->ccol;
         erase_end=vterm->cols-1;
         break;
      }
   }

   for(i=erase_start;i <= erase_end;i++)
   {
      vterm->cells[vterm->crow][i].ch = 0x20;
      vterm->cells[vterm->crow][i].attr = vterm->curattr;
   }

   return;
}

/* Interpret the 'insert blanks' sequence (ICH) */
void interpret_csi_ICH(vterm_t *vterm,int param[],int pcount)
{
   int i;
   int n=1;

   if(pcount && param[0]>0) n=param[0];

   for (i=vterm->cols-1;i >= vterm->ccol+n;i--)
   {
      vterm->cells[vterm->crow][i]=vterm->cells[vterm->crow][i-n];
   }

   for(i=vterm->ccol;i < vterm->ccol+n;i++)
   {
      vterm->cells[vterm->crow][i].ch=0x20;
      vterm->cells[vterm->crow][i].attr = vterm->curattr;
   }

   return;
}

/* Interpret an 'insert line' sequence (IL) */
void interpret_csi_IL(vterm_t *vterm,int param[],int pcount)
{
   int i, j;
   int n=1;

   if(pcount && param[0] > 0) n=param[0];

   for(i=vterm->scroll_max;i >= vterm->crow+n;i--)
   {
      memcpy(vterm->cells[i],vterm->cells[i - n],
         sizeof(vterm_cell_t)*vterm->cols);
   }

   for(i=vterm->crow;i < vterm->crow+n; i++)
   {
      if(i>vterm->scroll_max) break;

      for(j=0;j < vterm->cols; j++)
      {
         vterm->cells[i][j].ch = 0x20;
         vterm->cells[i][j].attr=vterm->curattr;
      }
   }

   return;
}

void interpret_csi_RESTORECUR(vterm_t *vterm,int param[],int pcount)
{
   vterm->ccol=vterm->saved_x;
   vterm->crow=vterm->saved_y;

   return;
}

void interpret_csi_SAVECUR(vterm_t *vterm,int param[],int pcount)
{
   vterm->saved_x=vterm->ccol;
   vterm->saved_y=vterm->crow;

   return;
}

/*
   VT100 SGR documentation
   From http://vt100.net/docs/vt510-rm/SGR table 5-16
   0  All attributes off
   1  Bold
   4  Underline
   5  Blinking
   7  Negative image
   8  Invisible image
   10    The ASCII character set is the current 7-bit
         display character set (default) - SCO Console only.
   11    Map Hex 00-7F of the PC character set codes
         to the current 7-bit display character set
         - SCO Console only.
   12    Map Hex 80-FF of the current character set to
         the current 7-bit display character set - SCO
         Console only.
   22    Bold off
   24    Underline off
   25    Blinking off
   27    Negative image off
   28    Invisible image off
*/

/* interprets a 'set attribute' (SGR) CSI escape sequence */
void interpret_csi_SGR(vterm_t *vterm, int param[], int pcount)
{
   int   i;
   short colors;
   short default_fg,default_bg;

   if(pcount==0)
   {
      vterm->curattr=A_NORMAL;                        // reset attributes
      return;
   }

   for(i=0;i<pcount;i++)
   {
      if(param[i]==0)                                 // reset attributes
      {
         vterm->curattr=A_NORMAL;
         continue;
      }

      if(param[i]==1 || param[i]==2 || param[i]==4)   // bold on
      {
         vterm->curattr |= A_BOLD;
         continue;
      }

      if(param[i]==5)                                 // blink on
      {
         vterm->curattr |= A_BLINK;
         continue;
      }

      if(param[i]==7 || param[i]==27)                 // reverse on
      {
         vterm->curattr |= A_REVERSE;
         continue;
      }

      if(param[i]==8)                                 // invisible on
      {
         vterm->curattr |= A_INVIS;
         continue;
      }

      if(param[i]==10)                                // rmacs
      {
         vterm->state &= ~STATE_ALT_CHARSET;
         continue;
      }

		if(param[i]==11)                                // smacs
      {
         vterm->state |= STATE_ALT_CHARSET;
         continue;
      }

      if(param[i]==22 || param[i]==24)                // bold off
      {
         vterm->curattr &= ~A_BOLD;
         continue;
      }

      if(param[i]==25)                                // blink off
      {
         vterm->curattr &= ~A_BLINK;
         continue;
      }

      if(param[i]==28)                                // invisible off
      {
         vterm->curattr &= ~A_INVIS;
         continue;
      }

      if(param[i] >= 30 && param[i] <= 37)            // set fg color
      {
         vterm->fg=param[i]-30;
         colors=find_color_pair(vterm->fg,vterm->bg);
         if(colors==-1) colors=0;
         vterm->curattr |= COLOR_PAIR(colors);
         continue;
      }

      if(param[i] >= 40 && param[i] <= 47)            // set bg color
      {
         vterm->bg=param[i]-40;
         colors=find_color_pair(vterm->fg,vterm->bg);
         if(colors==-1) colors=0;
         vterm->curattr |= COLOR_PAIR(colors);
         continue;
      }

      if(param[i]==39)                                // reset fg color
      {
         pair_content(vterm->colors,&default_fg,&default_bg);
         vterm->fg=default_fg;
         colors=find_color_pair(vterm->fg,vterm->bg);
         if(colors==-1) colors=0;
         vterm->curattr |= COLOR_PAIR(colors);
         continue;
      }

      if(param[i]==49)                                // reset bg color
      {
         pair_content(vterm->colors,&default_fg,&default_bg);
         vterm->bg=default_bg;
         colors=find_color_pair(vterm->fg,vterm->bg);
         if(colors==-1) colors=0;
         vterm->curattr |= COLOR_PAIR(colors);
         continue;
      }
   }
}
