/*
 * lisp.c
 * Part of the Yaw text editor
 *
 * Originally forked from Andru Luvisi's 'Minimal Lisp Interpreter', the
 * copyright notive for which is shown below.
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

#include "lisp.h"

#include "buffer.h"

struct Buffer *lisp_tokenize(struct Line *ln)
{
	for (ln->pos = 0; ln->pos <= ln->size; ln->pos++) {
		if (ln->data[ln->pos] == L'(' || ln->data[ln->pos] == L')') {
			line_add(ln, L' ');
			ln->pos++;
			line_add(ln, L' ');
		}
	}

	struct Buffer *tkns = buffer_new();
	t_char *data;
	t_char *state;
	int i;

	data = wcstok(ln->data, L" ", &state);
	while (data != NULL) {
		for (i = 0; i < wcslen(data); i++)
			buffer_add(tkns, data[i]);

		buffer_new_line(tkns);

		data = wcstok(NULL, L" ", &state);
	}

	return tkns;
}
