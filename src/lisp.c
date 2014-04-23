/*
 * lisp.c
 * Part of the Yaw text editor
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

#include "binds.h"
#include "buffer.h"

#include <stdlib.h>
#include <wctype.h>

enum Atom_Type {
	A_INT,	/* Integer */
	A_DBL,	/* Double */
	A_SYM,	/* Symbol */
};

union Atom_Value {
	int a_int;
	double a_dbl;
	t_char *a_sym;
};

struct Atom {
	union Atom_Value atom;
	enum Atom_Type type;
};

enum Elem_Type {
	E_ATOM,
	E_LIST,
};

struct List;

union Elem_Value {
	struct Atom *atom;
	struct List *list;
};

struct Elem {
	union Elem_Value elem;
	enum Elem_Type type;
};

struct List {
	int capacity;
	int size;
	struct Elem **elems;
};

struct Elem *elem_new(union Elem_Value elem, enum Elem_Type type)
{
	struct Elem *e = malloc(sizeof(struct Elem));
	e->elem = elem;
	e->type = type;
	return e;
}

void elem_free(struct Elem *e)
{
	free(e);
}

struct Elem *atom_as_elem(struct Atom *atom)
{
	union Elem_Value v;
	v.atom = atom;
	return elem_new(v, E_ATOM);
}

struct Elem *list_as_elem(struct List *list)
{
	union Elem_Value v;
	v.list = list;
	return elem_new(v, E_LIST);
}

#define LIST_BLOCK_SIZE 8

struct List *list_new()
{
	struct List *list = malloc(sizeof(struct List));
	list->capacity = LIST_BLOCK_SIZE;
	list->size = 0;
	list->elems = malloc(sizeof(struct Elem) * list->capacity);
	return list;
}

void list_free(struct List *list)
{
	int i;
	for (i = 0; i < list->size; i++)
		elem_free(list->elems[i]);

	free(list->elems);
	free(list);
}

void list_free_recursive(struct List *list)
{
	int i;
	for (i = 0; i < list->size; i++)
		if (list->elems[i]->type == E_LIST)
			list_free_recursive(list->elems[i]->elem.list);

	list_free(list);
}

void list_double_capacity_if_full(struct List *list)
{
	if (list->capacity - 1 <= list->size) {
		list->capacity += LIST_BLOCK_SIZE;
		list->elems = realloc(list->elems,
				      sizeof(struct Elem *) * list->capacity);
	}
}

void list_add_elem(struct List *list, struct Elem *elem)
{
	list_double_capacity_if_full(list);
	list->elems[list->size++] = elem;
}

struct Atom *list_get_atom(struct List *list, int index)
{
	return list->elems[index]->elem.atom;
}

void atom_to_string(struct Atom *atom, t_char *buf, int buflen)
{
	if (atom->type == A_INT)
		swprintf(buf, buflen, L"%d", atom->atom.a_int);
	else if (atom->type == A_DBL)
		;//swprintf(buf, buflen, L"%f", atom->atom.a_dbl);
	else if (atom->type == A_SYM)
		wcscpy(buf, atom->atom.a_sym);
}

void list_dump_recursive(struct List *list, int x, int y, int spacing)
{
	int i;
	for (i = 0; i < list->size; i++) {
		if (list->elems[i]->type == E_LIST) {
			list_dump_recursive(list->elems[i]->elem.list,
					    x, y, spacing + 2);
		} else {
			t_char spaces[spacing];
			wmemset(spaces, L' ', spacing);

			t_char *buf = malloc(sizeof(t_char) * 128);
			atom_to_string(list_get_atom(list, i), buf, 128);
			t_mv_wprint(stdscr, x, y++, L"%s%s", spaces, buf);
			free(buf);
		}
	}
}

#define list_dump(x) list_dump_recursive(x, 0, 10, 2)

struct Buffer *lisp_tokenize(struct Line *ln)
{

	for (ln->pos = 0; ln->pos <= ln->size; ln->pos++) {
		if (ln->data[ln->pos] == L'(' || ln->data[ln->pos] == L')') {
			line_add(ln, L' ');
			ln->pos++;
			line_add(ln, L' ');
		}
	}

	struct Buffer *tkns = buffer_new();
	t_char *data;
	t_char *state;
	int i;

	data = wcstok(ln->data, L" ", &state);
	while (data != NULL) {
		for (i = 0; i < wcslen(data); i++)
			buffer_add(tkns, data[i]);

		buffer_new_line(tkns);

		data = wcstok(NULL, L" ", &state);
	}

	return tkns;
}

struct Atom *lisp_get_atom(t_char *tok)
{
	int double_flag = 0;
	int nan_flag = 0;

	int i;
	for (i = 0; i < wcslen(tok); i++) {
		if (tok[i] == L'.') {
			if (double_flag)
				return NULL;
			else
				double_flag = 1;
		}

		if (!iswdigit(tok[i])) {
			nan_flag = 1;
			break;
		}
	}

