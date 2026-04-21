#ifndef JACKC_STD_UTILS_H
#define JACKC_STD_UTILS_H

#include "std/jackc_syscalls.h"
#include <stddef.h>
#include <stdint.h>
#define STD_STRING_CLASS "String"
#define STD_MEMORY_CLASS "Memory"

/**
 * Lists a subset of STD subroutines in Jack
 * Those are used during vm code gen
 */
typedef enum {
    STD_STRING_NEW = 0,
    STD_STRING_APPEND_CHAR,
    STD_MEMORY_ALLOC,
    NUMBER_OF_STD_FUNCTIONS
} std_subroutine_kind;

typedef struct {
    std_subroutine_kind kind;

    union {
        struct {
            size_t length;
        } string_new;
        struct {
            uint8_t c;
            bool fetch_this_from_temp;
        } string_append_char;
        struct {
            uint32_t words_to_allocate;
        } memory_alloc;
    };
} std_subroutine_call;

void emit_std_call(FD fd, std_subroutine_call subroutine);

typedef struct {
    const char* name;
    uint8_t n_args;
} std_subroutine;

#endif
