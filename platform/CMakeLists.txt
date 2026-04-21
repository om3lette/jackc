message(STATUS "Platform = ${PLATFORM}")
if (PLATFORM STREQUAL "linux")
    set(PLATFORM_DIR "linux")
    set(libc true)
elseif (PLATFORM STREQUAL "windows")
    set(PLATFORM_DIR "windows")
    set(libc true)
elseif (PLATFORM STREQUAL "rars")
    set(PLATFORM_DIR "rars")
else()
    message(FATAL_ERROR "Unsupported platform: ${PLATFORM}")
endif()

add_library(platform_impl STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/std/io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/std/process.c
)
# RARS specific files
if (PLATFORM STREQUAL "rars")
    message(${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/std/memory.c)
    target_sources(platform_impl PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/rars_syscalls.c
        ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/std/memory.c
    )
endif()

if (libc)
    set(STD_IMPL "libc")
else()
    set(STD_IMPL "nolibc")
endif()
message(STATUS "STD_DIR = ${STD_DIR}")
add_library(jackc_std STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/std/${STD_IMPL}/${STD_IMPL}_memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/std/${STD_IMPL}/${STD_IMPL}_string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/std/jackc_common_string.c
)
target_link_libraries(jackc_std
    PUBLIC
        platform_impl
)
