/*
 * lisp.c
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

#include "lisp.h"

#include "binds.h"

#include <wctype.h>

t_char *lisp_eval(struct Line *ln)
{
	// Reset Line Pos
	ln->pos = 0;

	// Make sure first non-whitespace char is '('
	int i;
	for (i = ln->pos; i < ln->size; i++) {
		if (!iswspace(ln->data[i])) {
			if (ln->data[i] != L'(') {
				return L"Invalid char";
			} else {
				break;
			}
		}
	}

	// i is now the index of the first '('

	return L"Thingy";
}

int lisp_run(struct Screen *scrn)
{
	struct Line *ln = line_new();
	t_char ch;
	int y = 0;
	while (t_getch(&ch) != TUI_ERR && ch != BIND_EXIT) {
		/* Read */
		switch (ch) {
		case TK_ENTER:
			break;
		case TK_LEFT:
			line_move_backward(ln);
			t_wmove(scrn->bwin, ln->pos, y);
			t_wrefresh(scrn->bwin);
			continue;
		case TK_RIGHT:
			line_move_forward(ln);
			t_wmove(scrn->bwin, ln->pos, y);
			t_wrefresh(scrn->bwin);
			continue;
		case TK_BKSPC:
			line_backspace(ln);
			t_mv_wprint(scrn->bwin, 0, y, ln->data);
			t_wclrtoeol(scrn->bwin);
			t_wmove(scrn->bwin, ln->pos, y);
			t_wrefresh(scrn->bwin);
			continue;
		case TK_DELETE:
			if (ln->pos >= ln->size)
				continue;
			line_move_forward(ln);
			line_backspace(ln);
			t_mv_wprint(scrn->bwin, 0, y, ln->data);
			t_wclrtoeol(scrn->bwin);
			t_wmove(scrn->bwin, ln->pos, y);
			t_wrefresh(scrn->bwin);
			continue;
		default:
			line_add(ln, ch);
			t_mv_wprint(scrn->bwin, 0, y, ln->data);
			t_wmove(scrn->bwin, ln->pos, y);
			t_wrefresh(scrn->bwin);
			continue;
		};

		/* Eval */
		t_char *value = lisp_eval(ln);

		/* Print */
		t_mv_wprint(scrn->bwin, 0, ++y, value);

		/* Reset */
		ln = line_new();
		t_wmove(scrn->bwin, 0, ++y);
		t_wrefresh(scrn->bwin);
	}

	line_free(ln);

	return 0;
}

void lisp_init()
{

}

void lisp_destroy()
{

}
