/*
 * context.c
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

#include "context.h"

#include <stdlib.h>

#define INITIAL_FUNCTIONS 16
#define INITIAL_VARS 16

typedef void (*lisp_function)(struct Context *ctx, ...);

struct Context {
	struct Context *parent;
	lisp_function *funcs;
};

struct Context *context_new()
{
	struct Context *ctx = malloc(sizeof(struct Context));
	ctx->parent = NULL;

	return ctx;
}

struct Context *subcontext_new(struct Context *parent)
{
	struct Context *ctx = malloc(sizeof(struct Context));
	ctx->parent = parent;

	return ctx;
}





