	struct Atom *atom;

	if (nan_flag) {
		atom->atom.a_sym = tok;
		atom->type = A_SYM;
	} else {
		t_char *end;

		if (double_flag) {
			atom->atom.a_dbl = wcstod(tok, &end);
			atom->type = A_DBL;
		} else {
			atom->atom.a_int = wcstol(tok, &end, 10);
			atom->type = A_INT;
		}
	}

	return atom;
}

enum BuildListVal {
	BLE_SUCCESS,
	BLE_NULL_LIST,
	BLE_PARENS,
};

int BUILD_LIST_ERRNO = 0;

#define lisp_build_list_err(x) BUILD_LIST_ERRNO = x

/*
 * Recursively build lists from tkns between start and end inclusive.
 * If end == -1 then the entire token stream is evaluated
 */
struct List *lisp_build_list(struct Buffer *tkns, int start, int end)
{
	struct List *l = list_new();

	if (tkns->size < 1) {
		lisp_build_list_err(BLE_NULL_LIST);
		return NULL;
	}

	if (end == -1)
		end = tkns->size - 1;

	for ( ; start <= end; start++) {
		t_char *tok = tkns->data[start]->data;
		list_add_elem(l, atom_as_elem(lisp_get_atom(tok)));
/*
		if (wcscmp(tok, L"(") == 0) {
			int new_start = start;
			int new_end = start;

			while (wcscmp(tok, L")") != 0)
				tok = tkns->data[++new_end]->data;

			struct List *new_list = lisp_build_list(tkns,
								new_start,
								new_end);

			list_add_elem(l, list_as_elem(new_list));
		} else if (wcscmp(tok, L")") == 0) {
			return l;
		} else {
			list_add_elem(l, atom_as_elem(lisp_get_atom(tok)));
		}
		* */
	}

	return l;
}

inline struct List *lisp_build_abstract_syntax_tree(struct Buffer *tkns)
{
	return lisp_build_list(tkns, 0, -1);
}

t_char *lisp_parse(struct Buffer *tkns)
{
	if (tkns->size < 1)
		return L"Error: Unexpected end of line whilst reading";

	tkns->pos = 0;
	struct List *ast = lisp_build_abstract_syntax_tree(tkns);

	if (ast == NULL) {
		switch (BUILD_LIST_ERRNO) {
		case BLE_NULL_LIST:
			return L"Error: Null List";
		case BLE_PARENS:
			return L"Error: Mismatched parentheses";
		default:
			return L"Error: Impossible parse";
		}
	}

	list_dump(ast);

	list_free_recursive(ast);

	return L"Success";
}

t_char *lisp_eval(struct Line *ln)
{
	struct Buffer *tkns = lisp_tokenize(ln);
	t_char *value = lisp_parse(tkns);
	buffer_free(tkns);
	return value;
}

int lisp_run(struct Screen *scrn)
{
	struct Line *ln = line_new();
	t_char ch;
	int y = 0;
	while (t_getch(&ch) != TUI_ERR) {// && ch != BIND_EXIT) {
		/* Read */
		switch (ch) {
		case TK_ENTER:
			break;
		case TK_LEFT:
			line_move_backward(ln);
			t_wmove(scrn->bwin, ln->pos, y);
			t_wrefresh(scrn->bwin);
			continue;
		case TK_RIGHT:
			line_move_forward(ln);
			t_wmove(scrn->bwin, ln->pos, y);
			t_wrefresh(scrn->bwin);
			continue;
		case TK_BKSPC:
			line_backspace(ln);
			t_mv_wprint(scrn->bwin, 0, y, ln->data);
			t_wclrtoeol(scrn->bwin);
			t_wmove(scrn->bwin, ln->pos, y);
			t_wrefresh(scrn->bwin);
			continue;
		case TK_DELETE:
			if (ln->pos >= ln->size)
				continue;
			line_move_forward(ln);
			line_backspace(ln);
			t_mv_wprint(scrn->bwin, 0, y, ln->data);
			t_wclrtoeol(scrn->bwin);
			t_wmove(scrn->bwin, ln->pos, y);
			t_wrefresh(scrn->bwin);
			continue;
		default:
			line_add(ln, ch);
			t_mv_wprint(scrn->bwin, 0, y, ln->data);
			t_wmove(scrn->bwin, ln->pos, y);
			t_wrefresh(scrn->bwin);
			continue;
		};

		/* Eval */
		t_char *value = lisp_eval(ln);

		/* Print */
		t_mv_wprint(scrn->bwin, 0, ++y, value);

		/* Reset */
		line_free(ln);
		ln = line_new();
		t_wmove(scrn->bwin, 0, ++y);
		t_wrefresh(scrn->bwin);
	}

	line_free(ln);

	return 0;
}

void lisp_init()
{

}

void lisp_destroy()
{

}
