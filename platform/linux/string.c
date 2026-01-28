#include "jackc_string.h"
#include <string.h>

size_t jackc_strlen(const char* str) {
    return strlen(str);
}

int jackc_strcmp(const char* str1, const char* str2) {
    return strcmp(str1, str2);
}
