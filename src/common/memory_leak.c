#include "memory_leak.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "error_handling.h"

typedef struct AllocationInfo {
	void* address;
	size_t size;
	const char* file;
	int line;
	struct AllocationInfo* next;
} AllocationInfo;

static AllocationInfo* ALLOCATION_LIST = NULL;

static void add_allocation(void* address, const size_t size, const char* file, const int line) {
	AllocationInfo* new_allocation = malloc(sizeof(AllocationInfo));
	if (new_allocation == NULL) {
		WARN("Error: Failed to allocate memory for allocation tracking!\n");
		return;
	}
	new_allocation->address = address;
	new_allocation->size = size;
	new_allocation->file = file;
	new_allocation->line = line;
	new_allocation->next = ALLOCATION_LIST;
	ALLOCATION_LIST = new_allocation;
}

static void remove_allocation(const void* address) {
	AllocationInfo* current = ALLOCATION_LIST;
	AllocationInfo* previous = NULL;

	while (current != NULL) {
		if (current->address == address) {
			if (previous == NULL) {
				ALLOCATION_LIST = current->next;
			} else {
				previous->next = current->next;
			}
			free(current);
			return;
		}
		previous = current;
		current = current->next;
	}
}

// =========================================
// MARK: Advance Implementation for Macro use
// =========================================

void* imp_ml_malloc(void* (*custom_malloc)(size_t), const size_t size, const char* file, const int line) {
	void* ptr = custom_malloc(size);
	if (ptr != NULL) {
		add_allocation(ptr, size, file, line);
	}
	return ptr;
}

void* imp_ml_calloc(void* (*custom_calloc)(size_t, size_t), const size_t num, const size_t size, const char* file,
                    const int line) {
	void* ptr = custom_calloc(num, size);
	if (ptr != NULL) {
		const size_t total_size = num * size;
		add_allocation(ptr, total_size, file, line);
	}
	return ptr;
}

void* imp_ml_realloc(void* (*custom_realloc)(void*, size_t), void* ptr, const size_t size, const char* file,
                     const int line) {
	void* new_ptr = custom_realloc(ptr, size);
	if (new_ptr != NULL) {
		if (ptr != NULL) {
			remove_allocation(ptr);
		}

		add_allocation(new_ptr, size, file, line);
	}
	return new_ptr;
}

void imp_ml_free(void (*custom_free)(void*), void* ptr) {
	if (ptr != NULL) {
		const AllocationInfo* current = ALLOCATION_LIST;
		while (current != NULL) {
			if (current->address == ptr) {
				break;
			}
			current = current->next;
		}
		remove_allocation(ptr);
		custom_free(ptr);
	}
}

char* imp_ml_strdup(void* (*custom_malloc)(size_t), const char* s, const char* file, const int line) {
	if (s == NULL) return NULL;
	const size_t len = strlen(s) + 1;
	char* new_str = imp_ml_malloc(custom_malloc, len, file, line);
	if (new_str != NULL) {
		memcpy(new_str, s, len);
	}
	return new_str;
}

// =========================================
// MARK: Simple Callbacks
// =========================================

void* imp_ml_callback_malloc(size_t size) {
	return imp_ml_malloc(ML_SELECT_MALLOC, size, __FILE__, __LINE__);
}

void* imp_ml_callback_calloc(size_t num, size_t size) {
	return imp_ml_calloc(ML_SELECT_CALLOC, num, size, __FILE__, __LINE__);
}

void* imp_ml_callback_realloc(void* ptr, size_t size) {
	return imp_ml_realloc(ML_SELECT_REALLOC, ptr, size, __FILE__, __LINE__);
}

void imp_ml_callback_free(void* ptr) {
	imp_ml_free(ML_SELECT_FREE, ptr);
}

char* imp_ml_callback_strdup(const char* s) {
	// ReSharper disable once CppDFAMemoryLeak
	return imp_ml_strdup(ML_SELECT_MALLOC, s, __FILE__, __LINE__);
}

// =========================================
// MARK: Print
// =========================================

void imp_ml_print_memory_leaks() {
	const AllocationInfo* current = ALLOCATION_LIST;

	if (current == NULL) {
		return;
	}

	size_t total_leaked_memory = 0;
	size_t number_leaked_block = 0;

	printf("\n--------------------- [MEMORY LEAK DETECTED] ---------------------\n\n");

	while (current != NULL) {
		char size_str[11];
		static const int GB = 1024 * 1024 * 1024;
		static const int MB = 1024 * 1024;
		static const int KB = 1024;
		if (current->size >= GB) {
			snprintf(size_str, sizeof(size_str), "%.2f GB", (double) current->size / GB);
		} else if ((double) current->size >= MB) {
			snprintf(size_str, sizeof(size_str), "%.2f MB", (double) current->size / MB);
		} else if ((double) current->size >= KB) {
			snprintf(size_str, sizeof(size_str), "%.2f KB", (double) current->size / KB);
		} else {
			snprintf(size_str, sizeof(size_str), "%zu Bytes", current->size);
		}

		printf(
			"[%zu]    0x%p    %-11s    %s:%d\n",
			number_leaked_block,
			current->address, size_str,
			current->file, current->line
		);

		total_leaked_memory += current->size;
		current = current->next;
		number_leaked_block++;
	}

	printf(
		"\n"
		"------------------------------------------------------------------\n"
		"Blocks: %zu\t%zu bytes (%.2f KB, %.2f MB)\n"
		"------------------------------------------------------------------\n",
		number_leaked_block, total_leaked_memory, (double) total_leaked_memory / 1024.0,
		(double) total_leaked_memory / (1024.0 * 1024.0)
	);
}

void imp_ml_cleanup_memory_tracking() {
	AllocationInfo* current = ALLOCATION_LIST;

	while (current != NULL) {
		AllocationInfo* next = current->next;
		free(current);
		current = next;
	}

	ALLOCATION_LIST = NULL;
}
