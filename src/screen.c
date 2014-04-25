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

void screen_set_flag(struct Screen *scrn, enum Screen_Flag flag)
{
	scrn->FLAGS |= 1 << flag;
}

void screen_unset_flag(struct Screen *scrn, enum Screen_Flag flag)
{
	scrn->FLAGS &= ~(1 << flag);
}

void screen_toggle_flag(struct Screen *scrn, enum Screen_Flag flag)
{
	scrn->FLAGS ^= 1 << flag;
}

int screen_get_flag(struct Screen *scrn, enum Screen_Flag flag)
{
	return scrn->FLAGS & (1 << flag);
}

void screen_clear_flags(struct Screen *scrn)
{
	scrn->FLAGS ^= scrn->FLAGS;
}

/* TODO: readjusting line numbers when the number of digits in the number changes */

/* Draw a tab on the tab bar. Returns the xoffs of the end of the tab */
int screen_draw_tab_at(struct Screen *scrn, int num, char *filename,
			int xoffs, int active)
{
	if (active)
		t_wattron(scrn->tbar, CS_TAB_ACTIVE);
	else
		t_wattron(scrn->tbar, CS_TAB_INACTIVE);

	t_mv_wprint(scrn->tbar, xoffs, 0, L" %d - %s ", num, filename);

	if (active)
		t_wattroff(scrn->tbar, CS_TAB_ACTIVE);
	else
		t_wattroff(scrn->tbar, CS_TAB_INACTIVE);

	t_wattron(scrn->tbar, CS_TAB_SPACE);
	t_wprint(scrn->tbar, L" ");
	t_wattroff(scrn->tbar, CS_TAB_SPACE);

	return t_wgetcurx(scrn->tbar);
}

void screen_draw_tabs(struct Screen *scrn)
{
	t_wattrend(scrn->tbar);

	int i, nextx, active;
	for (i = nextx = 0; i < scrn->bw->num_bufs; i++) {
		active = (scrn->bw->buffers[i] == scrn->bw->curbuf) ? 1 : 0;
		nextx = screen_draw_tab_at(scrn, i,
					   scrn->bw->buffers[i]->filename,
					   nextx, active);
	}

	t_wclrtoeol(scrn->tbar);
	t_wrefresh(scrn->tbar);
}

