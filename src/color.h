/*
 * color.h
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

#ifndef COLOR_H
#define COLOR_H

#define COLOR_DEFAULT -1
#define TERM_FG COLOR_DEFAULT
#define TERM_BG COLOR_DEFAULT

typedef enum {
	COL_BLACK = 1,
	COL_RED,
	COL_GREEN,
	COL_BLUE,
	COL_YELLOW,
	COL_MAGENTA,
	COL_CYAN,
	COL_WHITE,
	COL_L_GRAY,
	COL_D_GRAY,
} Col;

/* Must not be 0 indexed */
typedef enum {
	SCH_TAB_ACTIVE = 1,
	SCH_TAB_INACTIVE,
	SCH_TAB_SPACE,
	SCH_BUFFER,
	SCH_LINE_NUM,
	SCH_TERM,
	SCH_TERM_DIV,
	SCH_BOTTOM_BAR,
	SCH_BOTTOM_ERR,
	SCH_CMD_BAR,
} ColScheme;

/* Color schemes */
#define CS_TAB_ACTIVE	COLOR_PAIR(SCH_TAB_ACTIVE)
#define CS_TAB_INACTIVE	COLOR_PAIR(SCH_TAB_INACTIVE)
#define CS_TAB_SPACE	COLOR_PAIR(SCH_TAB_SPACE)
#define CS_BUFFER	COLOR_PAIR(SCH_BUFFER)
#define CS_LINE_NUM	COLOR_PAIR(SCH_LINE_NUM)
#define CS_TERM		COLOR_PAIR(SCH_TERM)
#define CS_TERM_DIV	COLOR_PAIR(SCH_TERM_DIV)
#define CS_BOT_BAR	COLOR_PAIR(SCH_BOTTOM_BAR)
#define CS_BOT_ERR	COLOR_PAIR(SCH_BOTTOM_ERR)
#define CS_CMD_BAR	COLOR_PAIR(SCH_CMD_BAR)

void color_init();

#endif
