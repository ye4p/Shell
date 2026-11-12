cmake_minimum_required(VERSION 3.15)

project(tny VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tny src/main.cpp src/shell.cpp src/parser.cpp src/node.cpp)

target_compile_options(tny PRIVATE -Wall -Wextra -Werror)
