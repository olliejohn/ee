#ifndef COLOR_H
#define COLOR_H

/* Must not be 0 indexed */
typedef enum {
	SCH_RED_BLACK = 1,
	SCH_TITLE_BAR,
	SCH_BUFFER,
	SCH_CMD_BAR,
} ColScheme;

void color_init();

#endif
