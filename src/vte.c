/*
 * vte.c
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

#include "vte.h"

#include "color.h"

#include <stdlib.h>
#include <tui.h>
#include <vterm.h>

struct VTE {
	t_window *divider;
	t_window *win;
	vterm_t *vterm;
};

void vte_draw_divider(struct VTE *vte)
{
	t_wbkgd(vte->divider, CS_TERM_DIV);

	int i;
	for (i = 0; i < t_wgetmaxy(vte->divider); i++)
		t_mv_wprint(vte->divider, 0, i, L"|");

	t_wrefresh(vte->divider);
}

void vte_refresh(struct VTE *vte)
{
	t_wrefresh(vte->win);
}
/*
void vte_refresh(struct VTE *vte)
{
	if (vterm_read_pipe(vte->vterm) > 0) {
		vterm_wnd_update(vte->vterm);
		wrefresh(vte->win);
	}

	t_wrefresh(vte->win);
}
*/
struct VTE *vte_new(int x, int y, int w, int h)
{
	struct VTE *vte = malloc(sizeof(struct VTE));

	vte->divider = t_winit(x, y, 1, h);
	vte_draw_divider(vte);
	vte->win = t_winit(x + 1, y, w - 1, h);
	t_wrefresh(vte->win);
	vte->vterm = vterm_create(w - 1, h, VTERM_FLAG_VT100);

	vterm_set_colors(vte->vterm, TERM_FG, TERM_BG);
	vterm_wnd_set(vte->vterm, vte->win);
	vte_refresh(vte);

	return vte;
}

void vte_free(struct VTE *vte)
{
	t_wdestroy(vte->divider);
	t_wdestroy(vte->win);
	vterm_destroy(vte->vterm);
	free(vte);
}

void vte_process_char(struct VTE *vte, t_char ch)
{
/*	ssize_t bytes;

	while (ch != '3') {
		bytes = vterm_read_pipe(vte->vterm);

		if (bytes > 0) {
			vterm_wnd_update(vte->vterm);
			touchwin(vte->win);
			wrefresh(vte->win);
			refresh();
		}

		if (bytes == -1)
			break;

		t_getch(&ch);

		if (ch != ERR)
			vterm_write_pipe(vte->vterm, ch);
	}
	*/

	vterm_write_pipe(vte->vterm, ch);
	vterm_read_pipe(vte->vterm);
	vterm_wnd_update(vte->vterm);
	vte_refresh(vte);

}
