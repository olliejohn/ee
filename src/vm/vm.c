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
#include <stdlib.h>

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
	codes[OP_MOV] = (struct Instruction) { cb_mov, 2 };
	codes[OP_MOVR] = (struct Instruction) { cb_movr, 2 };
	codes[OP_ADD] = (struct Instruction) { cb_add, 2 };
	codes[OP_ADDR] = (struct Instruction) { cb_addr, 2 };
	codes[OP_SUB] = (struct Instruction) { cb_sub, 2 };
	codes[OP_SUBR] = (struct Instruction) { cb_subr, 2 };
	codes[OP_MUL] = (struct Instruction) { cb_mul, 2 };
	codes[OP_MULR] = (struct Instruction) { cb_mulr, 2 };
	codes[OP_DIV] = (struct Instruction) { cb_div, 2 };
	codes[OP_DIVR] = (struct Instruction) { cb_divr, 2 };
	codes[OP_MOD] = (struct Instruction) { cb_mod, 2 };
	codes[OP_MODR] = (struct Instruction) { cb_modr, 2 };
	codes[OP_INC] = (struct Instruction) { cb_inc, 1 };
	codes[OP_DEC] = (struct Instruction) { cb_dec, 1 };
	codes[OP_AND] = (struct Instruction) { cb_and, 2 };
	codes[OP_ANDR] = (struct Instruction) { cb_andr, 2 };
	codes[OP_OR] = (struct Instruction) { cb_or, 2 };
	codes[OP_ORR] = (struct Instruction) { cb_orr, 2 };
	codes[OP_XOR] = (struct Instruction) { cb_xor, 2 };
	codes[OP_XORR] = (struct Instruction) { cb_xorr, 2 };
	codes[OP_NOT] = (struct Instruction) { cb_not, 1 };
	codes[OP_NEG] = (struct Instruction) { cb_neg, 1 };
	codes[OP_CMP] = (struct Instruction) { cb_cmp, 2 };
	codes[OP_CMPR] = (struct Instruction) { cb_cmpr, 2 };
	codes[OP_CALL] = (struct Instruction) { cb_call, 1 };
	codes[OP_RET] = (struct Instruction) { cb_ret, 0 };
	codes[OP_RETP] = (struct Instruction) { cb_retp, 1 };
	codes[OP_JMP] = (struct Instruction) { cb_jmp, 1 };
	codes[OP_JE] = (struct Instruction) { cb_je, 1 };
	codes[OP_JZ] = (struct Instruction) { cb_je, 1 }; /* Same as je */
	codes[OP_JNE] = (struct Instruction) { cb_jne, 1 };
	codes[OP_JNZ] = (struct Instruction) { cb_jne, 1 }; /* Same as jne */
	codes[OP_JG] = (struct Instruction) { cb_jg, 1 };
	codes[OP_JGE] = (struct Instruction) { cb_jge, 1 };
	codes[OP_JL] = (struct Instruction) { cb_jl, 1 };
	codes[OP_JLE] = (struct Instruction) { cb_jle, 1 };
	codes[OP_STC] = (struct Instruction) { cb_stc, 0 };
	codes[OP_CLC] = (struct Instruction) { cb_clc, 0 };
	codes[OP_CMC] = (struct Instruction) { cb_cmc, 0 };
	codes[OP_STO] = (struct Instruction) { cb_sto, 0 };
	codes[OP_CLO] = (struct Instruction) { cb_clo, 0 };
	codes[OP_CMO] = (struct Instruction) { cb_cmo, 0 };
	codes[OP_STZ] = (struct Instruction) { cb_stz, 0 };
	codes[OP_CLZ] = (struct Instruction) { cb_clz, 0 };
	codes[OP_CMZ] = (struct Instruction) { cb_cmz, 0 };
	codes[OP_STS] = (struct Instruction) { cb_sts, 0 };
	codes[OP_CLS] = (struct Instruction) { cb_cls, 0 };
	codes[OP_CMS] = (struct Instruction) { cb_cms, 0 };
	codes[OP_ECHO] = (struct Instruction) { cb_echo, 1 };
	codes[OP_NOP] = (struct Instruction) { cb_nop, 0 };
	codes[OP_HLT] = (struct Instruction) { cb_hlt, 0 };
	codes[OP_DUP] = (struct Instruction) { cb_dup, 0 };
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

//	dump_regs();
//	dump_stack();

	return 0;
}

#define PROGRAM_INITIAL_CAPACITY 16

struct Program {
	int *code;
	unsigned int size;
	unsigned int capacity;
};

struct Program *program_new()
{
	struct Program *pgm = malloc(sizeof(struct Program));
	pgm->size = 0;
	pgm->capacity = PROGRAM_INITIAL_CAPACITY;
	pgm->code = malloc(pgm->capacity * sizeof(int));
	return pgm;
}

void program_free(struct Program *pgm)
{
	free(pgm->code);
	free(pgm);
}

void program_add(struct Program *pgm, int c)
{
	if (pgm->size >= pgm->capacity - 1) {
		pgm->capacity <<= 1;
		pgm->code = realloc(pgm->code, pgm->capacity * sizeof(int));
	}

	pgm->code[pgm->size++] = c;
}

int vm_execute_file(char *filename)
{
	FILE *file = fopen(filename, "r");

	if (file == NULL) {
		printf("Could not open file %s\n", filename);
		return -1;
	}

	struct Program *pgm = program_new();

	int num;
	unsigned int i;
	for (i = 0; fscanf(file, "%d ", &num) > 0; i++)
		program_add(pgm, num);

	fclose(file);

	num = vm_execute(pgm->code);

	program_free(pgm);

	return num;
}

void dump_regs()
{
	printf("\nRegisters:\n\
EAX: %d      EBX: %d      ECX: %d      EDX: %d\n\
CS: %d      DS: %d      ES: %d      FS: %d      GS: %d      SS: %d\n\
EDI: %d      ESI: %d      EBP: %d      ESP: %d      EIP: %d      EFLAGS: %d\n\n\
Flags:\n\
Carry: %d   Overflow: %d   Sign: %d   Zero: %d\n\n",
 		reg[EAX], reg[EBX], reg[ECX], reg[EDX], reg[CS], reg[DS],
		reg[ES], reg[FS], reg[GS], reg[SS], reg[EDI], reg[ESI],
		reg[EBP], reg[ESP], reg[EIP], reg[EFLAGS], get_flag(F_CARRY),
		get_flag(F_OVERFLOW), get_flag(F_SIGN), get_flag(F_ZERO));
}

void dump_stack()
{
	unsigned int i;
	for (i = reg[ESP]; i < STACK_SIZE; i++)
		printf("%d: %d\n", i, STACK[i].as_i);
}

struct VMHeader build_header()
{
	struct VMHeader hd = {
		.MAGIC = (HMGC[0] << 24) + (HMGC[1] << 16) +
					(HMGC[2] << 8) + HMGC[3],
		.VER_MAJ = VM_VERSION_MAJOR,
		.VER_MIN = VM_VERSION_MINOR,
		.VER_REV = VM_VERSION_REVISION,
		.hd_size = sizeof(struct VMHeader),
		.ph_off = 0,
		.sh_off = 0,
		.ph_size = 0,
		.sh_size = 0,
		.ph_num = 0,
		.sh_num = 0,
		.flags = 0
	};
	return hd;
}
