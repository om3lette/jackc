add_subdirectory(common)
add_subdirectory(vm-translator)
add_subdirectory(compiler)

add_library(jackc-obj OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)
target_compile_options(jackc-obj PRIVATE -S)

add_executable(jackc $<TARGET_OBJECTS:jackc-obj>)

target_link_libraries(jackc
    PRIVATE
        vm-translator
        compiler
)
