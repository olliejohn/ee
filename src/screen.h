/*
 * screen.h
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

#ifndef SCREEN_H
#define SCREEN_H

#include <tui.h>

#include "buffer.h"

struct Screen {
	t_window *tbar;	/* Top bar */
	t_window *bwin;	/* Buffer window */
	t_window *bbar;	/* Bottom bar */
	t_window *cbar;	/* Command bar */
	struct Buffer *buf;
	int WIDTH;
	int HEIGHT;
};

struct Screen *screen_new();
void screen_free(struct Screen *scrn);
void screen_set_colors(struct Screen *scrn);
int screen_run(struct Screen *scrn, char *filepath);

#endif
