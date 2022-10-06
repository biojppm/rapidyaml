#include "./test_group.hpp"
#include <string>

namespace c4 {
namespace yml {

// See also:
// https://github.com/biojppm/rapidyaml/issues/263
// https://github.com/biojppm/rapidyaml/pull/264

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
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_NE(child.val().str, nullptr);
            EXPECT_NE(child.val(), nullptr);
            EXPECT_EQ(child.val(), "");
            EXPECT_FALSE(child.val_is_null());
        }
    }
}

TEST(empty_scalar, flow_seq)
{
    test_check_emit_check("['', '']", [&](Tree const &t){
        ASSERT_TRUE(t.rootref().has_children());
        for(ConstNodeRef ch : t.rootref().children())
        {
            EXPECT_TRUE(ch.is_val_quoted());
            EXPECT_FALSE(ch.val_is_null());
            EXPECT_EQ(ch.val().len, 0);
            EXPECT_NE(ch.val().str, nullptr);
            EXPECT_NE(ch.val(), nullptr);
        }
    });
    test_check_emit_check("[ , ]", [&](Tree const &t){
        ASSERT_TRUE(t.rootref().has_children());
        for(ConstNodeRef ch : t.rootref().children())
        {
            EXPECT_FALSE(ch.is_val_quoted());
            EXPECT_TRUE(ch.val_is_null());
            EXPECT_EQ(ch.val().len, 0);
            EXPECT_EQ(ch.val().str, nullptr);
            EXPECT_EQ(ch.val(), nullptr);
        }
    });
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
            EXPECT_EQ(child.val().str, nullptr);
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_TRUE(child.val_is_null());
        }
    }
}

TEST(empty_scalar, std_string)
{
    std::string stdstr;
    csubstr stdss = to_csubstr(stdstr);
    csubstr nullss;
    EXPECT_NE(stdss, nullptr);
    EXPECT_NE(stdss.str, nullptr);
    EXPECT_EQ(stdss.len, 0u);
    EXPECT_EQ(nullss, nullptr);
    EXPECT_EQ(nullss.str, nullptr);
    EXPECT_EQ(nullss.len, 0u);
    Tree tree = parse_in_arena("{ser: {}, eq: {}}");
    tree["ser"]["stdstr"] << stdss;
    tree["ser"]["nullss"] << nullss;
    tree["eq"]["stdstr"] = stdss;
    tree["eq"]["nullss"] = nullss;
    EXPECT_EQ(emitrs_yaml<std::string>(tree),
             "ser:\n"
             "  stdstr: ''\n"
             "  nullss: \n"
             "eq:\n"
             "  stdstr: ''\n"
             "  nullss: \n"
              );
}

