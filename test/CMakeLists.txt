# Download and link GoogleTest
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(
	"../include"
	"../src"
)

# Add test executable
add_executable(yaclap_tests
	"../src/cmdargs.cpp"
	"testAppCmdargs.cpp"
	"testUnmatchedArgs.cpp"
)

if (MSVC_VERSION GREATER_EQUAL "1900")
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
	if (_cpp_latest_flag_supported)
		add_compile_options("/std:c++latest")
	endif()
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
	set_property(TARGET yaclap_tests PROPERTY CXX_STANDARD 20)
	set_property(TARGET yaclap_tests PROPERTY CXX_STANDARD_REQUIRED ON)
endif()

set_property(TARGET yaclap_tests PROPERTY COMPILE_WARNING_AS_ERROR ON)

if (UNICODE STREQUAL "_Unicode")
	target_compile_definitions(yaclap_tests PUBLIC UNICODE _UNICODE)
endif()

# Link the test executable with GoogleTest
target_link_libraries(yaclap_tests gtest_main)

# Set debug flags for the test target
target_compile_options(yaclap_tests PRIVATE
	$<$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>:/DEBUG>
)
# Discover tests with CTest
include(GoogleTest)
gtest_discover_tests(yaclap_tests)
