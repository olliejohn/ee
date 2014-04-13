/*
 * buffer.h
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

#ifndef BUFFER_H
#define BUFFER_H

#include "config.h"

struct Line {
	int size;
	int pos;
	int capacity;
	char *data;
};

struct Buffer {
	int size;
	int pos;
	int pref_line_pos;
	int capacity;
	struct Line **data;
};

struct Line *line_new();
void line_free(struct Line *line);
void line_append(struct Line *line, char c);
int line_move_forward(struct Line *line);
int line_move_backward(struct Line *line);
void line_set(struct Line *line, int index, char c);
void line_add(struct Line *line, char c);
int line_backspace(struct Line *line);

struct Buffer *buffer_new();
void buffer_free(struct Buffer *buf);
void buffer_add(struct Buffer *buf, char c);
void buffer_new_line(struct Buffer *buf);
int buffer_backspace(struct Buffer *buf);
int buffer_delete(struct Buffer *buf);
void buffer_move_forward(struct Buffer *buf);
void buffer_move_backward(struct Buffer *buf);
void buffer_move_up(struct Buffer *buf);
void buffer_move_down(struct Buffer *buf);

#endif
