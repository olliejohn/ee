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
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <tui.h>
#include <wchar.h>

#include "binds.h"
#include "buffer.h"
#include "color.h"
#include "config.h"
#include "window.h"

#define INFO_NAME 	"ee"
#define INFO_VERSION 	"0.01"
#define INFO_AUTHOR 	"Ollie Etherington"
#define INFO_LICENSE	"GNU GPLv2"
#define INFO_COPYRIGHT	"Copyright 2014 Ollie Etherignton"
#define INFO_DESC	"A simple command-line text editor"
#define INFO_WEBSITE	"http://github.com/olliejohn/ee"

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

void run_cmd_loop(t_window *twin, t_window *bwin, t_window *cwin,
		  struct Buffer *buf)
{

}

#define CUR_LINUM	buf->pos
#define CUR_LINE	buf->data[CUR_LINUM]
#define CUR_CHAR	CUR_LINE->pos
#define CSR_X		line_get_cur_pos(CUR_LINE)

void print_info_to_bar(t_window *bar, struct Buffer *buf)
{
	print_to_win(bar, L"L:%d/%d    C:%d/%d      ",
		     CUR_LINUM, buf->size, CUR_CHAR, CUR_LINE->size);
}

int run_buffer(t_window *twin, t_window *bwin, t_window *cwin, char *filepath)
{
	struct Buffer *buf = buffer_new();
/*
	int wpathlen = strlen(filepath) + 1;
	t_char *wpath = calloc(wpathlen, sizeof(t_char));
	mbstowcs(wpath, filepath, wpathlen);

	if (filepath != NULL) {
		//buffer_open(buf, filepath, 0, 0);
		//print_to_win(twin, wpath);
	} else {
		//print_to_win(twin, "Untitled");
	}

	free(wpath);
*/
	print_to_win(twin, L"Untitled");

	int i;
	for (i = 0; i < buf->size; i++)
		t_mv_wprint(bwin, 0, i, L"%ls", buf->data[i]->data);

	print_info_to_bar(cwin, buf);

	t_wrefresh(twin);
	t_wrefresh(cwin);

	t_wmove(bwin, CUR_CHAR, CUR_LINUM);
	t_wrefresh(bwin);

	t_char ch;
	while (t_getch(&ch) != TUI_ERR && ch != BIND_EXIT &&
						ch != BIND_SAVE_EXIT) {
		switch (ch) {
		case '^':
			run_cmd_loop(twin, bwin, cwin, buf);
			break;
		case TK_BKSPC:
			if (buffer_backspace(buf) == 0) {
				t_wclear(bwin);
				int i;
				for (i = 0; i < buf->size; i++)
					t_mv_wprint(bwin, 0, i, L"%ls",
						    buf->data[i]->data);
				/*
				t_wmove(bwin, 0, CUR_LINUM);
				t_wclrtoeol(bwin);
				t_wmove(bwin, CUR_CHAR, CUR_LINUM);
				t_mv_wprint(bwin, 0, CUR_LINUM, L"%ls",
					    CUR_LINE->data);
				*/
			} else {
				t_wclear(bwin);
				int i;
				for (i = 0; i < buf->size; i++)
					t_mv_wprint(bwin, 0, i, L"%ls",
						    buf->data[i]->data);
			}

			break;
		case TK_DELETE:
			if (buffer_delete(buf) == 0) {
				t_wmove(bwin, 0, CUR_LINUM);
				t_wclrtoeol(bwin);
				t_wmove(bwin, CUR_CHAR, CUR_LINUM);
				t_mv_wprint(bwin, 0, CUR_LINUM, L"%ls",
					    CUR_LINE->data);
			} else {
				t_wclear(bwin);
				int i;
				for (i = 0; i < buf->size; i++)
					t_mv_wprint(bwin, 0, i, L"%ls",
						    buf->data[i]->data);
			}

			break;
		case TK_ENTER:
			buffer_new_line(buf);
			t_wclear(bwin);
			int i;
			for (i = 0; i < buf->size; i++)
				t_mv_wprint(bwin, 0, i, L"%ls",
					    buf->data[i]->data);
			/*
			for (i = CUR_LINUM - 1; i < buf->size; i++) {
				t_wmove(bwin, 0, i);
				t_wclrtoeol(bwin);
				t_mv_wprint(bwin, 0, i, L"%ls",
					    buf->data[i]->data);
			}*/
			break;
		case TK_LEFT:
			buffer_move_backward(buf);
			break;
		case TK_RIGHT:
			buffer_move_forward(buf);
			break;
		case TK_UP:
			buffer_move_up(buf);
			break;
		case TK_DOWN:
			buffer_move_down(buf);
			break;
		default:
			//if (isprint(ch) || ch == '\t') {
				buffer_add(buf, ch);
				t_mv_wprint(bwin, 0, CUR_LINUM, L"%ls",
					    CUR_LINE->data);
			//}
		}

		t_wrefresh(twin);
		print_info_to_bar(cwin, buf);
		t_wrefresh(cwin);
		t_wmove(bwin, CUR_CHAR, CUR_LINUM);
		t_wrefresh(bwin);
	}

	buffer_free(buf);

	if (ch == BIND_SAVE_EXIT)
		return 1;
	else
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

	/* tui init */
	t_init(TUI_RAW | TUI_COLOR | TUI_KEYPAD);

	/* Run program */
	int WIDTH, HEIGHT;
	t_getmaxxy(WIDTH, HEIGHT);

	color_init();

	t_refresh();

	if (config_init() == -1)
		;// ERR: "Couldn't load or create config - using default"

	t_window *title_win = t_winit(0, 0, WIDTH, 1);
	t_window *buffer_win = t_winit(0, 1, WIDTH, HEIGHT - 2);
	t_window *cmd_win = t_winit(0, HEIGHT - 1, WIDTH, 1);

	t_wbkgd(title_win, CS_TITLE_BAR);
	t_wbkgd(buffer_win, CS_BUFFER);
	t_wbkgd(cmd_win, CS_CMD_BAR);

	t_wrefresh(title_win);
	t_wrefresh(buffer_win);
	t_wrefresh(cmd_win);

	char *filepath = NULL;
	if (FLAG_OPEN)
		filepath = argv[1];

	int status = run_buffer(title_win, buffer_win, cmd_win, filepath);

	t_wdestroy(title_win);
	t_wdestroy(buffer_win);
	t_wdestroy(cmd_win);

	config_destroy();

	/* tui destroy */
	t_destroy();

	return status;
}
