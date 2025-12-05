cmake_minimum_required(VERSION 3.19)
project(yaclap_fuzzing)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)

# Find packages
find_package(ICU REQUIRED COMPONENTS uc i18n)

# Include fuzztest driver code and test adapter code
add_subdirectory(external/fuzztest)
enable_testing()
include(GoogleTest)

# Enable fuzz testing flags
fuzztest_setup_fuzzing_flags()

# Source code for the test executable
add_executable(
  yaclap_fuzz_test
  yaclap_fuzz_test.cc
)

# Enable coverage tracking
target_compile_options(yaclap_fuzz_test PRIVATE -g -O0 -fprofile-instr-generate -fcoverage-mapping)
target_link_options(yaclap_fuzz_test PRIVATE -fprofile-instr-generate -fcoverage-mapping)

# Enable Unicode conversion
target_link_libraries(yaclap_fuzz_test PRIVATE ICU::uc ICU::i18n)

# Link fuzz test dependencies
link_fuzztest(yaclap_fuzz_test)

# Make fuzz test discoverable
gtest_discover_tests(yaclap_fuzz_test)
