/*
 * parser.c
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

#include "walker.h"

#include <stdlib.h>

struct Walker {
	struct AST *ast;
	int pos;
	unsigned int size;
};

struct Walker *walker_new(struct AST *ast)
{
	struct Walker *wkr = malloc(sizeof(struct Walker));
	wkr->ast = ast;
	wkr->pos = -1;
	wkr->size = ast_get_size(ast);
	return wkr;
}

void walker_free(struct Walker *wkr)
{
	free(wkr);
}

void walker_set_to_end(struct Walker *wkr)
{
	wkr->pos = wkr->size;
}

int walker_get_position(struct Walker *wkr)
{
	return wkr->pos;
}

enum NextType walker_scout(struct Walker *wkr)
{
	if (++wkr->pos >= wkr->size)
		return WKR_END;

	if (ast_get_as_atom(wkr->ast, wkr->pos) == NULL)
		return WKR_AST;

	return WKR_ATM;
}

enum NextType walker_negative_scout(struct Walker *wkr)
{
	if (--wkr->pos <= -1)
		return WKR_END;

	if (ast_get_as_atom(wkr->ast, wkr->pos) == NULL)
		return WKR_AST;

	return WKR_ATM;
}

struct AST *walker_get_current_as_ast(struct Walker *wkr)
{
	return ast_get_as_ast(wkr->ast, wkr->pos);
}

wchar_t *walker_get_current_as_atom(struct Walker *wkr)
{
	return ast_get_as_atom(wkr->ast, wkr->pos);
}

