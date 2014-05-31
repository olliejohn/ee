/*
 * builtin.c
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

#include "builtin.h"

#include "comms.h"
#include "heap_tracker.h"
#include "lisp.h"
#include "stack.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_DIGITS_IN_SLONG 20 /* Max digits in a signed long plus 1 padding */

inline void push_nil()
{
	push(L"nil");
}

inline void push_t()
{
	push(L"t");
}

void push_from_long(long value)
{
	wchar_t *ret = calloc(MAX_DIGITS_IN_SLONG, sizeof(wchar_t));
	heap_tracker_add(HT, ret);
	swprintf(ret, MAX_DIGITS_IN_SLONG, L"%ld", value);
	push(ret);
}

inline long pop_to_long()
{
	return wcstol(pop(), NULL, 10);
}

void builtin_add(struct Context *ctx, unsigned int stack_track)
{
	long result;
	for (result = 0; stack_track; stack_track--)
		result += pop_to_long();

	push_from_long(result);
}

void builtin_minus(struct Context *ctx, unsigned int stack_track)
{
	if (stack_track == 0) {
		push_from_long(0L);
		return;
	}

	if (stack_track == 1) {
		push_from_long(-(pop_to_long()));
		return;
	}

	long result = pop_to_long();
	for (stack_track--; stack_track; stack_track--)
		result -= pop_to_long();

	push_from_long(result);
}

void builtin_mult(struct Context *ctx, unsigned int stack_track)
{
	if (stack_track == 0) {
		push_from_long(0L);
		return;
	}

	if (stack_track == 1) {
		push_from_long(pop_to_long());
		return;
	}

	long result = pop_to_long();
	for (stack_track--; stack_track; stack_track--)
		result *= pop_to_long();

	push_from_long(result);
}

void builtin_div(struct Context *ctx, unsigned int stack_track)
{
	if (stack_track == 0) {
		push_from_long(0L);
		return;
	}

	if (stack_track == 1) {
		push_from_long(pop_to_long());
		return;
	}

	long result = pop_to_long();
	for (stack_track--; stack_track; stack_track--)
		result /= pop_to_long();

	push_from_long(result);
}

void builtin_not(struct Context *ctx, unsigned int stack_track)
{
	if (stack_track == 1) {
		if (wcscasecmp(pop(), L"nil") == 0)
			push_t();
		else
			push_nil();
	} else {
		push_nil();
	}
}

void builtin_gt(struct Context *ctx, unsigned int stack_track)
{
	if (stack_track < 2) {
		push_nil();
		return;
	}

	unsigned int result = 1;
	long previous = pop_to_long(), current;
	for (stack_track-- ; stack_track; stack_track--) {
		current = pop_to_long();

		if (previous <= current) {
			result--;
			break;
		}

		previous = current;
	}

	if (result)
		push_t();
	else
		push_nil();
}

void builtin_lt(struct Context *ctx, unsigned int stack_track)
{
	if (stack_track < 2) {
		push_nil();
		return;
	}

	unsigned int result = 1;
	long previous = pop_to_long(), current;
	for (stack_track-- ; stack_track; stack_track--) {
		current = pop_to_long();

		if (previous >= current) {
			result--;
			break;
		}

		previous = current;
	}

	if (result)
		push_t();
	else
		push_nil();
}

void builtin_gte(struct Context *ctx, unsigned int stack_track)
{
	if (stack_track < 2) {
		push_nil();
		return;
	}

	unsigned int result = 1;
	long previous = pop_to_long(), current;
	for (stack_track-- ; stack_track; stack_track--) {
		current = pop_to_long();

		if (previous < current) {
			result--;
			break;
		}

		previous = current;
	}

	if (result)
		push_t();
	else
		push_nil();
}

void builtin_lte(struct Context *ctx, unsigned int stack_track)
{
	if (stack_track < 2) {
		push_nil();
		return;
	}

	unsigned int result = 1;
	long previous = pop_to_long(), current;
	for (stack_track-- ; stack_track; stack_track--) {
		current = pop_to_long();

		if (previous > current) {
			result--;
			break;
		}

		previous = current;
	}

	if (result)
		push_t();
	else
		push_nil();
}

