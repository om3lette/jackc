add_library(compiler-obj OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/tmp.c
)
target_compile_options(compiler-obj PRIVATE -S)

add_library(compiler STATIC $<TARGET_OBJECTS:vm-translator-obj>)
target_link_libraries(compiler PUBLIC
    common
)
