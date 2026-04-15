add_library(compiler STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_lexer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_lexer_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/compiler_reserved_keywords.c

    ${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler_parser_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/compiler_parser_utils.c

    ${CMAKE_CURRENT_SOURCE_DIR}/symtable/compiler_symtable.c

    ${CMAKE_CURRENT_SOURCE_DIR}/diagnostics-engine/engine.c

    ${CMAKE_CURRENT_SOURCE_DIR}/ast/ast.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/traversals.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/traversals/ast_traversal_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/traversals/symtab_traversal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/traversals/semantic_validity_traversal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/traversals/code-gen/std_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/traversals/code-gen/code_gen_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/traversals/code-gen/vm_code_gen_traversal.c

    ${CMAKE_CURRENT_SOURCE_DIR}/function-registry/function_registry.c

    ${CMAKE_CURRENT_SOURCE_DIR}/frontend.c
)
target_link_libraries(compiler
    PUBLIC
        core
        common
)

add_executable(jackc_frontend
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)
target_link_libraries(jackc_frontend
    PUBLIC
        compiler
)
