/*
 * ltype.h
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

#ifndef LTYPE_H
#define LTYPE_H

#include <wchar.h>

typedef enum {
	LT_INT,
	LT_DBL,
	LT_NIL,
	LT_OTHER,
} LispType;

unsigned int is_an_int(const wchar_t *atom);
unsigned int is_a_double(const wchar_t *atom);
unsigned int is_nil(const wchar_t *atom);

inline unsigned int is_a_number(const wchar_t *atom)
{
	return (is_an_int(atom) | is_a_double(atom));
}

inline unsigned int is_t(const wchar_t *atom)
{
	return !(is_nil(atom));
}

#endif
