cmake_minimum_required(VERSION 3.31)
project(jackc)

if(MSVC)
    message(FATAL_ERROR "MSVC is not supported")
endif()

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -fsanitize=address -fsanitize=undefined)
    add_link_options(-fsanitize=address -fsanitize=undefined)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
else()
    message(FATAL_ERROR "Unsupported build type: ${CMAKE_BUILD_TYPE}")
endif()

include(FetchContent)
FetchContent_Declare(
    Tau
    URL https://github.com/jasmcaus/tau/archive/dev.zip
)
FetchContent_MakeAvailable(Tau)

include_directories(jackc)
add_subdirectory(jackc)

enable_testing()
add_subdirectory(tests)
