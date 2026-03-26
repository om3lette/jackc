include_directories(include)

add_executable(compiler_lexer_tests
    main.c
    compiler/lexer/eof.c
    compiler/lexer/internal.c
    compiler/lexer/numbers.c
    compiler/lexer/comments.c
    compiler/lexer/operations.c
    compiler/lexer/tabs_and_spaces.c
    compiler/lexer/line_col_counting.c
    compiler/lexer/unknown_characters.c
    compiler/lexer/string_literals.c
    compiler/lexer/lexer_test_utils.c
)
target_link_libraries(compiler_lexer_tests
    PRIVATE
        Tau
        compiler
        common
)

add_executable(compiler_symtab_tests
    main.c
    compiler/symtab/push_pop.c
    compiler/symtab/find.c
    compiler/symtab/insert.c
)
target_link_libraries(compiler_symtab_tests
    PRIVATE
        Tau
        compiler
        common
)

add_executable(data_structure_tests
    main.c
    data-structures/fixed_hashmap.c
)
target_link_libraries(data_structure_tests
    PRIVATE
        Tau
        common
)

add_executable(allocators_tests
    main.c
    allocators/arena_allocator.c
)
target_link_libraries(allocators_tests
    PRIVATE
        Tau
        common
)

add_executable(compiler_parser_tests
    main.c
    compiler/lexer/lexer_test_utils.c
    compiler/parser/test_parser_utils.c
    compiler/parser/var_dec.c
    compiler/parser/class_var_dec.c
    compiler/parser/param_list.c
    compiler/parser/expression.c
    compiler/parser/statements.c
    compiler/parser/subroutine.c
)
target_link_libraries(compiler_parser_tests
    PRIVATE
        Tau
        compiler
)

add_test(NAME compiler_lexer_tests COMMAND compiler_lexer_tests)
add_test(NAME compiler_symtab_tests COMMAND compiler_symtab_tests)
add_test(NAME data_structure_tests COMMAND data_structure_tests)
add_test(NAME allocators_tests COMMAND allocators_tests)
add_test(NAME compiler_parser_tests COMMAND compiler_parser_tests)
