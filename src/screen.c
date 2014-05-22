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
#include "lisp/lisp.h"

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#define bar_new(y) t_winit(0, y, t_getmaxx(), 1)

/* This will eventually be replaced with an option in the Settings struct */
int SHOW_VTE = 1;

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

unsigned int screen_get_flag(struct Screen *scrn, enum Screen_Flag flag)
{
	return scrn->FLAGS & (1 << flag);
}

void screen_clear_flags(struct Screen *scrn)
{
	scrn->FLAGS ^= scrn->FLAGS;
}

/* Draw a tab on the tab bar. Returns the xoffs of the end of the tab */
static int screen_draw_tab_at(struct Screen *scrn, int num, char *filename,
			      int xoffs, int active, dirty_t dirty)
{
	int ATTR;

	if (active && dirty == BUF_DIRTY)	/* Active and dirty */
		ATTR = CS_TAB_ACTIVE_DIRTY;
	else if (active) 			/* Active and clean */
		ATTR = CS_TAB_ACTIVE;
	else if (dirty == BUF_DIRTY)		/* Inactive and dirty */
		ATTR = CS_TAB_INACTIVE_DIRTY;
	else 					/* Inactive and clean */
		ATTR = CS_TAB_INACTIVE;

	t_wattron(scrn->tbar, ATTR);
	t_mv_wprint(scrn->tbar, xoffs, 0, L" %d - %s ", num, filename);
	t_wattroff(scrn->tbar, ATTR);

	t_wattron(scrn->tbar, CS_TAB_SPACE);
	t_wprint(scrn->tbar, L" ");
	t_wattroff(scrn->tbar, CS_TAB_SPACE);

	return t_wgetcurx(scrn->tbar);
}

static void screen_draw_tabs(struct Screen *scrn)
{
	t_wattrend(scrn->tbar);

	int i, nextx, active;
	for (i = nextx = 0; i < scrn->bw->num_bufs; i++) {
		active = (scrn->bw->buffers[i] == scrn->bw->curbuf) ? 1 : 0;
		nextx = screen_draw_tab_at(scrn, i,
					   scrn->bw->buffers[i]->filename,
					   nextx, active,
					   scrn->bw->buffers[i]->dirty);
	}

	t_wclrtoeol(scrn->tbar);
	t_wnoutrefresh(scrn->tbar);
}

