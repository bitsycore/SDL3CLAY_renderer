#ifndef ARENA2_H
#define ARENA2_H

#include <stdlib.h>
#include <stddef.h>

#include "../common/error_handling.h"

typedef struct {
	size_t capacity;
	void* current;
	void* buf;
} Arena;

#define Arena_requiredSize(size) (sizeof(Arena) + size)

#define Arena_init(buffer, size) ({ \
	Arena* arena = (Arena*)(buffer); \
	EXIT_IF(\
		(size) <= sizeof(Arena),\
		"Capacity must be greater than the size of Arena, "\
		"use Arena_requiredSize to calculate the required size"\
	);\
	arena->capacity = (size) - sizeof(Arena); \
	arena->current = (char*)arena + sizeof(Arena); \
	arena->buf = arena->current; \
	arena; \
})

#define Arena_alloc(arena, size) ({ \
	EXIT_IF(arena == NULL, "Arena is NULL"); \
	EXIT_IF(size <= 0, "Size must be greater than 0"); /*NOLINT(*-sizeof-expression)*/ \
	void* aligned_current = (void*)((uintptr_t)arena->current + (sizeof(max_align_t) - 1) & ~(sizeof(max_align_t) - 1));\
	arena->buf + arena->capacity - aligned_current < size\
		? NULL\
		: ({\
			void* ptr = aligned_current;\
			arena->current = (char*) aligned_current + size;\
			ptr;\
		});\
})

#define Arena_reset(arena) EXIT_IF(arena == NULL, "Arena is NULL");\
	arena->current = arena->buf

#endif