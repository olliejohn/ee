/*
 * binds.c
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

#include "binds.h"

#include <stdlib.h>
#include <string.h>

#define MAX_BINDS 128

struct Screen *curscrn;
struct Bind *keytab[MAX_BINDS];
int NUM_BINDS;

struct Bind *bind_new(int key, cb_ptr callback)
{
	struct Bind *b = malloc(sizeof(struct Bind));
	b->key = key;
	b->callback = callback;
	return b;
}

void bind_free(struct Bind *b)
{
	free(b);
}

void binds_init()
{
	NUM_BINDS = 0;
}

void binds_destroy()
{
	int i;
	for (i = 0; i < NUM_BINDS; i++)
		bind_free(keytab[i]);
}

void binds_set_current_screen(struct Screen *scrn)
{
	curscrn = scrn;
}

int binds_get_index_of(int key)
{
	int i;
	for (i = 0; i < NUM_BINDS; i++)
		if (keytab[i]->key == key)
			return i;

	return -1;
}

cb_ptr binds_get_callback_for(int key)
{
	int index = binds_get_index_of(key);

	if (index == -1)
		return NULL;
	else
		return keytab[index]->callback;
}

int bind(int key, cb_ptr callback)
{
	if (NUM_BINDS >= MAX_BINDS)
		return -1;

	int index = binds_get_index_of(key);

	if (index > -1)
		keytab[index]->callback = callback;
	else
		keytab[NUM_BINDS++] = bind_new(key, callback);

	return 0;
}

void unbind(int key)
{
	int index = binds_get_index_of(key);

	if (index == -1)
		return;

	bind_free(keytab[index]);

	memmove(keytab[index - 1],
		keytab[index],
		sizeof(struct Bind) * (NUM_BINDS - index));
}
