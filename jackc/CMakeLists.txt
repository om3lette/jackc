add_subdirectory(core)
add_subdirectory(common)
add_subdirectory(vm-translator)
add_subdirectory(compiler)

add_executable(jackc
    main.c
)
target_link_libraries(jackc
    PRIVATE
        compiler
        vm-translator
)
