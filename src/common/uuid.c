#include "uuid.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

static bool RNG_INIT = false;
static uint32_t seed;

// Xorshift PRNG
static uint32_t xorshift32(uint32_t* state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static void initRandom() {
    if (!RNG_INIT) {
        seed = (uint32_t)time(NULL);
        seed ^= (uint32_t)clock();
        RNG_INIT = true;
    }
}

UUID UUID_new() {
    UUID uuid;

    initRandom();

    const uint32_t r1 = xorshift32(&seed);
    const uint32_t r2 = xorshift32(&seed);
    const uint32_t r3 = xorshift32(&seed);
    const uint32_t r4 = xorshift32(&seed);

    memcpy(uuid.data, &r1, 4);
    memcpy(uuid.data + 4, &r2, 4);
    memcpy(uuid.data + 8, &r3, 4);
    memcpy(uuid.data + 12, &r4, 4);

    uuid.data[6] = uuid.data[6] & 0x0f | 0x40;
    uuid.data[8] = uuid.data[8] & 0x3f | 0x80;

    return uuid;
}

UUID_Result UUID_toString(const UUID uuid, char *buffer) {
    static const char hex_digits[] = "0123456789abcdef";
    const unsigned char *ptr = (unsigned char *) &uuid;
    char *buff_ptr = buffer;

    for (int i = 0; i < 16; i++) {
        const unsigned char val = *ptr++;
        *buff_ptr++ = hex_digits[val >> 4];
        *buff_ptr++ = hex_digits[val & 0x0f];
        if (i == 3 || i == 5 || i == 7 || i == 9) {
            *buff_ptr++ = '-';
        }
    }

    *buff_ptr = '\0';

    return UUID_SUCCESS;
}


UUID_Result UUID_fromString(const char *buffer, UUID *uuid) {
    if (buffer == NULL) {
        return UUID_INPUT_BUFFER_NULL;
    }

    if (uuid == NULL) {
        return UUID_INPUT_UUID_NULL;
    }

    const char *pos = buffer;
    char *end_ptr;

    for (int i = 0; i < UUID_LENGTH; ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10) {
            if (*pos != '-') {
                fprintf(stderr, "Invalid UUID format: Missing dash at position %d\n", i);
                return UUID_INVALID_STRING_FORMAT;
            }
            pos++;
        }

        errno = 0;
        const unsigned long value = strtoul(pos, &end_ptr, 16);

        if (errno != 0 || end_ptr != pos + 2) {
            fprintf(stderr, "Invalid UUID format: Invalid hex characters at position %d\n", i);
            return UUID_INVALID_STRING_FORMAT;
        }


        if (value > 255) {
            fprintf(stderr, "Invalid UUID format: Hex value out of range at position %d\n", i);
            return UUID_INVALID_STRING_FORMAT;
        }

        uuid->data[i] = (unsigned char)value;
        pos = end_ptr;
    }

    if (*pos != '\0') {
        fprintf(stderr, "Invalid UUID format: Extra characters at the end\n");
        return UUID_INVALID_STRING_FORMAT;
    }

    return UUID_SUCCESS;
}