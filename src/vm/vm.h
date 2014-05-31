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
void vm_execute(unsigned int program[]);

enum Opcode {
	OP_PUSH,
	OP_POP,
	OP_PUSHA,
	OP_POPA,
	OP_PUSHI,
	OP_PUSHF,
	OP_POPF,
	OP_EXCH,
	OP_DUP,
	OP_ADDF,
	OP_ADD,
	OP_SUBF,
	OP_SUB,
	OP_MULF,
	OP_MUL,
	OP_DIVF,
	OP_DIV,
	OP_MOD,
	OP_INC,
	OP_DEC,
	OP_AND,
	OP_OR,
	OP_XOR,
	OP_NOT,
	OP_NEG,
	OP_CMP,
	OP_NOP,
	OP_HLT,
	OP_CALL,
	OP_RET,
	OP_JMP,
	// jcc
	// io codes
};

#endif
