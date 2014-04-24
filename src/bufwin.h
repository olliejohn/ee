/*
 * bufwin.h
 * Part of the Yaw text editor
 *
 * Copyright 2014 Ollie Etherington.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef BUFWIN_H
#define BUFWIN_H

#include <tui.h>
#include "buffer.h"

#define MAX_BUFS 64

struct BufWin {
	t_window *win;
	struct Buffer **buffers;
	int num_bufs;
	struct Buffer *curbuf;
	int WIDTH;
	int HEIGHT;
	int ywinoffs;
	int linumoffs;
};

struct BufWin *bufwin_new(int x, int y, int w, int h);
void bufwin_free(struct BufWin *bufwin);
void bufwin_redraw(struct BufWin *bufwin);
void bufwin_process_char(struct BufWin *bufwin, t_char ch);
void bufwin_refresh(struct BufWin *bufwin);
void bufwin_set_color_scheme(struct BufWin *bufwin, int colpair);
int bufwin_add_buffer(struct BufWin *bufwin);
int bufwin_add_buffer_from_file(struct BufWin *bufwin, char *file);
void bufwin_set_active_buffer(struct BufWin *bufwin, int index);

#endif
