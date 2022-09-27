#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include <c4/fs/fs.hpp>

#include "./test_case.hpp"

#include <gtest/gtest.h>

namespace c4 {
namespace yml {

template<class Emit>
std::string emit2file(Emit &&fn)
{
    C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4996) // fopen unsafe
    std::string filename = fs::tmpnam<std::string>();
    FILE *f = fopen(filename.c_str(), "wb");
    C4_CHECK(f != nullptr);
    fn(f);
    fflush(f);
    fclose(f);
    std::string result = fs::file_get_contents<std::string>(filename.c_str());
    fs::rmfile(filename.c_str());
    return result;
    C4_SUPPRESS_WARNING_MSVC_POP
}

template<class Emit>
std::string emit2stream(Emit &&fn)
{
    std::ostringstream ss;
    fn(ss);
    return ss.str();
}

template<class Emit>
std::string emit2buf(Emit &&fn)
{
    std::string buf;
    buf.resize(2048);
    substr out = fn(to_substr(buf));
    if(out.len > buf.size())
    {
        buf.resize(out.len);
        out = fn(to_substr(buf));
    }
    buf.resize(out.len);
    return buf;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(as_json, basic)
{
    Tree et;
    {
        as_json j(et);
        EXPECT_EQ(j.tree, &et);
    }
    Tree t = parse_in_arena("[foo, bar]");
    {
        as_json j(t);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, t.root_id());
    }
    {
        as_json j(t, 2u);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, 2u);
    }
    {
        as_json j(t[0]);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, 1u);
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<class ...Args>
void test_emits(Tree const& t, size_t id, std::string const& expected, std::string const& expected_json)
{
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, id, buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t, id, buf); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t, id, f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t, id, f); }), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(t, id), expected);
    EXPECT_EQ(emitrs_json<std::string>(t, id), expected_json);
}

template<class ...Args>
void test_emits(Tree const& t, std::string const& expected, std::string const& expected_json)
{
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t, buf); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t, f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t, f); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostream& s){ s << t; }), expected);
    EXPECT_EQ(emit2stream([&](std::ostream& s){ s << as_json(t); }), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(t), expected);
    EXPECT_EQ(emitrs_json<std::string>(t), expected_json);
}

template<class ...Args>
void test_emits(ConstNodeRef t, std::string const& expected, std::string const& expected_json)
{
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t, buf); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t, f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t, f); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostream& s){ s << t; }), expected);
    EXPECT_EQ(emit2stream([&](std::ostream& s){ s << as_json(t); }), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(t), expected);
    EXPECT_EQ(emitrs_json<std::string>(t), expected_json);
}


TEST(emit, empty_tree)
{
    const Tree t; // must be const!
    std::string expected = R"()";
    test_emits(t, expected, expected);
}

TEST(emit, existing_tree)
{
    const Tree t = parse_in_arena("[foo, bar]");
    std::string expected = "- foo\n- bar\n";
    std::string expected_json = R"(["foo","bar"])";
    test_emits(t, expected, expected_json);
}

