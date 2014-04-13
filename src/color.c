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
	init_pair(SCH_RED_BLACK, 	COLOR_RED, 	COLOR_BLACK);
	init_pair(SCH_TITLE_BAR, 	COLOR_BLACK, 	COLOR_WHITE);
	init_pair(SCH_BUFFER, 		COLOR_WHITE, 	COLOR_BLACK);
	init_pair(SCH_CMD_BAR, 		COLOR_BLACK, 	COLOR_WHITE);
}
