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

#include <stdlib.h>

struct BufWin *bufwin_new(int x, int y, int w, int h)
{
	struct BufWin *bw = malloc(sizeof(struct BufWin));
	bw->win = t_winit(x, y, w, h);
	bw->buffers = malloc(sizeof(struct Buffer *) * MAX_BUFS);

	int i;
	for (i = 0; i < MAX_BUFS; i++)
		bw->buffers[i] = NULL;

	bw->active_bufs = 0;
	bw->curbuf = NULL;
	return bw;
}

void bufwin_free(struct BufWin *bufwin)
{
	int i;
	for (i = 0; i < bufwin->active_bufs; i++)
		buffer_free(bufwin->buffers[i]);

	t_wdestroy(bufwin->win);
	bufwin->curbuf = NULL;
	free(bufwin->buffers);
	free(bufwin);
}

void bufwin_refresh(struct BufWin *bufwin)
{
	t_wrefresh(bufwin->win);
}

void bufwin_set_bkgrd(struct BufWin *bufwin, int colpair)
{
	t_wbkgd(bufwin->win, colpair);
}

int bufwin_add_buffer(struct BufWin *bufwin)
{
	if (bufwin->active_bufs >= MAX_BUFS)
		return -1;

	bufwin->buffers[bufwin->active_bufs++] = buffer_new();
	return 0;
}

int bufwin_add_buffer_from_file(struct BufWin *bufwin, char *file)
{
	return 0;
}

void bufwin_set_active_buffer(struct BufWin *bufwin, int index)
{
	bufwin->curbuf = bufwin->buffers[index];
}
