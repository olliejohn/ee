/*
 * parser.h
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

#ifndef PARSER_H
#define PARSER_H

#include <wchar.h>

struct AST;

struct AST *ast_new_from_parse(wchar_t *data);
void ast_free(struct AST *ast);
unsigned int ast_get_size(struct AST *ast);
struct AST *ast_get_as_ast(struct AST *ast, int index);
wchar_t *ast_get_as_atom(struct AST *ast, int index);

#endif
