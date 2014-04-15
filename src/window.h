/*
 * window.h
 * Part of the ee text editor
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

#ifndef WINDOW_H
#define WINDOW_H

#include <tui.h>

#define v_print_to_win(win, msg, ...) t_vwprint(win, msg, __VA_ARGS__)
/*
enum Justification {
	J_LEFT,
	J_CENTER,
	J_RIGHT,
};

struct Window {
	t_window *twin;	// Title window
	t_window *bwin;	// Buffer window
	t_window *cwin;	// Command window
};
*/
void print_to_win(t_window *win, t_char *msg, ...);
/*
struct Window *window_new();
void window_free(struct Window *win);
*/
#endif
