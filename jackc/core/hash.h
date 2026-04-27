#ifndef JACKC_HASH_H
#define JACKC_HASH_H

#include <stddef.h>
#include <stdint.h>

/**
 * Computes the OAT hash of a string.
 * Thanks: https://gist.github.com/sgsfak/9ba382a0049f6ee885f68621ae86079b
 *
 * @param s String to hash.
 * @param len Length of the string.
 * @return Hash value.
 */
uint32_t oat_hash(const char *s, size_t len);

#endif
