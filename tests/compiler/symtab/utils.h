#ifndef SYMTAB_UTILS_H
#define SYMTAB_UTILS_H

#include "compiler/symtable/symtable_token.h"

static inline sym_table_token create_token(jack_type type, jack_variable_type var_type, char* name, char* str_type) {
    jackc_string name_str = jackc_string_from_str(name);
    jackc_string str_type_str = jackc_string_from_str(str_type);
    return sym_table_token_init(type, var_type, &name_str, &str_type_str);
}

#define REQUIRE_SYMTAB_OK(result) do { \
    REQUIRE(result == SYMTAB_OK); \
} while(0)

#endif
