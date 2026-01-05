add_executable(tests
    main.c
)

target_link_libraries(tests
    PRIVATE
        vm-translator
        compiler
        Tau
)
