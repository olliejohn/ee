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
#define INITIAL_VARIABLES 16

struct Context {
	struct Context *parent;
	struct LispFunc **funcs;
	unsigned int num_funcs;
	unsigned int cap_funcs;
	wchar_t **vars;
	unsigned int num_vars;
	unsigned int cap_vars;
};

struct Context *context_new()
{
	struct Context *ctx = malloc(sizeof(struct Context));
	ctx->parent = NULL;
	ctx->num_funcs = 0;
	ctx->cap_funcs = INITIAL_FUNCTIONS;
	ctx->funcs = malloc(sizeof(struct LispFunc *) * ctx->cap_funcs);
	ctx->num_vars = 0;
	ctx->cap_vars = INITIAL_VARIABLES;
	ctx->vars = malloc(sizeof(wchar_t *) * ctx->cap_vars);
	return ctx;
}

struct Context *subcontext_new(struct Context *parent)
{
	struct Context *ctx = context_new();
	ctx->parent = parent;
	return ctx;
}

void context_free(struct Context *ctx)
{
	unsigned int i;
	for (i = 0; i < ctx->num_funcs; i++)
		function_free(ctx->funcs[i]);

	/* Free vars here */

	free(ctx->funcs);
	free(ctx->vars);

	free(ctx);
}

unsigned int context_get_num_funcs(struct Context *ctx)
{
	return (ctx->parent == NULL) ?
		ctx->num_funcs :
		ctx->num_funcs + context_get_num_funcs(ctx->parent);
}

void context_add_func(struct Context *ctx, struct LispFunc *func)
{
	if (ctx->num_funcs >= ctx->cap_funcs - 1) {
		ctx->cap_funcs <<= 1;
		ctx->funcs = realloc(ctx->funcs,
				    sizeof(struct LispFunc *) * ctx->cap_funcs);
	}

	ctx->funcs[ctx->num_funcs++] = func;
}

void context_add_var(struct Context *ctx, wchar_t *var)
{
	if (ctx->num_vars >= ctx->cap_vars - 1) {
		ctx->cap_vars <<= 1;
		ctx->vars = realloc(ctx->vars,
				    sizeof(wchar_t *) * ctx->cap_vars);
	}

	ctx->vars[ctx->num_vars++] = var;
}

struct LispFunc *context_lookup_function(struct Context *ctx, wchar_t *ident)
{
	unsigned int i;
	for (i = 0; i < ctx->num_funcs; i++)
		if (wcscasecmp(ident, ctx->funcs[i]->ident) == 0)
			return ctx->funcs[i];

	if (ctx->parent != NULL)
		return context_lookup_function(ctx->parent, ident);

	return NULL;
}
