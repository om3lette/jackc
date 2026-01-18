add_library(vm-translator-obj OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.c
)
target_compile_options(vm-translator-obj PRIVATE -S)

add_library(vm-translator STATIC $<TARGET_OBJECTS:vm-translator-obj>)
target_link_libraries(vm-translator PUBLIC
    common
    platform_impl
)
