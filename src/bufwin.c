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
 * open buffers, the window for the current buffer and the VTE.
 */

#include "bufwin.h"

#include "color.h"

#include <limits.h>
#include <stdlib.h>
#include <wctype.h>

/* These two will eventually be replaced with options in the Settings struct */
int DRAW_LINE_NUMS = 1;
int SHOW_VTE = 1;

/*
 * Create a new bufwin object at (x, y) with w columns and h rows. Note that
 * this includes the space for the VTE as well.
 */
struct BufWin *bufwin_new(int x, int y, int w, int h)
{
	struct BufWin *bw = malloc(sizeof(struct BufWin));

	int termx = (w - x) / 2;
	bw->win = t_winit(x, y, termx - 1, h);
	bw->vte = vte_new(termx, y, w - termx, h);

	bw->buffers = malloc(sizeof(struct Buffer *) * MAX_BUFS);

	int i;
	for (i = 0; i < MAX_BUFS; i++)
		bw->buffers[i] = NULL;

	bw->num_bufs = 0;
	bw->curbuf = NULL;
	bw->WIDTH = w;
	bw->HEIGHT = h;
	bw->ywinoffs = 0;
	bw->linumoffs = 0;

	return bw;
}

/*
 * Destroy the bufwin including all buffers (which should be saved beforehand
 * in the calling code) and the VTE.
 */
void bufwin_free(struct BufWin *bw)
{
	int i;
	for (i = 0; i < bw->num_bufs; i++)
		buffer_free(bw->buffers[i]);

	t_wdestroy(bw->win);
	vte_free(bw->vte);
	bw->curbuf = NULL;
	free(bw->buffers);
	free(bw);
}

/* Helper function for counting the number of digits in an int */
int count_int_digits(int n)
{
	if (n < 0)
		return count_int_digits((n == INT_MIN) ? INT_MAX : -n);

	if (n < 10)
		return 1;

	return 1 + count_int_digits(n / 10);
}

/* Returns the number of digits needed for line numbers in the current buffer */
int bufwin_get_linum_digits(struct BufWin *bw)
{
	return count_int_digits(bw->curbuf->size + 1);
}

/*
 * Draw a line from the current buffer to the screen. Note that the line number
 * is the position on the screen, NOT the line number in the buffer. This
 * function also draws the line number to the screen if that option is turned on
 */
void bufwin_render_line(struct BufWin *bw, int line)
{
	int x = 0;

	if (DRAW_LINE_NUMS) {
		t_wattron(bw->win, CS_LINE_NUM);
		t_mv_wprint(bw->win, 0, line, L" %*d %n", bw->linumdigits,
			    line + bw->ywinoffs + 1, &x);
		t_wattroff(bw->win, CS_LINE_NUM);
	}

	bw->linumoffs = x;

	t_mv_wprint(bw->win, x, line,
		    bw->curbuf->data[line + bw->ywinoffs]->data);
}

/* Redraw the entire buffer screen */
void bufwin_redraw(struct BufWin *bw)
{
	t_wclear(bw->win);

	int i;
	for (i = 0; i <= bw->curbuf->size && i < bw->HEIGHT; i++)
		bufwin_render_line(bw, i);

	t_wmove(bw->win,
		bw->curbuf->data[bw->curbuf->pos]->pos,
		bw->curbuf->pos);

	t_wrefresh(bw->win);
}

/*
 * Move left in the buffer. This function does not yet, but will in future be
 * responsible for placing the cursor correctly when the line contains tabs
 */
void bufwin_move_left(struct BufWin *bw)
{
	buffer_move_backward(bw->curbuf);
}

/*
 * Move right in the buffer. This function does not yet, but will in future be
 * responsible for placing the cursor correctly when the line contains tabs
 */
void bufwin_move_right(struct BufWin *bw)
{
	buffer_move_forward(bw->curbuf);
}

/* Move up in the buffer - this is the function that handles scrolling up */
void bufwin_move_up(struct BufWin *bw)
{
	if (buffer_move_up(bw->curbuf) == -1)
		return;

	if (t_wgetcury(bw->win) == 0 && bw->curbuf->pos >= 0) {
		bw->ywinoffs--;
		bufwin_redraw(bw);
	}
}

/* Move up in the buffer - this is the function that handles scrolling down */
void bufwin_move_down(struct BufWin *bw)
{
	if (buffer_move_down(bw->curbuf) == -1)
		return;

	if (t_wgetcury(bw->win) >= bw->HEIGHT - 1 &&
	    bw->curbuf->pos < bw->curbuf->size) {
		bw->ywinoffs++;
		bufwin_redraw(bw);
	}
}

