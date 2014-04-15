/*
 * window.c
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

#include "window.h"

#include <stdlib.h>

/*
void print_to_win_at(t_window *win, const char *msg, ...)
{
	wclear(win);

	va_list arg;
	va_start(arg, msg);
	v_print_to_win(win, msg, arg);
	va_end(arg);

	wrefresh(win);
}
*/

void print_to_win(t_window *win, t_char *msg, ...)
{
	wclear(win);

	va_list arg;
	va_start(arg, msg);
	t_vwprint(win, msg, arg);
	va_end(arg);

	wrefresh(win);
}
