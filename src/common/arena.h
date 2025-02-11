//
// Created by Doge on 07/02/2025.
//

#ifndef AREA_H
#define AREA_H

#include <stdbool.h>
#include <stddef.h>

// =========================================
// MARK: CONFIG
// =========================================

#define ARENA_DEFAULT_SIZE (1024 * 1024 * 4)

// =========================================
// MARK: Type Definitions
// =========================================

typedef void* (*ArenaAllocFunc)(size_t size);
typedef void (*ArenaFreeFunc)(void* ptr);

typedef struct Arena {
	void* start_ptr;
	void* next_ptr;
	void* end_ptr;
	size_t size;

	bool expandable;
	ArenaAllocFunc alloc_func;
	ArenaFreeFunc free_func;

	struct Arena* next_arena;
} Arena;

// =========================================
// MARK: Functions
// =========================================

Arena* arena_new(size_t size);
Arena* arena_new_custom(size_t size, ArenaAllocFunc alloc_cb, ArenaFreeFunc free_cb, bool expandable);
void arena_destroy(Arena** arena);

void* arena_global_alloc(size_t size);
Arena* arena_global_get();
void arena_global_destroy();

void* arena_alloc(Arena* arena, size_t size);
void* arena_alloc_align(Arena* arena, size_t size, size_t alignment);
void arena_reset(Arena* arena);

#endif //AREA_H
