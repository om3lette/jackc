cmake_minimum_required(VERSION 3.31)
project(jackc)

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "MSVC is not supported")
endif()

message(STATUS "Cross compiling: ${CMAKE_CROSSCOMPILING}")

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion
    -Wshadow
    # -Wcast-qual TODO: Think about void*
)

# CFLAGS / LDFLAGS
if (NOT CMAKE_CROSSCOMPILING)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
    else()
        message(FATAL_ERROR "Unsupported build type: ${CMAKE_BUILD_TYPE}")
    endif()
endif()

# Dependencies
include(FetchContent)
if(NOT CMAKE_CROSSCOMPILING)
    FetchContent_Declare(
        Tau
        URL https://github.com/jasmcaus/tau/archive/dev.zip
        PATCH_COMMAND patch -p1 --input=${CMAKE_CURRENT_SOURCE_DIR}/patches/tau.patch
    )
    FetchContent_MakeAvailable(Tau)
endif()

include_directories(jackc)
include_directories(platform)
add_subdirectory(platform)
add_subdirectory(jackc)

# Cross compilation
if (CMAKE_CROSSCOMPILING)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sources/backend)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sources/frontend)

    add_custom_target(jackc_backend.s ALL
        COMMAND
            ${CMAKE_COMMAND} -E rm -f ${CMAKE_CURRENT_BINARY_DIR}/backend_build/*.s
        COMMAND
            ${CMAKE_C_COMPILER}
                -I${CMAKE_CURRENT_SOURCE_DIR}/jackc
                -I${CMAKE_CURRENT_SOURCE_DIR}/platform
                -DNDEBUG
                -D__rars__
                -S
                $<TARGET_PROPERTY:platform_impl,SOURCES>
                $<TARGET_PROPERTY:jackc_std,SOURCES>
                $<TARGET_PROPERTY:core,SOURCES>
                $<TARGET_PROPERTY:vm-translator,SOURCES>
                $<TARGET_PROPERTY:jackc_backend,SOURCES>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sources/backend
        COMMAND
            python3 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/build-helper.py
                ${CMAKE_CURRENT_BINARY_DIR}/sources/backend
                ${CMAKE_CURRENT_BINARY_DIR}/jackc_backend.s
        COMMAND_EXPAND_LISTS
        VERBATIM
    )

    add_custom_target(jackc_frontend.s ALL
        COMMAND
            ${CMAKE_COMMAND} -E rm -f ${CMAKE_CURRENT_BINARY_DIR}/frontend_build/*.s
        COMMAND
            ${CMAKE_C_COMPILER}
                -I${CMAKE_CURRENT_SOURCE_DIR}/jackc
                -I${CMAKE_CURRENT_SOURCE_DIR}/platform
                -DNDEBUG
                -D__rars__
                -S
                $<TARGET_PROPERTY:platform_impl,SOURCES>
                $<TARGET_PROPERTY:jackc_std,SOURCES>
                $<TARGET_PROPERTY:core,SOURCES>
                $<TARGET_PROPERTY:diagnostics_engine,SOURCES>
                $<TARGET_PROPERTY:compiler,SOURCES>
                $<TARGET_PROPERTY:jackc_frontend,SOURCES>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sources/frontend
        COMMAND
            python3 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/build-helper.py
                ${CMAKE_CURRENT_BINARY_DIR}/sources/frontend
                ${CMAKE_CURRENT_BINARY_DIR}/jackc_frontend.s
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
endif()

if(NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_subdirectory(tests)
endif()
