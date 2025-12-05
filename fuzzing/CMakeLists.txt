cmake_minimum_required(VERSION 3.19)
project(yaclap_fuzzing)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external/fuzztest)

enable_testing()

include(GoogleTest)

fuzztest_setup_fuzzing_flags()

add_executable(
  yaclap_fuzz_test
  yaclap_fuzz_test.cc
)

target_compile_options(yaclap_fuzz_test PRIVATE -g -O0 -fprofile-instr-generate -fcoverage-mapping)

target_link_options(yaclap_fuzz_test PRIVATE -fprofile-instr-generate -fcoverage-mapping)

# If you have other dependencies than FuzzTest, link them:
# target_link_libraries(yaclap_fuzz_test PRIVATE other_deps)

link_fuzztest(yaclap_fuzz_test)

gtest_discover_tests(yaclap_fuzz_test)

