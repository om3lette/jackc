#ifndef DATA_STRUCTURES_HASHTABLE_H
#define DATA_STRUCTURES_HASHTABLE_H

#include "core/allocators/allocators.h"
#include <stdint.h>

#ifndef FIXED_HASH_MAP_BUCKETS
#   ifdef __rars__
#       define FIXED_HASH_MAP_BUCKETS 128
#   else
#       define FIXED_HASH_MAP_BUCKETS 512
#   endif
#endif
static_assert((FIXED_HASH_MAP_BUCKETS & (FIXED_HASH_MAP_BUCKETS - 1)) == 0);

#define hash_t uint32_t

typedef hash_t (*hash_fn)(const void* key);
typedef int (*cmp_fn)(const void* a, const void* b);

typedef struct hash_map_node {
    struct hash_map_node* next;
} hash_map_node;

typedef struct {
    size_t key_size, value_size;
    unsigned long inserted;

    hash_fn hasher;
    cmp_fn comparator;

    Allocator* allocator;
    hash_map_node* buckets[FIXED_HASH_MAP_BUCKETS];
} fixed_hash_map;

#define fixed_hashmap_init(KEY_TYPE, VALUE_TYPE, hasher, comparator, allocator) \
    _fixed_hashmap_init(sizeof(KEY_TYPE), sizeof(VALUE_TYPE), hasher, comparator, allocator);

#define _key_ptr(node) ((void*)((char*)node + sizeof(hash_map_node)))

#define _value_ptr(map, node) (void*)ALIGN_UP((uintptr_t)_key_ptr(node) + (map)->key_size, ALIGNMENT)

#define _key_to_index(map, key) (size_t)((map)->hasher(key) & (FIXED_HASH_MAP_BUCKETS - 1))

#define _sizeof_node(map) \
    ALIGN_UP( \
        ALIGN_UP(sizeof(hash_map_node) + (map)->key_size, ALIGNMENT) + (map)->value_size, \
        ALIGNMENT \
    )

fixed_hash_map* _fixed_hashmap_init(size_t key_size, size_t value_size, hash_fn hasher, cmp_fn comparator, Allocator* allocator);

void fixed_hashmap_insert(fixed_hash_map* map, const void* key, const void* value);

int fixed_hashmap_find(const fixed_hash_map* map, const void* key, void* out);

double fixed_hashmap_load_factor(const fixed_hash_map* map);

void fixed_hashmap_free(fixed_hash_map** map);

#endif
