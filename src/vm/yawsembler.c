/*
 * yawsembler.c
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

#include "yawsembler.h"

#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AST_INITIAL_SIZE 32
#define LABEL_TABLE_INITIAL_LABELS 32
#define CODE_STREAM_INITIAL_SIZE 32

enum AsmOperand {
	AO_NUL,		/* No operand */
	AO_REG,		/* Register */
	AO_INT,		/* Integer */
	AO_FLT,		/* Float */
	AO_LAB,		/* Label (also accepts integers) */
};

struct AsmOpcode {
	char *str;
	unsigned int code;
	enum AsmOperand oper0;
	enum AsmOperand oper1;
};

/* These must be alphabetical for the binary search */
const static struct AsmOpcode opcodes[] = {
	{ "add",	OP_ADD,		AO_REG,		AO_INT,		},
	{ "addf",	OP_ADDF,	AO_REG,		AO_FLT,		},
	{ "addr",	OP_ADDR,	AO_REG,		AO_REG,		},
	{ "and",	OP_AND,		AO_REG,		AO_INT,		},
	{ "andr",	OP_ADDR,	AO_REG,		AO_REG,		},
	{ "call",	OP_CALL,	AO_LAB,		AO_NUL,		},
	{ "clc",	OP_CLC,		AO_NUL,		AO_NUL,		},
	{ "clo",	OP_CLC,		AO_NUL,		AO_NUL,		},
	{ "cls",	OP_CLS,		AO_NUL,		AO_NUL,		},
	{ "clz",	OP_CLZ,		AO_NUL,		AO_NUL,		},
	{ "cmc",	OP_CMC,		AO_NUL,		AO_NUL,		},
	{ "cmo",	OP_CMO,		AO_NUL,		AO_NUL,		},
	{ "cmp",	OP_CMP,		AO_REG,		AO_INT,		},
	{ "cmpr",	OP_CMPR,	AO_REG,		AO_REG,		},
	{ "cms",	OP_CMS,		AO_NUL,		AO_NUL,		},
	{ "cmz",	OP_CMZ,		AO_NUL,		AO_NUL,		},
	{ "dec",	OP_DEC,		AO_REG,		AO_NUL,		},
	{ "div",	OP_DIV,		AO_REG,		AO_INT,		},
	{ "divf",	OP_DIVF,	AO_REG,		AO_FLT,		},
	{ "divr",	OP_DIVR,	AO_REG,		AO_REG,		},
	{ "dup",	OP_DUP,		AO_NUL,		AO_NUL,		},
	{ "echo",	OP_ECHO,	AO_REG,		AO_NUL,		},
	{ "end",	OP_END,		AO_NUL,		AO_NUL,		},
	{ "hlt",	OP_HLT,		AO_NUL,		AO_NUL,		},
	{ "inc",	OP_INC,		AO_REG,		AO_NUL,		},
	{ "je",		OP_JE,		AO_LAB,		AO_NUL,		},
	{ "jg",		OP_JG,		AO_LAB,		AO_NUL,		},
	{ "jge",	OP_JGE,		AO_LAB,		AO_NUL,		},
	{ "jl",		OP_JL,		AO_LAB,		AO_NUL,		},
	{ "jle",	OP_JLE,		AO_LAB,		AO_NUL,		},
	{ "jmp",	OP_JMP,		AO_LAB,		AO_NUL,		},
	{ "jne",	OP_JNE,		AO_LAB,		AO_NUL,		},
	{ "jnz",	OP_JNZ,		AO_LAB,		AO_NUL,		},
	{ "jz",		OP_JZ,		AO_LAB,		AO_NUL,		},
	{ "mod",	OP_MOD,		AO_REG,		AO_INT,		},
	{ "modr",	OP_MODR,	AO_REG,		AO_REG,		},
	{ "mov",	OP_MOV,		AO_REG,		AO_INT,		},
	{ "movr",	OP_MOVR,	AO_REG,		AO_REG,		},
	{ "mul",	OP_MUL,		AO_REG,		AO_INT,		},
	{ "mulf",	OP_MULF,	AO_REG,		AO_FLT,		},
	{ "mulr",	OP_MULR,	AO_REG,		AO_REG,		},
	{ "neg",	OP_NEG,		AO_REG,		AO_NUL,		},
	{ "nop",	OP_NOP,		AO_NUL,		AO_NUL,		},
	{ "not",	OP_NOT,		AO_REG,		AO_NUL,		},
	{ "or",		OP_OR,		AO_REG,		AO_INT,		},
	{ "orr",	OP_ORR,		AO_REG,		AO_REG,		},
	{ "pop",	OP_POP,		AO_REG,		AO_NUL,		},
	{ "popa",	OP_POPA,	AO_NUL,		AO_NUL,		},
	{ "popf",	OP_POPF,	AO_NUL,		AO_NUL,		},
	{ "push",	OP_PUSH,	AO_REG,		AO_NUL,		},
	{ "pusha",	OP_PUSHA,	AO_NUL,		AO_NUL,		},
	{ "pushf",	OP_PUSHF,	AO_NUL,		AO_NUL,		},
	{ "ret",	OP_RET,		AO_NUL,		AO_NUL,		},
	{ "retp",	OP_RETP,	AO_INT,		AO_NUL,		},
	{ "stc",	OP_STC,		AO_NUL,		AO_NUL,		},
	{ "sto",	OP_STO,		AO_NUL,		AO_NUL,		},
	{ "sts",	OP_STS,		AO_NUL,		AO_NUL,		},
	{ "stz",	OP_STZ,		AO_NUL,		AO_NUL,		},
	{ "sub",	OP_SUB,		AO_REG,		AO_INT,		},
	{ "subf",	OP_SUBF,	AO_REG,		AO_FLT,		},
	{ "subr",	OP_SUBR,	AO_REG,		AO_REG,		},
	{ "xor",	OP_XOR,		AO_REG,		AO_INT,		},
	{ "xorr",	OP_XORR,	AO_REG,		AO_REG,		},
};