void builtin_eq(struct Context *ctx, unsigned int stack_track)
{
	if (stack_track == 0) {
		push_nil();
		return;
	}

	if (stack_track == 1) {
		push_t();
		return;
	}

	unsigned int result = 1;
	long previous = pop_to_long(), current;
	for (stack_track-- ; stack_track; stack_track--) {
		current = pop_to_long();
		if (previous != current)
			result = 0;
		previous = current;
	}

	if (result)
		push_t();
	else
		push_nil();
}

void builtin_cons(struct Context *ctx, unsigned int stack_track)
{
	if (stack_track != 2) {
		push_nil();
		return;
	}

	wchar_t *elem0 = pop();
	wchar_t *elem1 = pop();
	unsigned int elem0len = wcslen(elem0);
	unsigned int elem1len = wcslen(elem1);

	/* 4 padding for '(', ' ', ')' and null terminator */
	unsigned int len = 4 + elem0len + elem1len;

	wchar_t *cons = malloc(len * sizeof(wchar_t));
	heap_tracker_add(HT, cons);

	cons[0] = L'(';
	wcscpy(&cons[1], elem0);


	if (elem1[0] == L'(')
		wcscpy(&cons[elem0len + 1], elem1);
	else
		wcscpy(&cons[elem0len + 2], elem1);

	/* Do this after copying elem1 to remove it's leading paren */
	cons[elem0len + 1] = L' ';

	cons[len - 2] = L')';
	cons[len - 1] = 0;

	push(cons);
}

void builtin_car(struct Context *ctx, unsigned int stack_track)
{

}

void builtin_cdr(struct Context *ctx, unsigned int stack_track)
{

}

void builtin_load(struct Context *ctx, unsigned int stack_track)
{
	/*
	 * NOTE: This function if currently only vague - there's a lot of
	 * memory errors
	 */

	if (stack_track != 1)
		return;

	wchar_t *wide_filename = pop();
	char *filename = calloc(wcslen(wide_filename) + 1, sizeof(char));
	sprintf(filename, "%ls", wide_filename);

	FILE *input_file = fopen(filename, "rb");

	free(filename);

	if (input_file == NULL)
		return;

	fseek(input_file, 0, SEEK_END);
	long input_file_size = ftell(input_file);
	rewind(input_file);

	char *file_contents = malloc((input_file_size + 1) * (sizeof(char)));
	fread(file_contents, sizeof(char), input_file_size, input_file);
	file_contents[input_file_size] = 0;

	fclose(input_file);

	wchar_t *wide_file = malloc((input_file_size + 1) * (sizeof(wchar_t)));
	swprintf(wide_file, input_file_size + 1, L"%s", file_contents);
	free(file_contents);

	lisp_silent_execute(wide_file);

	free(wide_file);
}

void populate_global_context(struct Context *gbl)
{
	context_add_func(gbl, function_new_from_builtin(L"+", builtin_add));
	context_add_func(gbl, function_new_from_builtin(L"-", builtin_minus));
	context_add_func(gbl, function_new_from_builtin(L"*", builtin_mult));
	context_add_func(gbl, function_new_from_builtin(L"/", builtin_div));
	context_add_func(gbl, function_new_from_builtin(L"not", builtin_not));
	context_add_func(gbl, function_new_from_builtin(L">", builtin_gt));
	context_add_func(gbl, function_new_from_builtin(L"<", builtin_lt));
	context_add_func(gbl, function_new_from_builtin(L">=", builtin_gte));
	context_add_func(gbl, function_new_from_builtin(L"<=", builtin_lte));
	context_add_func(gbl, function_new_from_builtin(L"=", builtin_eq));
	context_add_func(gbl, function_new_from_builtin(L"cons", builtin_cons));
	context_add_func(gbl, function_new_from_builtin(L"car", builtin_car));
	context_add_func(gbl, function_new_from_builtin(L"cdr", builtin_cdr));
	context_add_func(gbl, function_new_from_builtin(L"load", builtin_load));
}
