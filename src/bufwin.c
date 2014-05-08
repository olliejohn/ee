/*
 * bufwin.c
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

/*
 * The BufWin is the main structure for using a text document in a buffer.
 * The Buffer struct is not aware of any screen output so it's the job of BufWin
 * to interface it with ncurses (through TUI) whilst handling scrolling,
 * displaying '\t' correctly, optionally printing line numbers and all other
 * idiosyncrasies of display it to the screen. It also contains the array of
 * open buffers and the window for the current buffer.
 */

#include "bufwin.h"

#include "color.h"
#include "tools.h"

#include <limits.h>
#include <stdlib.h>
#include <wctype.h>

/* This will eventually be replaced with an option in the Settings struct */
int DRAW_LINE_NUMS = 1;

/* Create a new bufwin object at (x, y) with w columns and h rows */
struct BufWin *bufwin_new(int x, int y, unsigned int w, unsigned int h)
{
	struct BufWin *bw = malloc(sizeof(struct BufWin));

	if (DRAW_LINE_NUMS) {
	/* 3 is a single character digit (1) with a padding space either side */
		bw->linumwin = t_winit(x, y, 3, h);
		bw->win = t_winit(x + 3, y, w - 3, h);
	} else {
		bw->linumwin = NULL;
		bw->win = t_winit(x, y, w, h);
	}

	bw->buffers = malloc(sizeof(struct Buffer *) * MAX_BUFS);

	unsigned int i;
	for (i = 0; i < MAX_BUFS; i++)
		bw->buffers[i] = NULL;

	bw->num_bufs = 0;
	bw->curbuf = NULL;
	bw->WIDTH = w;
	bw->HEIGHT = h;
	bw->ywinoffs = 0;
	bw->relcursy = 0;
	bw->linumdigits = 0;

	return bw;
}

/*
 * Destroy the bufwin including all buffers (which should be saved beforehand
 * in the calling code if required).
 */
void bufwin_free(struct BufWin *bw)
{
	unsigned int i;
	for (i = 0; i < bw->num_bufs; i++)
		buffer_free(bw->buffers[i]);

	t_wdestroy(bw->linumwin);
	t_wdestroy(bw->win);
	bw->curbuf = NULL;
	free(bw->buffers);
	free(bw);
}

/* Returns the number of digits needed for line numbers in the current buffer */
static unsigned int bufwin_get_linum_digits(struct BufWin *bw)
{
	register unsigned int n = bw->curbuf->size + 1;
	if (n < 10)		return 1;
 	if (n < 100)		return 2;
 	if (n < 1000)		return 3;
 	if (n < 10000)		return 4;
 	if (n < 100000)		return 5;
 	if (n < 1000000)	return 6;
 	if (n < 10000000)	return 7;
 	if (n < 100000000)	return 8;
 	if (n < 1000000000)	return 9;
 	return 10; /* Max number of digits in an int */
}

/*
 * Checks if the linum window and buffer window need to be resized to acount for
 * a change in the number of digits in the maximum line number and redraws
 * accordingly.
 */
void bufwin_resize_linums(struct BufWin *bw)
{
	if (!DRAW_LINE_NUMS)
		return;

	/* Calculated linum width and actual current linum width */
	unsigned int lw = bufwin_get_linum_digits(bw) + 2; /*2 padding spaces*/
	unsigned int oldlw = t_wgetmaxx(bw->linumwin);

	if (lw != oldlw) {
		t_wclear(bw->win);
		t_wclear(bw->linumwin);

		t_wresize(bw->linumwin, lw, t_wgetmaxy(bw->linumwin));

		unsigned int bwid = t_wgetmaxx(bw->win) +
				    t_wgetmaxx(bw->linumwin) - lw;
		t_wresize(bw->win, bwid, t_wgetmaxy(bw->win));
		t_mvwin(bw->win, lw, t_wgetbegy(bw->win));

		t_wrefresh(bw->win);
		t_wrefresh(bw->linumwin);

		bw->linumdigits = lw - 2;

		bufwin_redraw(bw);
	}
}

/*
 * Check if the number of digits needed for the line numbers has changed - if it
 * has, it automatically redraws the screen.
 */
void bufwin_check_line_number_digit_change(struct BufWin *bw)
{
	if (DRAW_LINE_NUMS)
		if (bw->linumdigits != bufwin_get_linum_digits(bw))
			bufwin_resize_linums(bw);
}

/*
 * When passed a line index from the current buffer in 'ln', this function
 * returns it's y-position relative to the screen origin or -1 if the given
 * line is currently scrolled off of the screen (or if it doesn't exist).
 */
int bufwin_get_line_screen_position(struct BufWin *bw, unsigned int ln)
{
	int pos = ln - bw->ywinoffs;
	if (pos < 0 || pos > bw->HEIGHT)
		return -1;
	return pos;
}

