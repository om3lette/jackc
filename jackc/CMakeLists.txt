add_subdirectory(core)
add_subdirectory(common)
add_subdirectory(vm-translator)
add_subdirectory(compiler)

add_executable(jackc
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)
target_link_libraries(jackc
    PRIVATE
        compiler
        vm-translator
)

add_custom_command(TARGET jackc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:jackc>
        ${CMAKE_CURRENT_SOURCE_DIR}/../tests/stdlib/libs/jackc
    COMMENT "Copy jackc executable to stdlib tests directory"
)
