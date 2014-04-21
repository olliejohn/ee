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
#include "lisp.h"

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#define bar_new(y) t_winit(0, y, t_getmaxx(), 1)

#define CSR_X line_get_cur_pos(buf->data[buf->pos])

void screen_set_title(struct Screen *scrn, t_char *title, ...)
{
	va_list args;
	va_start(args, title);
	t_mv_vwprint(scrn->tbar, 0, 0, title, args);
	va_end(args);

	t_wclrtoeol(scrn->tbar);
	t_wrefresh(scrn->tbar);
}

void screen_vset_title(struct Screen *scrn, t_char *title, va_list args)
{
	t_mv_vwprint(scrn->tbar, 0, 0, title, args);
	t_wclrtoeol(scrn->tbar);
	t_wrefresh(scrn->tbar);
}

void screen_set_status(struct Screen *scrn, t_char *status, ...)
{
	va_list args;
	va_start(args, status);
	t_mv_vwprint(scrn->bbar, 0, 0, status, args);
	va_end(args);

	t_wclrtoeol(scrn->bbar);
	t_wrefresh(scrn->bbar);
}

void screen_vset_status(struct Screen *scrn, t_char *status, va_list args)
{
	t_mv_vwprint(scrn->bbar, 0, 0, status, args);
	t_wclrtoeol(scrn->bbar);
	t_wrefresh(scrn->bbar);
}

/* Prints line number and character number information */
#define CH_INFO_FMT L"L:%d/%d    C:%d/%d        "
#define CH_INFO_OFFS 24
void screen_print_ch_info(struct Screen *scrn, struct Buffer *buf)
{
	t_mv_wprint(scrn->bbar,
		    scrn->WIDTH - CH_INFO_OFFS, 0,
		    CH_INFO_FMT,
		    buf->pos + 1,
		    buf->size + 1,
		    buf->data[buf->pos]->pos + 1,
		    buf->data[buf->pos]->size + 1);
}

void run_current_cmd(struct Screen *scrn)
{
	/* lisp_run(scrn); */

	struct Line *cur_cmd = scrn->cmds->data[scrn->cmds->pos];

	if (wcscmp(cur_cmd->data, L"w") == 0) {

	}
}

void cmd_process_char(struct Screen *scrn, t_char ch)
{
	struct Line *cur_cmd = scrn->cmds->data[scrn->cmds->pos];

	switch (ch) {
		case TK_ENTER:
			run_current_cmd(scrn);
			buffer_new_line(scrn->cmds);
			cur_cmd = scrn->cmds->data[scrn->cmds->pos];
			t_wclear(scrn->cbar);
			break;
		case TK_BKSPC:
			line_backspace(cur_cmd);
			t_wclear(scrn->cbar);
			break;
		case TK_DELETE:
			if (line_move_forward(cur_cmd) == 0) {
				line_backspace(cur_cmd);
				t_wclear(scrn->cbar);
			}

			break;
		case TK_LEFT:
			line_move_backward(cur_cmd);
			break;
		case TK_RIGHT:
			line_move_forward(cur_cmd);
			break;
		case TK_UP:
			if (scrn->cmds->pos > 0) {
				cur_cmd = scrn->cmds->data[--scrn->cmds->pos];
				t_wclear(scrn->cbar);
			}

			break;
		case TK_DOWN:
			if (scrn->cmds->pos < scrn->cmds->size) {
				cur_cmd = scrn->cmds->data[++scrn->cmds->pos];
				t_wclear(scrn->cbar);
			}

			break;
		default:
			buffer_add(scrn->cmds, ch);
	}

	t_mv_wprint(scrn->cbar, 0, 0, L"%ls", cur_cmd->data);
	t_wmove(scrn->cbar, cur_cmd->pos, 0);
	t_wrefresh(scrn->cbar);
}

