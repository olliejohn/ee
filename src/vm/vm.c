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

int reg[NUM_REGS];
union stk_elem STACK[STACK_SIZE];
struct Instruction codes[NUM_OPS];
int *exec_ctx;
int vm_errpos;

void init_instructions()
{
	codes[OP_PUSH] = (struct Instruction) { cb_push, 1 };
	codes[OP_POP] = (struct Instruction) { cb_pop, 1 };
	codes[OP_PUSHA] = (struct Instruction) { cb_pusha, 0 };
	codes[OP_POPA] = (struct Instruction) { cb_popa, 0 };
	codes[OP_PUSHF] = (struct Instruction) { cb_pushf, 0 };
	codes[OP_POPF] = (struct Instruction) { cb_popf, 0 };
	codes[OP_DUP] = (struct Instruction) { cb_dup, 0 };
	codes[OP_ADD] = (struct Instruction) { cb_add, 0 };
	codes[OP_INC] = (struct Instruction) { cb_inc, 0 };
	codes[OP_DEC] = (struct Instruction) { cb_dec, 0 };
	codes[OP_MOV] = (struct Instruction) { cb_mov, 2 };
}

void vm_init()
{
	/* Build the instruction table */
	init_instructions();

	/* Set up the stack */
	reg[EBP] = STACK_SIZE;
	reg[ESP] = reg[EBP];

	/* Zero the flags */
	reg[EFLAGS] = 0;

	/* Set the current program to NULL */
	exec_ctx = NULL;
}

void vm_destroy()
{

}

int vm_execute(int *program)
{
	exec_ctx = program;
	reg[EIP] = 0;

	for ( ; exec_ctx[reg[EIP]] != OP_END; reg[EIP]++)
		if (reg[EIP] >= NUM_OPS || codes[exec_ctx[reg[EIP]]].cb() != 0){
			vm_errpos = reg[EIP];
			return -1;
		}

	dump_regs();
	dump_stack();

	return 0;
}

void dump_regs()
{
	printf("EAX: %d\nEBX: %d\nECX: %d\nEDX: %d\nCS: %d\nDS: %d\nES: %d\n\
FS: %d\nGS: %d\nSS: %d\nEDI: %d\nESI: %d\nEBP: %d\nESP: %d\nEIP: %d\nEFLAGS: \
%d\n", 		reg[EAX], reg[EBX], reg[ECX], reg[EDX], reg[CS], reg[DS],
		reg[ES], reg[FS], reg[GS], reg[SS], reg[EDI], reg[ESI],
		reg[EBP], reg[ESP], reg[EIP], reg[EFLAGS]);
}

void dump_stack()
{
	unsigned int i;
	for (i = reg[ESP]; i < STACK_SIZE; i++)
		printf("%d: %d\n", i, STACK[i].as_i);
}
