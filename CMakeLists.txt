# CMakeList.txt : CMake project for yaclap, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

project(yaclap
    VERSION "1.0"
    DESCRIPTION "Yet Another Command Line Argument Parser"
    LANGUAGES CXX
)

# cpp 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(/Zc:__cplusplus)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
