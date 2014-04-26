/*
 * config.h
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

#ifndef CONFIG_H
#define CONFIG_H

/*
 * New settings must be added to:
 * - This struct
 * - The 'settings_new' function
 * - The 'parse_config' function
 * - Optionally, the 'DEFAULT_CFG' macro
 * All but the struct are located in config.c
 */
struct Settings {
	int usetabs;
	int tabsize;
	int linnums;
	int bufexpg;
};

extern struct Settings *CFG;

int config_init();
int config_refresh();
int config_load_from_file(char *path);
void config_destroy();

#endif
