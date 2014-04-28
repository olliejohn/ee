/*
 * tui.h
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

/*
 * Make sure that this is the first file included in any other applications
 * Make sure that the first thing the other application does is either t_init()
 * or t_init_unicode()
 *
 * ncurses(w) needn't be included in the souce file as it is included here
 * Link with -lncursesw. Some systems will also require -ltinfo
 *
 * Mixing tui functions with the original ncurses functions is generally
 * discouraged but entirely possible. In particular, feel free to use any
 * ncurses functions that I've forgotten to add a wrapper for.
 */

#ifndef TUI_H
#define TUI_H

/* ncursesw requires this to enable wide functions */
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif

#include <ncursesw/ncurses.h>
#include <stdarg.h>
#include <wchar.h>

/*
 * Maximum number of characters that can be printed by a single function call.
 * Only applies to UTF-8 functions - ASCII functions can print more.
 */
#define MAX_PRINT_LEN 512

/* Enable passing 0 va_args to variadic macros */
#define VA_ARGS(...) , ##__VA_ARGS__

/* Return values */
#define TUI_SUCCESS 	0
#define TUI_FAIL 	-1
#define TUI_FUNC_KEY 	KEY_CODE_YES
#define TUI_CHAR 	OK
#define TUI_ERR 	ERR

/* Typedefed ncurses structs for consistency - use these in your application */
typedef struct _win_st t_window;
typedef wchar_t t_char;

/* Bool values */
typedef enum {
	TUI_TRUE = 1,
	TUI_FALSE = 0,
} TUI_BOOL;

/*
 * Flags for enabling/disabling certain ncurses features. All can be toggled
 * except color which cannot be disabled
 */
enum TUI_FLAG {
	TUI_NULL = 0,
	TUI_COLOR = 1,
	TUI_ECHO = 2,
	TUI_COOKED = 4,
	TUI_KEYPAD = 8,
	TUI_RAW = 16,
	TUI_NO_DELAY = 32,
};

/*
 * Return values of the t_enable_color function - this can fail in lots of
 * different ways. If the init fails, one of these values will be returned,
 * otherwise the maximum number of colors will be returned. If you do not need
 * to know WHY the call failed, only that it did, all of the fail return values
 * are below 0 and so you can test for them all at once by comparing to 0.
 */
enum {
	TUI_COL_NOT_SUPPORTED = -1,
	TUI_NONE_SPECIFIED = -2,
	TUI_NO_TERMCAP_ACCESS = -3,
	TUI_TERM_NOT_DEFINED = -4,
	TUI_TERM_NOT_SUPPORTED = -5,
	TUI_COL_NOT_IN_CAPS = -6,
	TUI_PAIRS_NOT_IN_CAPS = -7,
};

/*
 * Init/destroy functions. Unicode must be enabled before your application does
 * ANYTHING else. t_init will do this, but if you need to do something else
 * before initialising the window you can call t_init_unicode first, then do
 * your other work, then call t_init. If t_init is the first function you call,
 * you do not need to call n_init_unicode at all. t_init() returns the return
 * value from initialising the flags you pass to it.
 */
void t_init_unicode();
int t_init(int flags);
void t_destroy();

/*
 * Fatal functions exit the program then print an error message. They don't
 * perform full garbage collection on exit but they do attempt to return the
 * host terminal to it's previous working state.
 */
void t_vfatal(char *err, va_list args);
void t_fatal(char *err, ...);

/*
 * The function for initialising 256 color support. It's called implicilty by
 * t_set_flags() if you pass it TUI_COLOR.
 */
int t_enable_color();
void t_disable_color();
int t_in_color_mode();
int t_get_max_colors();
int t_get_max_pairs();
TUI_BOOL t_can_change_colors();

/*
 * Used with enum TUI_FLAG. Multiple flags can be used at once by or'ing them.
 * When setting flags, the return value will always be 0 unless the TUI_COLOR
 * flag is used, in which case it will return the return value of t_enable_color
 */
int t_set_flags(int flags);
void t_unset_flags(int flags);

/*
 * The t_half_delay_start() routine is used for half-delay mode, which  is
 * similar to cbreak() mode in that characters typed by the user are immediately
 * available to the program. However, after blocking for tenths tenths of
 * seconds, TUI_ERR is returned if nothing has been typed. The value of tenths
 * must be a number between 1 and 255. t_half_delay_end() exits this mode with
 * the option to enter cooked mode or not.
 */
