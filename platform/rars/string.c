#include "jackc_string.h"
#include <stdint.h>

int jackc_strcmp(const char* str1, const char* str2) {
    while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }
    return (int)(*str1 - *str2);
}

size_t jackc_strlen(const char* str) {
    const char* str_end = str;
    while (*str_end++) {}
    return (size_t)(str_end - str - 1);
}

void jackc_strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}
