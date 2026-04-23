message(STATUS "System Name = ${CMAKE_SYSTEM_NAME}")
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_DIR "windows")
    set(libc TRUE)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Rars")
    set(PLATFORM_DIR "rars")
    set(libc FALSE)
else()
    set(PLATFORM_DIR "linux")
    set(libc TRUE)
endif()

add_library(platform_impl STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/std/io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/std/process.c
)
# RARS specific files
if (CMAKE_SYSTEM_NAME STREQUAL "Rars")
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
message(STATUS "libc = ${libc}")
add_library(jackc_std STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/std/${STD_IMPL}/${STD_IMPL}_memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/std/${STD_IMPL}/${STD_IMPL}_string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/std/jackc_common_string.c
)
target_link_libraries(jackc_std
    PUBLIC
        platform_impl
)
