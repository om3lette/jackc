add_library(compiler STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/tmp.c
)
target_link_libraries(compiler PUBLIC
    common
)
