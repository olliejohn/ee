/*
 * callback.c
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

#include "callback.h"
#include "vm_private.h"

#include <stdio.h>

int cb_push()
{
	if (exec_ctx[++reg[EIP]] == OP_END || exec_ctx[reg[EIP]] >= NUM_REGS)
		return -1;

	pushint(reg[exec_ctx[reg[EIP]]]);
	return 0;
}

int cb_pop()
{
	if (exec_ctx[++reg[EIP]] == OP_END || exec_ctx[reg[EIP]] >= NUM_REGS)
		return -1;

	reg[exec_ctx[reg[EIP]]] = popint();
	return 0;
}

int cb_pusha()
{
	pushint(reg[EAX]);
	pushint(reg[EBX]);
	pushint(reg[ECX]);
	pushint(reg[EDX]);
	pushint(reg[ESP]);
	pushint(reg[EBP]);
	pushint(reg[ESI]);
	pushint(reg[EDI]);
	return 0;
}

int cb_popa()
{
	reg[EDI] = popint();
	reg[ESI] = popint();
	reg[EBP] = popint();
	reg[EAX] = popint(); /* No pop esp here - just increment */
	reg[EDX] = popint();
	reg[ECX] = popint();
	reg[EBX] = popint();
	reg[EAX] = popint();
	return 0;
}

int cb_pushf()
{
	pushint(reg[EFLAGS]);
	return 0;
}

int cb_popf()
{
	reg[EFLAGS] = popint();
	return 0;
}

int cb_dup()
{
	if (reg[ESP] >= STACK_SIZE)
		return -1;

	pushint(STACK[reg[ESP]].as_i);
	return 0;
}

int cb_add()
{
	return 0;
}

int cb_inc()
{
	if (exec_ctx[++reg[EIP]] == OP_END || exec_ctx[reg[EIP]] >= NUM_REGS)
		return -1;

	reg[exec_ctx[reg[EIP]]]++;
	return 0;
}

int cb_dec()
{
	if (exec_ctx[++reg[EIP]] == OP_END || exec_ctx[reg[EIP]] >= NUM_REGS)
		return -1;

	reg[exec_ctx[reg[EIP]]]--;
	return 0;
}

int cb_mov()
{
	/* Make sure the register operand exists and is a register */
	if (exec_ctx[++reg[EIP]] == OP_END || exec_ctx[reg[EIP]] >= NUM_REGS)
		return -1;

	int dest_reg = exec_ctx[reg[EIP]];

	/* Make sure the value exists */
	if (exec_ctx[++reg[EIP]] == OP_END)
		return -1;

	/* Do the move */
	reg[dest_reg] = exec_ctx[reg[EIP]];

	return 0;
}

int cb_movr()
{
	/* Make sure the dest register operand exists and is a register */
	if (exec_ctx[++reg[EIP]] == OP_END || exec_ctx[reg[EIP]] >= NUM_REGS)
		return -1;

	int dest_reg = exec_ctx[reg[EIP]];

	/* Make sure the value register operand exists and is a register */
	if (exec_ctx[++reg[EIP]] == OP_END || exec_ctx[reg[EIP]] >= NUM_REGS)
		return -1;

	/* Do the move */
	reg[dest_reg] = reg[exec_ctx[reg[EIP]]];

	return 0;
}







