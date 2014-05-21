/*
 * buffer.c
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

#include "buffer.h"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#define LINE_INITAL_CAPACITY 64
#define BUFFER_INITAL_CAPACITY 32

/* Create a new struct Line */
struct Line *line_new()
{
	struct Line *line = malloc(sizeof(struct Line));
	line->size = 0;
	line->pos = 0;
	line->capacity = LINE_INITAL_CAPACITY;
	line->data = calloc(LINE_INITAL_CAPACITY, sizeof(t_char));
	return line;
}

/* Destroy a line and it's data */
void line_free(struct Line *line)
{
	free(line->data);
	free(line);
}

/* Get the cursor position in the line accounting for tabs */
unsigned int line_get_curs_pos(struct Line *line)
{
	unsigned int i, curspos = 0;

	for (i = 0; i < line->pos; i++, curspos++)
		if (line->data[i] == TK_TAB)
			while (++curspos % CFG->tabsize != CFG->tabsize - 1);

	return curspos;
}

/*
 * Local function to check if a line is at it's maximum capacity and reallocate
 * if neccesary
 */
static void line_double_capacity_if_full(struct Line *line)
{
	if (line->size < line->capacity - 1)
		return;

	unsigned int oldcap = line->capacity;
	line->capacity <<= 1;
	line->data = realloc(line->data, sizeof(t_char) * line->capacity);
	memset(line->data + oldcap, 0,
	       sizeof(t_char) * (line->capacity - oldcap));
}

/*
 * Increment the cursor position. Returns 0 on success or -1 if we're at the
 * end of the line
 */
int line_move_forward(struct Line *line)
{
	if (++line->pos > line->size) {
		line->pos = line->size;
		return -1;
	}

	return 0;
}

/*
 * Decrement the cursor position. Returns 0 on success or -1 if we're already at
 * the beginning of the line
 */
int line_move_backward(struct Line *line)
{
	if (line->pos == 0)
		return -1;

	line->pos--;

	return 0;
}

/*
 * Jump to the previous word in the line - this implementation is not yet
 * functional
 */
int line_prev_word(struct Line *line)
{
	for ( ; iswalnum(line->data[line->pos]); line->pos--)
		if (line->pos < 0)
			return -1;

	return 0;
}

/*
 * Jump to the next word in the line - this implementation is not yet
 * functional
 */
int line_next_word(struct Line *line)
{
	for ( ; iswalnum(line->data[line->pos]); line->pos++)
		if (line->pos >= line->size - 1)
			return -1;

	return 0;
}

/* Move the cursor to position 0 in the line */
void line_home(struct Line *line)
{
	line->pos = 0;
}

/* Move the cursor to the end of the line */
void line_end(struct Line *line)
{
	line->pos = line->size;
}

/* Set the char at INDEX in LINE to C */
void line_set(struct Line *line, int index, t_char c)
{
	if (index < line->size)
		line->data[index] = c;
}

