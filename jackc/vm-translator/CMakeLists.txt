add_library(vm-translator STATIC
    parser.c
)

target_link_libraries(vm-translator PUBLIC
    common
    platform_impl
)
