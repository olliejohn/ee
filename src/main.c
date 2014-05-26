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

#include <malloc.h>
#include <string.h>

#include "binds.h"
#include "callback.h"
#include "color.h"
#include "config.h"
#include "screen.h"
#include "lisp/lisp.h"
#include "tui/tui.h"

#define INFO_NAME 	"Yaw"
#define INFO_VERSION 	"0.0.1"
#define INFO_AUTHOR 	"Ollie Etherington"
#define INFO_LICENSE	"GNU GPLv2"
#define INFO_COPYRIGHT	"Copyright 2014 Ollie Etherignton"
#define INFO_DESC	"A command-line text editor"
#define INFO_WEBSITE	"http://github.com/olliejohn/Yaw"

int version()
{
	printf("%s - Version %s\n", INFO_NAME, INFO_VERSION);
	return 0;
}

int about()
{
	printf( "\n%s - %s by %s\nVersion %s\n"
		"Available as free software under the %s\n%s\n%s\n"
		"Use the --help flag for usage information\n"
		"Type 'info yaw' to read the full user manual\n\n",
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
		"yaw [filename | flag]\n\n"
		"Flags:\n"
		"-a    --about     Display the about dialog\n"
		"-v    --version   Display version information\n"
		"-h    --help      Display this help dialog\n");
	return 0;
}

void register_default_binds()
{
	bind(TK_CTRL_Q,	cb_exit		);
	bind(TK_F(3), 	cb_save_exit	);
	bind(TK_CTRL_S, cb_save		);
	bind(TK_CTRL_N, cb_new		);
	bind(TK_CTRL_O, cb_open		);
	bind(TK_CTRL_W, cb_close_buffer	);
	bind(TK_CTRL_B, cb_visit_buf	);
	bind(TK_CTRL_G, cb_visit_cli	);
	bind(TK_CTRL_T, cb_visit_term	);
	bind(TK_CTRL_X, cb_cut		);
	bind(TK_CTRL_C, cb_copy		);
	bind(TK_CTRL_V, cb_paste	);
}

#define MALLOPT_IGNORE_AND_CONTINUE 			0
#define MALLOPT_PRINT_DETAILED_AND_CONTINUE 		1
#define MALLOPT_ABORT 					2
#define MALLOPT_PRINT_DETAILED_WITH_STACK_AND_ABORT 	3
#define MALLOPT_PRINT_SIMPLE_AND_CONTINUE 		5
#define MALLOPT_PRINT_SIMPLE_WITH_STACK_AND_ABORT 	7

int main(int argc, char **argv)
{
	/* Enable unicode */
	void t_init_unicode();

	/* Set mallopt options */
#ifdef DEBUG
	mallopt(M_CHECK_ACTION, MALLOPT_PRINT_DETAILED_WITH_STACK_AND_ABORT);
#else	/* DEBUG not defined */
	mallopt(M_CHECK_ACTION, MALLOPT_IGNORE_AND_CONTINUE);
#endif	/* DEBUG */

	/* Parse args */
	int FLAG_OPEN = 0;

	if (argc == 2) {
		if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version"))
			return version();
		else if (!strcmp(argv[1], "-a") || !strcmp(argv[1], "--about"))
			return about();
		else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
			return usage();
		else
			FLAG_OPEN++;
	}

	if (argc > 2) {
		printf("Invalid arguments");
		return 0;
	}

	/* Lisp engine init */
	lisp_init();

	/* tui init */
	t_init(TUI_RAW | TUI_COLOR | TUI_KEYPAD);

	/* Run program */
	binds_init();

	int status = config_init();

	color_init();

	t_refresh();

	char *filepath = NULL;
	if (FLAG_OPEN)
		filepath = argv[1];

	struct Screen *scrn = screen_new();

	binds_set_current_screen(scrn);
	register_default_binds();

	if (status == -1)
		screen_set_flag(scrn, SF_NO_CONFIG);

	status = screen_run(scrn, filepath);
	screen_free(scrn);

	config_destroy();
	binds_destroy();

	/* tui destroy */
	t_destroy();

	/* Lisp engine destroy */
	lisp_destroy();

	return status;
}
