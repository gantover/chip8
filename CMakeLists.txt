cmake_minimum_required(VERSION 3.0.0)

project("chip8")

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra")

find_package(SDL2 REQUIRED)

add_executable(main main.cpp)

add_library(window STATIC ./window.cpp)
add_library(hardware STATIC ./hardware.cpp)

target_link_libraries(main SDL2 window hardware)