void screen_change_to_buffer(struct Screen *scrn, unsigned int new)
{
	if (new >= scrn->bw->num_bufs)
		return;

	bufwin_set_active_buffer(scrn->bw, new);
	screen_draw_tabs(scrn);

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
	t_wnoutrefresh(scrn->bbar);
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
void screen_print_ch_info(struct Screen *scrn)
{
	t_mv_wprint(scrn->bbar,
		    scrn->WIDTH - CH_INFO_OFFS, 0,
		    CH_INFO_FMT,
		    scrn->bw->curbuf->pos + 1,
		    scrn->bw->curbuf->size + 1,
		    scrn->bw->curbuf->data[scrn->bw->curbuf->pos]->pos + 1,
		    scrn->bw->curbuf->data[scrn->bw->curbuf->pos]->size + 1);
}

void screen_do_save_prompt(struct Screen *scrn)
{
	screen_set_status(scrn, L"Saving: Enter filename...");
	t_wclear(scrn->cbar);
	t_wrefresh(scrn->cbar);

	struct Line *filename = line_new();
	char *shortfn;
	t_char ch = 0;

	for (;;) {
		t_getch(&ch);

		switch (ch) {
		case TK_BKSPC:
			line_backspace(filename);
			t_wclear(scrn->cbar);
			break;
		case TK_DELETE:
			if (line_move_forward(filename) == 0) {
				line_backspace(filename);
				t_wclear(scrn->cbar);
			}
			break;
		case TK_LEFT:
			line_move_backward(filename);
			break;
		case TK_RIGHT:
			line_move_forward(filename);
			break;
		case TK_ENTER:
			goto do_save;
		default:
			if (iswprint(ch))
				line_add(filename, ch);
			else
				continue;
		}

		t_mv_wprint(scrn->cbar, 0, 0, filename->data);
		t_wrefresh(scrn->cbar);
		t_wmove(scrn->cbar, filename->pos, 0);
	}

do_save:
	shortfn = calloc(filename->size + 1, sizeof(char));
	sprintf(shortfn, "%ls", filename->data);


	if (buffer_save_as(scrn->bw->curbuf, shortfn) == 0)
		screen_set_status(scrn, L"Wrote buffer to '%s'", shortfn);
	else
		screen_set_status(scrn, L"Couldn't write to '%s'", shortfn);

	free(shortfn);
	line_free(filename);

	t_wclear(scrn->cbar);
	t_wrefresh(scrn->cbar);
}

#define curcmd scrn->cmds->data[scrn->cmds->pos]->data
static void run_current_cmd(struct Screen *scrn)
{
	/* lisp_run(scrn); */

	if (wcscmp(curcmd, L"save") == 0 || wcscmp(curcmd, L"s") == 0) {

	} else if (wcscmp(curcmd, L"new") == 0 || wcscmp(curcmd, L"n") == 0) {
		bufwin_add_buffer(scrn->bw);
		screen_change_to_buffer(scrn, scrn->bw->num_bufs - 1);
	} else if (iswdigit(curcmd[0])) {
		t_char *pEnd = NULL;
		screen_change_to_buffer(scrn, wcstol(curcmd, &pEnd, 10));
	}
}
#undef curcmd

static void cmd_process_char(struct Screen *scrn, t_char ch)
{
	if (ch == TUI_ERR)
		return;

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
	t_wnoutrefresh(scrn->cbar);
}

static void buffer_process_char(struct Screen *scrn, t_char ch)
{
	if (ch == TUI_ERR)
		return;

	bufwin_process_char(scrn->bw, ch);

	screen_print_ch_info(scrn);
	t_wnoutrefresh(scrn->bbar);

	bufwin_place_cursor(scrn->bw);
}

/* Returns 1 if a save is requested or 0 if we just want to exit */
#define buf scrn->bw->curbuf
#define win scrn->bw->win
int screen_run(struct Screen *scrn, char *filepath)
{
	enum Focus {
		FOCUS_BUF,
		FOCUS_CLI,
		FOCUS_TERM,
	};

	if (filepath == NULL) {
		screen_set_status(scrn, L"Created new buffer");
		bufwin_add_buffer(scrn->bw);
	} else {
		bufwin_add_buffer_from_file(scrn->bw, filepath);
		screen_set_status(scrn, L"Loaded buffer from %s", filepath);
	}

	if (screen_get_flag(scrn, SF_NO_CONFIG)) {
		screen_set_status(scrn,
			L"Error: Couldn't load config - using default");
		screen_unset_flag(scrn, SF_NO_CONFIG);
	}

	bufwin_set_active_buffer(scrn->bw, 0);

	screen_draw_tabs(scrn);
	bufwin_redraw(scrn->bw);
	screen_print_ch_info(scrn);

	t_wnoutrefresh(scrn->tbar);
	t_wnoutrefresh(scrn->bbar);
	t_wnoutrefresh(scrn->cbar);

	t_nodelay(TRUE);
	vte_refresh(scrn->vte);
	t_nodelay(FALSE);

	bufwin_place_cursor(scrn->bw);
	t_wnoutrefresh(win);

	t_doupdate();

	enum Focus FOCUS = FOCUS_BUF;

	t_char ch;
	cb_ptr callback;
	dirty_t persistent_dirty = BUF_CLEAN;
run_loop:
	for (;;) {
		t_getch(&ch);

		if ((callback = binds_get_callback_for(ch)) == NULL) {
			if (FOCUS == FOCUS_BUF) {
				buffer_process_char(scrn, ch);
			} else if (FOCUS == FOCUS_CLI) {
				cmd_process_char(scrn, ch);
			} else if (FOCUS == FOCUS_TERM) {
				vte_process_char(scrn->vte, ch);
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
			t_nodelay(FALSE);
			bufwin_place_cursor(scrn->bw);
			t_wnoutrefresh(win);
			screen_unset_flag(scrn, SF_BUF);
		} else if (screen_get_flag(scrn, SF_CLI)) {
			FOCUS = FOCUS_CLI;
			t_nodelay(FALSE);
			t_wnoutrefresh(scrn->cbar);
			screen_unset_flag(scrn, SF_CLI);
		} else if (screen_get_flag(scrn, SF_TERM)) {
			FOCUS = FOCUS_TERM;
			t_nodelay(TRUE);
			vte_refresh(scrn->vte);
			screen_unset_flag(scrn, SF_TERM);
		}

		if (buf->dirty != persistent_dirty) {
			persistent_dirty = buf->dirty;
			screen_draw_tabs(scrn);

			if (FOCUS == FOCUS_BUF)
				bufwin_place_cursor(scrn->bw);
			else if (FOCUS == FOCUS_CLI)
				t_wnoutrefresh(scrn->cbar);
			else /* FOCUS == FOCUS_TERM */
				vte_refresh(scrn->vte);
		}

		t_doupdate();
	}

	/* This should never be reached */
	screen_set_status(scrn, L"ERROR: Invalid input");
	goto run_loop;
}
#undef buf
#undef win

void screen_set_colors(struct Screen *scrn)
{
	t_wbkgd(scrn->tbar, CS_TAB_SPACE);
	t_wbkgd(scrn->bbar, CS_BOT_BAR);
	t_wbkgd(scrn->cbar, CS_CMD_BAR);
	bufwin_set_color_scheme(scrn->bw, CS_BUFFER);
	bufwin_set_linum_color_scheme(scrn->bw, CS_LINE_NUM);

	t_wnoutrefresh(scrn->tbar);
	t_wnoutrefresh(scrn->bbar);
	t_wnoutrefresh(scrn->cbar);
	bufwin_refresh(scrn->bw);
	t_doupdate();
}

struct Screen *screen_new()
{
	struct Screen *scrn = malloc(sizeof(struct Screen));

	t_getmaxxy(scrn->WIDTH, scrn->HEIGHT);

	scrn->tbar = bar_new(0);
	scrn->bbar = bar_new(scrn->HEIGHT - 2);
	scrn->cbar = bar_new(scrn->HEIGHT - 1);

	int termx = scrn->WIDTH / 2;
	scrn->bw = bufwin_new(0, 1, termx - 1, scrn->HEIGHT - 3);
	scrn->vte = vte_new(termx, 1, scrn->WIDTH - termx, scrn->HEIGHT - 3);

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
	vte_free(scrn->vte);
	buffer_free(scrn->cmds);
	free(scrn);
}
