#ifndef JACKC_STRING_H
#define JACKC_STRING_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char* data;
    size_t length;
} jackc_string;

#define jackc_string_empty() (jackc_string){.data="", .length=0}

#define jackc_string_create(str, len) (jackc_string){.data=str, .length=(len)}

[[ nodiscard ]] bool jackc_strcmp(const jackc_string* str1, const char* str2);

[[ nodiscard ]] size_t jackc_strlen(const char* str);

[[ nodiscard ]] int32_t jackc_atoi(const jackc_string* str);

[[ nodiscard ]] char jackc_tolower(char c);

#endif
