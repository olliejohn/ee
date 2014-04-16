/*
 * screen.c
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

#include "screen.h"

#include "binds.h"
#include "color.h"
#include "window.h"

#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

#define bar_new(y) t_winit(0, y, t_getmaxx(), 1)

#define CSR_X line_get_cur_pos(buf->data[buf->pos])

/* This doesn't really work at all yet... */
void run_cmd_loop(struct Screen *scrn)
{
	struct Line *cmd = line_new();

	t_char ch;
	while (t_getch(&ch) != TUI_ERR && ch != BIND_EXIT &&
						ch != BIND_SAVE_EXIT) {
		switch (ch) {
		case L'š':	/* Shift+tab */
			goto cmd_loop_exit;
		default:
			line_add(cmd, ch);
		}

		t_wclear(scrn->cbar);
		t_mv_wprint(scrn->cbar, 0, 0, L"%ls", cmd);
		t_wrefresh(scrn->cbar);
	}

cmd_loop_exit:
	line_free(cmd);
}

/* Prints line number and character number information */
void screen_print_ch_info(t_window *bar, struct Buffer *buf)
{
	print_to_win(bar, L"L:%d/%d    C:%d/%d      ",
		     buf->pos + 1,
		     buf->size + 1,
		     buf->data[buf->pos]->pos + 1,
		     buf->data[buf->pos]->size + 1);
}

/* Returns 1 if a save is requested or 0 if we just want to exit */
int screen_run(struct Screen *scrn, char *filepath)
{
	struct Buffer *buf = scrn->buf;
/*
	int wpathlen = strlen(filepath) + 1;
	t_char *wpath = calloc(wpathlen, sizeof(t_char));
	mbstowcs(wpath, filepath, wpathlen);

	if (filepath != NULL) {
		//buffer_open(buf, filepath, 0, 0);
		//print_to_win(twin, wpath);
	} else {
		//print_to_win(twin, "Untitled");
	}

	free(wpath);
*/
	print_to_win(scrn->tbar, L"Untitled");

	int i;
	for (i = 0; i < buf->size; i++)
		t_mv_wprint(scrn->bwin, 0, i, L"%ls", buf->data[i]->data);

	screen_print_ch_info(scrn->bbar, buf);

	t_wrefresh(scrn->tbar);
	t_wrefresh(scrn->cbar);

	t_wmove(scrn->bwin, buf->data[buf->pos]->pos, buf->pos);
	t_wrefresh(scrn->bwin);

	t_char ch;
	while (t_getch(&ch) != TUI_ERR && ch != BIND_EXIT &&
						ch != BIND_SAVE_EXIT) {
		switch (ch) {
		case L'š':	/* Shift+tab */
			run_cmd_loop(scrn);
			break;
		case TK_BKSPC:
			if (buffer_backspace(buf) == 0) {
				t_wmove(scrn->bwin, 0, buf->pos);
				t_wclrtoeol(scrn->bwin);
				t_mv_wprint(scrn->bwin, 0, buf->pos, L"%ls",
					    buf->data[buf->pos]->data);
			} else {
				for (i = buf->pos - 1; i <= buf->size; i++)
					t_wmove(scrn->bwin, 0, i);
					t_wclrtoeol(scrn->bwin);
					t_mv_wprint(scrn->bwin, 0, i, L"%ls",
						    buf->data[i]->data);
				t_wmove(scrn->bwin, 0, buf->size + 1);
				t_wclrtoeol(scrn->bwin);
			}

			break;
		case TK_DELETE:
			if (buffer_delete(buf) == 0) {
				t_wmove(scrn->bwin, 0, buf->pos);
				t_wclrtoeol(scrn->bwin);
				t_mv_wprint(scrn->bwin, 0, buf->pos, L"%ls",
					    buf->data[buf->pos]->data);
			} else {
				t_wclear(scrn->bwin);
				for (i = 0; i < buf->size; i++)
					t_mv_wprint(scrn->bwin, 0, i, L"%ls",
						    buf->data[i]->data);
			}

			break;
		case TK_ENTER:
			buffer_new_line(buf); /* This function is buggy - fix it */
			i = (buf->pos == 0) ? 0 : buf->pos - 1;
			for ( ; i <= buf->size; i++) {
				t_wmove(scrn->bwin, 0, i);
				t_wclrtoeol(scrn->bwin);
				t_mv_wprint(scrn->bwin, 0, i, L"%ls",
					    buf->data[i]->data);
			}
			break;
		case TK_LEFT:
			buffer_move_backward(buf);
			break;
		case TK_RIGHT:
			buffer_move_forward(buf);
			break;
		case TK_UP:
			buffer_move_up(buf);
			break;
		case TK_DOWN:
			buffer_move_down(buf);
			break;
		default:
			if (iswprint(ch) || ch == L'\t') {
				buffer_add(buf, ch);
				t_mv_wprint(scrn->bwin, 0, buf->pos, L"%ls",
					    buf->data[buf->pos]->data);
			}
		}

		t_wrefresh(scrn->tbar);

		screen_print_ch_info(scrn->bbar, buf);
		t_wrefresh(scrn->bbar);

		t_wrefresh(scrn->cbar);

		t_wmove(scrn->bwin, buf->data[buf->pos]->pos, buf->pos);
		t_wrefresh(scrn->bwin);
	}

	if (ch == BIND_SAVE_EXIT)
		return 1;
	else
		return 0;
}

struct Screen *screen_new()
{
	struct Screen *scrn = malloc(sizeof(struct Screen));

	t_getmaxxy(scrn->WIDTH, scrn->HEIGHT);

	scrn->tbar = bar_new(0);
	scrn->bwin = t_winit(0, 1, scrn->WIDTH, scrn->HEIGHT - 2);
	scrn->bbar = bar_new(scrn->HEIGHT - 2);
	scrn->cbar = bar_new(scrn->HEIGHT - 1);

	t_wbkgd(scrn->tbar, CS_TITLE_BAR);
	t_wbkgd(scrn->bwin, CS_BUFFER);
	t_wbkgd(scrn->bbar, CS_BOT_BAR);
	t_wbkgd(scrn->cbar, CS_CMD_BAR);

	t_wrefresh(scrn->tbar);
	t_wrefresh(scrn->bwin);
	t_wrefresh(scrn->bbar);
	t_wrefresh(scrn->cbar);

	scrn->buf = buffer_new();

	return scrn;
}

void screen_free(struct Screen *scrn)
{
	t_wdestroy(scrn->tbar);
	t_wdestroy(scrn->bwin);
	t_wdestroy(scrn->bbar);
	t_wdestroy(scrn->cbar);
	buffer_free(scrn->buf);
	free(scrn);
}
