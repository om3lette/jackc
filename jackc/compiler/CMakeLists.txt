add_library(compiler STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_lexer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_lexer_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_reserved_keywords.c
    ${CMAKE_CURRENT_SOURCE_DIR}/symtable/compiler_symtable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diagnostics-engine/engine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/ast.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler_parser_diagnostics.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler_parser_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler_parser_utils.c
)
target_link_libraries(compiler
    PUBLIC
        common
)
