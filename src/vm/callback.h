/*
 * callback.h
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

#ifndef CALLBACK_H
#define CALLBACK_C

int cb_push();
int cb_pop();
int cb_pusha();
int cb_popa();
int cb_pushf();
int cb_popf();
int cb_dup();
int cb_add();
int cb_addr();
int cb_sub();
int cb_subr();
int cb_mul();
int cb_mulr();
int cb_div();
int cb_divr();
int cb_inc();
int cb_dec();
int cb_mov();
int cb_movr();
int cb_echo();

#endif
