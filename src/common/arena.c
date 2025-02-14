#include "arena.h"

#include <stdlib.h>
#include <stddef.h>

#include "error_handling.h"

size_t Arena_requiredSize(const size_t capacity) {
	return sizeof(Arena) + capacity;
}

Arena* Arena_init(void* buffer, const size_t capacity) {
	EXIT_IF_NOT(
		capacity > sizeof(Arena),
		"Capacity must be greater than the size of arena_t, "
		"use arena_required_size to calculate the required size"
	);

	Arena* arena = buffer;

	arena->capacity = capacity - sizeof(Arena);
	arena->buf = (char*) buffer + sizeof(Arena);
	arena->current = arena->buf;

	return arena;
}

void Arena_reset(Arena* arena) {
	EXIT_IF_NOT(arena != NULL, "Arena is NULL");
	arena->current = arena->buf;
}

void* Arena_alloc(Arena* arena, const size_t size) {
	EXIT_IF_NOT(arena != NULL, "Arena is NULL");
	EXIT_IF_NOT(size > 0, "Size must be greater than 0");

	const ptrdiff_t remaining = (char*) arena->buf + arena->capacity - (char*) arena->current;

	if ((size_t) remaining < size) {
		return NULL;
	}

	void* ptr = arena->current;
	arena->current = (char*) arena->current + size;
	return ptr;
}
