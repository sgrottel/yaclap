# CMakeList.txt : CMake project for yaclap, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("yaclap")

# Add source to this project's executable.
add_executable (yaclap
	"main.cpp"
	"cmdargs.h"
	"cmdargs.cpp"
	"../include/yaclap.hpp"
)

include_directories(../include)

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET yaclap PROPERTY CXX_STANDARD 20)
endif()

if (UNICODE STREQUAL "_Unicode")
	target_compile_definitions(yaclap PUBLIC UNICODE _UNICODE)
endif()
