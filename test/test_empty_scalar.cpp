#include "./test_group.hpp"
#include <string>

namespace c4 {
namespace yml {

// See also:
// https://github.com/biojppm/rapidyaml/issues/263
// https://github.com/biojppm/rapidyaml/pulls/264

C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")

constexpr const NodeType_e DQV = (NodeType_e)(DOC | QV);

TEST(empty_scalar, parse_zero_length_strings)
{
    char inp[] = R"(
seq:
  - ""
  - ''
  - >
  - |
map:
  a: ""
  b: ''
  c: >
  d: |
)";
    const Tree tr = parse_in_place(inp);
    EXPECT_TRUE(tr["seq"].has_key());
    EXPECT_TRUE(tr["map"].has_key());
    EXPECT_TRUE(tr["seq"].is_seq());
    EXPECT_TRUE(tr["map"].is_map());
    for(const char *name : {"seq", "map"})
    {
        ConstNodeRef node = tr[to_csubstr(name)];
        ASSERT_EQ(node.num_children(), 4);
        for(const auto &child : node.children())
        {
            EXPECT_TRUE(child.is_val_quoted());
            EXPECT_EQ(child.val(), "");
            EXPECT_EQ(child.val(), nullptr);
            EXPECT_FALSE(child.val_is_null());
        }
    }
}

TEST(empty_scalar, parse_empty_strings)
{
    char inp[] = R"(
# use multiple empty entries to ensure the parser
# correctly deals with the several cases
seq:
  -
  -
  - 
  -
map:
  a:
  b:
  c: 
  d: 
)";
    const Tree tr = parse_in_place(inp);
    for(const char *name : {"seq", "map"})
    {
        ConstNodeRef node = tr[to_csubstr(name)];
        ASSERT_EQ(node.num_children(), 4);
        for(const auto &child : node.children())
        {
            EXPECT_FALSE(child.type().is_val_quoted());
            EXPECT_EQ(child.val(), "");
            EXPECT_EQ(child.val(), nullptr);
            EXPECT_TRUE(child.val_is_null());
        }
    }
}

TEST(empty_scalar, gcc_error)
{
    Tree tr;
    csubstr nullstr = {};
    ASSERT_EQ(nullstr.str, nullptr);
    ASSERT_EQ(nullstr.len, 0);
    std::cout << "\nserializing with empty arena...\n";
    csubstr result = tr.to_arena(nullstr);
    EXPECT_EQ(result.str, nullptr); // fails!
    EXPECT_EQ(result.len, 0);
    std::cout << "\nserializing with nonempty arena...\n";
    result = tr.to_arena(nullstr);
    EXPECT_EQ(result.str, nullptr); // fails!
    EXPECT_EQ(result.len, 0);
}

TEST(empty_scalar, build_zero_length_string)
{
    Tree tr;
    NodeRef root = tr.rootref();
    root |= MAP;
    auto addseq = [&root](csubstr name) { NodeRef n = root[name]; n |= SEQ; return n; };

    // try both with nonnull-zero-length and null-zero-length
    csubstr empty = csubstr("nonempty").first(0);
    csubstr nullss = {};

    // these are the conditions we wish to cover:
    ASSERT_TRUE(empty.str != nullptr);
    ASSERT_TRUE(empty.len == 0u);
    ASSERT_TRUE(nullss.str == nullptr);
    ASSERT_TRUE(nullss.len == 0u);

    // = and << must have exactly the same behavior where nullity is
    // regarded

    // quoted cases will never be null, regardless of the
    // incoming scalar
    NodeRef quoted = addseq("quoted");
    {NodeRef r = quoted.append_child(); r = ""      ; r.set_type(r.type() | VALQUO);}
    {NodeRef r = quoted.append_child(); r << ""     ; r.set_type(r.type() | VALQUO);}
    {NodeRef r = quoted.append_child(); r = empty   ; r.set_type(r.type() | VALQUO);}
    {NodeRef r = quoted.append_child(); r << empty  ; r.set_type(r.type() | VALQUO);}
    {NodeRef r = quoted.append_child(); r = nullss  ; r.set_type(r.type() | VALQUO);}
    {NodeRef r = quoted.append_child(); r << nullss ; r.set_type(r.type() | VALQUO);}
    {NodeRef r = quoted.append_child(); r = nullptr ; r.set_type(r.type() | VALQUO);}
    {NodeRef r = quoted.append_child(); r << nullptr; r.set_type(r.type() | VALQUO);}
    ASSERT_TRUE(quoted.has_children());
    for(ConstNodeRef child : quoted.cchildren())
    {
        EXPECT_TRUE(child.is_val_quoted());
        EXPECT_EQ(child.val(), "");
        EXPECT_EQ(child.val(), nullptr);
        EXPECT_FALSE(child.val_is_null());
    }

    // ... but according to the incoming scalar, non quoted cases may
    // or may not be null
    NodeRef non_quoted = addseq("nonquoted");
    NodeRef non_quoted_null = addseq("nonquoted_null");
    non_quoted.append_child() = "";
    non_quoted.append_child() << "";
    non_quoted.append_child() = empty;
    non_quoted.append_child() << empty;
    non_quoted_null.append_child() = nullss;
    non_quoted_null.append_child() << nullss;
    non_quoted_null.append_child() = nullptr;
    non_quoted_null.append_child() << nullptr;
    ASSERT_TRUE(non_quoted.has_children());
    ASSERT_TRUE(non_quoted_null.has_children());
    size_t pos = 0;
    for(ConstNodeRef child : non_quoted.cchildren())
    {
        EXPECT_TRUE(child.is_val()) << "pos=" << pos;
        EXPECT_EQ(child.val(), "") << "pos=" << pos;
        EXPECT_EQ(child.val(), nullptr) << "pos=" << pos;
        EXPECT_EQ(child.val().len, 0u) << "pos=" << pos;
        EXPECT_NE(child.val().str, nullptr) << "pos=" << pos;
        EXPECT_FALSE(child.val_is_null()) << "pos=" << pos;
        ++pos;
    }
    pos = 0;
    for(ConstNodeRef child : non_quoted_null.cchildren())
    {
        EXPECT_TRUE(child.is_val()) << "pos=" << pos;
        EXPECT_EQ(child.val(), "") << "pos=" << pos;
        EXPECT_EQ(child.val(), nullptr) << "pos=" << pos;
        EXPECT_EQ(child.val().len, 0u) << "pos=" << pos;
        EXPECT_EQ(child.val().str, nullptr) << "pos=" << pos;
        EXPECT_TRUE(child.val_is_null()) << "pos=" << pos;
        ++pos;
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