void buffer_process_char(struct Screen *scrn, struct Buffer *buf, t_char ch)
{
	switch (ch) {
	case TK_BKSPC:
		if (buffer_backspace(buf) == 0) {
			t_wmove(scrn->bwin, 0, buf->pos);
			t_wclrtoeol(scrn->bwin);
			t_mv_wprint(scrn->bwin, 0, buf->pos, L"%ls",
				    buf->data[buf->pos]->data);
		} else {
			int i;
			for (i = buf->pos; i <= buf->size; i++) {
				t_mv_wprint(scrn->bwin, 0, i, L"%ls",
					    buf->data[i]->data);
				t_wclrtoeol(scrn->bwin);
			}

			t_wmove(scrn->bwin, 0, buf->size + 1);
			t_wclrtoeol(scrn->bwin);
		}

		break;
	case TK_DELETE:
		if (buf->pos >= buf->size &&
		    buf->data[buf->pos]->pos >= buf->data[buf->pos]->size)
			break;

		buffer_move_forward(buf);
		if (buffer_backspace(buf) == 0) {
			t_wmove(scrn->bwin, 0, buf->pos);
			t_wclrtoeol(scrn->bwin);
			t_mv_wprint(scrn->bwin, 0, buf->pos, L"%ls",
				    buf->data[buf->pos]->data);
		} else {
			int i;
			for (i = buf->pos; i <= buf->size; i++) {
				t_mv_wprint(scrn->bwin, 0, i, L"%ls",
					    buf->data[i]->data);
				t_wclrtoeol(scrn->bwin);
			}

			t_wmove(scrn->bwin, 0, buf->size + 1);
			t_wclrtoeol(scrn->bwin);
		}

		break;
	case TK_ENTER:
		buffer_new_line(buf);
		int i = (buf->pos == 0) ? 0 : buf->pos - 1;
		for ( ; i <= buf->size; i++) {
			t_wmove(scrn->bwin, 0, i);
			t_wclrtoeol(scrn->bwin);
			t_mv_wprint(scrn->bwin, 0, i, L"%ls",
				    buf->data[i]->data);
		}

		t_wmove(scrn->bwin, 0, buf->size + 1);
		t_wclrtoeol(scrn->bwin);

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

	screen_print_ch_info(scrn, buf);
	t_wrefresh(scrn->bbar);

	t_wrefresh(scrn->cbar);

	t_wmove(scrn->bwin, buf->data[buf->pos]->pos, buf->pos);
	t_wrefresh(scrn->bwin);
}

/* Returns 1 if a save is requested or 0 if we just want to exit */
int screen_run(struct Screen *scrn, char *filepath)
{
	struct Buffer *buf = scrn->buf;
	int i;

	if (filepath == NULL) {
		screen_set_title(scrn, L"Untitled");
		screen_set_status(scrn, L"Created new buffer");
	} else {
		buffer_open(buf, filepath, 0, 0);

		int pathlen = strlen(filepath);
		t_char *widepath = malloc(sizeof(t_char) * pathlen);

		for (i = 0; i < pathlen; i++)
			widepath[i] = filepath[i];

		screen_set_title(scrn, widepath);

		free(widepath);

		for (i = 0; i < buf->size; i++)
			t_mv_wprint(scrn->bwin, 0, i, L"%ls",
				    buf->data[i]->data);
	}

	screen_print_ch_info(scrn, buf);

	t_wrefresh(scrn->tbar);
	t_wrefresh(scrn->bbar);
	t_wrefresh(scrn->cbar);

	t_wmove(scrn->bwin, buf->data[buf->pos]->pos, buf->pos);
	t_wrefresh(scrn->bwin);

	int CMD_LOOP_FLAG = 0;

	t_char ch;
	while (t_getch(&ch) != TUI_ERR && ch != BIND_EXIT &&
						ch != BIND_SAVE_EXIT) {
		if (ch == BIND_TOOGLE_CMD) {
			if(CMD_LOOP_FLAG ^= 1)
				t_wrefresh(scrn->cbar);
			else
				t_wrefresh(scrn->bwin);

			continue;
		}

		if (CMD_LOOP_FLAG)
			cmd_process_char(scrn, ch);
		else
			buffer_process_char(scrn, buf, ch);
	}

	if (ch == BIND_SAVE_EXIT)
		return 1;
	else
		return 0;
}

void screen_set_colors(struct Screen *scrn)
{
	t_wbkgd(scrn->tbar, CS_TITLE_BAR);
	t_wbkgd(scrn->bwin, CS_BUFFER);
	t_wbkgd(scrn->bbar, CS_BOT_BAR);
	t_wbkgd(scrn->cbar, CS_CMD_BAR);

	t_wrefresh(scrn->tbar);
	t_wrefresh(scrn->bwin);
	t_wrefresh(scrn->bbar);
	t_wrefresh(scrn->cbar);
}

struct Screen *screen_new()
{
	struct Screen *scrn = malloc(sizeof(struct Screen));

	t_getmaxxy(scrn->WIDTH, scrn->HEIGHT);

	scrn->tbar = bar_new(0);
	scrn->bwin = t_winit(0, 1, scrn->WIDTH, scrn->HEIGHT - 2);
	scrn->bbar = bar_new(scrn->HEIGHT - 2);
	scrn->cbar = bar_new(scrn->HEIGHT - 1);

	screen_set_colors(scrn);

	scrn->cmds = buffer_new();
	scrn->buf = buffer_new();

	return scrn;
}

void screen_free(struct Screen *scrn)
{
	t_wdestroy(scrn->tbar);
	t_wdestroy(scrn->bwin);
	t_wdestroy(scrn->bbar);
	t_wdestroy(scrn->cbar);
	buffer_free(scrn->cmds);
	buffer_free(scrn->buf);
	free(scrn);
}
