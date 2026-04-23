cmake_minimum_required(VERSION 3.31)
project(jackc)

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "MSVC is not supported")
endif()

message(STATUS "Cross compiling: ${CMAKE_CROSSCOMPILING}")

# TODO: Remove __COUNTER__
# set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(
    -std=c2y
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion
    -Wshadow
)

# CFLAGS / LDFLAGS
if (CMAKE_CROSSCOMPILING)
    add_compile_options(-D__rars__)
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
            add_compile_options(-fsanitize=address,undefined)
            add_link_options(-fsanitize=address,undefined)
        endif()
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
                $<TARGET_PROPERTY:common,SOURCES>
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
                $<TARGET_PROPERTY:common,SOURCES>
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
