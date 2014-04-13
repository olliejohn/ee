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
#include "color.h"
#include "config.h"

#define INFO_NAME 	"ee"
#define INFO_VERSION 	"0.01"
#define INFO_AUTHOR 	"Ollie Etherington"
#define INFO_LICENSE	"GNU GPLv2"
#define INFO_COPYRIGHT	"Copyright 2014 Ollie Etherignton"
#define INFO_DESC	"A simple command-line text editor"
#define INFO_WEBSITE	"http://github.com/olliejohn/ee"

#define v_print_to_win(win, msg, ...) vw_printw(win, msg, __VA_ARGS__);

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

WINDOW *init_window(int startx, int starty, int width, int height)
{
	WINDOW *win = newwin(height, width, starty, startx);
	keypad(win , TRUE);
	wrefresh(win);
	return win;
}

void destroy_window(WINDOW *win)
{
	wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wclear(win);
	wrefresh(win);
	delwin(win);
}

void print_to_win(WINDOW *win, const char *msg, ...)
{
	wclear(win);

	va_list arg;
	va_start(arg, msg);
	v_print_to_win(win, msg, arg);
	va_end(arg);

	wrefresh(win);
}

int run(WINDOW *twin, WINDOW *bwin, WINDOW *cwin)
{
	struct Buffer *buf = buffer_new();

	int i;
	for (i = 0; i < buf->size; i++)
		mvwprintw(bwin, i, 0, "%s", buf->data[i]->data);

	print_to_win(twin, "TITLE BAR");

	print_to_win(cwin, "L:%d    C:%d    ",
		     buf->pos, buf->data[buf->pos]->pos);

	wrefresh(twin);
	wrefresh(cwin);

	wmove(bwin, buf->pos, buf->data[buf->pos]->pos);
	wrefresh(bwin);

	int ch = wgetch(bwin);

	while (ch != BIND_EXIT && ch != BIND_SAVE_EXIT) {
		switch (ch) {
		case KEY_EXIT:
			//exit(0);
		case KEY_BACKSPACE:
			if (buffer_backspace(buf) == 0) {
				wmove(bwin, buf->pos, 0);
				wclrtoeol(bwin);
				wmove(bwin, buf->pos, buf->data[buf->pos]->pos);
				mvwprintw(bwin, buf->pos, 0, "%s",
					  buf->data[buf->pos]->data);
			} else {
				wclear(bwin);
				int i;
				for (i = 0; i < buf->size; i++)
					mvwprintw(bwin, i, 0, "%s",
						  buf->data[i]->data);
			}

			break;
		case KEY_DC:
			if (buffer_delete(buf) == 0) {
				wmove(bwin, buf->pos, 0);
				wclrtoeol(bwin);
				wmove(bwin, buf->pos, buf->data[buf->pos]->pos);
				mvwprintw(bwin, buf->pos, 0, "%s",
					  buf->data[buf->pos]->data);
			} else {
				wclear(bwin);
				int i;
				for (i = 0; i < buf->size; i++)
					mvwprintw(bwin, i, 0, "%s",
						  buf->data[i]->data);
			}

			break;
		case '\n':
			buffer_new_line(buf);
			int i;
			for (i = buf->pos - 1; i < buf->size; i++) {
				wmove(bwin, i, 0);
				wclrtoeol(bwin);
				mvwprintw(bwin, i, 0, "%s", buf->data[i]->data);
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
				mvwprintw(bwin, buf->pos, 0, "%s",
					  buf->data[buf->pos]->data);
			} else {
				ch = wgetch(bwin);
				continue;
			}
		}

		print_to_win(cwin, "L:%d    C:%d    ",
			     buf->pos, buf->data[buf->pos]->pos);
		refresh();
		wmove(bwin, buf->pos, buf->data[buf->pos]->pos);
		wrefresh(bwin);
		ch = wgetch(bwin);
	}

	buffer_free(buf);

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
	cbreak();
	keypad(stdscr, TRUE);
	noecho();

	 if(has_colors() == FALSE) {
		endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}

	start_color();
	color_init();

	/* Run program */
	int WIDTH, HEIGHT;

	refresh();

	getmaxyx(stdscr, HEIGHT, WIDTH);

	if (config_init() == -1) {
		// Print message: couldn't load or create cfg, using default
	}

	WINDOW *title_win = init_window(0, 0, WIDTH, 1);
	WINDOW *buffer_win = init_window(0, 1, WIDTH, HEIGHT - 2);
	WINDOW *cmd_win = init_window(0, HEIGHT - 1, WIDTH, 1);

	wbkgd(title_win, COLOR_PAIR(SCH_TITLE_BAR));
	wbkgd(buffer_win, COLOR_PAIR(SCH_BUFFER));
	wbkgd(cmd_win, COLOR_PAIR(SCH_CMD_BAR));

	int status = run(title_win, buffer_win, cmd_win);

	destroy_window(title_win);
	destroy_window(buffer_win);
	destroy_window(cmd_win);

	config_destroy();

	/* ncurses destroy */
	endwin();

	return status;
}
