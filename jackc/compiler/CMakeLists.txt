add_library(compiler STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_lexer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_lexer_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_reserved_keywords.c
)
target_link_libraries(compiler PUBLIC
    common
)
