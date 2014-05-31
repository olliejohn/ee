/*
 * vm.h
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

#ifndef VM_H
#define VM_H

void vm_init();
void vm_destroy();
int vm_execute(int *program);
void dump_regs();
void dump_stack();

extern int vm_errpos;

enum Opcode {
	/* Stack Management */
	OP_PUSH,	/* Push from register */
	OP_POP,		/* Pop to register */
	OP_PUSHA,	/* Push all registers */
	OP_POPA,	/* Pop all registers */
	OP_PUSHF,	/* Push from flags */
	OP_POPF,	/* Pop to flags */

	/* Moving */
	OP_MOV,		/* Move a numeric literal to a register */
	OP_MOVR,	/* Move a value from one register to another */

	/* Integer Math */
	OP_ADD,		/* Add a literal to a register */
	OP_ADDR,	/* Add a register to another register */
	OP_SUB,		/* Subtract a literal from a register */
	OP_SUBR,	/* Subtract a register from another register */
	OP_MUL,		/* Multiply a register by a literal */
	OP_MULR,	/* Multiply a register by another register */
	OP_DIV,		/* Divide a register by a literal */
	OP_DIVR,	/* Divide a register by another register */
	OP_MOD,		/* Modulo a register with a literal */
	OP_MODR,	/* Modulo a register with another register */
	OP_INC,		/* Increment the value in the given register */
	OP_DEC,		/* Deccrement the value in the given register */

	/* Floating Point Math */
	OP_ADDF,	/*  */
	OP_SUBF,	/*  */
	OP_MULF,	/*  */
	OP_DIVF,	/*  */

	/* Logical/Bit Manipulation */
	OP_AND,		/*  */
	OP_ANDR,	/*  */
	OP_OR,		/*  */
	OP_ORR,		/*  */
	OP_XOR,		/*  */
	OP_XORR,	/*  */
	OP_NOT,		/*  */
	OP_NEG,		/*  */

	/* Comparing/Jumping */
	OP_CMP,		/*  */
	OP_CALL,	/*  */
	OP_RET,		/*  */
	OP_JMP,		/*  */
	// jcc go here

	/* Builtin IO */
	OP_ECHO,	/* Dump the value of a register to stdout */
	// io codes go here

	/* Misc */
	OP_NOP,		/* No affect but guaranteed not to be optimized out */
	OP_DUP,		/* Duplicate the element on the top of the stack */
	OP_HLT,		/* Enter the halted state */
	OP_END,		/* Exit running program */
};

/* Get number of opcodes from last entry */
#define NUM_OPS (OP_END + 1)

enum Register {
	/* General purpose */
	EAX,	/* Accumulator */
	EBX,	/* Base */
	ECX,	/* Counter */
	EDX,	/* Data */

	/* Segments */
	CS,	/* Code */
	DS,	/* Data */
	ES,	/* Extra */
	FS,	/* Extra */
	GS,	/* Extra */
	SS,	/* Stack */

	/* Indexes */
	EDI,	/* Destination */
	ESI,	/* Source */

	/* Pointers */
	EBP,	/* Stack Base */
	ESP,	/* Stack */
	EIP,	/* Index */

	/* Flags */
	EFLAGS,
};

/* Get the number of registers from the last register listed */
#define NUM_REGS (EFLAGS + 1)

#endif
