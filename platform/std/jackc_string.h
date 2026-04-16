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

#define jackc_string_from_str(str) (jackc_string){.data=str, .length=jackc_strlen(str)}

void jackc_string_print(const jackc_string* str);

[[ nodiscard ]] int jackc_strcmp(const char* str1, const char* str2);

[[ nodiscard ]] int jackc_string_cmp(const jackc_string* str1, const jackc_string* str2);

[[ nodiscard ]] bool jackc_streq(const jackc_string* str1, const char* str2);

[[ nodiscard ]] size_t jackc_strlen(const char* str);

[[ nodiscard ]] int32_t jackc_atoi(const jackc_string* str);

[[ nodiscard ]] char jackc_tolower(char c);

[[ nodiscard ]] bool jackc_isalpha(char c);

[[ nodiscard ]] bool jackc_isdigit(char c);

[[ nodiscard ]] bool jackc_iseol(char c);

[[ nodiscard ]] char* jackc_strrchr(const char* s, char c);

void jackc_strcpy(char* dest, const char* src);

typedef struct {
    uint32_t start;
    uint32_t end;
} jackc_span;

[[ nodiscard ]] jackc_span jackc_span_from_str(const jackc_string* source, const jackc_string* str);

[[ nodiscard ]] jackc_span jackc_span_between(const jackc_span* first, const jackc_span* second);
#endif
