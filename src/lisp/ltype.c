/*
 * ltype.c
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

#include "ltype.h"

#include <wctype.h>

unsigned int is_an_int(const wchar_t *atom)
{
	size_t len = wcslen(atom);

	unsigned int i;
	for (i = 0; i < len; i++)
		if (!iswdigit(atom[i]))
			return 1;

	return 0;
}

unsigned int is_a_double(const wchar_t *atom)
{
	size_t len = wcslen(atom);

	unsigned int i, dot = 0;
	for (i = 0; i < len; i++) {
		if (atom[i] == L'.') {
			if (dot)
				return 1;
			dot++;
		} else if (!iswdigit(atom[i])) {
			return 1;
		}
	}

	return 0;
}

unsigned int is_nil(const wchar_t *atom)
{
	if (wcscasecmp(atom, L"nil") == 0)
		return 1;

	if (wcscmp(atom, L"()") == 0)
		return 1;

	return 0;
}
