#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"
#include <string>

namespace c4 {
namespace yml {

// See also:
// https://github.com/biojppm/rapidyaml/issues/263
// https://github.com/biojppm/rapidyaml/pull/264

C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")

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

void test_empty_squo(ConstNodeRef ch)
{
    SCOPED_TRACE(ch.id());
    EXPECT_NE((ch.type() & VALQUO), 0u);
    EXPECT_NE((ch.type() & VAL_SQUO), 0u);
    EXPECT_TRUE((ch.type() & VAL_SQUO) == VAL_SQUO);
    EXPECT_TRUE(ch.tree()->is_val_quoted(ch.id()));
    EXPECT_TRUE(ch.is_val_quoted());
    EXPECT_FALSE(ch.val_is_null());
    EXPECT_EQ(ch.val().len, 0);
    EXPECT_NE(ch.val().str, nullptr);
    EXPECT_NE(ch.val(), nullptr);
}
TEST(empty_scalar, flow_seq0)
{
    test_check_emit_check("['', '']", [&](Tree const &t){
        EXPECT_TRUE(t.rootref().has_children());
        EXPECT_EQ(t.rootref().num_children(), 2);
        for(ConstNodeRef ch : t.rootref().children())
            test_empty_squo(ch);
    });
}
TEST(empty_scalar, flow_seq1)
{
    test_check_emit_check("['', ]", [&](Tree const &t){
        EXPECT_TRUE(t.rootref().has_children());
        EXPECT_EQ(t.rootref().num_children(), 1);
        for(ConstNodeRef ch : t.rootref().children())
            test_empty_squo(ch);
    });
}

TEST(empty_scalar, parse_empty_strings)
{
    // use multiple empty entries to ensure the parser
    // correctly deals with the several cases
    char inp[] = R"(
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
    #ifdef RYML_DBG
    print_tree(tr);
    #endif
    for(const char *name : {"seq", "map"})
    {
        SCOPED_TRACE(name);
        ConstNodeRef node = tr[to_csubstr(name)];
        ASSERT_EQ(node.num_children(), 4);
        size_t pos = 0;
        for(const auto &child : node.children())
        {
            SCOPED_TRACE(pos);
            EXPECT_FALSE(child.type().is_val_quoted());
            EXPECT_EQ(child.val(), "");
            EXPECT_EQ(child.val(), nullptr);
            EXPECT_EQ(child.val().str, nullptr);
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_TRUE(child.val_is_null());
            ++pos;
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
              "{ser: {stdstr: '',nullss: },eq: {stdstr: '',nullss: }}");
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
    {
        SCOPED_TRACE("serializing with empty arena");
        csubstr result = tr.to_arena(nullstr);
        EXPECT_EQ(result.str, nullptr); // fails!
        EXPECT_EQ(result.len, 0);
    }
    {
        SCOPED_TRACE("serializing with nonempty arena");
        csubstr result = tr.to_arena(nullstr);
        EXPECT_EQ(result.str, nullptr); // fails!
        EXPECT_EQ(result.len, 0);
    }
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
        NodeRef quoted = addseq("s-quoted");
        {NodeRef r = quoted.append_child(); r = ""      ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted.append_child(); r << ""     ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted.append_child(); r = empty   ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted.append_child(); r << empty  ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted.append_child(); r = stdss   ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted.append_child(); r << stdss  ; r.set_type(r.type() | VAL_SQUO);}
    }
    {
        NodeRef quoted = addseq("d-quoted");
        {NodeRef r = quoted.append_child(); r = ""      ; r.set_type(r.type() | VAL_DQUO);}
        {NodeRef r = quoted.append_child(); r << ""     ; r.set_type(r.type() | VAL_DQUO);}
        {NodeRef r = quoted.append_child(); r = empty   ; r.set_type(r.type() | VAL_DQUO);}
        {NodeRef r = quoted.append_child(); r << empty  ; r.set_type(r.type() | VAL_DQUO);}
        {NodeRef r = quoted.append_child(); r = stdss   ; r.set_type(r.type() | VAL_DQUO);}
        {NodeRef r = quoted.append_child(); r << stdss  ; r.set_type(r.type() | VAL_DQUO);}
    }
    {
        NodeRef quoted_null = addseq("quoted_null");
        {NodeRef r = quoted_null.append_child(); r = nullss  ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted_null.append_child(); r << nullss ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted_null.append_child(); r = nullptr ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted_null.append_child(); r << nullptr; r.set_type(r.type() | VAL_SQUO);}
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
                SCOPED_TRACE(pos);
                EXPECT_TRUE(child.is_val_quoted());
                EXPECT_EQ(child.val().len, 0u);
                EXPECT_NE(child.val().str, nullptr);
                EXPECT_NE(child.val(), nullptr);
                EXPECT_EQ(child.val(), "");
                EXPECT_FALSE(child.val_is_null());
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
            SCOPED_TRACE(pos);
            EXPECT_TRUE(child.is_val_quoted());
            EXPECT_FALSE(child.val_is_null()); // because it's quoted
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_EQ(child.val().str, nullptr);
            EXPECT_EQ(child.val(), nullptr);
            EXPECT_EQ(child.val(), "");
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
            SCOPED_TRACE(pos);
            EXPECT_TRUE(child.is_val());
            EXPECT_FALSE(child.val_is_null()); // because it's quoted
            EXPECT_EQ(child.val(), "");
            EXPECT_NE(child.val(), nullptr);
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_NE(child.val().str, nullptr);
            ++pos;
        }
    };
    auto test_non_quoted_null = [](ConstNodeRef node){
        SCOPED_TRACE(node.key());
        ASSERT_TRUE(node.has_children());
        size_t pos = 0;
        for(ConstNodeRef child : node.cchildren())
        {
            SCOPED_TRACE(pos);
            EXPECT_TRUE(child.is_val());
            EXPECT_EQ(child.val(), "");
            EXPECT_EQ(child.val(), nullptr);
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_EQ(child.val().str, nullptr);
            EXPECT_TRUE(child.val_is_null());
            ++pos;
        }
    };

    std::string yaml = emitrs_yaml<std::string>(tr);
    #ifdef RYML_DBG
    print_tree(tr);
    printf("~~~~~\n%.*s~~~~\n", (int)yaml.size(), yaml.c_str());
    #endif

    {
        SCOPED_TRACE("input tree");
        test_quoted_empty(tr["s-quoted"]);
        test_quoted_empty(tr["d-quoted"]);
        // in the built tree, the values will be quoted and null
        test_quoted_null(tr["quoted_null"]);
        test_non_quoted_empty(tr["nonquoted"]);
        test_non_quoted_null(tr["nonquoted_null"]);
    }

    test_check_emit_check(to_csubstr(yaml), [&](Tree const &t){
        SCOPED_TRACE("output tree");
        test_quoted_empty(t["s-quoted"]);
        test_quoted_empty(t["d-quoted"]);
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
                  N(VS, "")
);
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_POP
