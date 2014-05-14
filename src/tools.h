/*
 * tools.h
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

#ifndef TOOLS_H
#define TOOLS_H

#include "tui/tui.h"

/* Use extensions by default */
#ifndef USE_GCC_EXTENSIONS
#define USE_GCC_EXTENSIONS 1
#endif

#if (USE_GCC_EXTENSIONS)
	#define likely(x)	__builtin_expect(!!(x), 1)
	#define unlikely(x)	__builtin_expect(!!(x), 0)
#else
	#define likely(x)	x
	#define unlikely(x)	x
#endif /* USE_GCC_EXTENSIONS */

/* Debug dump macro */
#ifdef DEBUG
	/* This uses the TUI macro for passing 0 args to a variadic macro */
	#define DEBUG_DUMP(fmt, ...) t_fatal(fmt VA_ARGS(__VA_ARGS__))
#else /* DEBUG not defined */
	#define DEBUG_DUMP(fmt, ...)
#endif /* DEBUG */

/* Get number of elements in an array */
#define array_size(x) (sizeof(x) / sizeof((x)[0]))

/* Determine whether of not a value is set  */
#define is_set(macro) is_set_(macro)
#define macrotest_1 ,
#define is_set_(value) is_set__(macrotest_##value)
#define is_set_(comma) is_set__(comma 1, 0)
#define is_set___(_, v, ...) v

#endif /* TOOLS_H */