/*
 * Check if the number of digits needed for the line numbers has changed - if it
 * has, it automatically redraws the screen.
 */
void bufwin_check_line_number_digit_change(struct BufWin *bw)
{
	if (DRAW_LINE_NUMS) {
		int new = bufwin_get_linum_digits(bw);

		if (bw->linumdigits != new) {
			bw->linumdigits = new;
			bufwin_redraw(bw);
		}
	}
}

/*
 * Process a character for the buffer - note that input for the VTE is NOT
 * handled here but rather in vte/vte_driver.c
 */
#define buf bw->curbuf
void bufwin_process_char(struct BufWin *bw, t_char ch)
{
	int i;

	switch (ch) {
	case TK_BKSPC:
		if (buffer_backspace(buf) == 0) {
			t_wmove(bw->win, 0, buf->pos);
			t_wclrtoeol(bw->win);
			bufwin_render_line(bw, buf->pos);
		} else {
			for (i = buf->pos; i <= buf->size; i++) {
				bufwin_render_line(bw, i);
				t_wclrtoeol(bw->win);
			}

			t_wmove(bw->win, 0, buf->size + 1);
			t_wclrtoeol(bw->win);
		}

		bufwin_check_line_number_digit_change(bw);

		break;
	case TK_DELETE:
		if (buf->pos >= buf->size &&
		    buf->data[buf->pos]->pos >= buf->data[buf->pos]->size)
			break;

		buffer_move_forward(buf);
		if (buffer_backspace(buf) == 0) {
			t_wmove(bw->win, 0, buf->pos);
			t_wclrtoeol(bw->win);
			bufwin_render_line(bw, buf->pos);
		} else {
			for (i = buf->pos; i <= buf->size; i++) {
				bufwin_render_line(bw, i);
				t_wclrtoeol(bw->win);
			}

			t_wmove(bw->win, 0, buf->size + 1);
			t_wclrtoeol(bw->win);
		}

		bufwin_check_line_number_digit_change(bw);

		break;
	case TK_ENTER:
		buffer_new_line(buf);

		i = (buf->pos == 0) ? 0 : buf->pos - 1;
		for ( ; i <= buf->size; i++) {
			t_wmove(bw->win, 0, i);
			t_wclrtoeol(bw->win);
			bufwin_render_line(bw, i);
		}

		bufwin_check_line_number_digit_change(bw);

		t_wmove(bw->win, 0, buf->size + 1);
		t_wclrtoeol(bw->win);

		break;
	case TK_LEFT:
		bufwin_move_left(bw);
		break;
	case TK_RIGHT:
		bufwin_move_right(bw);
		break;
	case TK_UP:
		bufwin_move_up(bw);
		break;
	case TK_DOWN:
		bufwin_move_down(bw);
		break;
	default:
		if (iswprint(ch) || ch == L'\t') {
			buffer_add(buf, ch);
			bufwin_render_line(bw, buf->pos);
		}
	}
}
#undef buf

/* Refresh the buffer window - doesn't refresh the VTE. */
void bufwin_refresh(struct BufWin *bufwin)
{
	t_wrefresh(bufwin->win);
}

/*
 * Set the colors for the buffer - line number are handled directly in color.c
 * and VTE colors and handled in vte/vte_driver.c
 */
void bufwin_set_color_scheme(struct BufWin *bw, int colpair)
{
	t_wbkgd(bw->win, colpair);
}

/* Add a new buffer - note that this does NOT switch to the new buffer */
int bufwin_add_buffer(struct BufWin *bw)
{
	if (bw->num_bufs >= MAX_BUFS)
		return -1;

	bw->buffers[bw->num_bufs++] = buffer_new();
	return 0;
}

/* TODO: Implement this */
int bufwin_add_buffer_from_file(struct BufWin *bw, char *file)
{
	return 0;
}

/* Set the active buffer and redraw the screen */
void bufwin_set_active_buffer(struct BufWin *bw, int index)
{
	bw->curbuf = bw->buffers[index];
	bw->linumdigits = bufwin_get_linum_digits(bw);
	bufwin_redraw(bw);
}

/*
 * Toggle whether or not line numbers should be drawn and the redraw with the
 * new setting
 */
void bufwin_toggle_draw_linums(struct BufWin *bw)
{
	DRAW_LINE_NUMS ^= 1;
	bufwin_redraw(bw);
}
