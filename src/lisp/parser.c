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

#include "parser.h"

#include "tokenizer.h"

#include <stdio.h> /* ONLY FOR DEBUGGING - REMOVE WHEN DONE */
#include <stdlib.h>

union Atom {
	struct AST *NODE;
	wchar_t *SYM;
	int INT;
};

enum AtomType {
	AT_NODE,
	AT_SYM,
	AT_INT,
};

struct AST {
	union Atom data;
	enum AtomType type;
	struct AST *next;
};

void parse(struct AST *ast, wchar_t *data)
{
	if (wcslen(data) < 1)
		return;

	struct TokenStream *ts = tokenize(data);

	if (ts == NULL)
		return;

	unsigned int i;
	for (i = 0; i < token_stream_get_size(ts); i++) {
		wchar_t *tok = token_stream_get(ts, i);

		if (tok == NULL)
			continue;

		printf("%ls\n", token_stream_get(ts, i));
	}

	// Parse the tokens here

	token_stream_free(ts);
}

struct AST * __attribute__((warn_unused_result)) ast_new(wchar_t *data)
{
	struct AST *ast = malloc(sizeof(struct AST));
	parse(ast, data);
	return ast;
}

void ast_free(struct AST* ast)
{
	free(ast);
}
