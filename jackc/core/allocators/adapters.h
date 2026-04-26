#ifndef ALLOCATORS_ADAPTERS_H
#define ALLOCATORS_ADAPTERS_H

#include "allocators.h"

/**
 * Creates an arena allocator adapter that wraps an arena allocator.
 *
 * @param allocator Arena allocator to wrap.
 * @return Allocator that wraps the arena allocator.
 */
Allocator arena_allocator_adapter();

#endif // ALLOCATORS_ADAPTERS_H
