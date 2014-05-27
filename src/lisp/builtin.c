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
#include "stack.h"

#include <stdarg.h>
#include <stdlib.h>

#define MAX_DIGITS_IN_SLONG 20 /* Max digits in a signed long plus 1 padding */

void builtin_add(struct Context *ctx, unsigned int stack_track)
{
	long result;

	for (result = 0; stack_track; stack_track--)
		result += wcstol(pop(), NULL, 10);

	/* This is a memory leak */
	wchar_t *ret = calloc(MAX_DIGITS_IN_SLONG, sizeof(wchar_t));
	swprintf(ret, MAX_DIGITS_IN_SLONG, L"%ld", result);

	push(ret);
}

void populate_global_context(struct Context *gbl)
{
	context_add_func(gbl, function_new_from_builtin(L"+", builtin_add));
}
