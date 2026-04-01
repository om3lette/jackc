add_library(vm-translator STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/vm_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/vm_parser_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-generation/vm_code_generator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-generation/vm_code_generation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/code-generation/vm_code_gen_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/backend.c
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