void t_half_delay_start(int tenths);
void t_half_delay_end(TUI_BOOL cooked);

/*
 * These functions optionally move and then get an input char and put it in ch.
 * They return TUI_FUNC_KEY if the input is a function key, TUI_CHAR if it's a
 * character or TUI_ERR otherwise.
 */
int t_getch(t_char *ch);
int t_wgetch(WINDOW *win, t_char *ch);
int t_mv_getch(int x, int y, t_char *ch);
int t_mv_wgetch(WINDOW *win, int x, int y, t_char *ch);

/*
 * Window constructor and destructor. As well as making the window, the
 * constructor also sets keypad to true by default. This can be disabled on a
 * per window basis with calls to t_unset_flags(TUI_KEYPAD)
 */
WINDOW *t_winit(int x, int y, int width, int height);
int t_wdestroy(WINDOW *win);

/* Window functions */
int t_vwprint_ascii(WINDOW *win, char *fmt, va_list args);
int t_wprint_ascii(WINDOW *win, char *fmt, ...);
int t_mv_wprint_ascii(WINDOW *win, int x, int y, char *fmt, ...);

int t_wprint(WINDOW *win, t_char *fmt, ...);
int t_wprintn(WINDOW *win, int n, t_char *fmt, ...);
int t_mv_wprint(WINDOW *win, int x, int y, t_char *fmt, ...);
int t_mv_wprintn(WINDOW *win, int x, int y, int n, t_char *fmt, ...);
int t_vwprint(WINDOW *win, t_char *fmt, va_list args);
int t_vwprintn(WINDOW *win, int n, t_char *fmt, va_list args);
int t_mv_vwprint(WINDOW *win, int x, int y, t_char *fmt, va_list args);
int t_mv_vwprintn(WINDOW *win, int x, int y, int n, t_char *fmt, va_list args);

int t_wbkgd(WINDOW *win, int data);
/*
int t_wbkgrnd(WINDOW *win, cchar_t *data);
void t_wbkgrndset(WINDOW *win, cchar_t *data);
int t_wgetbkgrnd(WINDOW *win, cchar_t *data);
*/

inline int t_wnodelay(t_window *win, TUI_BOOL value) {
	return (value) ? nodelay(win, TRUE) : nodelay(win, FALSE);
}

inline int t_wmove(WINDOW *win, int x, int y) { return wmove(win, y, x); }
inline int t_wrefresh(WINDOW *win) { return wrefresh(win); }

inline int t_wclear(WINDOW *win) { return wclear(win); }
inline int t_wclrtobot(WINDOW *win) { return wclrtobot(win); }
inline int t_wclrtoeol(WINDOW *win) { return wclrtoeol(win); }

int t_wborder(WINDOW *win, t_char ts, t_char rs, t_char bs, t_char ls,
	      t_char tl, t_char tr, t_char br, t_char bl);

inline int t_touchwin(t_window *win) { return touchwin(win); }
inline int t_untouchwin(t_window *win) { return untouchwin(win); }

inline int t_touchline(t_window *win, int start, int count) {
	return touchline(win, start, count);
}

inline int t_wtouchline(t_window *win, int y, int n, int changed) {
	return wtouchln(win, y, n, changed);
}

inline TUI_BOOL is_line_touched(t_window *win, int line) {
	return is_linetouched(win, line);
}

inline TUI_BOOL is_win_touched(t_window *win) {
	return is_wintouched(win);
}

/* Scrolling */
enum T_SCRL_DIR {
	T_SCRL_UP = -1,
	T_SCRL_DOWN = 1,
};

int t_enable_scroll(TUI_BOOL value);
int t_wenable_scroll(WINDOW *win, TUI_BOOL value);
int t_setscrreg(int top, int bottom);
int t_wsetscrreg(t_window *win, int top, int bottom);
int t_scrl(enum T_SCRL_DIR dir);
int t_wscrl(t_window *win, enum T_SCRL_DIR dir);
int t_scroll(t_window *win);

/* Attributes */
typedef attr_t t_attrib;

#define TA_NORMAL 	A_NORMAL,
#define TA_STANDOUT 	A_STANDOUT,
#define TA_UNDERLINE 	A_UNDERLINE,
#define TA_REVERSE 	A_REVERSE,
#define TA_BLINK 	A_BLINK,
#define TA_DIM 		A_DIM,
#define TA_BOLD 	A_BOLD,
#define TA_PROTECT 	A_PROTECT,
#define TA_INVIS 	A_INVIS,
#define TA_ALTCHARSET 	A_ALTCHARSET,
#define TA_CHARTEXT 	A_CHARTEXT,

