add_library(compiler STATIC
    tmp.c
)

target_link_libraries(compiler PUBLIC
    common
)
