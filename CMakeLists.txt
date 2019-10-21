cmake_minimum_required(VERSION 3.7.2)
project(lwnes)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 11)
add_definitions(-Wall -Wextra)
add_executable(lwnes
    src/core/cpu.cpp
    src/core/dma.cpp
    src/core/memory.cpp
    src/core/nes.cpp
    src/core/rom.cpp
    src/main.cpp)
include_directories(src)

option(PRINT_TRACE "Print CPU Trace")
if(PRINT_TRACE)
    add_definitions(-DPRINT_TRACE)
endif()