inline int t_attrget(t_attrib *aptr, short *colorptr, void *opts) {
	return attr_get(aptr, colorptr, opts);
}

inline int t_attron(t_attrib a) { return attron(a); }
inline int t_attroff(t_attrib a) { return attroff(a); }
inline int t_attrset(t_attrib a) { return attrset(a); }
inline int t_attrend() { return standend(); }
inline int t_attrout() { return standout(); }

inline int t_color_set(short color_pair, void *opts) {
	color_set(color_pair, opts);
}

inline int t_wattrget(t_window *win, t_attrib *aptr, short *colorptr,
		      void *opts) {
	return wattr_get(win, aptr, colorptr, opts);
}

inline int t_wattron(t_window *win, t_attrib a) { return wattron(win, a); }
inline int t_wattroff(t_window *win, t_attrib a) { return wattroff(win, a); }
inline int t_wattrset(t_window *win, t_attrib a) { return wattrset(win, a); }
inline int t_wattrend(t_window *win) { return wstandend(win); }
inline int t_wattrout(t_window *win) { return wstandout(win); }

inline int t_wcolor_set(t_window *win, short color_pair, void *opts) {
	wcolor_set(win, color_pair, opts);
}

/* 'Change at' attribute functions without moving the cursor */
inline int t_chgat(int n, t_attrib attr, short col, const void *opts)
{
	return chgat(n, attr, col, opts);
}

inline int t_mv_chgat(int x, int y, int n, t_attrib attr, short col,
		      const void *opts)
{
	return mvchgat(y, x, n, attr, col, opts);
}

inline int t_wchgat(t_window *win, int n, t_attrib attr,
		       short col, const void *opts)
{
	return wchgat(win, n, attr, col, opts);
}

inline int t_mv_wchgat(t_window *win, int x, int y, int n, t_attrib attr,
		       short col, const void *opts)
{
	return mvwchgat(win, y, x, n, attr, col, opts);
}

/* Macros to apply functions to stdscr by default */
#define t_print_ascii(fmt, ...) t_wprint(stdscr, fmt VA_ARGS(__VA_ARGS__))
#define t_mv_print_ascii(x, y, fmt, ...)	\
	t_wprint(x, y, stdscr, fmt VA_ARGS(__VA_ARGS__))

#define t_print(fmt, ...) t_wprint(stdscr, fmt VA_ARGS(__VA_ARGS__))
#define t_printn(n, fmt, ...) t_wprintn(stdscr, n, fmt VA_ARGS(__VA_ARGS__))
#define t_mv_print(x, y, fmt, ...)		\
	t_mv_wprint(stdscr, x, y, fmt VA_ARGS(__VA_ARGS__))
#define t_mv_printn(n, x, y, fmt, ...)		\
	t_mv_wprintn(stdscr, x, y, n, fmt VA_ARGS(__VA_ARGS__))

#define t_bkgd(data) t_wbkgd(stdscr, data)
#define t_bkgrnd(data) t_wbkgrnd(stdscr, data)
#define t_bkgrndset(data) t_wbkgrndset(stdscr, data)
#define t_getbkgrnd(data) t_wgetbkgrnd(stdscr, data)

#define t_set_scroll(v) t_wset_scroll(stdscr, v)
#define t_nodelay(value) t_wnodelay(stdscr, value)
#define t_move(x, y) t_wmove(stdscr, x, y)
#define t_refresh() t_wrefresh(stdscr)

#define t_clear() t_wclear(stdscr)
#define t_clrtobot() t_wclrtobot(stdscr)
#define t_clrtoeol() t_wclrtoeol(stdscr)

#define t_border(ts, rs, bs, ls, tl, tr, br, bl)	\
	t_wborder(stdscr, ts, rs, bs, ls, tl, tr, br, bl)

