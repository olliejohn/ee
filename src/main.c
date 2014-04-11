#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "binds.h"
#include "buffer.h"

#define INFO_NAME 	"ee"
#define INFO_VERSION 	"0.01"
#define INFO_AUTHOR 	"Ollie Etherington"
#define INFO_LICENSE	"GNU GPLv2"
#define INFO_COPYRIGHT	"Copyright 2014 Ollie Etherignton"
#define INFO_DESC	"A simple command-line text editor"
#define INFO_WEBSITE	"http://github.com/olliejohn/ee"

int save(char *file, struct Buffer *buf)
{
	FILE *f = fopen(file, "w");

	if (f == NULL) {
		printw("Error opening file\n");
		return -1;
	}

	int i;
	for (i = 0; i < buf->size - 1; i++)
		fprintf(f, "%s\n", buf->data[i]->data);

	fprintf(f, "%s", buf->data[++i]->data);

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
	while (( ch = fgetc(f)) != EOF ) {
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

	move(buf->pos, buf->data[buf->pos]->pos);
	refresh();

	int ch = getch();

	while (ch != BIND_EXIT && ch != BIND_SAVE_EXIT) {
		switch (ch) {
		case KEY_BACKSPACE:
			buffer_backspace(buf);
			move(buf->pos, 0);
			wclrtoeol(stdscr);
			move(buf->pos, buf->data[buf->pos]->pos);
			mvprintw(buf->pos, 0, "%s", buf->data[buf->pos]->data);
			break;
		case '\n':
			buffer_new_line(buf);
			int i;
			//for (i = buf->pos; i < buf->size; i++)
				//mvprintw(i, 0, "%s", buf->data[i]->data);
			clear();
			for (i = 0; i < buf->size; i++)
				mvprintw(i, 0, "%s", buf->data[i]->data);
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
			buffer_add(buf, ch);
			mvprintw(buf->pos, 0, "%s", buf->data[buf->pos]->data);
		}

		move(buf->pos, buf->data[buf->pos]->pos);
		ch = getch();
		refresh();
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

	if (FLAG_OPEN)
		open(argv[1], buf, 0, 0);

	if (run(buf) == 1)
		save(argv[1], buf);

	buffer_free(buf);

	/* ncurses destroy */
	endwin();

	return 0;
}
