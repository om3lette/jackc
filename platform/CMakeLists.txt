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

add_library(platform_impl STATIC)
target_sources(platform_impl PRIVATE
    ${PLATFORM_DIR}/io.c
    ${PLATFORM_DIR}/memory.c
    ${PLATFORM_DIR}/process.c
)

# RARS specific files
if (PLATFORM STREQUAL "rars")
    target_sources(platform_impl PRIVATE
        ${PLATFORM_DIR}/syscalls.c
    )
endif()
