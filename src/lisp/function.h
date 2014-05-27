/*
 * function.h
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

#ifndef FUNCTION_H
#define FUNCTION_H

#include <wchar.h>

struct Context;
typedef void (*builtin_fp)(struct Context *ctx, unsigned int stack_track);

union FuncData {
	builtin_fp as_builtin;
	wchar_t *as_defun;
};

enum FuncType {
	FT_BUILTIN,
	FT_DEFUN,
};

struct Function {
	union FuncData data;
	enum FuncType type;
};

struct LispFunc {
	wchar_t *ident;
	struct Function *func;
};

struct LispFunc *function_new_from_builtin(wchar_t *ident, builtin_fp builtin);
struct LispFunc *function_new_from_defun(wchar_t *ident, wchar_t *defun);
void function_free(struct LispFunc *func);

#endif
