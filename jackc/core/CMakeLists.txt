add_library(core STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/allocators/allocators.c
    ${CMAKE_CURRENT_SOURCE_DIR}/allocators/adapters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/asserts/jackc_assert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/data-structures/hashmap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logging/logger.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cli.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/file-utils/common_file_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/file-utils/${PLATFORM}_file_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/localization/locale.c
    ${CMAKE_CURRENT_SOURCE_DIR}/localization/translations/en.c
    ${CMAKE_CURRENT_SOURCE_DIR}/localization/translations/ru.c
)

target_link_libraries(core
    PUBLIC
        jackc_std
)
