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
	unsigned int num_bufs;
	struct Buffer *curbuf;
	unsigned int WIDTH;
	unsigned int HEIGHT;
	unsigned int ywinoffs;    /* Window offset for scrolling */
	unsigned int linumdigits; /* The number of digits in the line number */
};

struct BufWin *bufwin_new(int x, int y, unsigned int w, unsigned int h);
void bufwin_free(struct BufWin *bufwin);
void bufwin_resize_linums(struct BufWin *bufwin);
void bufwin_check_line_number_digit_change(struct BufWin *bw);
void bufwin_redraw(struct BufWin *bufwin);
void bufwin_process_char(struct BufWin *bufwin, t_char ch);
void bufwin_place_cursor(struct BufWin *bufwin);
void bufwin_refresh(struct BufWin *bufwin);
void bufwin_set_color_scheme(struct BufWin *bufwin, int colpair);
void bufwin_set_linum_color_scheme(struct BufWin *bufwin, int colpair);
int bufwin_add_buffer(struct BufWin *bufwin);
int bufwin_add_buffer_from_file(struct BufWin *bufwin, char *file);
void bufwin_close_buffer(struct BufWin *bufwin, unsigned int index);
void bufwin_set_active_buffer(struct BufWin *bufwin, int index);
void bufwin_toggle_draw_linums(struct BufWin *bufwin);

inline int bufwin_get_curbuf_index(struct BufWin *bufwin)
{
	for (int i = 0; i < bufwin->num_bufs; i++)
		if (bufwin->buffers[i] == bufwin->curbuf)
			return i;
	return -1;
}

inline void bufwin_close_current_buffer(struct BufWin *bufwin)
{
	register int index = bufwin_get_curbuf_index(bufwin);
	if (index > -1)
		bufwin_close_buffer(bufwin, index);
}

#endif
