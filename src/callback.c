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

#include "buffer.h"
#include "bufwin.h"
#include "vte/vte_driver.h"

#include <string.h>

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
	screen_save_current_buffer(scrn);
}

void cb_save_as(struct Screen *scrn)
{

}

void cb_new(struct Screen *scrn)
{
	screen_add_new_buffer(scrn);
}

void cb_open(struct Screen *scrn)
{
	screen_open_buffer_prompt(scrn);
}

void cb_close_buffer(struct Screen *scrn)
{
	screen_close_current_buffer(scrn);
}

void cb_visit_buf(struct Screen *scrn)
{
	screen_set_flag(scrn, SF_BUF);
}

void cb_visit_cli(struct Screen *scrn)
{
	screen_set_flag(scrn, SF_CLI);
}

void cb_visit_term(struct Screen *scrn)
{
	screen_set_flag(scrn, SF_TERM);
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
