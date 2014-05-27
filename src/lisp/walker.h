/*
 * walker.h
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

#ifndef WALKER_H
#define WALKER_H

#include "parser.h"

struct Walker;

enum NextType {
	WKR_END,
	WKR_ATM,
	WKR_AST,
};

struct Walker *walker_new(struct AST *ast);
void walker_free(struct Walker *wkr);
void walker_set_to_end(struct Walker *wkr);
int walker_get_position(struct Walker *wkr);
enum NextType walker_scout(struct Walker *wkr);
enum NextType walker_negative_scout(struct Walker *wkr);
struct AST *walker_get_current_as_ast(struct Walker *wkr);
wchar_t *walker_get_current_as_atom(struct Walker *wkr);

#endif
