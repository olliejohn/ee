/*
 * function.c
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

#include "function.h"

#include <stdlib.h>

struct LispFunc *function_new_from_builtin(wchar_t *ident, builtin_fp b)
{
	struct LispFunc *f = malloc(sizeof(struct LispFunc));
	f->ident = ident;
	f->func = malloc(sizeof(struct Function));
	f->func->data.as_builtin = b;
	f->func->type = FT_BUILTIN;
	return f;
}

struct LispFunc *function_new_from_defun(wchar_t *ident, wchar_t *d)
{
	struct LispFunc *f = malloc(sizeof(struct LispFunc));
	f->ident = ident;
	f->func = malloc(sizeof(struct Function));
	f->func->data.as_defun = d;
	f->func->type = FT_DEFUN;
	return f;
}

void function_free(struct LispFunc *func)
{
	free(func->func);
	free(func);
}
