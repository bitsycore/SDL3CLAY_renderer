#include "uuid.h"

#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

#include "error_handling.h"

static bool RNG_INIT = false;
static uint32_t SEED;
static bool D = false;

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
        SEED = (uint32_t)time(NULL);
        SEED ^= (uint32_t)clock();
        RNG_INIT = true;
    }
}

void UUID_setDebug(const bool value) {
    D = value;
}

UUID UUID_new() {
    UUID uuid;

    initRandom();

    const uint32_t r1 = xorshift32(&SEED);
    const uint32_t r2 = xorshift32(&SEED);
    const uint32_t r3 = xorshift32(&SEED);
    const uint32_t r4 = xorshift32(&SEED);

    memcpy(uuid.data, &r1, 4);
    memcpy(uuid.data + 4, &r2, 4);
    memcpy(uuid.data + 8, &r3, 4);
    memcpy(uuid.data + 12, &r4, 4);

    uuid.data[6] = uuid.data[6] & 0x0f | 0x40;
    uuid.data[8] = uuid.data[8] & 0x3f | 0x80;

    return uuid;
}

bool UUID_equal(const UUID uuid1, const UUID uuid2) {
    return memcmp(uuid1.data, uuid2.data, UUID_DATA_LENGTH) == 0;
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

UUID_Result UUID_fromString(const char *string_uuid, UUID *out_uuid) {
    if (string_uuid == NULL) {
        if (D)
            WARN("Parameter \"string_uuid\" is NULL");
        return UUID_INPUT_BUFFER_NULL;
    }

    if (out_uuid == NULL) {
        if (D)
            WARN("Parameter \"uuid\" is NULL");
        return UUID_INPUT_UUID_NULL;
    }

    char *end_ptr;
    int data_index = 0;

    for (int i = 0; i < UUID_STRING_LENGTH - 1;) {
        if (!isxdigit(string_uuid[i]) && (i != 8 && i != 13 && i != 18 && i != 23)) {
            if (D)
                WARN_FORMAT("Invalid hex character: %c", string_uuid[i]);
            return UUID_INVALID_STRING_FORMAT;
        }

        if (string_uuid[i] != '-') {
            const char hex_str[3] = { string_uuid[i], string_uuid[i + 1], '\0' };
            out_uuid->data[data_index] = strtoul(hex_str, &end_ptr, 16);
            if (*end_ptr != '\0') {
                WARN_FORMAT("Conversion error for string %s", string_uuid);
                return UUID_INVALID_STRING_FORMAT;
            }
            data_index++;
            i += 2;
        } else {
            i++;
        }
    }

    return UUID_SUCCESS;
}