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
#include "bufwin.h"

/* Flags are stored in an unsigned int allowing up to 32 */
enum Screen_Flag {
	SF_CLEAR_FLGS,
	SF_EXIT,
	SF_SAVE_EXIT,
	SF_CLI,
	SF_TERM,
};

struct Screen {
	t_window *tbar;	/* Tab bar */
	t_window *bbar;	/* Bottom bar */
	t_window *cbar;	/* Command bar */
	struct BufWin *bw;
	struct Buffer *cmds;
	unsigned int WIDTH;
	unsigned int HEIGHT;
	unsigned int FLAGS;
};

struct Screen *screen_new();
void screen_free(struct Screen *scrn);
int screen_run(struct Screen *scrn, char *filepath);
void screen_set_flag(struct Screen *scrn, enum Screen_Flag flag);
void screen_unset_flag(struct Screen *scrn, enum Screen_Flag flag);
void screen_toggle_flag(struct Screen *scrn, enum Screen_Flag flag);
int screen_get_flag(struct Screen *scrn, enum Screen_Flag flag);
void screen_change_to_buffer(struct Screen *scrn, int newtab);
void screen_set_status(struct Screen *scrn, t_char *status, ...);
void screen_vset_status(struct Screen *scrn, t_char *status, va_list args);

#endif
