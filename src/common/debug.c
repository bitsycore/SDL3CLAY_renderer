#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =====================================================================================================================
#ifdef _WIN32 // MARK: WINDOWS
// =====================================================================================================================

// =====================================================================================================================
#ifdef _MSC_VER // MARK: Visual Studio
// =====================================================================================================================

#include <windows.h>
#include <dbghelp.h>

char* print_call_stack() {
	void* stack[100];
	void* process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);

	const unsigned short frames = CaptureStackBackTrace(1, 100, stack, NULL);
	SYMBOL_INFO* symbol = calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	// Allocate initial buffer
	size_t buffer_size = 1024;
	char* buffer = malloc(buffer_size);
	if (!buffer) {
		free(symbol);
		return NULL;
	}
	buffer[0] = '\0';

	for (unsigned int i = 0; i < frames; i++) {
		SymFromAddr(process, (DWORD64) (stack[i]), 0, symbol);
		size_t needed = snprintf(NULL, 0, "  %i: %s - 0x%0X\n", frames - i - 1, symbol->Name,
		                         (unsigned long) symbol->Address) + 1;
		if (strlen(buffer) + needed >= buffer_size) {
			buffer_size *= 2;
			char* temp = realloc(buffer, buffer_size);
			if (!temp) {
				free(symbol);
				free(buffer);
				return NULL;
			}
			buffer = temp;
		}
		snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "  %i: %s - 0x%0X\n", frames - i - 1,
		         symbol->Name, (unsigned long) symbol->Address);
	}

	free(symbol);
	return buffer;
}

// =====================================================================================================================
#else // MARK: MinGW
// =====================================================================================================================

char* print_call_stack() {
	const char msg[] = "  Unavailable on MinGW\n";
	char* heap_msg = malloc(strlen(msg) + 1);
	if (!heap_msg) {
		return NULL;
	}
	return strcpy(heap_msg, msg);
}

// =====================================================================================================================
#endif
// =====================================================================================================================

// =====================================================================================================================
#else // MARK: UNIX
// =====================================================================================================================

#include <execinfo.h>
#include <unistd.h>

char* print_call_stack() {
	void* stack[100];
	int frames = backtrace(stack, 100);
	char** symbols = backtrace_symbols(stack, frames);

	if (!symbols) {
		return NULL; // backtrace_symbols failed
	}

	size_t buffer_size = 1024;
	char* buffer = (char*)malloc(buffer_size);
	if (!buffer) {
		free(symbols);
		return NULL;
	}
	buffer[0] = '\0';

	for (int i = 0; i < frames; i++) {
		size_t needed = snprintf(NULL, 0, "  %d: %s\n", frames - i - 1, symbols[i]) + 1;
		if (strlen(buffer) + needed >= buffer_size) {
			buffer_size *= 2;
			char* temp = (char*)realloc(buffer, buffer_size);
			if (!temp) {
				free(symbols);
				free(buffer);
				return NULL;
			}
			buffer = temp;
		}
		snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), "  %d: %s\n", frames - i - 1, symbols[i]);
	}

	free(symbols);
	return buffer;
}

#ifdef CLEAN_GNU
#undef __USE_GNU
#endif

// =====================================================================================================================
#endif
// =====================================================================================================================
