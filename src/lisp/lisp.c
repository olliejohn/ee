/*
 * lisp.c
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

#include "lisp.h"

#include "context.h"
#include "parser.h"

lisp_out_function lprintf;
struct Context *GLOBAL;

void lisp_set_out_function(lisp_out_function out)
{
	lprintf = out;
}

void interpret(struct AST *ast, struct Context *ctx)
{
	//lprintf(L"Hello World\n");
}

void lisp_execute(wchar_t *data)
{
	struct AST *ast = ast_new_from_parse(data);
	interpret(ast, GLOBAL);
	ast_free(ast);
}

void lisp_init()
{
	// Set up GLOBAL context
}

void lisp_destroy()
{

}
