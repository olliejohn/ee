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

#include <pty.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <utmp.h>

#include "vterm.h"
#include "vterm_private.h"
#include "vterm_write.h"

vterm_t *vterm_create(int width, int height, int flags)
{
	vterm_t        *vterm;
	struct passwd  *user_profile;
	char           *user_shell = NULL;
	pid_t          child_pid;
	int            master_fd;
	struct winsize ws = { .ws_xpixel = 0, .ws_ypixel = 0 };
	int            i;

	if (height <= 0 || width <= 0)
		return NULL;

	vterm = calloc(1, sizeof(vterm_t));

	/* Record dimensions */
	vterm->rows=height;
	vterm->cols=width;

	/* Create the cell matrix */
	vterm->cells = calloc(height, sizeof(vterm_cell_t *));

	for(i = 0; i < height; i++)
		vterm->cells[i] = calloc(width, sizeof(vterm_cell_t));

	/* Initialize all cells with defaults */
	vterm_erase(vterm);

	/* Initialization of other public fields */
	vterm->crow = 0;
	vterm->ccol = 0;

	/* Default active colors */
	vterm->colors = 0;
	vterm->curattr = COLOR_PAIR(vterm->colors);

	/* Initial scrolling area is the whole window */
	vterm->scroll_min = 0;
	vterm->scroll_max = height - 1;

	vterm->flags = flags;

	memset(&ws, 0, sizeof(ws));
	ws.ws_row = height;
	ws.ws_col = width;

	child_pid = forkpty(&master_fd, NULL, NULL, &ws);
	vterm->pty_fd = master_fd;

	if (child_pid < 0) {
		vterm_destroy(vterm);
		return NULL;
	}

	if (child_pid == 0) {
		signal(SIGINT, SIG_DFL);

		if (flags & VTERM_FLAG_VT100)
			setenv("TERM", "vt100", 1);
		else
			setenv("TERM", "rxvt", 1);

		user_profile=getpwuid(getuid());

		if (user_profile == NULL)
			user_shell = "/bin/sh";
		else if (user_profile->pw_shell == NULL)
			user_shell = "/bin/sh";
		else
			user_shell = user_profile->pw_shell;

		if (user_shell == NULL)
			user_shell = "/bin/sh";

		/* Start the shell */
		if (execl(user_shell, user_shell, "-l", NULL) == -1)
			exit(EXIT_FAILURE);

		exit(EXIT_SUCCESS);
	}

	vterm->child_pid = child_pid;

	if (ttyname_r(master_fd, vterm->ttyname, sizeof(vterm->ttyname)-1) != 0)
		snprintf(vterm->ttyname, sizeof(vterm->ttyname) - 1, "vterm");

	vterm->write = (flags & VTERM_FLAG_VT100)
			? vterm_write_vt100
			: vterm_write_rxvt;

	return vterm;
}

void vterm_destroy(vterm_t *vterm)
{
	int i;

	if (vterm == NULL)
		return;

	for (i = 0; i < vterm->rows; i++)
		free(vterm->cells[i]);
	free(vterm->cells);
	free(vterm);
}

pid_t vterm_get_pid(vterm_t *vterm)
{
	if (vterm == NULL)
		return -1;
	return vterm->child_pid;
}

int vterm_get_pty_fd(vterm_t *vterm)
{
	if (vterm == NULL)
		return -1;
	return vterm->pty_fd;
}

const char *vterm_get_ttyname(vterm_t *vterm)
{
	if (vterm == NULL)
		return NULL;
	return (const char *) vterm->ttyname;
}
