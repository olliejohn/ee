/*
 * context.h
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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "function.h"

#include <wchar.h>

struct Context;

struct Context *context_new();
struct Context *subcontext_new(struct Context *parent);
void context_free(struct Context *ctx);
unsigned int context_get_num_funcs(struct Context *ctx);
void context_add_func(struct Context *ctx, struct LispFunc *func);
void context_add_var(struct Context *ctx, wchar_t *var);
struct LispFunc *context_lookup_function(struct Context *ctx, wchar_t *ident);

#endif
