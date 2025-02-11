#include "arena.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "error_handling.h"
#include "memory_leak.h"

// =========================================
// MARK: Macro Helpers
// =========================================

#if defined(__STDALIGN_H__) && defined(max_align_t)
    #include <stdalign.h>
    #define ARENA_MAX_ALIX alignof(max_align_t);
#else
#define ARENA_MAX_ALIX sizeof(void*);
#endif

// =========================================
// MARK: Global Arena
// =========================================

static Arena* GLOBAL_ARENA = NULL;

// =========================================
// MARK: Default Allocators
// =========================================

ArenaAllocFunc DEFAULT_ALLOC = ml_callback_malloc;
ArenaFreeFunc DEFAULT_FREE = ml_callback_free;

// =========================================
// MARK: Internal
// =========================================

static Arena* internal_arena_new_instance(
	const size_t size,
	const ArenaAllocFunc alloc_cb,
	const ArenaFreeFunc free_cb,
	const bool expandable
) {
	Arena* arena = alloc_cb ? alloc_cb(sizeof(Arena)) : DEFAULT_ALLOC(sizeof(Arena));

	if (!arena) {
		return NULL;
	}
	const size_t arena_size = size == 0 ? ARENA_DEFAULT_SIZE : size;
	arena->start_ptr = alloc_cb ? alloc_cb(arena_size) : DEFAULT_ALLOC(arena_size);

	if (!arena->start_ptr) {
		free_cb ? free_cb(arena) : DEFAULT_FREE(arena);
		return NULL;
	}

	arena->next_ptr = arena->start_ptr;
	arena->end_ptr = arena->start_ptr + arena_size;
	arena->size = arena_size;
	arena->alloc_func = alloc_cb ? alloc_cb : DEFAULT_ALLOC;
	arena->free_func = free_cb ? free_cb : DEFAULT_FREE;
	arena->next_arena = NULL;
	arena->expandable = expandable;

	return arena;
}

// =========================================
// MARK: Arena Management
// =========================================

Arena* arena_new(const size_t size) {
	return internal_arena_new_instance(size, NULL, NULL, false);
}


Arena* arena_new_custom(const size_t size, const ArenaAllocFunc alloc_cb, const ArenaFreeFunc free_cb, const bool expandable) {
	return internal_arena_new_instance(size, alloc_cb, free_cb, expandable);
}

void arena_destroy(Arena** arena) {
	if (!arena) return;
	if (!*arena) return;

	Arena* current_arena = *arena;
	while (current_arena) {
		Arena* next = current_arena->next_arena;
		current_arena->free_func(current_arena->start_ptr);
		current_arena->free_func(current_arena);
		current_arena = next;
	}
	*arena = NULL;
}

// =========================================
// MARK: Arena Global Function
// =========================================

void* arena_global_alloc(const size_t size) {
	return arena_alloc_align(arena_global_get(), size, 0);
}

Arena* arena_global_get() {
	if (!GLOBAL_ARENA) {
		GLOBAL_ARENA = internal_arena_new_instance(ARENA_DEFAULT_SIZE, NULL, NULL, true);
		if (GLOBAL_ARENA == NULL) {
			WARN("Error: Failed to initialize global arena.");
			return NULL;
		}
	}
	return GLOBAL_ARENA;
}

void arena_global_destroy() {
	if (GLOBAL_ARENA) {
		arena_destroy(&GLOBAL_ARENA);
	}
}

// =========================================
// MARK: Malloc
// =========================================

void* arena_alloc(Arena* arena, const size_t size) {
	return arena_alloc_align(arena, size, 0);
}

void* arena_alloc_align(Arena* arena, const size_t size, size_t alignment) {
	if (size == 0) return NULL;
	if (arena == NULL) arena = arena_global_get();
	if (arena == NULL) return NULL;

	const size_t default_alignment = ARENA_MAX_ALIX;

	// Ensure alignment is a power of 2
	if (alignment == 0) alignment = default_alignment;
	else if (alignment & (alignment - 1)) {
		WARN("Alignment must be a power of 2. Using Default Alignment.");
		alignment = default_alignment;
	}

	Arena* current_arena = arena;

	while (current_arena) {
		char* current_mem = current_arena->next_ptr;
		const uintptr_t ptr_val = (uintptr_t) current_mem;
		const size_t padding = ptr_val % alignment ? alignment - ptr_val % alignment : 0;
		char* aligned_ptr = current_mem + padding;

		// If allocation fits within this arena
		if (aligned_ptr + size <= (char*) current_arena->end_ptr) {
			current_arena->next_ptr = aligned_ptr + size;
			return aligned_ptr;
		}

		// Move to next arena if available
		if (!current_arena->next_arena) break;
		current_arena = current_arena->next_arena;
	}


	// No space in existing arenas;
	// allocate a new linked one if authorized

	if (arena->expandable == false) return NULL;
	Arena* new_arena = internal_arena_new_instance(arena->size, arena->alloc_func, arena->free_func, arena->expandable);
	if (!new_arena) return NULL;

	arena->next_arena = new_arena;
	return arena_alloc_align(new_arena, size, alignment);
}

// =========================================
// MARK: Arena Cleanup
// =========================================

void arena_reset(Arena* arena) {
	if (!arena) return;
	Arena* current_arena = arena;
	while (current_arena) {
		current_arena->next_ptr = current_arena->start_ptr;
		current_arena = current_arena->next_arena;
	}
}
