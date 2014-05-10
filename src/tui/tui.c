/*
 * tui.c
 * Part of the TUI terminal user interface library
 *
 * Copyright (C) 2014 Ollie Etherington.
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

#include "tui.h"

#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>
#include <termios.h>
#include <unistd.h>

/* Variables for color capabilities */
static char term_buffer[8192];
static char *termtype = NULL;
static char *c_setfore = NULL;
static char *c_setback = NULL;
static char *c_resetcolor = NULL;
static char *c_standstr = NULL;
static char *c_exit_stand = NULL;
static char *c_inicolor_str = NULL;
static char *c_clrnames  = NULL;
static char *c_bold = NULL;
static char *c_dim = NULL;
static char *c_reverse = NULL;
static char *c_blinking = NULL;
static char *c_exit_attr = NULL;
static char *c_opair = NULL;
static int maxcolors = 0;
static int maxpairs = 0;
static int COL_MODE = 0;

void __attribute__((noreturn)) t_vfatal(char *err, va_list args)
{
	endwin();
	vfprintf(stderr, err, args);
	exit(TUI_FAIL);
}

void __attribute__((noreturn)) t_fatal(char *err, ...)
{
	endwin();
	va_list args;
	va_start(args, err);
	vfprintf(stderr, err, args);
	va_end(args);
	exit(TUI_FAIL);
}

WINDOW *t_winit(int x, int y, int width, int height)
{
	WINDOW *win = newwin(height, width, y, x);
	keypad(win, TRUE);
	return win;
}

int t_wdestroy(WINDOW *win)
{
	wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wclear(win);
	wrefresh(win);
	return delwin(win);
}

int t_vwprint_ascii(WINDOW *win, char *fmt, va_list args)
{
	return vwprintw(win, fmt, args);
}

int t_wprint_ascii(WINDOW *win, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int status = vwprintw(win, fmt, args);
	va_end(args);
	return status;
}

int t_mv_wprint_ascii(WINDOW *win, int x, int y, char *fmt, ...)
{
	t_wmove(win, x, y);
	va_list args;
	va_start(args, fmt);
	int status = vwprintw(win, fmt, args);
	va_end(args);
	return status;
}

int t_wprint(WINDOW *win, t_char *fmt, ...)
{
	wchar_t buf[MAX_PRINT_LEN];
	va_list args;
	va_start(args, fmt);
	vswprintf(buf, MAX_PRINT_LEN, fmt, args);
	va_end(args);

	return waddwstr(win, buf);
}

int t_wprintn(WINDOW *win, int n, t_char *fmt, ...)
{
	wchar_t buf[MAX_PRINT_LEN];
	va_list args;
	va_start(args, fmt);
	vswprintf(buf, MAX_PRINT_LEN, fmt, args);
	va_end(args);

	return waddnwstr(win, buf, n);
}

int t_mv_wprint(WINDOW *win, int x, int y, t_char *fmt, ...)
{
	wchar_t buf[MAX_PRINT_LEN];
	va_list args;
	va_start(args, fmt);
	vswprintf(buf, MAX_PRINT_LEN, fmt, args);
	va_end(args);

	return mvwaddwstr(win, y, x, buf);
}

int t_mv_wprintn(WINDOW *win, int x, int y, int n, t_char *fmt, ...)
{
	wchar_t buf[MAX_PRINT_LEN];
	va_list args;
	va_start(args, fmt);
	vswprintf(buf, MAX_PRINT_LEN, fmt, args);
	va_end(args);

	return mvwaddnwstr(win, y, x, buf, n);
}

int t_vwprint(WINDOW *win, t_char *fmt, va_list args)
{
	wchar_t buf[MAX_PRINT_LEN];
	vswprintf(buf, MAX_PRINT_LEN, fmt, args);
	return waddwstr(win, buf);
}

int t_vwprintn(WINDOW *win, int n, t_char *fmt, va_list args)
{
	wchar_t buf[MAX_PRINT_LEN];
	vswprintf(buf, MAX_PRINT_LEN, fmt, args);
	return waddnwstr(win, buf, n);
}

int t_mv_vwprint(WINDOW *win, int x, int y, t_char *fmt, va_list args)
{
	wchar_t buf[MAX_PRINT_LEN];
	vswprintf(buf, MAX_PRINT_LEN, fmt, args);
	return mvwaddwstr(win, y, x, buf);
}

int t_mv_vwprintn(WINDOW *win, int x, int y, int n, t_char *fmt, va_list args)
{
	wchar_t buf[MAX_PRINT_LEN];
	vswprintf(buf, MAX_PRINT_LEN, fmt, args);
	return mvwaddnwstr(win, y, x, buf, n);
}

int t_wbkgd(WINDOW *win, int data)
{
	return wbkgd(win, data);
}

/*
int t_wbkgrnd(WINDOW *win, cchar_t *data)
{
	return wbkgrnd(win, data);
}

void t_wbkgrndset(WINDOW *win, cchar_t *data)
{
	return wbkgrndset(win, data);
}

int t_wgetbkgrnd(WINDOW *win, cchar_t *data)
{
	return wgetbkgrnd(win, data);
}
*/

int t_enable_scroll(TUI_BOOL value)
{
	if (value == TUI_TRUE)
		return (scrollok(stdscr, TRUE) && idlok(stdscr, TRUE)) ? 0 : -1;

	return (scrollok(stdscr, FALSE) && idlok(stdscr, FALSE)) ? 0 : -1;
}

