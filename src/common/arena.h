#ifndef ARENA2_H
#define ARENA2_H

#include <stdlib.h>

#include "../common/error_handling.h"

typedef struct {
	size_t capacity;
	uintptr_t current;
	uintptr_t buf;
} Arena;

#if !( defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L )
typedef union {
	long long ll;
	long double ld;
	void* p;
} max_align_t;
#else
#include <stddef.h>
#endif

#define Arena_requiredSize(size) (sizeof(Arena) + size)
#define Arena_init(buffer, size) ({ \
	Arena* arena = (buffer); \
	EXIT_IF(\
		(size) <= sizeof(Arena),\
		"Capacity must be greater than the size of Arena, "\
		"use Arena_requiredSize to calculate the required size"\
	);\
	arena->capacity = (size) - sizeof(Arena); \
	arena->current = (uintptr_t)arena + sizeof(Arena); \
	arena->buf = arena->current; \
	(void*)arena; \
})

#define Arena_alloc(arena, size) ({ \
	EXIT_IF(arena == NULL, "Arena is NULL"); \
	EXIT_IF(size == 0, "Size must be greater than 0"); \
	const uintptr_t aligned_current = arena->current + (sizeof(max_align_t) - 1) & ~(sizeof(max_align_t) - 1);\
	arena->buf + arena->capacity - aligned_current < size\
		? NULL\
		: ({\
			arena->current = aligned_current + size;\
			(void*)aligned_current;\
		});\
})

#define Arena_reset(arena) EXIT_IF(arena == NULL, "Arena is NULL");\
	arena->current = arena->buf

#endif