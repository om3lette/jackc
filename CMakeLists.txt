cmake_minimum_required(VERSION 3.31)
project(jackc)

if(MSVC)
    message(FATAL_ERROR "MSVC is not supported")
endif()

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

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Do not add debug flags for cross-compiling
    if(NOT CMAKE_CROSSCOMPILING)
        add_compile_options(-g -fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
else()
    message(FATAL_ERROR "Unsupported build type: ${CMAKE_BUILD_TYPE}")
endif()

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

add_custom_target(riscv_asm)

if(NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_subdirectory(tests)
endif()
