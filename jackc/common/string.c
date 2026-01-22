#include "jackc_stdlib.h"
#include "jackc_string.h"
#include <stdint.h>

bool jackc_strcmp(const jackc_string* str1, const char* str2) {
    const char* str1_ptr = str1->data;
    const char* str1_end = str1->data + str1->length;

    while (str1_ptr < str1_end && *str2 && *str1_ptr == *str2) {
        ++str1_ptr;
        ++str2;
    }
    return (str1_ptr == str1_end) && (*str2 == '\0');
}

int32_t jackc_atoi(const jackc_string* str) {
    int32_t result = 0;
    const char* str_ptr = str->data;
    const char* str_end = str->data + str->length;

    bool is_negative = false;
    if (*str_ptr == '-') {
        ++str_ptr;
        is_negative = true;
    }

    while (str_ptr < str_end && *str_ptr >= '0' && *str_ptr <= '9') {
        result = result * 10 + (*str_ptr - '0');
        ++str_ptr;
    }

    // Invalid input
    if (str_ptr != str_end) return 0;
    if (is_negative) result = -result;

    return result;
}

char jackc_tolower(char c) {
    return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}

void jackc_string_print(const jackc_string* str) {
    const char* str_ptr = str->data;
    const char* str_end = str->data + str->length;

    while (str_ptr < str_end) {
        jackc_putchar(*str_ptr);
        ++str_ptr;
    }
    jackc_putchar('\n');

    return;
}
