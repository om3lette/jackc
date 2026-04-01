#include "core/allocators/allocators.h"
#include "tau.h"

struct arena_fixture {
    arena_allocator* allocator;
};

TEST_F_SETUP(arena_fixture) {
    tau->allocator = arena_allocator_init();
}

TEST_F_TEARDOWN(arena_fixture) {
    arena_allocator_destroy(tau->allocator);
}

static inline uint32_t arena_number_of_blocks(arena_allocator* allocator) {
    uint32_t count = 0;
    for (arena_block* block = allocator->current_block; block != NULL; block = (arena_block*)block->next) {
        count++;
    }
    return count;
}

TEST_F(arena_fixture, initialization) {
    REQUIRE_EQ(tau->allocator->current_block->size, 0);
    REQUIRE_EQ(tau->allocator->current_block->capacity, ARENA_START_BLOCK_SIZE);
    REQUIRE_EQ((void*)tau->allocator->current_block->next, NULL);
}

TEST_F(arena_fixture, new_block_allocation) {
    const arena_block* const first_block = tau->allocator->current_block;

    const uint32_t allocated = ARENA_START_BLOCK_SIZE + 1;
    void* ptr = arena_allocator_alloc(tau->allocator, allocated);
    (void) ptr;

    REQUIRE_EQ(tau->allocator->current_block->size, ALIGN_UP(allocated, ALIGNMENT));
    REQUIRE_EQ(tau->allocator->current_block->capacity, ARENA_START_BLOCK_SIZE * 2);
    REQUIRE_EQ((void*)tau->allocator->current_block->next, (void*)first_block);
    REQUIRE_EQ(arena_number_of_blocks(tau->allocator), 2);
}

TEST_F(arena_fixture, new_block_allocation_max_size) {
    const uint32_t allocated = ARENA_START_BLOCK_SIZE;
    void* ptr = arena_allocator_alloc(tau->allocator, allocated);
    (void) ptr;

    REQUIRE_EQ(tau->allocator->current_block->size, ALIGN_UP(ARENA_START_BLOCK_SIZE, ALIGNMENT));
    REQUIRE_EQ(tau->allocator->current_block->capacity, ARENA_START_BLOCK_SIZE);
    REQUIRE_EQ((void*)tau->allocator->current_block->next, NULL);
    REQUIRE_EQ(arena_number_of_blocks(tau->allocator), 1);
}

TEST_F(arena_fixture, new_block_huge_allocation_power_of_two) {
    // More than x2
    const uint32_t allocated = 4 * ARENA_START_BLOCK_SIZE;
    void* ptr = arena_allocator_alloc(tau->allocator, allocated);
    (void) ptr;

    REQUIRE_EQ(tau->allocator->current_block->size, ALIGN_UP(allocated, ALIGNMENT));
    REQUIRE_EQ(tau->allocator->current_block->capacity, allocated);
}

TEST_F(arena_fixture, new_block_huge_allocation_not_power_of_two) {
    // More than x2
    const uint32_t allocated = 3 * ARENA_START_BLOCK_SIZE;
    void* ptr = arena_allocator_alloc(tau->allocator, allocated);
    (void) ptr;

    REQUIRE_EQ(tau->allocator->current_block->size, ALIGN_UP(allocated, ALIGNMENT));
    REQUIRE_EQ(tau->allocator->current_block->capacity, 4 * ARENA_START_BLOCK_SIZE);
}
