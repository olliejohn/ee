/*
 * tokenizer.c
 * Part of the Lisp subsystem in the Yaw text editor
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

#include "tokenizer.h"

#include <stdlib.h>

#define TOKEN_STREAM_INITIAL_SIZE 16

struct TokenStream {
	wchar_t **tkns;
	unsigned int size;
	unsigned int capacity;
};

struct TokenStream *token_stream_new()
{
	struct TokenStream *ts = malloc(sizeof(struct TokenStream));
	ts->capacity = TOKEN_STREAM_INITIAL_SIZE;
	ts->tkns = malloc(ts->capacity * sizeof(wchar_t *));
	ts->size = 0;
	return ts;
}

void token_stream_free(struct TokenStream *ts)
{
	unsigned int i;
	for (i = 0; i < ts->size; i++)
		free(ts->tkns[i]);

	free(ts->tkns);
	free(ts);
}

void token_stream_add(struct TokenStream *ts, wchar_t *data)
{
	if (data == NULL)
		return;

	if (ts->size >= ts->capacity - 1) {
		ts->capacity <<= 1;
		ts->tkns = realloc(ts->tkns, ts->capacity * sizeof(wchar_t *));
	}

	ts->tkns[ts->size] = calloc(wcslen(data) + 1, sizeof(wchar_t));
	wcscpy(ts->tkns[ts->size], data);
	ts->size++;
}

int token_stream_get_size(struct TokenStream *ts)
{
	return ts->size;
}

wchar_t *token_stream_get(struct TokenStream *ts, int i)
{
	return (i < ts->size) ? ts->tkns[i] : NULL;
}

wchar_t *token_delimit(wchar_t *data)
{
	size_t dlen = wcslen(data);
	size_t alloced_len = dlen + 1;
	wchar_t *alloced_data = calloc(alloced_len, sizeof(wchar_t));

	unsigned int i, insert_offs;
	for (i = 0, insert_offs = 0; i < dlen; i++) {
		if (data[i] == L'(' || data[i] == L')') {
			alloced_len += 2;
			alloced_data = realloc(alloced_data,
					       alloced_len * sizeof(wchar_t));
			alloced_data[alloced_len - 1] = 0;
			alloced_data[alloced_len - 2] = 0;
			alloced_data[i + insert_offs + 0] = L' ';
			alloced_data[i + insert_offs + 1] = data[i];
			alloced_data[i + insert_offs + 2] = L' ';
			insert_offs += 2;
		} else {
			alloced_data[i + insert_offs] = data[i];
		}
	}

	return alloced_data;
}

struct TokenStream *token_split(wchar_t *data)
{
	if (data == NULL)
		return NULL;

	struct TokenStream *ts = token_stream_new();
	wchar_t *split = NULL, *state;

	split = wcstok(data, L" ", &state);
	while (split != NULL) {
		token_stream_add(ts, split);
		split = wcstok(NULL, L" ", &state);
	}

	return ts;
}

struct TokenStream *tokenize(wchar_t *data)
{
	wchar_t *delimited = token_delimit(data);
	struct TokenStream *ts = token_split(delimited);
	free(delimited);
	return ts;
}
