/*
 * main.c
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

#include <ctype.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "binds.h"
#include "buffer.h"
#include "config.h"

#define INFO_NAME 	"ee"
#define INFO_VERSION 	"0.01"
#define INFO_AUTHOR 	"Ollie Etherington"
#define INFO_LICENSE	"GNU GPLv2"
#define INFO_COPYRIGHT	"Copyright 2014 Ollie Etherignton"
#define INFO_DESC	"A simple command-line text editor"
#define INFO_WEBSITE	"http://github.com/olliejohn/ee"

/* #define DEBUG */

int save(char *file, struct Buffer *buf)
{
	FILE *f = fopen(file, "w");

	if (f == NULL) {
		printw("Error opening file\n");
		return -1;
	}

	int i;
	for (i = 0; i < buf->size; i++)
		fprintf(f, "%s\n", buf->data[i]->data);

	fclose(f);

	return 0;
}

int open(char *file, struct Buffer *buf, int x, int y)
{
	FILE *f = fopen(file, "r");

	if (f == NULL) {
		printw("Error opening file\n");
		return -1;
	}

	char ch;
	while ((ch = fgetc(f)) != EOF) {
		if (ch == '\n') {
			buffer_new_line(buf);
			continue;
		}

		buffer_add(buf, ch);
	}

	fclose(f);

	buf->pos = y;
	buf->data[buf->pos]->pos = x;

	return 0;
}

int run(struct Buffer *buf)
{
	int i;
	for (i = 0; i < buf->size; i++)
		mvprintw(i, 0, "%s", buf->data[i]->data);

#ifdef DEBUG
	mvprintw(1, 60, "L: %d      C: %d  ",
		 buf->pos, buf->data[buf->pos]->pos);
	mvprintw(2, 60, "This line is %d chars    ",
		 buf->data[buf->pos]->size);
#endif

	move(buf->pos, buf->data[buf->pos]->pos);
	refresh();

	int ch = getch();

	while (ch != BIND_EXIT && ch != BIND_SAVE_EXIT) {
		switch (ch) {
		case KEY_BACKSPACE:
			if (buffer_backspace(buf) == 0) {
				move(buf->pos, 0);
				wclrtoeol(stdscr);
				move(buf->pos, buf->data[buf->pos]->pos);
				mvprintw(buf->pos, 0, "%s",
					 buf->data[buf->pos]->data);
			} else {
				clear();
				int i;
				for (i = 0; i < buf->size; i++)
					mvprintw(i, 0, "%s",
						 buf->data[i]->data);
			}

			break;
		case '\n':
			buffer_new_line(buf);
			int i;
			for (i = buf->pos - 1; i < buf->size; i++) {
				move(i, 0);
				wclrtoeol(stdscr);
				mvprintw(i, 0, "%s", buf->data[i]->data);
			}
			break;
		case KEY_LEFT:
			buffer_move_backward(buf);
			break;
		case KEY_RIGHT:
			buffer_move_forward(buf);
			break;
		case KEY_UP:
			buffer_move_up(buf);
			break;
		case KEY_DOWN:
			buffer_move_down(buf);
			break;
		default:
			if (isprint(ch)) {
				buffer_add(buf, ch);
				mvprintw(buf->pos, 0, "%s",
					 buf->data[buf->pos]->data);
			} else {
				ch = getch();
				continue;
			}
		}

#ifdef DEBUG
		mvprintw(1, 60, "L: %d      C: %d  ",
			 buf->pos, buf->data[buf->pos]->pos);
		mvprintw(2, 60, "This line is %d chars    ",
			 buf->data[buf->pos]->size);
#endif

		move(buf->pos, buf->data[buf->pos]->pos);
		refresh();
		ch = getch();
	}

	if (ch == BIND_SAVE_EXIT)
		return 1;
	else
		return 0;
}

void version()
{
	printf("%s - Version %s\n", INFO_NAME, INFO_VERSION);
	exit(0);
}

void about()
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
	exit(0);
}

void usage()
{
	printf("Usage:\n"
		"ee [filename | flag]\n\n"
		"Flags:\n"
		"--about     Display the about dialog\n"
		"--version   Display version information\n"
		"--help      Display this help dialog\n");
	exit(0);
}

int main(int argc, char **argv)
{
	/* Parse args */
	int FLAG_OPEN = 0;

	if (argc == 2) {
		if (strcmp(argv[1], "--version") == 0)
			version();
		else if (strcmp(argv[1], "--about") == 0)
			about();
		else if (strcmp(argv[1], "--help") == 0)
			usage();
		else
			FLAG_OPEN++;
	}

	if (argc > 2) {
		printf("Invalid arguments");
		exit(0);
	}

	/* ncurses init */
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();

	/* Run program */
	struct Buffer *buf = buffer_new();
	struct Settings *settings = settings_new();
	load_init_config(settings);

	if (FLAG_OPEN)
		open(argv[1], buf, 0, 0);

	if (run(buf) == 1)
		save(argv[1], buf);

	settings_free(settings);
	buffer_free(buf);

	/* ncurses destroy */
	endwin();

	return 0;
}
