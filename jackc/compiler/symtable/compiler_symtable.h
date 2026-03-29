#ifndef JACKC_COMPILER_SYMTABLE_H
#define JACKC_COMPILER_SYMTABLE_H

#include "core/allocators/allocators.h"
#include "core/data-structures/hashmap.h"
#include "compiler/symtable/symtable_token.h"

typedef enum {
    SYMTAB_OK = 0,
    SYMTAB_ALREADY_EXISTS,
} sym_table_return_code;

typedef struct sym_table sym_table;
struct sym_table {
    fixed_hash_map* tokens;
    sym_table* prev;
    Allocator* allocator;
    uint32_t static_idx;
    uint32_t field_idx;
    uint32_t local_idx;
    uint32_t argument_idx;
};

/**
 * Initializes a new symbol table with the given previous table.
 *
 * @param prev The previous symbol table, or NULL if this is the top-level table.
 * @param allocator The allocator to use for memory allocation.
 * @return A new symbol table.
 */
[[ nodiscard ]] sym_table* sym_table_init(sym_table* prev, Allocator* allocator);

/**
 * Frees the given symbol table and all its children.
 *
 * @param symtab The symbol table to free.
 */
void sym_table_free(sym_table** symtab);

/**
 * Pushes a new symbol table onto the stack, returning the new top.
 *
 * @param current The current symbol table.
 * @return The new top symbol table.
 */
[[ nodiscard ]] sym_table* sym_table_push(sym_table* current, Allocator* allocator);

/**
 * Pops the top symbol table from the stack, returning the new top.
 *
 * @param current The current symbol table.
 * @return The new top symbol table.
 */
sym_table* sym_table_pop(sym_table* current);

/**
 * Inserts a token into the symbol table.
 *
 * @param table The symbol table.
 * @param token The token to insert.
 * @return SYMTAB_OK if the token was inserted successfully, SYMTAB_ALREADY_EXISTS if the token already exists.
 */
[[ nodiscard ]] sym_table_return_code sym_table_insert(sym_table* table, sym_table_token* token);

/**
 * Finds a token in the symbol table by name.
 *
 * @param table The symbol table.
 * @param name The name of the token to find.
 * @param found The found token, or NULL if not found.
 * @return true if the token was found, false otherwise.
 */
[[ nodiscard ]] bool sym_table_find(const sym_table* table, const jackc_string* name, sym_table_token* found);

/**
 * Checks if a token exists in the local scope of the symbol table.
 *
 * @param table The symbol table.
 * @param name The name of the token to check.
 * @return true if the token exists locally, false otherwise.
 */
[[ nodiscard ]] bool sym_table_exists_local(const sym_table* table, const jackc_string* name);

#endif
