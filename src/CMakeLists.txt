# CMakeList.txt : CMake project for yaclap, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

project ("yaclap")

# Add source to this project's executable.
add_executable (yaclap
	"main.cpp"
	"cmdargs.hpp"
	"cmdargs.cpp"
	"../include/yaclap.hpp"
)

include_directories(../include)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

if (MSVC_VERSION GREATER_EQUAL "1900")
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
	if (_cpp_latest_flag_supported)
		add_compile_options("/std:c++latest")
	endif()
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET yaclap PROPERTY CXX_STANDARD 20)
	set_property(TARGET yaclap PROPERTY CXX_STANDARD_REQUIRED ON)
endif()

set_property(TARGET yaclap PROPERTY COMPILE_WARNING_AS_ERROR ON)

if (UNICODE STREQUAL "_Unicode")
	target_compile_definitions(yaclap PUBLIC UNICODE _UNICODE)
endif()
