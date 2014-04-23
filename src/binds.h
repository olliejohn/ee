/*
 * binds.h
 * Part of the Yaw text editor
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

#ifndef BINDS_H
#define BINDS_H

#include <tui.h>

#include "screen.h"

typedef void (*cb_ptr)(struct Screen *scrn);

struct Bind {
	int key;
	cb_ptr callback;
};

void binds_init();
void binds_destroy();
void binds_set_current_screen(struct Screen *scrn);
int bind(int key, void (*callback)());
void unbind(int key);
cb_ptr binds_get_callback_for(int key);
struct Bind *bind_new(int key, cb_ptr callback);
void bind_free(struct Bind *b);

/*
#define BIND_EXIT 	TK_F(1)
#define BIND_SAVE_EXIT 	TK_F(3)
#define BIND_TOOGLE_CMD	TK_CTRL_B
#define BIND_SAVE	TK_CTRL_S
#define BIND_CUT	TK_CTRL_X
#define BIND_COPY	TK_CTRL_C
#define BIND_PASTE	TK_CTRL_V
#define BIND_SELECT_ALL	TK_CTRL_A
#define BIND_UNDEF_1	TK_CTRL_2
#define BIND_UNDEF_2	TK_CTRL_4
#define BIND_UNDEF_3	TK_CTRL_5
#define BIND_UNDEF_4	TK_CTRL_6
#define BIND_UNDEF_5	TK_CTRL_7
*/

#endif
