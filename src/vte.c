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
#include <string.h>
#include <tui.h>

struct VTE {
	t_window *divider;
	t_window *win;
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

void vte_put_prompt(struct VTE *vte)
{
	char *pwd = getenv("PWD");
	char *home = getenv("HOME");
	int l = home ? strlen(home) : 0;

	if (l > 1 &&
	    strncmp(home, pwd, l) == 0 &&
	    (!pwd[l] || pwd[l] == '/')) {
		pwd += l - 1;
		pwd[0] = '~';
	}

	t_mv_wprint(vte->win, 0, 0, L"%s> ", pwd);
	vte_refresh(vte);
}

void vte_process_char(struct VTE *vte, t_char ch)
{

}

struct VTE *vte_new(int x, int y, int w, int h)
{
	struct VTE *vte = malloc(sizeof(struct VTE));
	vte->divider = t_winit(x, y, 1, h);
	vte->win = t_winit(x + 1, y, w - 1, h);

	vte_draw_divider(vte);
	vte_put_prompt(vte);

	return vte;
}

void vte_free(struct VTE *vte)
{
	t_wdestroy(vte->divider);
	t_wdestroy(vte->win);
	free(vte);
}
