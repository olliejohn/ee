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

#include "builtin.h"
#include "context.h"
#include "function.h"
#include "heap_tracker.h"
#include "parser.h"
#include "stack.h"
#include "walker.h"

lisp_out_function lprintf;
struct Context *GLOBAL;
wchar_t *STACK[STACK_SIZE];
unsigned int SP;
struct HeapTracker *HT;

void lisp_silent_execute(wchar_t *data);

void lisp_set_out_function(lisp_out_function out)
{
	lprintf = out;
}

void interpret(struct Walker *wkr, struct Context *ctx)
{
	struct Context *local = subcontext_new(ctx);

	walker_set_to_end(wkr);

	unsigned int stack_track = 0;
	enum NextType next;
	while ((next = walker_negative_scout(wkr)) != WKR_END) {
		if (next == WKR_AST) {
			struct Walker *subwkr =
				walker_new(walker_get_current_as_ast(wkr));
			interpret(subwkr, local);
			walker_free(subwkr);
			stack_track++;
		} else {
			wchar_t *atom = walker_get_current_as_atom(wkr);

			if (atom[0] == L'(') {
				lisp_silent_execute(atom);
				continue;
			}

			if (walker_get_position(wkr) == 0) {
				struct LispFunc *f =
					context_lookup_function(local, atom);

				if (f == NULL) {
					/* Handle malformed function calls */
				}

				if (f->func->type == FT_BUILTIN) {
					f->func->data.as_builtin(local,
								 stack_track);
				} else { /* f->func->type == FT_DEFUN */

				}

#ifdef DEBUG
				lprintf(L"Lisp Function Call: %ls\n", atom);
#endif /* DEBUG */
				break;
			} else {
				push(atom);
				stack_track++;
			}
		}
	}

	context_free(local);
}

void lisp_silent_execute(wchar_t *data)
{
	struct AST *ast = ast_new_from_parse(data);
	struct Walker *wkr = walker_new(ast);
	interpret(wkr, GLOBAL);
	walker_free(wkr);
	ast_free(ast);
}

void lisp_execute(wchar_t *data)
{
#ifdef DEBUG
	lprintf(L"Executing: %ls\n\n", data);
#endif /* DEBUG */

	lisp_silent_execute(data);

	unsigned int i;
#ifdef DEBUG
	lprintf(L"\nFINAL:\n");
	for (i = SP; i < STACK_SIZE; i++)
		lprintf(L"Value: %ls at %d\n\n", pop(), i);
#else /* DEBUG not defined */
	for (i = SP; i < STACK_SIZE; i++)
		lprintf(L"%ls\n", pop());
#endif /* DEBUG */

	heap_tracker_clean(HT);
}

void lisp_init()
{
	/* Set up the stack */
	SP = STACK_SIZE;
	HT = heap_tracker_new();

	/* Set up the environment */
	GLOBAL = context_new();
	populate_global_context(GLOBAL);
}

void lisp_destroy()
{
	/* Destroy the environment */
	context_free(GLOBAL);

	/* Destroy the stack */
	heap_tracker_free(HT);
}
