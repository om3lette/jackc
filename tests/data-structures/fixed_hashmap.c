#include "tau.h"
#include "core/data-structures/hashmap.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include <stdint.h>

uint32_t hasher(const void* value) {
    return *(uint32_t*)value;
}

int comparator(const void* a, const void* b) {
    uint32_t aa = *(const uint32_t*)a;
    uint32_t bb = *(const uint32_t*)b;
    return aa == bb ? 0 : 1;
}

struct hashmap_fixture {
    fixed_hash_map* hashmap;
    Allocator allocator;
};

TEST_F_SETUP(hashmap_fixture) {
    tau->allocator = arena_allocator_adapter();
    tau->hashmap = fixed_hashmap_init(uint32_t, uint32_t, hasher, comparator, &tau->allocator);
}

TEST_F_TEARDOWN(hashmap_fixture) {
    fixed_hashmap_free(&tau->hashmap);
    arena_allocator_destroy(tau->allocator.context);
}

TEST_F(hashmap_fixture, insert_one) {
    int32_t key = 10;
    int32_t value = 15;
    fixed_hashmap_insert(tau->hashmap, &key, &value);

    int32_t found_value;
    fixed_hashmap_find(tau->hashmap, &key, &found_value);
    REQUIRE_EQ(found_value, value);
}

TEST_F(hashmap_fixture, find_no_return_valuej) {
    int32_t key = 10;
    int32_t value = 15;
    fixed_hashmap_insert(tau->hashmap, &key, &value);

    REQUIRE(fixed_hashmap_find(tau->hashmap, &key, nullptr));
}

TEST_F(hashmap_fixture, insert_many) {
    const uint32_t ITERATIONS = 1e4;
    for (uint32_t i = 0; i < ITERATIONS; ++i) {
        uint32_t key = i + 10;
        uint32_t value = i;
        fixed_hashmap_insert(tau->hashmap, &key, &value);
    }

    uint32_t found_value;
    for (uint32_t i = 0; i < ITERATIONS; ++i) {
        uint32_t key = i + 10;
        uint32_t value = i;
        fixed_hashmap_find(tau->hashmap, &key, &found_value);
        REQUIRE_EQ(found_value, value);
    }
}
