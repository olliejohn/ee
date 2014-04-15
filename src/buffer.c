/*
 * buffer.c
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

 /*
  * Found another bug: Sometimes longer lines appear to end 6 chars
  * early, but only when the file is loaded - not when it's typed
  */

#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define LINE_INITAL_CAPACITY 128
#define BUFFER_INITAL_CAPACITY 32

#define TAB_SIZE 8

struct Line *line_new()
{
	struct Line *line = malloc(sizeof(struct Line));
	line->size = 0;
	line->pos = 0;
	line->coffs = 0;
	line->capacity = LINE_INITAL_CAPACITY;
	line->data = calloc(LINE_INITAL_CAPACITY, sizeof(t_char));
	return line;
}

void line_free(struct Line *line)
{
	free(line->data);
	free(line);
}

int line_get_curs_pos(struct Line *line)
{
	return line->pos + line->coffs;
}

void line_double_capacity_if_full(struct Line *line)
{
	if (line->size < line->capacity)
		return;

	line->capacity <<= 1;
	line->data = realloc(line->data, sizeof(t_char) * line->capacity);
}

void line_append(struct Line *line, t_char c)
{
	line_double_capacity_if_full(line);
	line->data[line->size++] = c;
}

int line_move_forward(struct Line *line)
{
	if (++line->pos > line->size) {
		line->pos = line->size;
		return -1;
	}

	if (line->data[line->pos] == '\t')
		line->coffs += TAB_SIZE;

	return 0;
}

int line_move_backward(struct Line *line)
{
	if (line->pos == 0)
		return -1;

	line->pos--;

	if (line->data[line->pos] == '\t')
		line->coffs -= TAB_SIZE;

	return 0;
}

void line_set(struct Line *line, int index, t_char c)
{
	if (index < line->size)
		line->data[index] = c;
}

void line_add(struct Line *line, t_char c)
{
	if (line->pos < 0 || line->pos > line->size)
		return;

	line_double_capacity_if_full(line);

	if (line->pos == line->size) {
		line->size++;
		line->data[line->pos++] = c;
	} else {
		line->size++;

		wmemmove(&line->data[line->pos + 1],
		         &line->data[line->pos],
		         line->size - 1 - line->pos);

		line->data[line->pos++] = c;
	}
}

int line_backspace(struct Line *line)
{
	if (line->pos <= 0)
		return -1;

	if (line->pos > line->size)
		return 1;

	wmemmove(&line->data[line->pos - 1],
	         &line->data[line->pos],
	         sizeof(t_char) * (line->size - line->pos));
	line->data[line->size - 1] = 0;
	line->size--;
	line->pos--;
	return 0;
}

struct Buffer *buffer_new()
{
	struct Buffer *buf = malloc(sizeof(struct Buffer));
	buf->size = 0;
	buf->pos = 0;
	buf->pref_line_pos = 0;
	buf->capacity = BUFFER_INITAL_CAPACITY;
	buf->data = malloc(sizeof(struct Line *) * BUFFER_INITAL_CAPACITY);
	buf->data[0] = line_new();
	return buf;
}

void buffer_free(struct Buffer *buf)
{
	/*
	 * Check this: changed < to <= in the for loop. This fixed a small
	 * memory leak but may cause a seg fault sometimes?
	 */
	int i;
	for (i = 0; i <= buf->size; i++)
		line_free(buf->data[i]);
	free(buf->data);
	free(buf);
}

void buffer_go_to(struct Buffer *buf, int x, int y)
{
	buf->pos = y;
	buf->data[buf->pos]->pos = x;
}

void buffer_double_capacity_if_full(struct Buffer *buf)
{
	if (buf->size < buf->capacity)
		return;

	if (CFG->bufexpg == 1)
		buf->capacity <<= 1;
	else
		buf->capacity += BUFFER_INITAL_CAPACITY;

	buf->data = realloc(buf->data, sizeof(struct Line) * buf->capacity);
}

void buffer_add(struct Buffer *buf, t_char c)
{
	buffer_double_capacity_if_full(buf);

	if (buf->data[buf->pos] == NULL) {
		buf->data[buf->pos] = line_new();
		buf->size++;
	}

	line_add(buf->data[buf->pos], c);
}

