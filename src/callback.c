/*
 * callback.c
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

#include "callback.h"

void cb_exit(struct Screen *scrn)
{
	screen_set_flag(scrn, SF_EXIT);
}

void cb_save_exit(struct Screen *scrn)
{
	screen_set_flag(scrn, SF_SAVE_EXIT);
}

void cb_save(struct Screen *scrn)
{
	//buffer_save(scrn->buf);
}

void cb_save_as(struct Screen *scrn)
{

}

void cb_toggle_cli(struct Screen *scrn)
{
	screen_toggle_flag(scrn, SF_CLI);
}

void cb_toggle_term(struct Screen *scrn)
{
	screen_toggle_flag(scrn, SF_TERM);
}

void cb_cut(struct Screen *scrn)
{

}

void cb_copy(struct Screen *scrn)
{

}

void cb_paste(struct Screen *scrn)
{

}

void cb_select_all(struct Screen *scrn)
{

}
