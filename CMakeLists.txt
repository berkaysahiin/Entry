cmake_minimum_required(VERSION 3.16)

project(Entry LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(entry ${SOURCES})

target_include_directories(entry PRIVATE include)

target_include_directories(entry PRIVATE vendor)
target_include_directories(entry PRIVATE vendor/json/single_include)

# TODO: These shouldnt be necessary
target_include_directories(entry PRIVATE /usr/include/lua5.4)

target_link_libraries(entry PRIVATE lua5.4)

# TODO: should be only in debug
target_compile_definitions(entry PRIVATE ENTRY_VERBOSE)
