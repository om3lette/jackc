#include "compiler_symtable.h"
#include "core/allocators/allocators.h"
#include "core/data-structures/hashmap.h"
#include "core/asserts/jackc_assert.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/symtable/symtable_token.h"
#include "jackc_stdlib.h"
#include "jackc_string.h"

static void* allocator_alloc(size_t bytes, void *context) {
    (void)context;
    return jackc_alloc(bytes);
}

static void allocator_free(void *ptr, size_t bytes, void *context) {
    (void)context; (void) bytes;
    jackc_free(ptr);
}

static uint32_t oat_hash(const char *s, size_t len) {
    const unsigned char* p = (const unsigned char*) s;
    uint32_t h = 0;

    while(len--) {
        h += *p++;
        h += (h << 10);
        h ^= (h >> 6);
    }

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    return h;
}

static hash_t symtab_key_hasher(const void* key) {
    const jackc_string* kkey = key;
    return oat_hash(kkey->data, kkey->length);
}

static int symtab_token_comparator(const void* a, const void* b) {
    return jackc_string_cmp(a, b);
}

sym_table* sym_table_init(sym_table* prev) {
    Allocator* allocator = jackc_alloc(sizeof(Allocator));
    allocator->alloc = allocator_alloc;
    allocator->free = allocator_free;
    allocator->context = NULL;

    sym_table* symtab = jackc_alloc(sizeof(sym_table));
    symtab->prev = (struct sym_table*)prev;
    symtab->tokens = fixed_hashmap_init(
        jackc_string,
        sym_table_token,
        symtab_key_hasher,
        symtab_token_comparator,
        allocator
    );
    symtab->static_idx = 0;
    symtab->field_idx = 0;
    symtab->local_idx = 0;
    symtab->argument_idx = 0;
    return symtab;
}

void sym_table_free(sym_table** symtab) {
    if (!symtab || !*symtab) return;

    sym_table* table = *symtab;
    while (table) {
        table = sym_table_pop(table);
    }
    *symtab = NULL;
}

sym_table* sym_table_push(sym_table* current) {
    return sym_table_init(current);
}

sym_table* sym_table_pop(sym_table* current) {
    jackc_assert(current != NULL && "Current symtable is NULL");

    sym_table* prev = (sym_table*)current->prev;
    fixed_hashmap_free(&current->tokens, true);
    jackc_free(current);
    return prev;
}

sym_table_return_code sym_table_insert(sym_table* table, sym_table_token* token) {
    if (sym_table_exists_local(table, &token->name)) {
        return SYMTAB_ALREADY_EXISTS;
    }
    switch (token->var.kind) {
        case VAR_STATIC:
            token->var.idx = table->static_idx++;
            break;
        case VAR_FIELD:
            token->var.idx = table->field_idx++;
            break;
        case VAR_LOCAL:
            token->var.idx = table->local_idx++;
            break;
        case VAR_ARG:
            token->var.idx = table->argument_idx++;
            break;
        default:
            jackc_assert(false && "Unknown variable type");
    }
    fixed_hashmap_insert(table->tokens, &token->name, token);
    return SYMTAB_OK;
}

bool sym_table_find(const sym_table* table, const jackc_string* name, sym_table_token* found) {
    while (table) {
        if (fixed_hashmap_find(table->tokens, name, found)) return true;
        table = (sym_table*)table->prev;
    }
    return false;
}

bool sym_table_exists_local(const sym_table* table, const jackc_string* name) {
    sym_table_token found;
    return fixed_hashmap_find(table->tokens, name, &found);
}
