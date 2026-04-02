#include "core/cli.h"
#include "core/allocators/allocators.h"
#include "core/asserts/jackc_assert.h"
#include "std/jackc_stdio.h"
#include "std/jackc_string.h"
#include "std/jackc_stdlib.h"

bool parse_args(
    void* base,
    arg_spec* specs,
    int n_specs,
    int argc,
    char** argv,
    Allocator* allocator
) {
    bool is_help = false;

    int i = FIRST_ARG_IDX;
    while (i < argc) {
        char* cur_arg = argv[i];

        // Global help handling
        if (jackc_strcmp(cur_arg, "-h") == 0 || jackc_strcmp(cur_arg, "--help") == 0) {
            is_help = true;
            print_specs(specs, n_specs);
            break;
        }

        bool matched = false;
        for (int j = 0; j < n_specs; ++j) {
            arg_spec* spec = &specs[j];

            if (
                (spec->long_name && jackc_strcmp(spec->long_name, cur_arg) == 0) ||
                (spec->short_name && jackc_strcmp(spec->short_name, cur_arg) == 0)
            ) {
                matched = true;
                spec->is_set = true;

                void* field = (char*)base + spec->offset;
                switch (spec->type) {
                    case ARG_BOOL:
                        *(bool*)field = true;
                        break;
                    case ARG_INT:
                    case ARG_UINT: {
                        if (i + 1 >= argc)
                            continue;

                        char* value = argv[++i];
                        if (value[0] == '-' && jackc_isdigit(value[1]))
                            continue;

                        *(int*)field = jackc_atoi(&jackc_string_from_str(value));
                        break;
                    }
                    case ARG_STRING: {
                        if (i + 1 >= argc)
                            continue;

                        char* value = argv[++i];
                        size_t len = jackc_strlen(value);
                        char* mem = allocator->alloc(len + 1, allocator->context);

                        jackc_memcpy(mem, value, len + 1);
                        *(char**)field = mem;
                        break;
                    }
                }
                break;
            }
        }

        if (!matched) {
            jackc_printf("Unknown argument: %s\n", cur_arg);
        }
        ++i;
    }
    // Do not print messages about missing required args
    if (is_help)
        return true;

    bool are_required_fields_filled = true;
    for (i = 0; i < n_specs; ++i) {
        arg_spec cur = specs[i];
        if (cur.required && !cur.is_set) {
            jackc_printf("Required argument '%s' was not provided\n", cur.short_name ? cur.short_name : cur.long_name);
            are_required_fields_filled = false;
        }
    }
    return !are_required_fields_filled;
}

void print_specs(arg_spec* specs, int n_specs) {
    size_t max_width = 0;

    // First pass: compute max width of flag strings
    for (int i = 0; i < n_specs; ++i) {
        arg_spec cur = specs[i];
        size_t len = 0;
        jackc_assert(cur.short_name || cur.long_name);

        if (cur.short_name)
            len += jackc_strlen(cur.short_name);

        if (cur.long_name) {
            if (cur.short_name)
                len += 2; // ", "
            len += jackc_strlen(cur.long_name);
        }

        if (len > max_width)
            max_width = len;
    }

    // Second pass: print aligned
    jackc_printf("Usage: jackc: [options]\n");
    jackc_printf("Options (* - required):\n");
    for (int i = 0; i < n_specs; ++i) {
        arg_spec cur = specs[i];

        if (cur.required)
            jackc_printf("  * ");
        else
            jackc_printf("    ");

        size_t len = 0;

        if (cur.short_name) {
            jackc_printf("%s", cur.short_name);
            len += jackc_strlen(cur.short_name);
        }

        if (cur.long_name) {
            if (cur.short_name) {
                jackc_printf(", ");
                len += 2;
            }
            jackc_printf("%s", cur.long_name);
            len += jackc_strlen(cur.long_name);
        }

        size_t padding = max_width - len;
        for (size_t j = 0; j < padding + 2; ++j) {
            jackc_putchar(' ');
        }

        if (cur.description)
            jackc_printf("%s", cur.description);
        jackc_putchar('\n');
    }
}
