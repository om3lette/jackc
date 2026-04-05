add_library(vm-translator STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/vm_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/vm_parser_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/vm_parser_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-gen/asm_code_gen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-gen/asm_emit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-gen/asm_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-gen/stack_frame.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-gen/virtual_stack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/backend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/common.c
)
target_link_libraries(vm-translator
    PUBLIC
        core
)

add_executable(jackc_backend
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)
target_link_libraries(jackc_backend
    PRIVATE
        vm-translator
)
