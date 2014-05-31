/*
 * vm.c
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

#include "vm.h"
#include "vm_private.h"
#include "callback.h"

#include <stdio.h>

int reg[NUM_REGISTERS];
union stk_elem STACK[STACK_SIZE];
struct Instruction codes[NUM_OPS];
unsigned int *exec_ctx;

void init_instructions()
{
	codes[OP_PUSH] = (struct Instruction) { cb_push, 1 };
}

void vm_init()
{
	init_instructions();
	/* Set up the stack */
	reg[EBP] = STACK_SIZE;
	reg[ESP] = reg[EBP];

	/* Zero the flags */
	reg[EFLAGS] = 0;
}

void vm_destroy()
{

}

void vm_execute(unsigned int program[])
{
	exec_ctx = program;
	reg[EIP] = 0;
	long unsigned int num_instructions = ARRAY_SIZE(program);

	printf("%ld\n", num_instructions);
}







