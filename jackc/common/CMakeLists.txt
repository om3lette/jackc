add_library(common STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jackc_assert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common_string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/read_file_content.c
)
