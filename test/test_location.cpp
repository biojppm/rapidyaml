#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/yml.hpp>
#endif
#include "./test_case.hpp"
#include <gtest/gtest.h>


namespace c4 {
namespace yml {

// The other test executables are written to contain the declarative-style
// YmlTestCases. This executable does not have any but the build setup
// assumes it does, and links with the test lib, which requires an existing
// get_case() function. So this is here to act as placeholder until (if?)
// proper test cases are added here. This was detected in #47 (thanks
// @cburgard).
Case const* get_case(csubstr)
{
    return nullptr;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

const ParseOptions use_locations{ParseOptions::TRACK_LOCATION};


TEST(locations, basic)
{
    csubstr yaml = R"(---
foo: definitely
bar:
  - 1
  - 2
baz:
    - 1_
    - 2_
    -     3_
)";
    Tree t;
    Parser parser(use_locations);
    parser.parse_in_arena("myfile.yml", yaml, &t);
    #define _checkloc(node, line_, col_, str) \
        {                                                               \
            const Location loc = parser.location(node);                 \
            EXPECT_EQ(loc.name, "myfile.yml");                          \
            EXPECT_EQ(loc.line, line_);                                 \
            EXPECT_EQ(loc.col, col_);                                   \
            EXPECT_EQ(t.arena().sub(loc.offset, csubstr(str).len), csubstr(str)); \
        }
    const NodeRef map = t.rootref().child(0);
    ASSERT_TRUE(map.is_map());
    ASSERT_TRUE(map.is_doc());
    _checkloc(t.rootref()  , 1u, 0u, "foo");
    _checkloc(map          , 1u, 0u, "foo");
    _checkloc(map["foo"]   , 1u, 0u, "foo");
    _checkloc(map["bar"]   , 2u, 0u, "bar");
    _checkloc(map["bar"][0], 3u, 4u, "1");
    _checkloc(map["bar"][1], 4u, 4u, "2");
    _checkloc(map["baz"]   , 5u, 0u, "baz");
    _checkloc(map["baz"][0], 6u, 6u, "1_");
    _checkloc(map["baz"][1], 7u, 6u, "2_");
    _checkloc(map["baz"][2], 8u, 10u, "3_");
}

} // namespace yml
} // namespace c4
