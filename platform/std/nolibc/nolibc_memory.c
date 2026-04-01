#include "std/jackc_stdlib.h"
#include <stddef.h>

void* jackc_memcpy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void* jackc_memset(void* dest, int c, size_t n) {
    if (n != 0) {
		unsigned char *d = dest;
		do {
		    *d++ = (unsigned char)c;
		} while (--n != 0);
	}
	return dest;
}

// GCC can add implicit calls to memcpy / memset. Not meant to be used directly.
void* memcpy(void* dest, const void* src, size_t n) {
    return jackc_memcpy(dest, src, n);
}
void* memset(void* dest, int c, size_t n) {
    return jackc_memset(dest, c, n);
}