/*
 * Draw a line from the current buffer to the screen. Note that the line number
 * is the position on the screen, NOT the line number in the buffer. This
 * function also draws the line number to the screen if that option is turned on
 */
#define scroll_offs line + bw->ywinoffs
void bufwin_render_screen_line(struct BufWin *bw, unsigned int line)
{
	if (DRAW_LINE_NUMS) {
		t_mv_wprint(bw->linumwin, 0, line, L" %*d ",
			    bw->linumdigits, line + bw->ywinoffs + 1);
		t_wrefresh(bw->linumwin);
	}

	t_char *text = buffer_get_text_at(bw->curbuf, scroll_offs);
	if (text != NULL)
		t_mv_wprint(bw->win, 0, line, text);
	else
		DEBUG_DUMP("Index out of bounds: bufwin_render_screen_line:\n"
			   "Fetching index: %d from a buffer of size %d\n"
			   "Y relative to screen: %d - Y window offset: %d\n",
			   scroll_offs, bw->curbuf->size, line, bw->ywinoffs);
}
#undef scroll_offs

/*
 * Exactly the same as bufwin_render_screen_line except the line index is
 * relative to the buffer, not the screen position.
 */
void bufwin_render_buffer_line(struct BufWin *bw, unsigned int bln)
{
	register int l = bufwin_get_line_screen_position(bw, bln);
	if (unlikely(l == -1))
		return;
	else
		bufwin_render_screen_line(bw, l);
}

/* Redraw the entire buffer screen */
void bufwin_redraw(struct BufWin *bw)
{
	t_wclear(bw->win);

	unsigned int i;
	for (i = 0; i < bw->HEIGHT && i + bw->ywinoffs <= bw->curbuf->size; i++)
		bufwin_render_screen_line(bw, i);

	bufwin_place_cursor(bw);  /* Implicitly refreshes window */
}

/* Scroll up one line - requires a redraw afterwards */
static inline void bufwin_scroll_up(struct BufWin *bw)
{
	bw->ywinoffs--;
}

/* Scroll down one line - requires a redraw afterwards */
static inline void bufwin_scroll_down(struct BufWin *bw)
{
	bw->ywinoffs++;
}

/* Move up in the buffer and scroll if needed */
void bufwin_move_up(struct BufWin *bw)
{
	if (buffer_move_up(bw->curbuf) == -1)
		return;

	if (t_wgetcury(bw->win) == 0 && bw->curbuf->pos >= 0) {
		bufwin_scroll_up(bw);
		bufwin_redraw(bw);
	}
}

/* Move down in the buffer and scroll if needed */
void bufwin_move_down(struct BufWin *bw)
{
	if (buffer_move_down(bw->curbuf) == -1)
		return;

	if (t_wgetcury(bw->win) >= bw->HEIGHT - 1 &&
	    bw->curbuf->pos <= bw->curbuf->size) {
		bufwin_scroll_down(bw);
		bufwin_redraw(bw);
	}
}

/* Backspace at the current buffer position and scroll if needed */
void bufwin_backspace(struct BufWin *bw)
{
	if (buffer_backspace(bw->curbuf) == 0) {
		t_wmove(bw->win, 0, bw->curbuf->pos);
		t_wclrtoeol(bw->win);
		bufwin_render_screen_line(bw, bw->curbuf->pos);
	} else {
		int i;
		for (i = bw->curbuf->pos; i <= bw->curbuf->size; i++) {
			bufwin_render_screen_line(bw, i);
			t_wclrtoeol(bw->win);
		}

		t_wmove(bw->win, 0, bw->curbuf->size + 1);
		t_wclrtoeol(bw->win);
		t_wmove(bw->linumwin, 0, bw->curbuf->size + 1);
		t_wclrtoeol(bw->linumwin);
		t_wrefresh(bw->linumwin);
	}

	bufwin_check_line_number_digit_change(bw);
}

/* Delete the char at the current position in the buffer and scroll if needed */
#define buf bw->curbuf
void bufwin_delete(struct BufWin *bw)
{
	if (buf->pos >= buf->size &&
	    buf->data[buf->pos]->pos >= buf->data[buf->pos]->size)
		return;

	buffer_move_forward(buf);
	if (buffer_backspace(buf) == 0) {
		t_wmove(bw->win, 0, buf->pos);
		t_wclrtoeol(bw->win);
		bufwin_render_screen_line(bw, buf->pos);
	} else {
		unsigned int i;
		for (i = buf->pos; i <= buf->size; i++) {
			bufwin_render_screen_line(bw, i);
			t_wclrtoeol(bw->win);
		}

		t_wmove(bw->win, 0, buf->size + 1);
		t_wclrtoeol(bw->win);
		t_wmove(bw->linumwin, 0, buf->size + 1);
		t_wclrtoeol(bw->linumwin);
		t_wrefresh(bw->linumwin);
	}

	bufwin_check_line_number_digit_change(bw);
}
#undef buf

