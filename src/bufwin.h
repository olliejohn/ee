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

/*
 * The BufWin is the main structure for using a text document in a buffer.
 * The Buffer struct is not aware of any screen output so it's the job of BufWin
 * to interface it with ncurses (through TUI) whilst handling scrolling,
 * displaying '\t' correctly, optionally printing line numbers and all other
 * idiosyncrasies of display it to the screen. It also contains the array of
 * open buffers, the window for the current buffer and the VTE.
 */

#include "buffer.h"
#include "tui/tui.h"
#include "vte/vte_driver.h"

#define MAX_BUFS 64

struct BufWin {
	t_window *win;
	t_window *linumwin;
	struct Buffer **buffers;
	int num_bufs;
	struct Buffer *curbuf;
	int WIDTH;
	int HEIGHT;
	int ywinoffs;
	int linumoffs;   /* Dist from left of screen to text char 0 */
	int linumdigits; /* The number of digits in the line number */
};

struct BufWin *bufwin_new(int x, int y, int w, int h);
void bufwin_free(struct BufWin *bufwin);
void bufwin_redraw(struct BufWin *bufwin);
void bufwin_process_char(struct BufWin *bufwin, t_char ch);
void bufwin_place_cursor(struct BufWin *bufwin);
void bufwin_refresh(struct BufWin *bufwin);
void bufwin_set_color_scheme(struct BufWin *bufwin, int colpair);
int bufwin_add_buffer(struct BufWin *bufwin);
int bufwin_add_buffer_from_file(struct BufWin *bufwin, char *file);
void bufwin_set_active_buffer(struct BufWin *bufwin, int index);
void bufwin_toggle_draw_linums(struct BufWin *bufwin);

#endif
