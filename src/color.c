/*
 * color.c
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

#include "color.h"

#include <ncurses.h>
#include <stdlib.h>

/*
 * Available colors:
 * Black
 * Red
 * Green
 * Blue
 * Magenta
 * Cyan
 * White
 */

void color_init()
{
	init_pair(SCH_TITLE_BAR, 	COLOR_BLACK, 	COLOR_WHITE);
	init_pair(SCH_TITLE_ERR, 	COLOR_RED, 	COLOR_WHITE);
	init_pair(SCH_BUFFER, 		COLOR_WHITE, 	COLOR_BLACK);
	init_pair(SCH_CMD_BAR, 		COLOR_BLACK, 	COLOR_WHITE);
	init_pair(SCH_CMD_ERR, 		COLOR_RED, 	COLOR_WHITE);
}