void screen_change_to_buffer(struct Screen *scrn, int new)
{
	if (new >= scrn->bw->num_bufs)
		return;

	scrn->bw->curbuf = scrn->bw->buffers[new];
	screen_draw_tabs(scrn);

	bufwin_redraw(scrn->bw);
	screen_print_ch_info(scrn);
	screen_set_status(scrn, L"Changed to buffer %d", new);
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
#define buf scrn->bw->curbuf
void screen_print_ch_info(struct Screen *scrn)
{
	t_mv_wprint(scrn->bbar,
		    scrn->WIDTH - CH_INFO_OFFS, 0,
		    CH_INFO_FMT,
		    buf->pos + 1,
		    buf->size + 1,
		    buf->data[buf->pos]->pos + 1,
		    buf->data[buf->pos]->size + 1);
}
#undef buf

void run_current_cmd(struct Screen *scrn)
{
	/* lisp_run(scrn); */

	struct Line *curln = scrn->cmds->data[scrn->cmds->pos];

	t_char *curcmd = malloc(sizeof(t_char) * (curln->size + 1));
	wcscpy(curcmd, curln->data);

	if (wcscmp(curcmd, L"save") == 0 || wcscmp(curcmd, L"s") == 0) {

	} else if (wcscmp(curcmd, L"new") == 0 || wcscmp(curcmd, L"n") == 0) {
		bufwin_add_buffer(scrn->bw);
		screen_change_to_buffer(scrn, scrn->bw->num_bufs - 1);
	} else if (iswdigit(curcmd[0])) {
		t_char *pEnd = NULL;
		screen_change_to_buffer(scrn, wcstol(curcmd, &pEnd, 10));
	}

	free(curcmd);
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

#define buf scrn->bw->curbuf
void buffer_process_char(struct Screen *scrn, t_char ch)
{
	bufwin_process_char(scrn->bw, ch);

	screen_print_ch_info(scrn);
	t_wrefresh(scrn->bbar);

	t_wmove(scrn->bw->win,
		buf->data[buf->pos]->pos + scrn->bw->linumoffs,
		buf->pos - scrn->bw->ywinoffs);

	t_wrefresh(scrn->bw->win);
}
#undef buf

enum Focus {
	FOCUS_BUF,
	FOCUS_CLI,
	FOCUS_TERM,
};

/* Returns 1 if a save is requested or 0 if we just want to exit */
#define buf scrn->bw->curbuf
#define win scrn->bw->win
int screen_run(struct Screen *scrn, char *filepath)
{
	if (filepath == NULL) {
		screen_set_status(scrn, L"Created new buffer");
	} else {
		buffer_open(buf, filepath, 0, 0);
		buffer_set_filename(buf, filepath);
		screen_set_status(scrn, L"Loaded buffer from %s", filepath);
	}

	screen_draw_tabs(scrn);
	bufwin_redraw(scrn->bw);
	screen_print_ch_info(scrn);

	t_wrefresh(scrn->tbar);
	t_wrefresh(scrn->bbar);
	t_wrefresh(scrn->cbar);

	t_wmove(win, buf->data[buf->pos]->pos + scrn->bw->linumoffs, buf->pos);
	t_wrefresh(win);

	int FOCUS = FOCUS_BUF;

	t_char ch;
	while (t_getch(&ch) != TUI_ERR) {
		cb_ptr callback;
		if ((callback = binds_get_callback_for(ch)) == NULL) {
			if (FOCUS == FOCUS_BUF) {
				buffer_process_char(scrn, ch);
			} else if (FOCUS == FOCUS_CLI) {
				cmd_process_char(scrn, ch);
			} else if (FOCUS == FOCUS_TERM) {
				vte_process_char(scrn->bw->vte, ch);
				FOCUS = FOCUS_BUF;
			} else {
				FOCUS = FOCUS_BUF;
				buffer_process_char(scrn, ch);
			}
		} else {
			callback(scrn);
		}

		if (screen_get_flag(scrn, SF_CLEAR_FLGS))
			screen_clear_flags(scrn);

		if (screen_get_flag(scrn, SF_EXIT))
			return 0;

		if (screen_get_flag(scrn, SF_SAVE_EXIT))
			return 1;

		if (screen_get_flag(scrn, SF_BUF)) {
			FOCUS = FOCUS_BUF;
			t_wrefresh(win);
			screen_unset_flag(scrn, SF_BUF);
		}

		if (screen_get_flag(scrn, SF_CLI)) {
			FOCUS = FOCUS_CLI;
			t_wrefresh(scrn->cbar);
			screen_unset_flag(scrn, SF_CLI);
		}

		if (screen_get_flag(scrn, SF_TERM)) {
			FOCUS = FOCUS_TERM;
			vte_refresh(scrn->bw->vte);
			screen_unset_flag(scrn, SF_TERM);
		}
	}

	/* This should never be reached */
	return 0;
}
#undef buf
#undef win

void screen_set_colors(struct Screen *scrn)
{
	t_wbkgd(scrn->tbar, CS_TAB_SPACE);
	t_wbkgd(scrn->bbar, CS_BOT_BAR);
	t_wbkgd(scrn->cbar, CS_CMD_BAR);
	bufwin_set_color_scheme(scrn->bw, CS_BUFFER);

	t_wrefresh(scrn->tbar);
	t_wrefresh(scrn->bbar);
	t_wrefresh(scrn->cbar);
	bufwin_refresh(scrn->bw);
}

struct Screen *screen_new()
{
	struct Screen *scrn = malloc(sizeof(struct Screen));

	t_getmaxxy(scrn->WIDTH, scrn->HEIGHT);

	scrn->tbar = bar_new(0);
	scrn->bbar = bar_new(scrn->HEIGHT - 2);
	scrn->cbar = bar_new(scrn->HEIGHT - 1);

	scrn->bw = bufwin_new(0, 1, scrn->WIDTH, scrn->HEIGHT - 3);
	bufwin_add_buffer(scrn->bw);
	bufwin_set_active_buffer(scrn->bw, 0);

	screen_set_colors(scrn);

	scrn->cmds = buffer_new();

	scrn->FLAGS = 0;

	return scrn;
}

void screen_free(struct Screen *scrn)
{
	t_wdestroy(scrn->tbar);
	t_wdestroy(scrn->bbar);
	t_wdestroy(scrn->cbar);
	bufwin_free(scrn->bw);

	buffer_free(scrn->cmds);
	free(scrn);
}