/* add char C to LINE at the current position */
void line_add(struct Line *line, t_char c)
{
	line_double_capacity_if_full(line);

	if (line->pos < 0 || line->pos > line->size)
		return;

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

/*
 * Perform a backspace at the current line position. Returns 0 on success, -1
 * if we're at the beginning of a line and, depending on the situation, may need
 * to clobber the previous line in a buffer or perform some other action, or 1
 * if the line position is greater that the line size - this is an error on
 * behalf of the programmer as the position should never be greater than the
 * size and should never occur.
 */
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

/* Create a new buffer */
struct Buffer *buffer_new()
{
	struct Buffer *buf = malloc(sizeof(struct Buffer));
	buf->size = 0;
	buf->pos = 0;
	buf->pref_line_pos = 0;
	buf->capacity = BUFFER_INITAL_CAPACITY;
	buf->data = malloc(sizeof(struct Line *) * BUFFER_INITAL_CAPACITY);
	buf->data[0] = line_new();
	buf->filename = NULL;
	buffer_set_filename(buf, DEFAULT_BUFFER_FILENAME);
	buf->dirty = BUF_CLEAN;
	return buf;
}

/* Destroy a buffer and all of it's data */
void buffer_free(struct Buffer *buf)
{
	int i;
	for (i = 0; i <= buf->size; i++)
		line_free(buf->data[i]);
	free(buf->data);
	free(buf->filename);
	free(buf);
}

/*
 * TODO: These filename functions will crash the program when the buffer is
 * destroyed if the reallocs fail - they could be made safer
 */
/* Set the buffer filename from a (char *) */
void buffer_set_filename(struct Buffer *buf, char *filename)
{
	buf->filename = realloc(buf->filename,
				sizeof(char) * (strlen(filename) + 1));

	strcpy(buf->filename, filename);
}

/* Set the buffer filename from a (t_char *) or (wchar_t *) */
void buffer_set_filename_from_wide(struct Buffer *buf, t_char *filename)
{
	int len = wcslen(filename) + 1;
	buf->filename = realloc(buf->filename, sizeof(char) * len);
	wcstombs(buf->filename, filename, len);
}

/* Go to position (x, y) in the buffer */
void buffer_go_to(struct Buffer *buf, int x, int y)
{
	buf->pos = y;
	buf->data[buf->pos]->pos = x;
}

/* Return the text from line number LINE or NULL if line LINE doesn't exist */
#define LN_MAX ((buf->size < 2) ? 1 : buf->size)
t_char *buffer_get_text_at(struct Buffer *buf, int line)
{
	return (line <= LN_MAX) ? buf->data[line]->data : NULL;
}

/*
 * Local function to check if a buffer is at it's maximum capacity and
 * reallocate if neccesary
 */
static void buffer_double_capacity_if_full(struct Buffer *buf)
{
	if (buf->size < buf->capacity)
		return;

	if (CFG->bufexpg == 1)
		buf->capacity <<= 1;
	else
		buf->capacity += BUFFER_INITAL_CAPACITY;

	buf->data = realloc(buf->data, sizeof(struct Line) * buf->capacity);
}

/* Add character C to the buffer at the current position */
void buffer_add(struct Buffer *buf, t_char c)
{
	buffer_double_capacity_if_full(buf);

	if (buf->data[buf->pos] == NULL) {
		buf->data[buf->pos] = line_new();
		buf->size++;
	}

	line_add(buf->data[buf->pos], c);

	buf->dirty = BUF_DIRTY;
}

/* Add a new line at the current position in the buffer */
/*
 * TODO: There's a massive memory error in the memmove call
 * This error can only be seen in debug builds as it's hidden by mallopt in
 * production builds
 */
void buffer_new_line(struct Buffer *buf)
{
	/* Make room for new line */
	buf->size++;
	buffer_double_capacity_if_full(buf);

	/* If we're in the middle of the buffer */
	if (buf->pos < buf->size - 1)
		memmove(&buf->data[buf->pos + 2],
			&buf->data[buf->pos + 1],
			sizeof(struct Line) * (buf->size - 1 - buf->pos));

	/* Make the new line */
	buf->data[buf->pos + 1] = line_new();

	/* If we're half way up a line, move what's left onto the new line */
#define cur_line buf->data[buf->pos]
#define new_line buf->data[buf->pos + 1]
	unsigned int i, diff = 0;
	for (i = cur_line->pos; i < cur_line->size; i++) {
		line_add(new_line, cur_line->data[i]);
		cur_line->data[i] = 0;
		diff++;
	}
	cur_line->size -= diff;
#undef cur_line
#undef new_line

	/* Move to the new line at pos 0 and note that the buffer's changed */
	buf->pos++;
	buf->data[buf->pos]->pos = 0;
	buf->dirty = BUF_DIRTY;
}

/*
 * Perform a backspace at the current buffer position. Returns 1 if the number
 * of lines was altered or 0 if all changes were internal to a line.
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

		line_free(buf->data[buf->pos]);

		memmove(&buf->data[buf->pos],
			&buf->data[buf->pos + 1],
			sizeof(struct Line *) * (buf->size - buf->pos));

		buf->size--;
		buf->pos--;

		buf->dirty = BUF_DIRTY;

		return 1;
	}

	buf->dirty = BUF_DIRTY;

	return 0;
}

/* Increment the cursor position */
void buffer_move_forward(struct Buffer *buf)
{
	struct Line *line = buf->data[buf->pos];

	if (line_move_forward(line) == -1 && buf->pos < buf->size)
		buf->data[++buf->pos]->pos = 0;
}

/* Decrement the cursor position */
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

/*
 * Move up a line in the buffer. Returns 0 on success or -1 if we're already
 * at the top of the buffer
 */
int buffer_move_up(struct Buffer *buf)
{
	if (buf->pos <= 0)
		return -1;

	int old_line_pos = buf->data[buf->pos]->pos;
	buf->pos--;

	struct Line *new_line = buf->data[buf->pos];
	if (old_line_pos <= new_line->size)
		new_line->pos = old_line_pos;
	else
		new_line->pos = (new_line->size == 0) ? 0 : new_line->size - 1;

	return 0;
}

/*
 * Move down a line in the buffer. Returns 0 on success or -1 if we're already
 * at the end of the buffer
 */
int buffer_move_down(struct Buffer *buf)
{
	if (buf->pos >= buf->size)
		return -1;

	int line_pos = buf->data[buf->pos]->pos;
	buf->pos++;

	struct Line *line = buf->data[buf->pos];
	if (line_pos < line->size)
		line->pos = line_pos;
	else
		line->pos = line->size;

	return 0;
}

/*
 * Jump to the previous word in the buffer - this implementation is not yet
 * functional
 */
void buffer_prev_word(struct Buffer *buf)
{
	while (line_prev_word(buf->data[buf->pos]) == -1) {
		buf->pos--;
		buf->data[buf->pos]->pos = buf->data[buf->pos]->size;
	}
}

/*
 * Jump to the next word in the buffer - this implementation is not yet
 * functional
 */
void buffer_next_word(struct Buffer *buf)
{
	while (line_next_word(buf->data[buf->pos]) == -1)
		buf->data[++buf->pos]->pos = 0;
}

/* Move the cursor to the beginning of the current line */
void buffer_home(struct Buffer *buf)
{
	line_home(buf->data[buf->pos]);
}

/* Move the cursor to the end of the current line */
void buffer_end(struct Buffer *buf)
{
	line_end(buf->data[buf->pos]);
}

/* Move the page up - this implementation is not yet functional */
void buffer_pgup(struct Buffer *buf, int pg_size)
{
	register int new = buf->pos - pg_size;
	buf->pos = (new >= 0) ? new : 0;
}

/* Move the page down - this implementation is not yet functional */
void buffer_pgdn(struct Buffer *buf, int pg_size)
{
	register int new = buf->pos + pg_size;
	buf->pos = (new < buf->size) ? new : buf->size;
}

/* Save the current buffer as FILE */
int buffer_save_as(struct Buffer *buf, char *file)
{
	FILE *f = fopen(file, "w");

	if (f == NULL)
		return -1;

	int i;
	for (i = 0; i < buf->size; i++)
		fwprintf(f, L"%ls\n", buf->data[i]->data);

	fwprintf(f, L"%ls", buf->data[buf->size]->data);

	fclose(f);

	buf->dirty = BUF_CLEAN;

	return 0;
}

/*
 * Open a file into buf and place that cursor at (x, y). If the buffer is
 * already in use, the file will be appended so ensure that it's a new buffer
 * for normal use. Returns 0 on success or -1 on failiure.
 */
int buffer_open_at(struct Buffer *buf, char *file, int x, int y)
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

	buffer_set_filename(buf, file);

	buf->dirty = BUF_CLEAN;

	return 0;
}
