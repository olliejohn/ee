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

#include <stdio.h>
#include <stdlib.h>
#include <wctype.h>

#define AST_INITIAL_SIZE 16

struct Cell;
void cell_free(struct Cell *cell);

struct AST {
	struct Cell **data;
	unsigned int size;
	unsigned int capacity;
};

enum CellType {
	CT_ATM,
	CT_AST,
};

union CellData {
	struct AST *as_ast;
	wchar_t *as_atom;
};

struct Cell {
	enum CellType type;
	union CellData data;
};

struct AST *ast_new()
{
	struct AST *ast = malloc(sizeof(struct AST));
	ast->capacity = AST_INITIAL_SIZE;
	ast->data = malloc(ast->capacity * sizeof(struct Cell *));
	ast->size = 0;
	return ast;
}

void ast_free(struct AST *ast)
{
	if (ast != NULL) {
		unsigned int i;
		for (i = 0; i < ast->size; i++)
			cell_free(ast->data[i]);

		free(ast->data);
		free(ast);
	}
}

void ast_add(struct AST *ast, struct Cell *cell)
{
	if (ast->size >= ast->capacity - 1) {
		ast->capacity <<= 1;
		ast->data = realloc(ast->data,
				    ast->capacity * sizeof(struct Cell *));
	}

	ast->data[ast->size++] = cell;
}

struct Cell *cell_new_from_token(wchar_t *token)
{
	struct Cell *cell = malloc(sizeof(struct Cell));
	cell->type = CT_ATM;
	cell->data.as_atom = calloc(wcslen(token) + 1, sizeof(wchar_t));
	wcscpy(cell->data.as_atom, token);
	return cell;
}

struct Cell *cell_new_from_ast(struct AST *ast)
{
	struct Cell *cell = malloc(sizeof(struct Cell));
	cell->type = CT_AST;
	cell->data.as_ast = ast;
	return cell;
}

void cell_free(struct Cell *cell)
{
	if (cell->type == CT_ATM)
		free(cell->data.as_atom);
	else
		ast_free(cell->data.as_ast);

	free(cell);
}

unsigned int AST_RESTART;
struct AST *build_ast_single(struct TokenStream *ts, unsigned int i)
{
	struct AST *ast = ast_new();

	const unsigned int size = token_stream_get_size(ts);
	wchar_t *tok;

	for ( ; i < size; i++) {
		tok = token_stream_get(ts, i);
		if (tok[0] == L'(') {
			ast_add(ast,
				cell_new_from_ast(build_ast_single(ts, i + 1)));
			i = AST_RESTART;
		} else if (tok[0] == L')') {
			AST_RESTART = i;
			return ast;
		} else {
			ast_add(ast, cell_new_from_token(tok));
		}
	}

	return ast;
}

inline struct AST *build_ast(struct TokenStream *ts)
{
	AST_RESTART = 0;
	return build_ast_single(ts, 0);
}

void token_stream_dump(struct TokenStream *ts)
{
#ifdef DEBUG
	unsigned int i;
	for (i = 0; i < token_stream_get_size(ts); i++)
		printf("%ls\n", token_stream_get(ts, i));
#endif /* DEBUG */
}

#define ast_dump(ast) ast_dump_deleg(ast, 0)
void ast_dump_deleg(struct AST *ast, int offs)
{
#ifdef DEBUG
	if (offs == 0)
		printf("AST Dump:\n");

	unsigned int i;
	for (i = 0; i < ast->size; i++)
		if (ast->data[i]->type == CT_ATM) {
			printf("%*s%ls\n", offs, "", ast->data[i]->data.as_atom);
		} else {
			printf("%*s%s\n", offs, "", "Node:");
			ast_dump_deleg(ast->data[i]->data.as_ast, offs + 4);
		}
#endif /* DEBUG */
}

struct AST *ast_new_from_parse(wchar_t *data)
{
	if (wcslen(data) < 1)
		return NULL;

	struct TokenStream *ts = tokenize(data);

	if (ts == NULL)
		return NULL;

	// token_stream_dump(ts);

	struct AST *ast = build_ast(ts);

	ast_dump(ast);

	token_stream_free(ts);

	return ast;
}
