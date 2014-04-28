/*
 * Part of the Yaw text editor virtual terminal emulator
 *
 * Based on the no-glib fork of libvte by Ollie Etherington (C) Copyright 2014
 * libvterm Copyright (C) 2009 Bryan Christ
 * libvterm is based on ROTE written by Bruno Takahashi C. de Oliveira
 *
 * As per previous releases, this program is available under the GNU GPL v2
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <curses.h>
#include <string.h>
#include <unistd.h>

#include "vterm.h"
#include "vterm_private.h"
#include "vterm_write.h"

void vterm_write_pipe(vterm_t *vterm, unsigned int keycode)
{
	if (vterm == NULL)
		return;
	vterm->write(vterm, keycode);
}

void vterm_write_rxvt(vterm_t *vterm, unsigned int keycode)
{
	char *buffer = NULL;

	switch (keycode) {
		case '\n':           buffer = "\r";      break;
		case KEY_UP:         buffer = "\e[A";    break;
		case KEY_DOWN:       buffer = "\e[B";    break;
		case KEY_RIGHT:      buffer = "\e[C";    break;
		case KEY_LEFT:       buffer = "\e[D";    break;
		case KEY_BACKSPACE:  buffer = "\b";      break;
		case KEY_IC:         buffer = "\e[2~";   break;
		case KEY_DC:         buffer = "\e[3~";   break;
		case KEY_HOME:       buffer = "\e[7~";   break;
		case KEY_END:        buffer = "\e[8~";   break;
		case KEY_PPAGE:      buffer = "\e[5~";   break;
		case KEY_NPAGE:      buffer = "\e[6~";   break;
		case KEY_SUSPEND:    buffer = "\x1A";    break;  // ctrl-z
		case KEY_F(1):       buffer = "\e[11~";  break;
		case KEY_F(2):       buffer = "\e[12~";  break;
		case KEY_F(3):       buffer = "\e[13~";  break;
		case KEY_F(4):       buffer = "\e[14~";  break;
		case KEY_F(5):       buffer = "\e[15~";  break;
		case KEY_F(6):       buffer = "\e[17~";  break;
		case KEY_F(7):       buffer = "\e[18~";  break;
		case KEY_F(8):       buffer = "\e[19~";  break;
		case KEY_F(9):       buffer = "\e[20~";  break;
		case KEY_F(10):      buffer = "\e[21~";  break;
		case KEY_F(11):      buffer = "\e[23~";  break;
		case KEY_F(12):      buffer = "\e[24~";  break;
	}

	if (buffer == NULL)
		write(vterm->pty_fd, &keycode, sizeof(char));
	else
		write(vterm->pty_fd, buffer, strlen(buffer));
}

void vterm_write_vt100(vterm_t *vterm, unsigned int keycode)
{
	char *buffer = NULL;

	switch (keycode) {
		case '\n':           buffer = "\r";      break;
		case KEY_UP:         buffer = "\e[A";    break;
		case KEY_DOWN:       buffer = "\e[B";    break;
		case KEY_RIGHT:      buffer = "\e[C";    break;
		case KEY_LEFT:       buffer = "\e[D";    break;
		case KEY_BACKSPACE:  buffer = "\b";      break;
		case KEY_IC:         buffer = "\e[2~";   break;
		case KEY_DC:         buffer = "\e[3~";   break;
		case KEY_HOME:       buffer = "\e[7~";   break;
		case KEY_END:        buffer = "\e[8~";   break;
		case KEY_PPAGE:      buffer = "\e[5~";   break;
		case KEY_NPAGE:      buffer = "\e[6~";   break;
		case KEY_SUSPEND:    buffer = "\x1A";    break;  // ctrl-z
		case KEY_F(1):       buffer = "\e[[A";   break;
		case KEY_F(2):       buffer = "\e[[B";   break;
		case KEY_F(3):       buffer = "\e[[C";   break;
		case KEY_F(4):       buffer = "\e[[D";   break;
		case KEY_F(5):       buffer = "\e[[E";   break;
		case KEY_F(6):       buffer = "\e[17~";  break;
		case KEY_F(7):       buffer = "\e[18~";  break;
		case KEY_F(8):       buffer = "\e[19~";  break;
		case KEY_F(9):       buffer = "\e[20~";  break;
		case KEY_F(10):      buffer = "\e[21~";  break;
	}

	if (buffer == NULL)
		write(vterm->pty_fd, &keycode, sizeof(char));
	else
		write(vterm->pty_fd, buffer, strlen(buffer));
}
