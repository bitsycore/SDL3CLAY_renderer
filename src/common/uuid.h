#ifndef UUID_H
#define UUID_H

#define UUID_LENGTH 16

typedef struct {
    unsigned char data[UUID_LENGTH];
} UUID;

typedef enum UUID_Result {
    UUID_SUCCESS = 0,
    UUID_INVALID_STRING_FORMAT = -1,
    UUID_INPUT_BUFFER_NULL = -2,
    UUID_INPUT_UUID_NULL = -3
} UUID_Result;

UUID UUID_new();
UUID_Result UUID_toString(UUID uuid, char *buffer);
UUID_Result UUID_fromString(const char *buffer, UUID *uuid);

#endif //UUID_H
