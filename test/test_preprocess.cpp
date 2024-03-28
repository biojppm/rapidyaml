#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/string.hpp>
#include <c4/yml/preprocess.hpp>
#endif
#include "./test_lib/test_case.hpp"
#include <gtest/gtest.h>

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(preprocess, rxmap_basic)
{
    #define _test(val, expected)                                \
        EXPECT_EQ(preprocess_rxmap<std::string>(val), expected)

    _test("{}", "{}");
    _test("a", "{a: 1}");
    _test("{a}", "{a: 1}");
    _test("a, b, c", "{a: 1, b: 1, c: 1}");
    _test("a,b,c", "{a,b,c: 1}");
    _test("a a a a, b, c", "{a a a a: 1, b: 1, c: 1}");
    _test(",", "{,}");

    _test("a: [b, c, d]", "{a: [b, c, d]}");
    _test("a:b: [b, c, d]", "{a:b: [b, c, d]}");
    _test("a,b: [b, c, d]", "{a,b: [b, c, d]}");
    _test("a: {b, c, d}", "{a: {b, c, d}}");
    _test("a: {b: {f, g}, c: {h, i}, d: {j, k}}",
          "{a: {b: {f, g}, c: {h, i}, d: {j, k}}}");
    _test("a: {b: {f g}, c: {f g}, d: {j, k}}",
          "{a: {b: {f g}, c: {f g}, d: {j, k}}}");

    #undef _test
}



// The other test executables are written to contain the declarative-style
// YmlTestCases. This executable does not have any but the build setup
// assumes it does, and links with the test lib, which requires an existing
// get_case() function. So this is here to act as placeholder until (if?)
// proper test cases are added here.
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
