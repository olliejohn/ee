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

#include "bufwin.h"

#include "color.h"

#include <limits.h>
#include <stdlib.h>
#include <wctype.h>

int DRAW_LINE_NUMS = 1;
int SHOW_VTE = 1;

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

int count_int_digits(int n)
{
	if (n < 0)
		return count_int_digits((n == INT_MIN) ? INT_MAX : -n);

	if (n < 10)
		return 1;

	return 1 + count_int_digits(n / 10);
}

int bufwin_get_linum_digits(struct BufWin *bw)
{
	return count_int_digits(bw->curbuf->size + 1);
}

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

void bufwin_move_left(struct BufWin *bw)
{
	buffer_move_backward(bw->curbuf);
}

void bufwin_move_right(struct BufWin *bw)
{
	buffer_move_forward(bw->curbuf);
}

void bufwin_move_up(struct BufWin *bw)
{
	if (buffer_move_up(bw->curbuf) == -1)
		return;

	if (t_wgetcury(bw->win) == 0 && bw->curbuf->pos >= 0) {
		bw->ywinoffs--;
		bufwin_redraw(bw);
	}
}

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

void bufwin_refresh(struct BufWin *bufwin)
{
	t_wrefresh(bufwin->win);
}

void bufwin_set_color_scheme(struct BufWin *bw, int colpair)
{
	t_wbkgd(bw->win, colpair);
}

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

void bufwin_set_active_buffer(struct BufWin *bw, int index)
{
	bw->curbuf = bw->buffers[index];
	bw->linumdigits = bufwin_get_linum_digits(bw);
}

void bufwin_toggle_draw_linums(struct BufWin *bw)
{
	DRAW_LINE_NUMS ^= 1;
	bufwin_redraw(bw);
}
