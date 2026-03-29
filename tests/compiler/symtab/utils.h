#ifndef SYMTAB_UTILS_H
#define SYMTAB_UTILS_H

#include "compiler/symtable/compiler_symtable.h"
#include "compiler/symtable/symtable_token.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"

struct symtab_fixture {
    Allocator allocator;
    sym_table* symtab;
};

static inline sym_table_token create_token(jack_type type, jack_variable_kind var_type, char* name, char* str_type) {
    jackc_string name_str = jackc_string_from_str(name);
    jackc_string str_type_str = jackc_string_from_str(str_type);
    return sym_table_token_init(type, var_type, &name_str, &str_type_str);
}

#define REQUIRE_SYMTAB_OK(result) do { \
    REQUIRE(result == SYMTAB_OK); \
} while(0)

static inline void test_symtab_common_init(struct symtab_fixture* fixture) {
    fixture->allocator = arena_allocator_adapter();
    fixture->symtab = sym_table_init(nullptr, &fixture->allocator);
}

static inline void test_symtab_common_teardown(struct symtab_fixture* fixture) {
    arena_allocator_destroy(fixture->allocator.context);
}

#endif
