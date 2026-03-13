#ifndef JACKC_COMPILER_SYMTABLE_H
#define JACKC_COMPILER_SYMTABLE_H

#include "common/data-structures/hashmap.h"
#include "compiler/symtable/symtable_token.h"

typedef enum {
    SYMTAB_OK = 0,
    SYMTAB_ALREADY_EXISTS,
} sym_table_return_code;

typedef struct {
    fixed_hash_map* tokens;
    struct sym_table* prev;
    uint32_t static_idx;
    uint32_t field_idx;
    uint32_t local_idx;
    uint32_t argument_idx;
} sym_table;

[[ nodiscard ]] sym_table* sym_table_init(sym_table* prev);

void sym_table_free(sym_table** symtab);

[[ nodiscard ]] sym_table* sym_table_push(sym_table* current);

sym_table* sym_table_pop(sym_table* current);

[[ nodiscard ]] sym_table_return_code sym_table_insert(sym_table* table, sym_table_token* token);

[[ nodiscard ]] bool sym_table_find(const sym_table* table, const jackc_string* name, sym_table_token* found);

[[ nodiscard ]] bool sym_table_exists_local(const sym_table* table, const jackc_string* name);

#endif
