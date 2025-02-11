#ifndef HASH_H
#define HASH_H

#include <stdint.h>

uint32_t hash_fnv1a(const char* str, uint32_t table_size);

uint32_t hash_fnv1a_optimized(const char* str, uint32_t table_size, uint32_t sample_size);

uint32_t hash_djb2(const char* str, uint32_t table_size);

uint32_t hash_sdbm(const char* str, uint32_t table_size);

uint32_t hash_murmur3_32(const char* str, uint32_t table_size);

#endif //HASH_H