TEST(emit, existing_seq_node)
{
    Tree nct = parse_in_arena("[foo, bar, [nested, seq], {nested: map}]");
    Tree const& t = nct;
    {
        std::string expected = "- foo\n- bar\n- - nested\n  - seq\n- nested: map\n";
        std::string expected_json = R"(["foo","bar",["nested","seq"],{"nested": "map"}])";
        {
            SCOPED_TRACE("rootref");
            test_emits(t.crootref(), expected, expected_json);
        }
        {
            SCOPED_TRACE("t");
            test_emits(t, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, t.root_id(), expected, expected_json);
        }
    }
    {
        ConstNodeRef n = t[0];
        std::string expected = "foo\n";
        std::string expected_json = "\"foo\"";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
        nct._add_flags(n.id(), _WIP_STYLE_FLOW_SL);
        expected = "foo";
        {
            SCOPED_TRACE("t, id");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        ConstNodeRef n = t[1];
        std::string expected = "bar\n";
        std::string expected_json = "\"bar\"";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
        nct._add_flags(n.id(), _WIP_STYLE_FLOW_SL);
        expected = "bar";
        {
            SCOPED_TRACE("t, id");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }

    }
    {
        ConstNodeRef n = t[2];
        std::string expected = "- nested\n- seq\n";
        std::string expected_json = "[\"nested\",\"seq\"]";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
        expected = "[nested,seq]";
        nct._add_flags(n.id(), _WIP_STYLE_FLOW_SL);
        {
            SCOPED_TRACE("t, id");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        ConstNodeRef n = t[3];
        std::string expected = "nested: map\n";
        std::string expected_json = "{\"nested\": \"map\"}";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
        expected = "{nested: map}";
        nct._add_flags(n.id(), _WIP_STYLE_FLOW_SL);
        {
            SCOPED_TRACE("t, id");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
}

TEST(emit, existing_map_node)
{
    Tree nct = parse_in_arena("{0: foo, 1: bar, 2: [nested, seq], 3: {nested: map}}");
    Tree const& t = nct;
    {
        std::string expected = "0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n";
        std::string expected_json = R"({"0": "foo","1": "bar","2": ["nested","seq"],"3": {"nested": "map"}})";
        {
            SCOPED_TRACE("rootref");
            test_emits(t.rootref(), expected, expected_json);
        }
        {
            SCOPED_TRACE("t");
            test_emits(t, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, t.root_id(), expected, expected_json);
        }
    }
    {
        ConstNodeRef n = t[0];
        std::string expected = "0: foo\n";
        std::string expected_json = "\"0\": \"foo\"";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
        expected = "0: foo";
        nct._add_flags(n.id(), _WIP_STYLE_FLOW_SL);
        {
            SCOPED_TRACE("t, id");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        ConstNodeRef n = t[1];
        std::string expected = "1: bar\n";
        std::string expected_json = "\"1\": \"bar\"";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
        expected = "1: bar";
        nct._add_flags(n.id(), _WIP_STYLE_FLOW_SL);
        {
            SCOPED_TRACE("t, id");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        ConstNodeRef n = t[2];
        std::string expected = "2:\n  - nested\n  - seq\n";
        std::string expected_json = "\"2\": [\"nested\",\"seq\"]";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
        expected = "2: [nested,seq]";
        nct._add_flags(n.id(), _WIP_STYLE_FLOW_SL);
        {
            SCOPED_TRACE("t, id");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        ConstNodeRef n = t[3];
        std::string expected = "3:\n  nested: map\n";
        std::string expected_json = "\"3\": {\"nested\": \"map\"}";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
        expected = "3: {nested: map}";
        nct._add_flags(n.id(), _WIP_STYLE_FLOW_SL);
        {
            SCOPED_TRACE("t, id");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
}

TEST(emit, percent_is_quoted)
{
    Tree ti = parse_in_arena("{}");
    ASSERT_TRUE(ti.rootref().is_map());
    ti["%ROOT"] = "%VAL";
    ti["%ROOT2"] |= SEQ;
    ti["%ROOT2"][0] = "%VAL";
    ti["%ROOT2"][1] = "%VAL";
    std::string yaml = emitrs_yaml<std::string>(ti);
    test_check_emit_check(to_csubstr(yaml), [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("%ROOT"));
        ASSERT_TRUE(t.rootref().has_child("%ROOT2"));
        ASSERT_EQ(t["%ROOT2"].num_children(), 2u);
        EXPECT_TRUE(t["%ROOT"].is_key_quoted());
        EXPECT_TRUE(t["%ROOT"].is_val_quoted());
        EXPECT_TRUE(t["%ROOT2"].is_key_quoted());
        EXPECT_TRUE(t["%ROOT2"][0].is_val_quoted());
        EXPECT_TRUE(t["%ROOT2"][1].is_val_quoted());
    });
}

TEST(emit, at_is_quoted__issue_309)
{
    Tree ti = parse_in_arena("{at: [], backtick: []");
    ti["at"][0] << "@test";
    ti["at"][1] = "@test2";
    ti["at"][2] << "@";
    ti["at"][3] = "@";
    ti["backtick"][0] << "`test";
    ti["backtick"][1] = "`test2";
    ti["backtick"][2] << "`";
    ti["backtick"][3] = "`";
    std::string yaml = emitrs_yaml<std::string>(ti);
    test_check_emit_check(to_csubstr(yaml), [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("at"));
        ASSERT_TRUE(t.rootref().has_child("backtick"));
        ASSERT_EQ(t["at"][0].val(), "@test");
        ASSERT_EQ(t["at"][1].val(), "@test2");
        ASSERT_EQ(t["at"][2].val(), "@");
        ASSERT_EQ(t["at"][3].val(), "@");
        ASSERT_TRUE(t["at"][0].is_val_quoted());
        ASSERT_TRUE(t["at"][1].is_val_quoted());
        ASSERT_TRUE(t["at"][2].is_val_quoted());
        ASSERT_TRUE(t["at"][3].is_val_quoted());
        ASSERT_EQ(t["backtick"][0].val(), "`test");
        ASSERT_EQ(t["backtick"][1].val(), "`test2");
        ASSERT_EQ(t["backtick"][2].val(), "`");
        ASSERT_EQ(t["backtick"][3].val(), "`");
        ASSERT_TRUE(t["backtick"][0].is_val_quoted());
        ASSERT_TRUE(t["backtick"][1].is_val_quoted());
        ASSERT_TRUE(t["backtick"][2].is_val_quoted());
        ASSERT_TRUE(t["backtick"][3].is_val_quoted());
    });
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
