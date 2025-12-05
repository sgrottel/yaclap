
#include "../include/yaclap.hpp"

#include "fuzztest/fuzztest.h"

#include "gtest/gtest.h"

void FuzzTest(int a, int b)
{
    using Parser = yaclap::Parser<char>;
    Parser parser{"yaclap-fuzzing", "Fuzz tests for fun and profit."};
    EXPECT_EQ(a + b, b + a);
}

FUZZ_TEST(Yaclap, FuzzTest);