/*
 * Binary search for the opcode associated with ident. Returns the index in the
 * opcodes table or -1 if no match is found.
 */
int get_code(char *ident)
{
	int min = 0, max = NUM_OPS - 1, cur, match;

	while (min <= max) {
		cur = (min + max) / 2;

		if ((match = strcmp(ident, opcodes[cur].str)) == 0)
			return cur;

		if (match > 0)
			min = cur + 1;
		else
			max = cur - 1;
	}

	return -1;
}

enum TreeNodeType {
	ANT_LABEL,
	ANT_OP,
};

struct Token {
	char *tkn;
	enum TreeNodeType type;
	unsigned int linum;
};

struct Tree {
	struct Token **tkns;
	unsigned int size;
	unsigned int capacity;
};

struct Tree *tree_new()
{
	struct Tree *tree = malloc(sizeof(struct Tree));
	tree->size = 0;
	tree->capacity = AST_INITIAL_SIZE;
	tree->tkns = malloc(tree->capacity * sizeof(struct Token *));
	return tree;
}

void tree_free(struct Tree *tree)
{
	unsigned int i;
	for (i = 0; i < tree->size; i++) {
		free(tree->tkns[i]->tkn);
		free(tree->tkns[i]);
	}

	free(tree->tkns);
	free(tree);
}

void tree_add(struct Tree *tree, char *tkn, size_t tknlen,
	     enum TreeNodeType type, unsigned int linum)
{
	if (tree->size >= tree->capacity) {
		tree->capacity <<= 1;
		tree->tkns = realloc(tree->tkns, tree->capacity * sizeof(char *));
	}

	tree->tkns[tree->size] = malloc(sizeof(struct Token));
	tree->tkns[tree->size]->tkn = malloc(tknlen * sizeof(char) + 1);
	strcpy(tree->tkns[tree->size]->tkn, tkn);
	tree->tkns[tree->size]->type = type;
	tree->tkns[tree->size]->linum = linum;
	tree->size++;
}

/*
 * Returns 0 if tkn is a label and -1 if it isn't. If it is, this function will
 * also remove the colon at the end.
 */
int is_a_label(char *tkn, size_t tknlen)
{
	if (tknlen < 2)
		return -1;

	int final = tknlen - 1;

	while (tkn[final] == ' ')
		final--;

	if (tkn[final] == ':') {
		tkn[final] = 0;
		return 0;
	}

	return -1;
}

struct Tree *parse(char *data)
{
	size_t len = strlen(data);
	unsigned int i;
	for (i = 0; i < len; i++)
		if (data[i] == '\t')
			data[i] = ' ';

	struct Tree *tree = tree_new();

	char *next = strtok(data, "\n");
	while (next != NULL) {
		size_t len = strlen(next);

		enum TreeNodeType type = ANT_OP;
		if (is_a_label(next, len) == 0)
			type = ANT_LABEL;

		tree_add(tree, next, len, type, 0);

		next = strtok(NULL, "\n");
	}

	return tree;
}

struct LabelTable {
	char **idents;
	int *ptrs;
	unsigned int size;
	unsigned int capacity;
};

struct LabelTable *label_table_new()
{
	struct LabelTable *lt = malloc(sizeof(struct LabelTable));
	lt->size = 0;
	lt->capacity = LABEL_TABLE_INITIAL_LABELS;
	lt->idents = malloc(lt->capacity * sizeof(char *));
	lt->ptrs = malloc(lt->capacity * sizeof(int));
	return lt;
}

void label_table_free(struct LabelTable *lt)
{
	free(lt->idents);
	free(lt->ptrs);
	free(lt);
}

void label_table_add(struct LabelTable *lt, char *ident, int ptr)
{
	if (lt->size >= lt->capacity) {
		lt->capacity <<= 1;
		lt->idents = realloc(lt->idents, lt->capacity * sizeof(char *));
		lt->ptrs = realloc(lt->ptrs, lt->capacity * sizeof(int));
	}

	lt->idents[lt->size] = ident;
	lt->ptrs[lt->size++] = ptr;
}

