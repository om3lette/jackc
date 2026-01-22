#include "jackc_string.h"

size_t jackc_strlen(const char* str) {
    const char* str_end = str;
    while (*str_end++) {}
    return (size_t)(str_end - str - 1);
}
