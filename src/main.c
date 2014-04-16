/*
 * main.c
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

#include <string.h>
#include <tui.h>

#include "color.h"
#include "config.h"
#include "screen.h"

#define INFO_NAME 	"Yaw"
#define INFO_VERSION 	"0.01"
#define INFO_AUTHOR 	"Ollie Etherington"
#define INFO_LICENSE	"GNU GPLv2"
#define INFO_COPYRIGHT	"Copyright 2014 Ollie Etherignton"
#define INFO_DESC	"A simple command-line text editor"
#define INFO_WEBSITE	"http://github.com/olliejohn/yaw"

int version()
{
	printf("%s - Version %s\n", INFO_NAME, INFO_VERSION);
	return 0;
}

int about()
{
	printf( "\n%s - %s by %s\nVersion %s\n"
		"Available under the %s\n%s\n%s\n"
		"Use the --help flag for usage information\n\n",
		INFO_NAME,
		INFO_DESC,
		INFO_AUTHOR,
		INFO_VERSION,
		INFO_LICENSE,
		INFO_COPYRIGHT,
		INFO_WEBSITE);
	return 0;
}

int usage()
{
	printf("Usage:\n"
		"ee [filename | flag]\n\n"
		"Flags:\n"
		"--about     Display the about dialog\n"
		"--version   Display version information\n"
		"--help      Display this help dialog\n");
	return 0;
}

int main(int argc, char **argv)
{
	/* Enable unicode */
	void t_init_unicode();

	/* Parse args */
	int FLAG_OPEN = 0;

	if (argc == 2) {
		if (strcmp(argv[1], "--version") == 0)
			return version();
		else if (strcmp(argv[1], "--about") == 0)
			return about();
		else if (strcmp(argv[1], "--help") == 0)
			return usage();
		else
			FLAG_OPEN++;
	}

	if (argc > 2) {
		printf("Invalid arguments");
		return 0;
	}

	/* tui init */
	t_init(TUI_RAW | TUI_COLOR | TUI_KEYPAD);

	/* Run program */
	color_init();

	t_refresh();

	if (config_init() == -1)
		;// ERR: "Couldn't load or create config - using default"

	char *filepath = NULL;
	if (FLAG_OPEN)
		filepath = argv[1];

	struct Screen *scrn = screen_new();
	int status = screen_run(scrn, filepath);
	screen_free(scrn);

	config_destroy();

	/* tui destroy */
	t_destroy();

	return status;
}
