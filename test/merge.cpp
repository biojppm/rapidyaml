#include <gtest/gtest.h>
#include <c4/yml/yml.hpp>
#include <c4/yml/std/std.hpp>
#include <initializer_list>
#include <string>
#include <iostream>

namespace c4 {
namespace yml {

void test_merge(std::initializer_list<csubstr> li, csubstr expected)
{
    Tree loaded, merged, ref;

    parse(expected, &ref);

    for(csubstr src : li)
    {
        loaded.clear(); // do not clear the arena
        parse(src, &loaded);
        merged.merge_with(&loaded);
    }

    auto buf_result = emitrs<std::string>(merged);
    auto buf_expected = emitrs<std::string>(ref);

    EXPECT_EQ(buf_result, buf_expected);
}

TEST(merge, simple_seq_no_overlap_explicit)
{
    test_merge(
        {"[0, 1, 2]", "[3, 4, 5]", "[6, 7, 8]"},
        "[0, 1, 2, 3, 4, 5, 6, 7, 8]"
    );
}

TEST(merge, simple_seq_no_overlap_implicit)
{
    test_merge(
        {"0, 1, 2", "3, 4, 5", "6, 7, 8"},
        "0, 1, 2, 3, 4, 5, 6, 7, 8"
    );
}

TEST(merge, simple_seq_overlap_explicit)
{
    test_merge(
        {"[0, 1, 2]", "[1, 2, 3]", "[2, 3, 4]"},
        "[0, 1, 2, 1, 2, 3, 2, 3, 4]"
        // or this? "[0, 1, 2, 3, 4]"
    );
}

TEST(merge, simple_seq_overlap_implicit)
{
    // now a bit more difficult
    test_merge(
        {"0, 1, 2", "1, 2, 3", "2, 3, 4"},
        "0, 1, 2, 1, 2, 3, 2, 3, 4"
        // or this? "0, 1, 2, 3, 4"
    );
}

TEST(merge, simple_map_orthogonal)
{
    test_merge(
        {"a: 0", "b: 1", "c: 2"},
        "{a: 0, b: 1, c: 2}"
    );
}

TEST(merge, simple_map_overriding)
{
    test_merge(
        {"a: 0", "{a: 1, b: 1}", "c: 2"},
        "{a: 1, b: 1, c: 2}"
    );
}

} // namespace yml
} // namespace c4
