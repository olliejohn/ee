/*
 * color.c
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

#include "color.h"

#include <stdlib.h>
#include <string.h>
#include <tui.h>

#define COLOR_DEFAULT -1

void color_init()
{
	char *termtype = getenv("TERM");

	if (strcasecmp(termtype, "xterm-256color") == 0) {
		init_color(COL_BLACK, 	0, 	0, 	0	);
		init_color(COL_RED, 	1000, 	0, 	0	);
		init_color(COL_GREEN, 	0, 	1000, 	0	);
		init_color(COL_BLUE, 	0, 	0, 	1000	);
		init_color(COL_YELLOW, 	1000, 	1000, 	0	);
		init_color(COL_MAGENTA,	1000, 	0, 	1000	);
		init_color(COL_CYAN, 	0, 	1000, 	1000	);
		init_color(COL_WHITE, 	1000, 	1000, 	1000	);
		init_color(COL_L_GRAY, 	830, 	830, 	830	);
		init_color(COL_D_GRAY, 	500, 	500, 	500	);

		/* TODO: Make the 3 tab pairs look nicer with 256 colors */
		init_pair(SCH_TAB_ACTIVE, 	COLOR_BLACK, 	COLOR_WHITE);
		init_pair(SCH_TAB_INACTIVE, 	COLOR_WHITE, 	COLOR_BLACK);
		init_pair(SCH_TAB_SPACE, 	COLOR_DEFAULT, 	COLOR_DEFAULT);
		init_pair(SCH_BUFFER, 		COL_WHITE, 	COL_BLACK);
		init_pair(SCH_BOTTOM_BAR, 	COL_BLACK, 	COL_D_GRAY);
		init_pair(SCH_BOTTOM_ERR, 	COL_RED, 	COL_D_GRAY);
		init_pair(SCH_CMD_BAR, 		COL_WHITE, 	COL_BLACK);
	} else {
		/* Presume we only have the default 8 term colors available */
		start_color();
		use_default_colors();

		init_pair(SCH_TAB_ACTIVE, 	COLOR_BLACK, 	COLOR_WHITE);
		init_pair(SCH_TAB_INACTIVE, 	COLOR_WHITE, 	COLOR_BLACK);
		init_pair(SCH_TAB_SPACE, 	COLOR_DEFAULT, 	COLOR_DEFAULT);
		init_pair(SCH_BUFFER, 		COLOR_DEFAULT, 	COLOR_DEFAULT);
		init_pair(SCH_BOTTOM_BAR, 	COLOR_BLACK, 	COLOR_WHITE);
		init_pair(SCH_BOTTOM_ERR, 	COLOR_RED, 	COLOR_WHITE);
		init_pair(SCH_CMD_BAR, 		COLOR_DEFAULT, 	COLOR_DEFAULT);
	}
}
