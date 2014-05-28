/*
 * heap_tracker.c
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

#include "heap_tracker.h"

#include <stdlib.h>

#define HEAP_TRACKER_INITIAL_SIZE 16

struct HeapTracker {
	wchar_t **allocs;
	unsigned int size;
	unsigned int capacity;
};

struct HeapTracker *heap_tracker_new()
{
	struct HeapTracker *ht = malloc(sizeof(struct HeapTracker));
	ht->size = 0;
	ht->capacity = HEAP_TRACKER_INITIAL_SIZE;
	ht->allocs = malloc(sizeof(wchar_t *) * ht->capacity);
	return ht;
}

void heap_tracker_free(struct HeapTracker *ht)
{
	free(ht->allocs);
	free(ht);
}

static void heap_tracker_double_capacity_if_full(struct HeapTracker *ht)
{
	if (ht->size >= ht->capacity - 1) {
		ht->capacity <<= 1;
		ht->allocs = realloc(ht->allocs,
				     sizeof(wchar_t *) * ht->capacity);
	}
}

void heap_tracker_add(struct HeapTracker *ht, wchar_t *pointer)
{
	heap_tracker_double_capacity_if_full(ht);
	ht->allocs[ht->size++] = pointer;
}

void heap_tracker_clean(struct HeapTracker *ht)
{

	unsigned int i;
	for (i = 0; i < ht->size; i++)
		free(ht->allocs[i]);

	heap_tracker_free(ht);
	HT = heap_tracker_new();
}