int label_table_lookup(struct LabelTable *lt, char *ident)
{
	unsigned int i;
	for (i = 0; i < lt->size; i++)
		if (strcmp(ident, lt->idents[i]) == 0)
			return lt->ptrs[i];

	return -1;
}

struct CodeStream *code_stream_new()
{
	struct CodeStream *cs = malloc(sizeof(struct CodeStream));
	cs->size = 0;
	cs->capacity = CODE_STREAM_INITIAL_SIZE;
	cs->ops = malloc(cs->capacity * sizeof(unsigned int));
	return cs;
}

void code_stream_free(struct CodeStream *cs)
{
	free(cs->ops);
	free(cs);
}

void code_stream_add(struct CodeStream *cs, unsigned int op)
{
	if (cs->size >= cs->capacity - 1) {
		cs->capacity <<= 1;
		cs->ops = realloc(cs->ops, cs->capacity * sizeof(unsigned int));
	}

	cs->ops[cs->size++] = op;
}

void code_stream_dump(struct CodeStream *cs)
{
	unsigned int i;
	for (i = 0; i < cs->size; i++)
		printf("%d ", cs->ops[i]);

	printf("\n");
}

int build_operation(struct CodeStream *cs, struct LabelTable *lt, char *tkn)
{
	unsigned int i, code;
	char *next = strtok(tkn, " ");
	for (i = 0; next != NULL; i++, next = strtok(NULL, "\n")) {
		if (i == 0) {
			code = get_code(next);

			if (code == -1)
				return -1;

			code_stream_add(cs, opcodes[code].code);
		} else {

		}
	}

	return 0;
}

struct CodeStream *assemble(char *data)
{
	/* Tokenize and parse */
	struct Tree *tree = parse(data);

	/* Code generation */
	struct CodeStream *cs = code_stream_new();
	struct LabelTable *lt = label_table_new();

	/* iter traverses the tree, ip is the instruction pointer */
	unsigned int i;
	for (i = 0; i < tree->size; i++) {
		if (tree->tkns[i]->type == ANT_LABEL) {
			label_table_add(lt, tree->tkns[i]->tkn, cs->size);
		} else {
			if (build_operation(cs, lt, tree->tkns[i]->tkn) == -1) {
				printf("Syntax error at line %d\n", i);
				return NULL;
			}
		}
	}

	code_stream_dump(cs);

	/* Clean up */
	tree_free(tree);
	label_table_free(lt);

	return cs;
}

#ifdef FREESTANDING_ASSEMBLER

#define NAME		"yawsembler"
#define VERSION 	"0.0.1"
#define AUTHOR		"Ollie Etherington"

void asm_version()
{
	printf("%s - Version %s\n", NAME, VERSION);
}

void asm_about()
{
	asm_version();
	printf("By %s\n", AUTHOR);
	printf("Available as free software under the GNU GPLv2\n");
}

void asm_help()
{
	printf("\nUsage: yawsembler [ FILE | FLAG ]\n\n\
Flags:\n\
--about		Information about yawsembler\n\
--help		Help about how to use yawsembler\n\
--version	Version information about yawsembler\n\
	\n");
}

/*
 * Note that several error conditions in this main function will cause a
 * memory leak but this doesn't matter as the program will exit and Linux will
 * tidy up.
 */
int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Yawsembler: Invalid Arguments\n");
		asm_help();
		return 0;
	}

	if (strcmp(argv[1], "--version") == 0) {
		asm_version();
		return 0;
	}

	if (strcmp(argv[1], "--about") == 0) {
		asm_about();
		return 0;
	}

	if (strcmp(argv[1], "--help") == 0) {
		asm_help();
		return 0;
	}

	char *buffer = NULL;
	long length;
	FILE *f = fopen(argv[1], "rb");

	if (f == NULL) {
		printf("yawsembler: Error opening file %s\n", argv[1]);
		return 0;
	}

	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = calloc(length + 1, sizeof(char));

	if (buffer == NULL) {
		printf("yawsembler: Error allocating memory for output\n");
		return 0;
	}

	fread(buffer, 1, length, f);

	fclose (f);

	if (buffer == NULL) {
		printf("yawsembler: Error assembling\n");
		return 0;
	}

	struct CodeStream *cs = assemble(buffer);

	free(buffer);

	if (cs == NULL)
		return 0;

	char *fout_path = strtok(argv[1], ".");

	FILE *fout = fopen(fout_path, "wb");

	if (fout == NULL) {
		printf("yawsembler: Error opening outfile %s\n", fout_path);
		return 0;
	}

	fwrite(cs->ops, sizeof(unsigned int), sizeof(cs->ops), fout);

	fclose(fout);

	code_stream_free(cs);

	return 0;
}

#endif /* FREESTANDING_ASSEMBLER */
