#ifndef ARENA2_H
#define ARENA2_H

#include <stdlib.h>
#include <stddef.h>

#include "error_handling.h"

typedef struct {
	size_t capacity;
	void* current;
	void* buf;
} arena_t;

static inline size_t arena_required_size(const size_t capacity) {
	return sizeof(arena_t) + capacity;
}

static inline arena_t* arena_init(void* buffer, const size_t capacity) {
	EXIT_IF_NOT(
		capacity > sizeof(arena_t),
		"Capacity must be greater than the size of arena_t, "
		"use arena_required_size to calculate the required size"
	);

	arena_t* arena = buffer;

	arena->capacity = capacity - sizeof(arena_t);
	arena->buf = (char*)buffer + sizeof(arena_t);
	arena->current = arena->buf;

	return arena;
}

static inline void arena_reset(arena_t* arena) {
	EXIT_IF_NOT(arena != NULL, "Arena is NULL");
	arena->current = arena->buf;
}

static inline void* arena_alloc(arena_t* arena, const size_t size) {
	EXIT_IF_NOT(arena != NULL, "Arena is NULL");
	EXIT_IF_NOT(size > 0, "Size must be greater than 0");

	const ptrdiff_t remaining = (char*)arena->buf + arena->capacity - (char*)arena->current;

	if ((size_t)remaining < size) {
		return NULL;
	}

	void* ptr = arena->current;
	arena->current = (char*)arena->current + size;
	return ptr;
}

#endif