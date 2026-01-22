#include "jackc_string.h"

size_t jackc_strlen(const char* str) {
    size_t len = 0;
    while (*str++) {
        ++len;
    }
    return len;
}