int t_wenable_scroll(WINDOW *win, TUI_BOOL value)
{
	if (value == TUI_TRUE)
		return (scrollok(win, TRUE) && idlok(win, TRUE)) ? 0 : -1;

	return (scrollok(win, FALSE) && idlok(win, FALSE)) ? 0 : -1;
}

int t_setscrreg(int top, int bottom)
{
	return setscrreg(top, bottom);
}

int t_wsetscrreg(t_window *win, int top, int bottom)
{
	return wsetscrreg(win, top, bottom);
}

int t_scrl(enum T_SCRL_DIR dir) {
	return scrl(dir);
}

int t_wscrl(t_window *win, enum T_SCRL_DIR dir)
{
	return wscrl(win, dir);
}

/* Scroll win up one line */
int t_scroll(t_window *win)
{
	return scroll(win);
}

int t_wborder(WINDOW *win, t_char ts, t_char rs, t_char bs, t_char ls,
	      t_char tl, t_char tr, t_char br, t_char bl)
{
	return wborder(win, ls, rs, ts, bs, tl, tr, bl, br);
}

void capability(char **buf,char *code,char **var)
{
	char *val = tgetstr(code, buf);
	if (!val)
		*var = NULL;
	else
		*var = strdup(val);
}

void get_capabilities()
{
	char buffer[2500] = "";
	char *buf=buffer;

	termtype = getenv("TERM");

	maxcolors = tgetnum("Co");
	maxpairs = tgetnum("pa");

	capability(&buf, "AF", &c_setfore);
	capability(&buf, "AB", &c_setback);
	capability(&buf, "op", &c_resetcolor);
	capability(&buf, "md", &c_bold);
	capability(&buf, "mh", &c_dim);
	capability(&buf, "mr", &c_reverse);
	capability(&buf, "mb", &c_blinking);
	capability(&buf, "so", &c_standstr);
	capability(&buf, "se", &c_exit_stand);
	capability(&buf, "me", &c_exit_attr);
	capability(&buf, "Ic", &c_inicolor_str);
	capability(&buf, "Yw", &c_clrnames);
	capability(&buf, "op", &c_opair);
}

/*
 * TODO: Implement method of setting TERM env var to xterm-256color. When I do
 * this with setenv() it freezes up the terminal after the application closes
 * for some reason.
 */
int t_enable_color()
{
	get_capabilities();

	if (termtype == 0)
		return TUI_NONE_SPECIFIED;

	if (strcasecmp(termtype, "xterm-256color") != 0)
		return TUI_TERM_NOT_SUPPORTED;

	if(has_colors() == FALSE)
		return TUI_COL_NOT_SUPPORTED;

	start_color();

	int status = tgetent(term_buffer, termtype);
	if (status < 0)
		return TUI_NO_TERMCAP_ACCESS;
	if (status == 0)
		return TUI_TERM_NOT_DEFINED;

	if (maxcolors <= 0)
		return TUI_COL_NOT_IN_CAPS;
	if (maxpairs <= 0)
		return TUI_PAIRS_NOT_IN_CAPS;

	COL_MODE = 1;

	return maxcolors;
}

void t_disable_color()
{
	COL_MODE = 0;
}

/* Returns 1 if we're in color mode or 0 if we're not */
int t_in_color_mode()
{
	return COL_MODE;
}

int t_get_max_colors()
{
	if (COL_MODE)
		return maxcolors;
	else
		return -1;
}

int t_get_max_pairs()
{
	if (COL_MODE)
		return maxpairs;
	else
		return -1;
}

TUI_BOOL t_can_change_colors()
{
	if (can_change_color() == TRUE)
		return TUI_TRUE;
	else
		return TUI_FALSE;
}

int t_set_flags(int flags)
{
	if (flags & TUI_KEYPAD)
		keypad(stdscr, TRUE);

	if (flags & TUI_NO_DELAY)
		nodelay(stdscr, TRUE);

	if (flags & TUI_ECHO)
		echo();

	if (flags & TUI_RAW)
		raw();

	if (flags & TUI_COOKED)
		nocbreak();

	int status = 0;
	if (flags & TUI_COLOR)
		status = t_enable_color();

	return status;
}

void t_unset_flags(int flags)
{
	if (flags & TUI_KEYPAD)
		keypad(stdscr, FALSE);

	if (flags & TUI_NO_DELAY)
		nodelay(stdscr, FALSE);

	if (flags & TUI_ECHO)
		noecho();

	if (flags & TUI_RAW)
		noraw();

	if (flags & TUI_COOKED)
		cbreak();
}

void t_half_delay_start(int tenths)
{
	halfdelay(tenths);
}

void t_half_delay_end(TUI_BOOL cooked)
{
	nocbreak();

	if (!(cooked & TUI_COOKED))
		cbreak();
}

int t_getch(t_char *ch)
{
	wint_t c;
	int status = get_wch(&c);
	*ch = (t_char) c;
	return status;
}

int t_wgetch(WINDOW *win, t_char *ch)
{
	wint_t c;
	int status = wget_wch(win, &c);
	*ch = (t_char) c;
	return status;
}

int t_mv_getch(int x, int y, t_char *ch)
{
	wint_t c;
	int status = mvget_wch(y, x, &c);
	*ch = (t_char) c;
	return status;
}

int t_mv_wgetch(WINDOW *win, int x, int y, t_char *ch)
{
	wint_t c;
	int status = mvwget_wch(win, y, x, &c);
	*ch = (t_char) c;
	return status;
}

void t_init_unicode()
{
	setlocale(LC_ALL, "");
}

int t_init(int flags)
{
	t_init_unicode();
	initscr();
	t_unset_flags(~flags);
	return t_set_flags(flags);
}

void t_destroy()
{
	endwin();
}
