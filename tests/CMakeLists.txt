add_executable(compiler_lexer_tests
    main.c
    compiler/lexer/internal.c
    compiler/lexer/numbers.c
    compiler/lexer/comments.c
    compiler/lexer/tabs_and_spaces.c
    compiler/lexer/line_col_counting.c
    compiler/lexer/unknown_characters.c
    compiler/lexer/lexer_test_utils.c
)
target_link_libraries(compiler_lexer_tests
    PRIVATE
        compiler
        Tau
        common
)
add_test(NAME compiler_lexer_tests COMMAND compiler_lexer_tests)
