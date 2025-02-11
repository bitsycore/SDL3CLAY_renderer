#ifndef MEMORY_LEAK_H
#define MEMORY_LEAK_H

// ============================================
// MARK: LEAK DETECTOR
// ============================================

#include <stdlib.h>

#ifndef ML_SELECT_MALLOC
#define ML_SELECT_MALLOC malloc
#endif
#ifndef ML_SELECT_CALLOC
#define ML_SELECT_CALLOC calloc
#endif
#ifndef ML_SELECT_REALLOC
#define ML_SELECT_REALLOC realloc
#endif
#ifndef ML_SELECT_FREE
#define ML_SELECT_FREE free
#endif
#ifndef ML_SELECT_STRDUP
#define ML_SELECT_STRDUP strdup
#endif

#if defined(ENABLE_LEAK_DETECTOR) && ENABLE_LEAK_DETECTOR != 0

void* imp_ml_malloc(void* (*custom_malloc)(size_t), size_t size, const char* file, int line);
void* imp_ml_calloc(void* (*custom_calloc)(size_t, size_t), size_t num, size_t size, const char* file, int line);
void* imp_ml_realloc(void* (*custom_realloc)(void*, size_t), void* ptr, size_t size, const char* file, int line);
void imp_ml_free(void (*custom_free)(void*), void* ptr);
char* imp_ml_strdup(void* (*custom_malloc)(size_t), const char* s, const char* file, int line);

void* imp_ml_callback_malloc(size_t size);
void* imp_ml_callback_calloc(size_t num, size_t size);
void* imp_ml_callback_realloc(void* ptr, size_t size);
void imp_ml_callback_free(void* ptr);
char* imp_ml_callback_strdup(const char* s);

void imp_ml_print_memory_leaks();
void imp_ml_cleanup_memory_tracking();

#define ml_malloc(size) imp_ml_malloc(ML_SELECT_MALLOC, size, __FILE__, __LINE__)
#define ml_calloc(num, size) imp_ml_calloc(ML_SELECT_CALLOC, num, size, __FILE__, __LINE__)
#define ml_realloc(ptr, size) imp_ml_realloc(ML_SELECT_REALLOC, ptr, size, __FILE__, __LINE__)
#define ml_free(ptr) imp_ml_free(ML_SELECT_FREE, ptr)
#define ml_strdup(s) imp_ml_strdup(ML_SELECT_MALLOC, s, __FILE__, __LINE__)

#define ml_callback_malloc imp_ml_callback_malloc
#define ml_callback_calloc imp_ml_callback_calloc
#define ml_callback_realloc imp_ml_callback_realloc
#define ml_callback_free imp_ml_callback_free
#define ml_callback_strdup imp_ml_callback_strdup

#define ml_print_memory_leaks imp_ml_print_memory_leaks
#define ml_cleanup_memory_tracking imp_ml_cleanup_memory_tracking

#else

#define ml_malloc(size) ML_SELECT_MALLOC (size)
#define ml_calloc(count, size) ML_SELECT_CALLOC (count, size)
#define ml_realloc(ptr, size) ML_SELECT_REALLOC (ptr, size)
#define ml_free(ptr) ML_SELECT_FREE (ptr)
#define ml_strdup(str) ML_SELECT_STRDUP (str)

#define ml_callback_malloc ML_SELECT_MALLOC
#define ml_callback_calloc ML_SELECT_CALLOC
#define ml_callback_realloc ML_SELECT_REALLOC
#define ml_callback_free ML_SELECT_FREE
#define ml_callback_strdup ML_SELECT_STRDUP

#define ml_print_memory_leaks() ;NULL
#define ml_cleanup_memory_tracking() ;NULL

#endif

#endif //MEMORY_LEAK_H