TEST(empty_scalar, to_arena)
{
    Tree tr;
    {
        const char *val = "";
        size_t num = to_chars(substr{}, val);
        ASSERT_EQ(num, 0u);
        char buf_[10];
        csubstr serialized = to_chars_sub(buf_, val);
        EXPECT_EQ(serialized.len, 0);
        EXPECT_NE(serialized.str, nullptr);
        EXPECT_NE(serialized, nullptr);
        csubstr r = tr.to_arena("");
        EXPECT_EQ(r.len, 0u);
        EXPECT_NE(r.str, nullptr);
        EXPECT_NE(r, nullptr);
    }
    {
        const char *val = nullptr;
        size_t num = to_chars(substr{}, val);
        ASSERT_EQ(num, 0u);
        char buf_[10];
        csubstr serialized = to_chars_sub(buf_, val);
        EXPECT_EQ(serialized.len, 0);
        EXPECT_NE(serialized.str, nullptr);
        EXPECT_NE(serialized, nullptr);
        csubstr r = tr.to_arena("");
        EXPECT_EQ(r.len, 0u);
        EXPECT_NE(r.str, nullptr);
        EXPECT_NE(r, nullptr);
        r = tr.to_arena(val);
        EXPECT_EQ(r.len, 0u);
        EXPECT_EQ(r.str, nullptr);
        EXPECT_EQ(r, nullptr);
    }
    {
        std::nullptr_t val = nullptr;
        size_t num = to_chars(substr{}, val);
        ASSERT_EQ(num, 0u);
        csubstr r = tr.to_arena(val);
        EXPECT_EQ(r.len, 0u);
        EXPECT_EQ(r.str, nullptr);
        EXPECT_EQ(r, nullptr);
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
    std::string stdstr;
    csubstr stdss = to_csubstr(stdstr);
    csubstr empty = csubstr("nonempty").first(0);
    csubstr nullss = {};

    // these are the conditions we wish to cover:
    ASSERT_TRUE(stdss.str != nullptr);
    ASSERT_TRUE(stdss.len == 0u);
    ASSERT_TRUE(empty.str != nullptr);
    ASSERT_TRUE(empty.len == 0u);
    ASSERT_TRUE(nullss.str == nullptr);
    ASSERT_TRUE(nullss.len == 0u);

    // = and << must have exactly the same behavior where nullity is
    // regarded

    {
        NodeRef quoted = addseq("quoted");
        {NodeRef r = quoted.append_child(); r = ""      ; r.set_type(r.type() | VALQUO);}
        {NodeRef r = quoted.append_child(); r << ""     ; r.set_type(r.type() | VALQUO);}
        {NodeRef r = quoted.append_child(); r = empty   ; r.set_type(r.type() | VALQUO);}
        {NodeRef r = quoted.append_child(); r << empty  ; r.set_type(r.type() | VALQUO);}
        {NodeRef r = quoted.append_child(); r = stdss   ; r.set_type(r.type() | VALQUO);}
        {NodeRef r = quoted.append_child(); r << stdss  ; r.set_type(r.type() | VALQUO);}
    }
    {
        NodeRef quoted_null = addseq("quoted_null");
        {NodeRef r = quoted_null.append_child(); r = nullss  ; r.set_type(r.type() | VALQUO);}
        {NodeRef r = quoted_null.append_child(); r << nullss ; r.set_type(r.type() | VALQUO);}
        {NodeRef r = quoted_null.append_child(); r = nullptr ; r.set_type(r.type() | VALQUO);}
        {NodeRef r = quoted_null.append_child(); r << nullptr; r.set_type(r.type() | VALQUO);}
    }
    {
        NodeRef non_quoted = addseq("nonquoted");
        non_quoted.append_child() = "";
        non_quoted.append_child() << "";
        non_quoted.append_child() = empty;
        non_quoted.append_child() << empty;
        non_quoted.append_child() = stdss;
        non_quoted.append_child() << stdss;
    }
    {
        NodeRef non_quoted_null = addseq("nonquoted_null");
        non_quoted_null.append_child() = nullss;
        non_quoted_null.append_child() << nullss;
        non_quoted_null.append_child() = nullptr;
        non_quoted_null.append_child() << nullptr;
    }

    // quoted cases will never be null, regardless of the
    // incoming scalar
    auto test_quoted_empty = [](ConstNodeRef node){
        SCOPED_TRACE(node.key());
        ASSERT_TRUE(node.has_children());
        {
            size_t pos = 0;
            for(ConstNodeRef child : node.cchildren())
            {
                EXPECT_TRUE(child.is_val_quoted()) << "pos=" << pos;
                EXPECT_EQ(child.val().len, 0u) << "pos=" << pos;
                EXPECT_NE(child.val().str, nullptr) << "pos=" << pos;
                EXPECT_NE(child.val(), nullptr) << "pos=" << pos;
                EXPECT_EQ(child.val(), "") << "pos=" << pos;
                EXPECT_FALSE(child.val_is_null()) << "pos=" << pos;
                pos++;
            }
        }
    };
    auto test_quoted_null = [](ConstNodeRef node){
        SCOPED_TRACE(node.key());
        ASSERT_TRUE(node.has_children());
        size_t pos = 0;
        for(ConstNodeRef child : node.cchildren())
        {
            EXPECT_TRUE(child.is_val_quoted()) << "pos=" << pos;
            EXPECT_FALSE(child.val_is_null()) << "pos=" << pos; // because it's quoted
            EXPECT_EQ(child.val().len, 0u) << "pos=" << pos;
            EXPECT_EQ(child.val().str, nullptr) << "pos=" << pos;
            EXPECT_EQ(child.val(), nullptr) << "pos=" << pos;
            EXPECT_EQ(child.val(), "") << "pos=" << pos;
            pos++;
        }
    };
    // ... but according to the incoming scalar, non quoted cases may
    // or may not be null
    auto test_non_quoted_empty = [](ConstNodeRef node){
        SCOPED_TRACE(node.key());
        ASSERT_TRUE(node.has_children());
        size_t pos = 0;
        for(ConstNodeRef child : node.cchildren())
        {
            EXPECT_TRUE(child.is_val()) << "pos=" << pos;
            EXPECT_FALSE(child.val_is_null()) << "pos=" << pos; // because it's quoted
            EXPECT_EQ(child.val(), "") << "pos=" << pos;
            EXPECT_NE(child.val(), nullptr) << "pos=" << pos;
            EXPECT_EQ(child.val().len, 0u) << "pos=" << pos;
            EXPECT_NE(child.val().str, nullptr) << "pos=" << pos;
            ++pos;
        }
    };
    auto test_non_quoted_null = [](ConstNodeRef node){
        SCOPED_TRACE(node.key());
        ASSERT_TRUE(node.has_children());
        size_t pos = 0;
        for(ConstNodeRef child : node.cchildren())
        {
            EXPECT_TRUE(child.is_val()) << "pos=" << pos;
            EXPECT_EQ(child.val(), "") << "pos=" << pos;
            EXPECT_EQ(child.val(), nullptr) << "pos=" << pos;
            EXPECT_EQ(child.val().len, 0u) << "pos=" << pos;
            EXPECT_EQ(child.val().str, nullptr) << "pos=" << pos;
            EXPECT_TRUE(child.val_is_null()) << "pos=" << pos;
            ++pos;
        }
    };

    {
        SCOPED_TRACE("input tree");
        test_quoted_empty(tr["quoted"]);
        // in the built tree, the values will be quoted and null
        test_quoted_null(tr["quoted_null"]);
        test_non_quoted_empty(tr["nonquoted"]);
        test_non_quoted_null(tr["nonquoted_null"]);
    }

    std::string yaml = emitrs_yaml<std::string>(tr);
    std::cout << yaml;
    test_check_emit_check(to_csubstr(yaml), [&](Tree const &t){
        SCOPED_TRACE("output tree");
        test_quoted_empty(t["quoted"]);
        // after a roundtrip, they will be nonnull, because the quotes win.
        test_quoted_empty(t["quoted_null"]);
        test_non_quoted_empty(t["nonquoted"]);
        test_non_quoted_null(t["nonquoted_null"]);
    });
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
