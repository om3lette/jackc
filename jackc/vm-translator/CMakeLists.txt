add_library(vm-translator STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vm_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vm_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-generation/code_gen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-generation/code_generation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-generation/code_gen_utils.c
)

target_link_libraries(vm-translator PUBLIC
    common
    platform_impl
)
