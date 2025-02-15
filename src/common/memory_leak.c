#include "memory_leak.h"

#if defined(ENABLE_LEAK_DETECTOR) && ENABLE_LEAK_DETECTOR != 0

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "error_handling.h"

typedef struct AllocationInfo {
	void* address;
	size_t size;
	const char* file;
	int line;
} AllocationInfo;

#define ALLOCATION_INFO_DEFAULT_SIZE 256
#define ALLOCATION_INFO_GROWTH_FACTOR 2

static AllocationInfo* ALLOCATION_LIST = NULL;
static size_t ALLOCATION_LIST_SIZE = 0;
static size_t ALLOCATION_LIST_CAPACITY = 0;

static void add_allocation(void* address, const size_t size, const char* file, const int line) {
    if (ALLOCATION_LIST == NULL) {
        ALLOCATION_LIST = malloc(sizeof(AllocationInfo) * ALLOCATION_INFO_DEFAULT_SIZE);
        if (ALLOCATION_LIST == NULL) {
            EXIT("Error: Failed to allocate initial memory for allocation tracking!\n");
        }
        ALLOCATION_LIST_CAPACITY = ALLOCATION_INFO_DEFAULT_SIZE;
        ALLOCATION_LIST_SIZE = 0;
    }

    if (ALLOCATION_LIST_SIZE >= ALLOCATION_LIST_CAPACITY) {
        size_t new_capacity = ALLOCATION_LIST_CAPACITY * ALLOCATION_INFO_GROWTH_FACTOR;
        AllocationInfo* new_list = realloc(ALLOCATION_LIST, sizeof(AllocationInfo) * new_capacity);

        if (new_list == NULL) {
            free(ALLOCATION_LIST);
            ALLOCATION_LIST = NULL; // Important to set to NULL after freeing.
            ALLOCATION_LIST_CAPACITY = 0;
            ALLOCATION_LIST_SIZE = 0;
            EXIT("Error: Failed to reallocate memory for allocation tracking!\n");
        }
        ALLOCATION_LIST = new_list;
        ALLOCATION_LIST_CAPACITY = new_capacity;
    }

    // Now that reallocation is handled, we can confidently add the new allocation
    ALLOCATION_LIST[ALLOCATION_LIST_SIZE].address = address;
    ALLOCATION_LIST[ALLOCATION_LIST_SIZE].size = size;
    ALLOCATION_LIST[ALLOCATION_LIST_SIZE].file = file;
    ALLOCATION_LIST[ALLOCATION_LIST_SIZE].line = line;
    ALLOCATION_LIST_SIZE++;
}

static void remove_allocation(const void* address) {
	for (size_t i = 0; i < ALLOCATION_LIST_SIZE; i++) {
		if (ALLOCATION_LIST[i].address == address) {
			// Move the last element to the current position if not already the last element
			if (i < ALLOCATION_LIST_SIZE - 1) {
				ALLOCATION_LIST[i] = ALLOCATION_LIST[ALLOCATION_LIST_SIZE - 1];
			}
			ALLOCATION_LIST_SIZE--;
			return;
		}
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
	if (ptr != NULL && ALLOCATION_LIST_SIZE > 0) {
		for (int i = 0; i < ALLOCATION_LIST_SIZE; i++) {
			if (ALLOCATION_LIST[i].address == ptr) {
				remove_allocation(ptr);
				break;
			}
		}
	}
	custom_free(ptr);
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

void imp_ml_cleanup_memory_tracking() {
	if (ALLOCATION_LIST != NULL) {
		free(ALLOCATION_LIST);
		ALLOCATION_LIST = NULL;
		ALLOCATION_LIST_SIZE = 0;
		ALLOCATION_LIST_CAPACITY = 0;
	}
}

void imp_ml_print_memory_leaks() {
	if (ALLOCATION_LIST == NULL || ALLOCATION_LIST_SIZE == 0) {
		return;
	}

	size_t total_leaked_memory = 0;
	size_t number_leaked_block = 0;

	printf("\n--------------------- [MEMORY LEAK DETECTED] ---------------------\n\n");

	for (size_t i = 0; i < ALLOCATION_LIST_SIZE; i++) {
		char size_str[20];
		static const size_t GB = 1024 * 1024 * 1024;
		static const size_t MB = 1024 * 1024;
		static const size_t KB = 1024;

		if (ALLOCATION_LIST[i].size >= GB) {
			snprintf(size_str, sizeof(size_str), "%.2f GB", (double)ALLOCATION_LIST[i].size / (double)GB);
		} else if (ALLOCATION_LIST[i].size >= MB) {
			snprintf(size_str, sizeof(size_str), "%.2f MB", (double)ALLOCATION_LIST[i].size / (double)MB);
		} else if (ALLOCATION_LIST[i].size >= KB) {
			snprintf(size_str, sizeof(size_str), "%.2f KB", (double)ALLOCATION_LIST[i].size / (double)KB);
		} else {
			snprintf(size_str, sizeof(size_str), "%zu Bytes", ALLOCATION_LIST[i].size);
		}

		printf(
			"[%zu]    0x%p    %-11s    %s:%d\n", i,
			ALLOCATION_LIST[i].address, size_str,
			ALLOCATION_LIST[i].file, ALLOCATION_LIST[i].line
		);

		total_leaked_memory += ALLOCATION_LIST[i].size;
		number_leaked_block++;
	}

	printf(
		"\n"
		"------------------------------------------------------------------\n"
		"Blocks: %zu\t%zu bytes (%.2f KB, %.2f MB)\n"
		"------------------------------------------------------------------\n",
		number_leaked_block, total_leaked_memory, (float)total_leaked_memory / 1024.0,
		(float)total_leaked_memory / (1024.0 * 1024.0)
	);

	imp_ml_cleanup_memory_tracking();
}

#endif