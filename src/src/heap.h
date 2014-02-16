/* heap.h - Abstract heap type
 * Copyright 2000-2001 srvx Development Team
 *
 * This file is part of srvx.
 *
 * srvx is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with srvx; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifndef HEAP_H
#define HEAP_H

typedef int (*comparator_f)(const void *a, const void *b);

/* a heap is implemented using a dynamically sized array */
typedef struct heap *heap_t;

/* operations on a heap */
heap_t heap_new(comparator_f comp);
void heap_insert(heap_t heap, void *key, void *data);
void heap_peek(heap_t heap, void **key, void **data);
void heap_pop(heap_t heap);
void heap_delete(heap_t heap);
unsigned int heap_size(heap_t heap);
int heap_remove_pred(heap_t heap, int (*pred)(void *key, void *data, void *extra), void *extra);

/* useful comparators */

/* int strcmp(const char *s1, const char *s2); from <string.h> can be used */
int ulong_comparator(const void*, const void*);

#endif /* ndef HEAP_H */
