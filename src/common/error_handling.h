#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

#define EH_STRINGIFY(x) #x

// ================================
// MARK: Warnings
// ================================

#define WARN_FORMAT(msg, ...) do { \
    char* callstack__ = print_call_stack();\
    fprintf(stderr, "\n---- [WARNING] ----\nInfo: " msg "\nFile: %s:%d\nFunction: %s\nCallstack:\n%s-------------------\n", __VA_ARGS__, __FILE__, __LINE__, __func__, callstack__); \
    free(callstack__); \
} while (0)

#define WARN(msg) do { \
    char* callstack__ = print_call_stack();\
    fprintf(stderr, "\n---- [WARNING] ----\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack:\n%s-------------------\n", msg, __FILE__, __LINE__, __func__, callstack__); \
    free(callstack__); \
} while (0)

#define WARN_IF(expr, msg) do { \
    if ((expr)) { \
        char* callstack__ = print_call_stack();\
        fprintf(stderr, "\n---- [WARNING] ----\nExpression: %s\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack:\n%s-------------------\n", EH_STRINGIFY(expr), msg, __FILE__, __LINE__, __func__, callstack__); \
        free(callstack__); \
    } \
} while (0)

#define WARN_IF_NOT(expr, msg) do { \
    if (!(expr)) { \
        char* callstack__ = print_call_stack();\
        fprintf(stderr, "\n---- [WARNING] ----\nExpression: %s\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack:\n%s-------------------\n", EH_STRINGIFY(expr), msg, __FILE__, __LINE__, __func__, callstack__); \
        free(callstack__); \
    } \
} while (0)

#define WARN_IF_EQUAL(expected, actual, msg) do { \
    if ((expected) == (actual)) { \
        char* callstack__ = print_call_stack();\
        fprintf(stderr, "\n---- [WARNING] ----\n%s\nEQUAL\n%s\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack:\n%s-------------------\n", \
        EH_STRINGIFY(expected), EH_STRINGIFY(actual), msg, __FILE__, __LINE__, __func__, callstack__); \
        free(callstack__); \
    } \
} while (0)

#define WARN_IF_NOT_EQUAL(expected, actual, msg) do { \
    if ((expected) != (actual)) { \
        char* callstack__ = print_call_stack();\
        fprintf(stderr, "\n---- [WARNING] ----\n%s\nNOT_EQUAL\n%s\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack:\n%s-------------------\n", \
        EH_STRINGIFY(expected), EH_STRINGIFY(actual), msg, __FILE__, __LINE__, __func__, callstack__); \
        free(callstack__); \
    } \
} while (0)

// =================================
// MARK: Critical Errors
// =================================

#define EXIT(msg) do { \
    char* callstack__ = print_call_stack();\
    fprintf(stderr, "\n----- [ERROR] -----\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack\n%s-------------------\n", msg, __FILE__, __LINE__, __func__, callstack__); \
    free(callstack__); \
    exit(EXIT_FAILURE); \
} while (0)

#define EXIT_IF(expr, msg) do { \
    if ((expr)) { \
        char* callstack__ = print_call_stack();\
        fprintf(stderr, "\n----- [ERROR] -----\nExpression: %s\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack\n%s-------------------\n", EH_STRINGIFY(expr), msg, __FILE__, __LINE__, __func__, callstack__); \
        free(callstack__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

#define EXIT_IF_NOT(expr, msg) do { \
    if (!(expr)) { \
        char* callstack__ = print_call_stack();\
        fprintf(stderr, "\n----- [ERROR] -----\nExpression: %s\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack\n%s-------------------\n", EH_STRINGIFY(expr), msg, __FILE__, __LINE__, __func__, callstack__); \
        free(callstack__); \
    exit(EXIT_FAILURE); \
    } \
} while (0)

#define EXIT_IF_NOT_EQUAL(expected, actual, msg) do { \
    if ((expected) != (actual)) { \
        char* callstack__ = print_call_stack(); \
        fprintf(stderr, "\n----- [ERROR] -----\n%s\nNOT_EQUAL\n%s\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack\n%s-------------------\n", \
        EH_STRINGIFY(expected), EH_STRINGIFY(actual), msg, __FILE__, __LINE__, __func__, callstack__); \
        free(callstack__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

#define EXIT_IF_EQUAL(expected, actual, msg) do { \
if ((expected) == (actual)) { \
        char* callstack__ = print_call_stack(); \
        fprintf(stderr, "\n----- [ERROR] -----\n%s\nEQUAL\n%s\nInfo: %s\nFile: %s:%d\nFunction: %s\nCallstack\n%s-------------------\n", \
        EH_STRINGIFY(expected), EH_STRINGIFY(actual), msg, __FILE__, __LINE__, __func__, callstack__); \
        free(callstack__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

#endif //ERROR_HANDLING_H