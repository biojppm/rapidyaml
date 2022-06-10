#include "./test_group.hpp"
#include <string>

namespace c4 {
namespace yml {

/**
    See also issue 263.
 */

C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")

constexpr const NodeType_e DQV = (NodeType_e)(DOC | QV);

TEST(empty_scalar, parse_zero_length_strings)
{
    char inp[] = R"(
- ""
- ''
- >
- |
)";

    Tree tr = parse_in_place(inp);

    ASSERT_EQ(tr.rootref().num_children(), 4);
    for(const auto &child : tr.rootref().children())
    {
        ASSERT_EQ(child.type(), QV);
        ASSERT_EQ(child.val(), "");
        ASSERT_FALSE(child.val_is_null());
    }
}

TEST(empty_scalar, build_zero_length_string)
{
    Tree tr;
    NodeRef root = tr.rootref();
    root |= MAP;

    NodeRef non_quoted = root["non_quoted"];
    non_quoted |= SEQ;

    const std::string e;  // empty std::string

    non_quoted.append_child() << "";
    non_quoted.append_child() = "";
    non_quoted.append_child() << e;

    NodeRef quoted = root["quoted"];
    quoted |= SEQ;
    {auto r = quoted.append_child(); r << ""; r.set_type(r.type() | VALQUO);}
    {auto r = quoted.append_child(); r = "";  r.set_type(r.type() | VALQUO);}
    {auto r = quoted.append_child(); r << e;  r.set_type(r.type() | VALQUO);}

    ASSERT_EQ(non_quoted.num_children(), 3);
    for(const auto &child : non_quoted.children())
    {
        EXPECT_EQ(child.type(), VAL);
        EXPECT_EQ(child.val(), "");
        EXPECT_FALSE(child.val_is_null());
    }

    ASSERT_EQ(quoted.num_children(), 3);
    for(const auto &child : quoted.children())
    {
        EXPECT_EQ(child.type(), VAL | VALQUO);
        EXPECT_EQ(child.val(), "");
        EXPECT_FALSE(child.val_is_null());
    }
}

CASE_GROUP(EMPTY_SCALAR)
{
ADD_CASE_TO_GROUP("empty scalar, single quoted",
                  "''",
                  N(DQV, "")
);
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_POP
