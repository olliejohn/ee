/*
 * vm_private.h
 * Part of the VM subsystem in the Yaw text editor
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

#ifndef VM_PRIVATE_H
#define VM_PRIVATE_H

#include "vm.h"

/* Get the number of registers from the last register listed */
#define NUM_REGISTERS (EFLAGS + 1)

extern int reg[NUM_REGISTERS];

union stk_elem {
	int as_i;
	float as_f;
};

#define STACK_SIZE 4096
extern union stk_elem STACK[STACK_SIZE];

enum stk_status {
	STK_GOOD,
	STK_OVERFLOW,
	STK_UNDERFLOW,
};

inline enum stk_status push(union stk_elem e)
{
	if (--reg[ESP] <= 0) {
		reg[ESP] = 0;
		return STK_OVERFLOW;
	}

	STACK[reg[ESP]] = e;
	return STK_GOOD;
}

inline enum stk_status pushint(int i)
{
	union stk_elem e;
	e.as_i = i;
	return push(e);
}

inline enum stk_status pushflt(float f)
{
	union stk_elem e;
	e.as_f = f;
	return push(e);
}

inline union stk_elem pop()
{
	if (reg[ESP] >= STACK_SIZE)
		return (union stk_elem) STK_UNDERFLOW;
	return STACK[reg[ESP]++];
}

inline int popint()
{
	return pop().as_i;
}

inline float popflt()
{
	return pop().as_f;
}

/* Get number of opcodes from last entry */
#define NUM_OPS (OP_JMP + 1)

typedef int (*callback)();

struct Instruction {
	callback cb;
	unsigned int nargs; /* Number of arguments/operands */
};

extern struct Instruction codes[NUM_OPS];

/* The current program being run is stored in exec_ctx */
extern int *exec_ctx;

#endif
