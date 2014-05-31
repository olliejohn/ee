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
	if (exec_ctx[++reg[EIP]] == OP_END)
		return -1;

	pushint(exec_ctx[reg[EIP]]);
	return 0;
}

int cb_pop()
{
	if (exec_ctx[++reg[EIP]] == OP_END)
		return -1;

	reg[exec_ctx[reg[EIP]]] = popint();
	return 0;
}





















