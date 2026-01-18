cmake_minimum_required(VERSION 3.31)
project(jackc)

if(MSVC)
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
    -Wcast-qual
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
    )
    FetchContent_MakeAvailable(Tau)
endif()

include_directories(jackc)
include_directories(platform)
add_subdirectory(platform)
add_subdirectory(jackc)

# Cross compilation
if (CMAKE_CROSSCOMPILING)
    set(ASM_DIR ${CMAKE_BINARY_DIR}/asm)
    add_custom_command(
        OUTPUT jackc.s
        COMMAND
            ${CMAKE_C_COMPILER}
                -I${CMAKE_CURRENT_SOURCE_DIR}/jackc
                -I${CMAKE_CURRENT_SOURCE_DIR}/platform
                -S
                $<TARGET_PROPERTY:common,SOURCES>
                $<TARGET_PROPERTY:vm-translator,SOURCES>
                $<TARGET_PROPERTY:compiler,SOURCES>
                $<TARGET_PROPERTY:platform_impl,SOURCES>
                $<TARGET_PROPERTY:jackc,SOURCES>
            && sh ${CMAKE_CURRENT_SOURCE_DIR}/cmake/merge-assembly.sh ${CMAKE_CURRENT_BINARY_DIR} ${ASM_DIR}
        DEPENDS
            $<TARGET_PROPERTY:common,SOURCES>
            $<TARGET_PROPERTY:vm-translator,SOURCES>
            $<TARGET_PROPERTY:compiler,SOURCES>
            $<TARGET_PROPERTY:platform_impl,SOURCES>
            $<TARGET_PROPERTY:jackc,SOURCES>
        COMMAND_EXPAND_LISTS
    )

    add_custom_target(rars-build ALL
        DEPENDS jackc.s
    )
endif()

if(NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_subdirectory(tests)
endif()
