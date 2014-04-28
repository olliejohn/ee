/*
 * test.c
 * This file is not actually part of the TUI library but is used for testing
 * it's functionality
 *
 * Copyright (C) 2014 Ollie Etherington.
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

#include "tui.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	t_init(TUI_RAW | TUI_KEYPAD);

	int y = 0;

	t_char ch;
	while (t_getch(&ch) != TUI_ERR && ch != 'D') {
		t_mv_print(0, y++, L"%d\n", ch);
		if (y > 6)
			y = 0;
	}

	t_destroy();

	return 0;
}
