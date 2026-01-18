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

if (CMAKE_CROSSCOMPILING)
    add_custom_command(
        OUTPUT final.s
        COMMAND
            ${CMAKE_C_COMPILER}
                -I${CMAKE_CURRENT_SOURCE_DIR}/jackc
                -I${CMAKE_CURRENT_SOURCE_DIR}/platform
                -DNDEBUG
                -S
                $<TARGET_PROPERTY:common-obj,SOURCES>
                $<TARGET_PROPERTY:vm-translator-obj,SOURCES>
                $<TARGET_PROPERTY:compiler-obj,SOURCES>
                $<TARGET_PROPERTY:platform_impl-obj,SOURCES>
                $<TARGET_PROPERTY:jackc-obj,SOURCES>
            && sh ${CMAKE_CURRENT_SOURCE_DIR}/cmake/rename-labels.sh ${CMAKE_CURRENT_BINARY_DIR}
            && cat ${CMAKE_CURRENT_BINARY_DIR}/*.s > jackc.s
        DEPENDS
            $<TARGET_PROPERTY:common-obj,SOURCES>
            $<TARGET_PROPERTY:vm-translator-obj,SOURCES>
            $<TARGET_PROPERTY:compiler-obj,SOURCES>
            $<TARGET_PROPERTY:platform_impl-obj,SOURCES>
            $<TARGET_PROPERTY:jackc-obj,SOURCES>
        COMMAND_EXPAND_LISTS
    )

    add_custom_target(riscv_asm ALL
        DEPENDS jackc.s
    )
endif()

if(NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_subdirectory(tests)
endif()
