if (NOT DEFINED PLATFORM)
    set(PLATFORM "linux")
endif()

if (PLATFORM STREQUAL "linux")
    set(PLATFORM_DIR "linux")
elseif (PLATFORM STREQUAL "rars")
    set(PLATFORM_DIR "rars")
else()
    message(FATAL_ERROR "Unsupported platform: ${PLATFORM}")
endif()

add_library(platform_impl STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/string.c
)

# RARS specific files
if (PLATFORM STREQUAL "rars")
    target_sources(platform_impl PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/syscalls.c
    )
endif()
