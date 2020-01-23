cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(drako_input STATIC "src/input_system.cpp")
target_link_libraries(drako_input PRIVATE drako::native_sys)

add_library(drako::input ALIAS drako_input)