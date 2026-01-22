add_library(vm-translator STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.c
)

target_link_libraries(vm-translator PUBLIC
    common
    platform_impl
)
