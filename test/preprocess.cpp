#include <c4/yml/std/string.hpp>
#include <c4/yml/preprocess.hpp>
#include <gtest/gtest.h>

namespace c4 {
namespace yml {


TEST(preprocess, json_basic)
{

}


TEST(preprocess, relaxed_map_basic)
{
    #define _test(val, expected)                                \
        EXPECT_EQ(preproc_relaxed_map<std::string>(val), expected)

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


} // namespace yml
} // namespace c4
