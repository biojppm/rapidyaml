#include "./test_group.hpp"

namespace c4 {
namespace yml {

TEST(scalar, names)
{
    using namespace c4::yml;
    EXPECT_TRUE(Parser::is_scalar_next("a"));
    EXPECT_TRUE(Parser::is_scalar_next(".foo"));
    EXPECT_TRUE(Parser::is_scalar_next("."));
    EXPECT_TRUE(Parser::is_scalar_next("-a"));
    EXPECT_TRUE(Parser::is_scalar_next("+a"));
    EXPECT_TRUE(Parser::is_scalar_next("/a"));
    EXPECT_TRUE(Parser::is_scalar_next(":a"));
    EXPECT_TRUE(Parser::is_scalar_next(":a"));
}

#define SCALAR_NAMES_CASES \
"many names",\
"many names, expl"\

#define _(name) N(#name) // makes it simpler

CASE_GROUP(SCALAR_NAMES)
{
    APPEND_CASES(

C("many names",
R"(
- a
- b
- c
- .foo
- .
- -a
- +b
- /b
- :c
- $g
)",
L{_(a), _(b), _(c), _(.foo), _(.), _(-a), _(+b), _(/b), _(:c), _($g)}
),

C("many names, expl", IGNORE_LIBYAML_PARSE_FAIL,
R"([a, b, c, .foo, ., -a, +b, /b, :c, $g])",
L{_(a), _(b), _(c), _(.foo), _(.), _(-a), _(+b), _(/b), _(:c), _($g)}
),
    )
}

INSTANTIATE_GROUP(SCALAR_NAMES);

} // namespace yml
} // namespace c4
