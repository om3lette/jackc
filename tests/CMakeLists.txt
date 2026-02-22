add_executable(tests
    main.c
    compiler/lexer/internal.c
    compiler/lexer/numbers.c
    compiler/lexer/comments.c
    compiler/lexer/line_col_counting.c
    compiler/lexer/unknown_characters.c
    compiler/lexer/lexer_test_utils.c
)

target_link_libraries(tests
    PRIVATE
        vm-translator
        compiler
        Tau
        common
)
