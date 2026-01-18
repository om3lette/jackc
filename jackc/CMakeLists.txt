add_subdirectory(common)
add_subdirectory(vm-translator)
add_subdirectory(compiler)

add_executable(jackc
    main.c
)

target_link_libraries(jackc
    PRIVATE
        vm-translator
        compiler
)
