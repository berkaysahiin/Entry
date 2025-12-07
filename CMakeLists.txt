cmake_minimum_required(VERSION 3.16)

project(Entry LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(entry ${SOURCES})

target_include_directories(entry PRIVATE include)

# TODO: should be only in debug
target_compile_definitions(entry PUBLIC ENTRY_VERBOSE)
