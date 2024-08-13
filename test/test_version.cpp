#ifndef RYML_SINGLE_HEADER
#include "c4/yml/version.hpp"
#endif
#include "./test_lib/test_case.hpp"
#include <gtest/gtest.h>

TEST(version, str)
{
    c4::csubstr v = c4::yml::version();
    EXPECT_GE(v.len, 5);
}

TEST(version, major)
{
    int v = c4::yml::version_major();
    EXPECT_GE(v, 0);
}

TEST(version, minor)
{
    int v = c4::yml::version_minor();
    EXPECT_GE(v, 0);
}

TEST(version, patch)
{
    int v = c4::yml::version_patch();
    EXPECT_GE(v, 0);
}
