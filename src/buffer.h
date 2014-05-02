/*
 * buffer.h
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

#ifndef BUFFER_H
#define BUFFER_H

#include "config.h"
#include "tui/tui.h"

#define DEFAULT_BUFFER_FILENAME "Untitled"

struct Line {
	int size;
	int pos;
	int capacity;
	t_char *data;
};

struct Buffer {
	int size;
	int pos;
	int pref_line_pos;
	int capacity;
	struct Line **data;
	char *filename;
};

struct Line *line_new();
void line_free(struct Line *line);
int line_get_curs_pos(struct Line *line);
int line_move_forward(struct Line *line);
int line_move_backward(struct Line *line);
int line_prev_word(struct Line *line);
int line_next_word(struct Line *line);
void line_home(struct Line *line);
void line_end(struct Line *line);
void line_set(struct Line *line, int index, t_char c);
void line_add(struct Line *line, t_char c);
int line_backspace(struct Line *line);

struct Buffer *buffer_new();
void buffer_free(struct Buffer *buf);
void buffer_set_filename(struct Buffer *buf, char *filename);
void buffer_set_filename_from_wide(struct Buffer *buf, t_char *filename);
void buffer_go_to(struct Buffer *buf, int x, int y);
void buffer_add(struct Buffer *buf, t_char c);
void buffer_new_line(struct Buffer *buf);
int buffer_backspace(struct Buffer *buf);
void buffer_move_forward(struct Buffer *buf);
void buffer_move_backward(struct Buffer *buf);
int buffer_move_up(struct Buffer *buf);
int buffer_move_down(struct Buffer *buf);
void buffer_prev_word(struct Buffer *buf);
void buffer_next_word(struct Buffer *buf);
void buffer_home(struct Buffer *buf);
void buffer_end(struct Buffer *buf);
void buffer_pgup(struct Buffer *buf, int pg_size);
void buffer_pgdn(struct Buffer *buf, int pg_size);
int buffer_save(struct Buffer *buf);
int buffer_save_as(struct Buffer *buf, char *file);
int buffer_open_at(struct Buffer *buf, char *file, int x, int y);

inline int buffer_open(struct Buffer *buf, char *file)
{
	return buffer_open_at(buf, file, 0, 0);
}

#endif
