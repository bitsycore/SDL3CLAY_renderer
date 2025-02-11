#include "hash.h"

#include <stdint.h>
#include <string.h>

uint32_t hash_fnv1a(const char* str, const uint32_t table_size) {
	uint32_t hash_value = 2166136261u;
	while (*str) {
		hash_value ^= (unsigned char) *str++;
		hash_value *= 16777619;
	}
	return hash_value % table_size;
}

uint32_t hash_fnv1a_optimized(const char* str, const uint32_t table_size, const uint32_t sample_size) {
	uint32_t hash_value = 2166136261u;

	// Hash first sample characters
	for (size_t i = 0; i < sample_size && *str; i++) {
		hash_value ^= (unsigned char) *str++;
		hash_value *= 16777619;
	}

	// Skip to the last "sample" characters if the string is longer
	if (*str) {
		const char* end = str;
		size_t length = 0;
		while (*end++) length++;
		if (length > sample_size) {
			str += length - sample_size;
		}

		// Hash last "sample" characters
		while (*str) {
			hash_value ^= (unsigned char) *str--;
			hash_value *= 16777619;
		}
	}

	return hash_value % table_size;
}

uint32_t hash_djb2(const char* str, const uint32_t table_size) {
	uint32_t hash_value = 5381;
	while (*str) {
		hash_value = (hash_value << 5) + hash_value + *str++; // hash * 33 + char
	}
	return hash_value % table_size;
}

uint32_t hash_sdbm(const char* str, const uint32_t table_size) {
	uint32_t hash_value = 0;
	while (*str) {
		hash_value = *str++ + (hash_value << 6) + (hash_value << 16) - hash_value;
	}
	return hash_value % table_size;
}

uint32_t hash_murmur3_32(const char* str, const uint32_t table_size) {
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	static const uint32_t r1 = 15;
	static const uint32_t r2 = 13;
	static const uint32_t m = 5;
	static const uint32_t n = 0xe6546b64;

	uint32_t hash = 0;
	const uint32_t len = strlen(str);
	const uint8_t* data = (const uint8_t*) str;
	const uint32_t nb_blocks = len / 4;

	// Process full 4-byte chunks
	const uint32_t* blocks = (const uint32_t*) data;
	for (int i = 0; i < nb_blocks; i++) {
		uint32_t k = blocks[i];
		k *= c1;
		// ReSharper disable once CppRedundantParentheses
		k = k << r1 | k >> (32 - r1);
		k *= c2;

		hash ^= k;
		// ReSharper disable once CppRedundantParentheses
		hash = hash << r2 | hash >> (32 - r2);
		hash = hash * m + n;
	}

	// Handle remaining bytes
	const uint8_t* tail = data + nb_blocks * 4;
	uint32_t k = 0;
	switch (len & 3) {
		case 3: k ^= (uint32_t) (tail[2] << 16);
		case 2: k ^= (uint32_t) (tail[1] << 8);
		case 1: k ^= (uint32_t) tail[0];
			k *= c1;
			// ReSharper disable once CppRedundantParentheses
			k = k << r1 | k >> (32 - r1);
			k *= c2;
			hash ^= k;
		default:;
	}

	// Finalization
	hash ^= len;
	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;

	return hash % table_size;
}
