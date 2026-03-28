add_subdirectory(diagnostics-engine)

add_library(compiler STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_lexer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_lexer_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_reserved_keywords.c
    ${CMAKE_CURRENT_SOURCE_DIR}/symtable/compiler_symtable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/ast.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/symtab_traversal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/function-registry/function_registry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler_parser_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler_parser_utils.c
)
target_link_libraries(compiler
    PUBLIC
        common
        diagnostics_engine
)

add_executable(jackc_frontend
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend_exec.c
)
target_link_libraries(jackc_frontend
    PUBLIC
        compiler
)
