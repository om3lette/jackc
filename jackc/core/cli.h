#ifndef JACKC_CLI_H
#define JACKC_CLI_H

#include "core/allocators/allocators.h"
#include <stddef.h>

typedef enum {
    ARG_INT,
    ARG_UINT,
    ARG_STRING,
    ARG_BOOL,
} arg_type;

typedef struct {
    const char* long_name;
    const char* short_name;
    const char* description;
    arg_type type;
    size_t offset;
    bool required;
    bool is_set;
} arg_spec;

[[ nodiscard ]] bool parse_args(
    void* base,
    arg_spec* specs,
    int n_specs,
    int argc,
    char** argv,
    Allocator* allocator
);

void print_specs(arg_spec* specs, int n_specs);

#define arg_spec_create(         \
    _short_name,                 \
    _long_name,                  \
    _description,                \
    _type,                       \
    _offset,                     \
    _required                    \
) {                              \
    .short_name = _short_name,   \
    .long_name = _long_name,     \
    .description = _description, \
    .offset = _offset,           \
    .type = _type,               \
    .required = _required,       \
    .is_set = false              \
}

#endif
