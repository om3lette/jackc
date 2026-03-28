#include "hashmap.h"
#include "core/allocators/allocators.h"
#include "core/asserts/jackc_assert.h"
#include "jackc_stdlib.h"

fixed_hash_map* _fixed_hashmap_init(size_t key_size, size_t value_size, hash_fn hasher, cmp_fn comparator, Allocator* allocator) {
    jackc_assert(hasher && "Hash function is null");
    jackc_assert(comparator && "Comparator function is null");
    jackc_assert(allocator && "Allocator is null");
    jackc_assert(key_size && "Key size is zero");
    jackc_assert(value_size && "Value size is zero");

    fixed_hash_map* map = allocator->alloc(sizeof(fixed_hash_map), allocator->context);
    jackc_assert(map && "Failed to allocate memory for fixed hashmap");

    map->key_size = key_size;
    map->value_size = value_size;
    map->hasher = hasher;
    map->comparator = comparator;
    map->allocator = allocator;
    map->inserted = 0;

    for (size_t i = 0; i < FIXED_HASH_MAP_BUCKETS; ++i) {
        map->buckets[i] = NULL;
    }

    return map;
}

void fixed_hashmap_insert(fixed_hash_map* map, const void* key, const void* value) {
    jackc_assert(map && "Fixed hashmap is null");
    jackc_assert(key && "Key is null");
    jackc_assert(value && "Value is null");

    size_t idx = _key_to_index(map, key);
    hash_map_node* node = map->buckets[idx];

    while (node) {
        if (map->comparator(key, _key_ptr(node)) == 0) {
            jackc_memcpy(_value_ptr(map, node), value, map->value_size);
            return;
        }
        node = node->next;
    }

    hash_map_node* new_node = (hash_map_node*)map->allocator->alloc(_sizeof_node(map), map->allocator->context);
    jackc_assert(new_node && "Failed to allocate hashmap node");

    jackc_memcpy(_key_ptr(new_node), key, map->key_size);
    jackc_memcpy(_value_ptr(map, new_node), value, map->value_size);

    new_node->next = map->buckets[idx];
    map->buckets[idx] = new_node;
    ++map->inserted;
}

int fixed_hashmap_find(fixed_hash_map* map, const void* key, void* out) {
    jackc_assert(map && "Fixed hashmap is null");
    jackc_assert(key && "Key is null");

    size_t idx = _key_to_index(map, key);
    hash_map_node* node = map->buckets[idx];

    while (node) {
        if (map->comparator(key, _key_ptr(node)) == 0) {
            if (out) jackc_memcpy(out, _value_ptr(map, node), map->value_size);
            return 1;
        }
        node = node->next;
    }

    return 0;
}

double fixed_hashmap_load_factor(const fixed_hash_map* map) {
    return (double)map->inserted / FIXED_HASH_MAP_BUCKETS;
}

void fixed_hashmap_free(fixed_hash_map** map_ptr, bool free_allocator) {
    if (!map_ptr || !*map_ptr) return;
    fixed_hash_map* map = *map_ptr;

    Allocator* allocator = map->allocator;
    size_t sizeof_node = _sizeof_node(map);

    for (size_t i = 0; i < FIXED_HASH_MAP_BUCKETS; ++i) {
        hash_map_node* node = map->buckets[i];
        while (node) {
            hash_map_node* next = node->next;
            allocator->free(node, sizeof_node, allocator->context);
            node = next;
        }
        map->buckets[i] = NULL;
    }
    allocator->free(map, sizeof(fixed_hash_map), allocator->context);
    if (free_allocator) {
        allocator->free(allocator, sizeof(Allocator), allocator->context);
    }
    *map_ptr = NULL;
}