/* Add a new line to the buffer in the current position and scroll if needed */
void bufwin_new_line(struct BufWin *bw)
{
	buffer_new_line(bw->curbuf);

	unsigned int screen_max = bw->HEIGHT - 1;
	if (t_wgetcury(bw->win) >= screen_max) {
		bufwin_scroll_down(bw);
		bufwin_redraw(bw);
		t_wmove(bw->win, 0, screen_max);
		t_wclrtoeol(bw->win);
		bufwin_render_screen_line(bw, screen_max);
	} else {
		unsigned int i = (bw->curbuf->pos == 0) ? 0 : bw->curbuf->pos-1;
		for ( ; i <= bw->curbuf->size; i++) {
			t_wmove(bw->win, 0, i);
			t_wclrtoeol(bw->win);
			bufwin_render_screen_line(bw, i);
		}
	}

	bufwin_check_line_number_digit_change(bw);

	t_wmove(bw->win, 0, bw->curbuf->size + 1);
	t_wclrtoeol(bw->win);
}

/* Add a character to the current buffer at the current position */
void bufwin_add(struct BufWin *bw, t_char ch)
{
	if (iswprint(ch) || ch == L'\t') {
		buffer_add(bw->curbuf, ch);
		bufwin_render_buffer_line(bw, bw->curbuf->pos);
	}
}

/* Process a character for the buffer */
void bufwin_process_char(struct BufWin *bw, t_char ch)
{
	switch (ch) {
	case TK_BKSPC:
		bufwin_backspace(bw);
		break;
	case TK_DELETE:
		bufwin_delete(bw);
		break;
	case TK_ENTER:
		bufwin_new_line(bw);
		break;
	case TK_LEFT:
		buffer_move_backward(bw->curbuf);
		break;
	case TK_RIGHT:
		buffer_move_forward(bw->curbuf);
		break;
	case TK_UP:
		bufwin_move_up(bw);
		break;
	case TK_DOWN:
		bufwin_move_down(bw);
		break;
	case TK_CTRL_LEFT:
		buffer_prev_word(bw->curbuf);
		break;
	case TK_CTRL_RIGHT:
		buffer_next_word(bw->curbuf);
		break;
	case TK_HOME:
		buffer_home(bw->curbuf);
		break;
	case TK_END:
		buffer_end(bw->curbuf);
		break;
	case TK_PGUP:
		buffer_pgup(bw->curbuf, t_wgetmaxy(bw->win));
		break;
	case TK_PGDN:
		buffer_pgdn(bw->curbuf, t_wgetmaxy(bw->win));
		break;
	default:
		bufwin_add(bw, ch);
	}
}

/* Put the cursor in the current position account for scrolling and tabs */
#define curline bw->curbuf->data[bw->curbuf->pos]
void bufwin_place_cursor(struct BufWin *bw)
{
	t_wmove(bw->win,
		line_get_curs_pos(curline),
		bw->curbuf->pos - bw->ywinoffs);

	bufwin_refresh(bw);
}
#undef curline

/* Refresh the buffer window */
void bufwin_refresh(struct BufWin *bw)
{
	if (DRAW_LINE_NUMS)
		t_wrefresh(bw->linumwin);
	t_wrefresh(bw->win);
}

/* Set the colors for the buffer; line number are handled directly in color.c */
void bufwin_set_color_scheme(struct BufWin *bw, int colpair)
{
	t_wbkgd(bw->win, colpair);
}

void bufwin_set_linum_color_scheme(struct BufWin *bw, int colpair)
{
	t_wbkgd(bw->linumwin, colpair);
}

/* Add a new buffer - note that this does NOT switch to the new buffer */
int bufwin_add_buffer(struct BufWin *bw)
{
	if (bw->num_bufs >= MAX_BUFS)
		return -1;

	bw->buffers[bw->num_bufs++] = buffer_new();
	return 0;
}

/*
 * Create a new buffer and open the file in it  - note that this DOES switch to
 * the new buffer
 */
int bufwin_add_buffer_from_file(struct BufWin *bw, char *file)
{
	bufwin_add_buffer(bw);
	bufwin_set_active_buffer(bw, bw->num_bufs - 1);
	int status = buffer_open(bw->curbuf, file);
	/* Second call to set_active_buffer handles rendering correctly */
	bufwin_set_active_buffer(bw, bw->num_bufs - 1);
	return status;
}

/* Set the active buffer and redraw the screen */
void bufwin_set_active_buffer(struct BufWin *bw, int index)
{
	bw->curbuf = bw->buffers[index];
	bw->linumdigits = bufwin_get_linum_digits(bw);
	bufwin_resize_linums(bw);
	bufwin_redraw(bw);
}

/*
 * Toggle whether or not line numbers should be drawn and then redraw with the
 * new setting
 */
void bufwin_toggle_draw_linums(struct BufWin *bw)
{
	DRAW_LINE_NUMS ^= 1;
	bufwin_redraw(bw);
}
