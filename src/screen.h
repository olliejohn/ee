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

#include "buffer.h"
#include "bufwin.h"
#include "tui/tui.h"
#include "vte/vte_driver.h"

/* Flags are stored in an unsigned int allowing up to 32 */
enum Screen_Flag {
	SF_CLEAR_FLGS,	/* Clear all flags */
	SF_EXIT,	/* Exit the program */
	SF_SAVE_EXIT,	/* Save and then exit the program */
	SF_BUF,		/* Move focus to the current active buffer */
	SF_CLI,		/* Move focus to the command line */
	SF_TERM,	/* Move focus to the terminal */
	SF_NO_CONFIG,	/* Couldn't load the config on program start */
};

struct Screen {
	t_window *tbar;	/* Tab bar */
	t_window *bbar;	/* Bottom bar */
	t_window *cbar;	/* Command bar */
	struct BufWin *bw;
	struct VTE *vte;
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
unsigned int screen_get_flag(struct Screen *scrn, enum Screen_Flag flag);
void screen_clear_flags(struct Screen *scrn);
void screen_change_to_buffer(struct Screen *scrn, unsigned int newtab);
void screen_set_status(struct Screen *scrn, t_char *status, ...);
void screen_vset_status(struct Screen *scrn, t_char *status, va_list args);
void screen_print_ch_info(struct Screen *scrn);
void screen_do_save_prompt(struct Screen *scrn);

#endif
