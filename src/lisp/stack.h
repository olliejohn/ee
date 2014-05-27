/*
 * stack.h
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

#ifndef STACK_H
#define STACK_H

#include "comms.h"

#include <wchar.h>

enum StackValue {
	STACK_GOOD,
	STACK_OVERFLOW,
	STACK_UNDERFLOW,
};

#define STACK_SIZE 1500

extern wchar_t *STACK[STACK_SIZE];
extern unsigned int SP;

inline enum StackValue push(wchar_t *val)
{
#ifdef DEBUG
	lprintf(L"Pushing to %d: %ls\n", SP - 1, val);
#endif /* DEBUG */

	if (SP == 0) {
#ifdef DEBUG
		lprintf(L"*** STACK OVERFLOW ***");
#endif /* DEBUG */
		return STACK_OVERFLOW;
	}

	STACK[--SP] = val;
	return STACK_GOOD;
}

inline wchar_t *pop()
{
#ifdef DEBUG
	lprintf(L"Popping from %d\n", SP);
#endif /* DEBUG */

	if (SP >= STACK_SIZE) {
#ifdef DEBUG
		lprintf(L"*** STACK UNDERFLOW ***");
#endif /* DEBUG */
		return (wchar_t *) STACK_UNDERFLOW;
	}

	return STACK[SP++];
}

#endif
