add_library(core STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/allocators/allocators.c
    ${CMAKE_CURRENT_SOURCE_DIR}/allocators/adapters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/asserts/jackc_assert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/data-structures/hashmap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logging/logger.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hash.c
)

target_link_libraries(core
    PUBLIC
        platform_impl
)
