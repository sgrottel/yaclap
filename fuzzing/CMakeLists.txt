cmake_minimum_required(VERSION 3.19)
project(first_fuzz_project)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external/fuzztest)

enable_testing()

include(GoogleTest)

fuzztest_setup_fuzzing_flags()

add_executable(
  first_fuzz_test
  first_fuzz_test.cc
)

# If you have other dependencies than FuzzTest, link them:
# target_link_libraries(first_fuzz_test PRIVATE other_deps)

link_fuzztest(first_fuzz_test)
gtest_discover_tests(first_fuzz_test)

