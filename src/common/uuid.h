#ifndef UUID_H
#define UUID_H

#include <stdbool.h>

/**
 * The length of the UUID data in bytes
 */
#define UUID_DATA_LENGTH 16

/**
 * The length of the UUID string representation (including null terminator)
 */
#define UUID_STRING_LENGTH 37

/**
 * A UUID struct of 16 bytes
 */
typedef struct {
    unsigned char data[UUID_DATA_LENGTH];
} UUID;

/**
 * Result codes for UUID convertion functions
 */
typedef enum UUID_Result {
    UUID_SUCCESS = 0,
    UUID_INVALID_STRING_FORMAT = -1,
    UUID_INPUT_BUFFER_NULL = -2,
    UUID_INPUT_UUID_NULL = -3,
    UUID_CONVERSION_ERROR = -4,
} UUID_Result;

/**
 * Generate a new UUID
 * @return A new UUID
 */
UUID UUID_new();

/**
 * Compare two UUIDs
 * @param uuid1 first uuid to compare
 * @param uuid2 second uuid to compare
 * @return true if the UUIDs are equal, false otherwise
 */
bool UUID_equal(UUID uuid1, UUID uuid2);

/**
 *  Convert a UUID to a string
 * @param uuid The UUID to convert
 * @param buffer A buffer of at least 37 characters (UUID_STRING_LENGTH) to store the null terminated string
 * representation of the UUID
 * @return UUID_SUCCESS if the UUID was successfully converted, an error code otherwise
 */
UUID_Result UUID_toString(UUID uuid, char *buffer);

/**
 * Parse a UUID from a string
 * @param string_uuid A string representation of a UUID of at least 36 characters
 * @param out_uuid A pointer to a UUID struct that will be filled with the parsed UUID
 * @return UUID_SUCCESS if the UUID was successfully parsed, an error code otherwise
 */
UUID_Result UUID_fromString(const char *string_uuid, UUID *out_uuid);

/**
 * Set the debug mode for the UUID module, this toggle the warning messages.
 * @param value true to enable debug mode, false to disable
 */
void UUID_setDebug(bool value);

#endif //UUID_H
