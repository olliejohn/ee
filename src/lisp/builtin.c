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
#include "stack.h"

#include <stdarg.h>
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
		result *= wcstol(pop(), NULL, 10);

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
		result /= wcstol(pop(), NULL, 10);

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

		if (previous != current) {
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
}