/* Size functions */
inline int t_wgetcurx(t_window *win) 	{ return getcurx(win); }
inline int t_wgetcury(t_window *win) 	{ return getcury(win); }
inline int t_getcurx() 			{ return getcurx(stdscr); }
inline int t_getcury() 			{ return getcury(stdscr); }
inline int t_wgetbegx(t_window *win) 	{ return getcurx(win); }
inline int t_wgetbegy(t_window *win) 	{ return getcury(win); }
inline int t_getbegx() 			{ return getbegx(stdscr); }
inline int t_getbegy() 			{ return getbegy(stdscr); }
inline int t_wgetmaxx(t_window *win) 	{ return getmaxx(win); }
inline int t_wgetmaxy(t_window *win) 	{ return getmaxy(win); }
inline int t_getmaxx() 			{ return getmaxx(stdscr); }
inline int t_getmaxy() 			{ return getmaxy(stdscr); }
inline int t_wgetparx(t_window *win) 	{ return getparx(win); }
inline int t_wgetpary(t_window *win) 	{ return getpary(win); }
inline int t_getparx() 			{ return getparx(stdscr); }
inline int t_getpary() 			{ return getpary(stdscr); }
#define t_wgetxy(win, x, y)   	(y = getcury(win), x = getcurx(win))
#define t_wgetbegxy(win, x, y)	(y = getbegy(win), x = getbegx(win))
#define t_wgetmaxxy(win, x, y)	(y = getmaxy(win), x = getmaxx(win))
#define t_wgetparxy(win, x, y)	(y = getpary(win), x = getparx(win))
#define t_getxy(x, y)   	(y = getcury(stdscr), x = getcurx(stdscr))
#define t_getbegxy(x, y)	(y = getbegy(stdscr), x = getbegx(stdscr))
#define t_getmaxxy(x, y)	(y = getmaxy(stdscr), x = getmaxx(stdscr))
#define t_getparxy(x, y)	(y = getpary(stdscr), x = getparx(stdscr))

/* Key macros */
#define TK_F0		0410
#define TK_F(n)		(TK_F0+(n))
#define TK_INSERT 	0513
#define TK_DELETE 	0512
#define TK_HOME		0406
#define TK_END 		0550
#define TK_PGUP		0522
#define TK_PGDN 	0523
#define TK_DOWN 	0402
#define TK_UP		0403
#define TK_LEFT 	0404
#define TK_RIGHT 	0405
#define TK_BKSPC	0407
#define TK_TAB		'\t'
#define TK_ENTER 	'\n'
#define TK_SPACE 	' '

#define TK_CTRL_TILDE 	0x00
#define TK_CTRL_2 	0x00 // clash with 'CTRL_TILDE'
#define TK_CTRL_A 	0x01
#define TK_CTRL_B 	0x02
#define TK_CTRL_C 	0x03
#define TK_CTRL_D 	0x04
#define TK_CTRL_E 	0x05
#define TK_CTRL_F 	0x06
#define TK_CTRL_G 	0x07
#define TK_CTRL_BKSPC 	0x08
#define TK_CTRL_H 	0x08 // clash with 'CTRL_BACKSPACE'
#define TK_CTRL_TAB	0x09
#define TK_CTRL_I 	0x09 // clash with 'CTRL + TAB'
#define TK_CTRL_J	0x0A
#define TK_CTRL_K 	0x0B
#define TK_CTRL_L 	0x0C
#define TK_CTRL_M 	0x0D // clash with 'ENTER'
#define TK_CTRL_N 	0x0E
#define TK_CTRL_O 	0x0F
#define TK_CTRL_P 	0x10
#define TK_CTRL_Q 	0x11
#define TK_CTRL_R 	0x12
#define TK_CTRL_S 	0x13
#define TK_CTRL_T 	0x14
#define TK_CTRL_U 	0x15
#define TK_CTRL_V 	0x16
#define TK_CTRL_W 	0x17
#define TK_CTRL_X 	0x18
#define TK_CTRL_Y 	0x19
#define TK_CTRL_Z 	0x1A
#define TK_ESC		0x1B // NOTE: If a key is pressed with alt it sends out
			     // two chars - first esc and then the key pressed
			     // with it
#define TK_CTRL_LSQBKT 	0x1B // Left square bracket - clash with 'ESC'
#define TK_CTRL_3 	0x1B // clash with 'ESC'
#define TK_CTRL_4 	0x1C
#define TK_CTRL_BKSLSH 	0x1C // clash with 'CTRL_4'
#define TK_CTRL_5 	0x1D
#define TK_CTRL_RSQBKT 	0x1D // Right square bracket - clash with 'CTRL_5'
#define TK_CTRL_6 	0x1E
#define TK_CTRL_7 	0x1F
#define TK_CTRL_SLASH 	0x1F // clash with 'CTRL_7'
#define TK_CTRL_UNDRSCR	0x1F // clash with 'CTRL_7'
#define TK_BKSPC2 	0x7F
#define TK_CTRL_8 	0x7F //clash with 'DELETE'

#endif