void buffer_new_line(struct Buffer *buf)
{
	buffer_double_capacity_if_full(buf);
	buf->size++;

	if (buf->pos < buf->size - 1) {
		memmove(&buf->data[buf->pos + 2],
			&buf->data[buf->pos + 1],
			sizeof(struct Line) * (buf->size - 1 - buf->pos));

		buf->data[buf->pos + 1] = line_new();

		struct Line *cur_line = buf->data[buf->pos];
		if (cur_line->pos <= cur_line->size - 1) {
			struct Line *new_line = buf->data[buf->pos + 1];
			int i, diff = 0;
			for (i = cur_line->pos; i < cur_line->size; i++) {
				line_add(new_line, cur_line->data[i]);
				cur_line->data[i] = 0;
				diff++;
			}
			cur_line->size -= diff;
		}

		buf->pos++;
		buf->data[buf->pos]->pos = 0;
	} else {
		buf->data[++buf->pos] = line_new();
	}
}

/*
 * Returns 1 if the number of lines was altered or 0 if all changes were
 * internal to a line
 */
int buffer_backspace(struct Buffer *buf)
{
	if (line_backspace(buf->data[buf->pos]) == -1) {
		if (buf->pos == 0)
			return 0;

		buf->data[buf->pos - 1]->pos = buf->data[buf->pos - 1]->size;

		int i;
		for (i = 0; i < buf->data[buf->pos]->size; i++)
			line_add(buf->data[buf->pos - 1],
				 buf->data[buf->pos]->data[i]);

		buf->data[buf->pos - 1]->pos = buf->data[buf->pos - 1]->size -
						buf->data[buf->pos]->size;

		memmove(&buf->data[buf->pos],
			&buf->data[buf->pos + 1],
			sizeof(struct Line *) * (buf->size - buf->pos));

		buf->size--;
		buf->pos--;

		return 1;
	}

	return 0;
}

int buffer_delete(struct Buffer *buf)
{
	struct Line *line = buf->data[buf->pos];
	if (buf->pos >= buf->size - 1 && line->pos >= line->size)
		return 0;

	buffer_move_forward(buf);
	return buffer_backspace(buf);
}

void buffer_move_forward(struct Buffer *buf)
{
	struct Line *line = buf->data[buf->pos];

	if (line_move_forward(line) == -1 && buf->pos < buf->size - 1) {
		if (buf->pos == buf->size - 1)
			return;

		buf->data[++buf->pos]->pos = 0;
	}
}

void buffer_move_backward(struct Buffer *buf)
{
	struct Line *line = buf->data[buf->pos];

	if (line_move_backward(line) == -1) {
		if (buf->pos == 0)
			return;

		line = buf->data[--buf->pos];
		line->pos = line->size;
	}
}

void buffer_move_up(struct Buffer *buf)
{
	if (buf->pos <= 0)
		return;

	int old_line_pos = buf->data[buf->pos]->pos;
	buf->pos--;

	struct Line *new_line = buf->data[buf->pos];
	if (old_line_pos <= new_line->size)
		new_line->pos = old_line_pos;
	else
		new_line->pos = (new_line->size == 0) ? 0 : new_line->size - 1;
}

void buffer_move_down(struct Buffer *buf)
{
	if (buf->pos >= buf->size)
		return;

	int line_pos = buf->data[buf->pos]->pos;
	buf->pos++;

	struct Line *line = buf->data[buf->pos];
	if (line_pos < line->size)
		line->pos = line_pos;
	else
		line->pos = line->size;
}

int buffer_save(struct Buffer *buf, char *file)
{
	FILE *f = fopen(file, "w");

	if (f == NULL)
		return -1;

	int i;
	for (i = 0; i < buf->size; i++)
		;//fprintf(f, "%s\n", buf->data[i]->data);

	fclose(f);

	return 0;
}

/*
 * Opens a file into buf and places that cursor at (x, y). If the buffer is
 * already in use, the file will be appended so ensure that it's a new buffer
 * for normal use. Returns 0 on success or -1 on failiure.
 */
int buffer_open(struct Buffer *buf, char *file, int x, int y)
{
	FILE *f = fopen(file, "r");

	if (f == NULL)
		return -1;

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
