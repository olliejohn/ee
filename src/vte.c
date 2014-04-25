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
	char *ps1;
};

void vte_draw_divider(struct VTE *vte)
{
	t_wbkgd(vte->divider, CS_TERM_DIV);

	int i;
	for (i = 0; i < t_wgetmaxy(vte->divider); i++)
		t_mv_wprint(vte->divider, 0, i, L"|");

	t_wrefresh(vte->divider);
}

int check_substring_at_start(char *substring, char *tocheck)
{
	int substringlen = strlen(substring);

	if (strlen(tocheck) < substringlen)
		return -1;

	int i;
	for (i = 0; i < substringlen; i++)
		if (substring[i] != tocheck[i])
			return -1;

	return 0;
}

void vte_get_ps1(struct VTE *vte)
{
	char *pwd = getenv("PS1");
	char *home = getenv("HOME");

	if (check_substring_at_start(home, pwd) == 0) {
		int pwdlen = strlen(pwd);
		int homlen = strlen(home);
		int size = sizeof(char) * (pwdlen - homlen + 3);
		vte->ps1 = realloc(vte->ps1, size);
		strcat(vte->ps1, "~");
		strcat(vte->ps1, pwd + homlen);
	} else {
		vte->ps1 = realloc(vte->ps1, sizeof(char) * (strlen(pwd) + 3));
		strcpy(vte->ps1, pwd);
	}

	strcat(vte->ps1, "> ");
}

void vte_refresh(struct VTE *vte)
{
	t_wrefresh(vte->win);
}

void vte_put_prompt(struct VTE *vte)
{
	t_mv_wprint(vte->win, 0, 0, L"%ls", vte->ps1);
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
	vte->ps1 = NULL;

	vte_draw_divider(vte);
	//vte_get_ps1(vte);

	//vte_put_prompt(vte);

	return vte;
}

void vte_free(struct VTE *vte)
{
	t_wdestroy(vte->divider);
	t_wdestroy(vte->win);
	free(vte->ps1);
	free(vte);
}
