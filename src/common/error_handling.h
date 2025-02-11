#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#define WARN(msg) do { \
    char* callstack__ = print_call_stack();\
    fprintf(stderr, "\n---- [WARNING] ----\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack:\n%s-------------------\n", msg, __FILE__, __LINE__, __func__, callstack__); \
    free(callstack__); \
} while (0)

#define EXIT_ERROR(msg) do { \
    char* callstack__ = print_call_stack();\
    fprintf(stderr, "\n----- [ERROR] -----\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack\n%s-------------------\n", msg, __FILE__, __LINE__, __func__, callstack__); \
    free(callstack__); \
    exit(EXIT_FAILURE); \
} while (0)

#endif //ERROR_HANDLING_H
