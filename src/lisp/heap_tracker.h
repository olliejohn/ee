/*
 * heap_tracker.h
 * Part of the Lisp subsystem in the Yaw text editor
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

#ifndef HEAP_TRACKER_H
#define HEAP_TRACKER_H

#include <wchar.h>

struct HeapTracker;

extern struct HeapTracker *HT;

struct HeapTracker *heap_tracker_new();
void heap_tracker_free(struct HeapTracker *ht);
void heap_tracker_add(struct HeapTracker *ht, wchar_t *pointer);
void heap_tracker_clean(struct HeapTracker *ht);

#endif
