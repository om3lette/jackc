add_executable(tests
    main.c
    compiler/lexer/internal.c
    compiler/lexer/lexer_test_utils.c
)

target_link_libraries(tests
    PRIVATE
        vm-translator
        compiler
        Tau
        common
)
