add_library(common-obj OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.c
)
target_compile_options(common-obj PRIVATE -S)

add_library(common STATIC $<TARGET_OBJECTS:common-obj>)
