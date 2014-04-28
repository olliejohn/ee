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

#ifndef _VTERM_H_
#define _VTERM_H_

#include <sys/types.h>

#include <curses.h>

#define	LIBVTERM_VERSION		"0.99.7"

#define  VTERM_FLAG_VT100     (1<<1)

typedef struct _vterm_s		   vterm_t;

vterm_t*       vterm_create(int width, int height, int flags);
void           vterm_destroy(vterm_t *vterm);
pid_t          vterm_get_pid(vterm_t *vterm);
int            vterm_get_pty_fd(vterm_t *vterm);
const char*    vterm_get_ttyname(vterm_t *vterm);

ssize_t        vterm_read_pipe(vterm_t *vterm);
void           vterm_write_pipe(vterm_t *vterm,unsigned int keycode);

void           vterm_wnd_set(vterm_t *vterm,WINDOW *window);
WINDOW*        vterm_wnd_get(vterm_t *vterm);
void           vterm_wnd_update(vterm_t *vterm);

int            vterm_set_colors(vterm_t *vterm,short fg,short bg);
short          vterm_get_colors(vterm_t *vterm);

void           vterm_erase(vterm_t *vterm);
void           vterm_erase_row(vterm_t *vterm,int row);
void           vterm_erase_rows(vterm_t *vterm,int start_row);
void           vterm_erase_col(vterm_t *vterm,int col);
void           vterm_erase_cols(vterm_t *vterm,int start_cols);
void           vterm_scroll_up(vterm_t *vterm);
void           vterm_scroll_down(vterm_t *vterm);

void           vterm_resize(vterm_t *vterm,unsigned int width,
                            unsigned int height);

#endif
