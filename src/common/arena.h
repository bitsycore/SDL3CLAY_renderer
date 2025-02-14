#ifndef ARENA2_H
#define ARENA2_H

#include <stdlib.h>
#include <stddef.h>

#include "error_handling.h"

typedef struct {
	size_t capacity;
	void* current;
	void* buf;
} Arena;

size_t Arena_requiredSize(size_t capacity);

Arena* Arena_init(void* buffer, size_t capacity);

void Arena_reset(Arena* arena);

void* Arena_alloc(Arena* arena, size_t size);

#endif
