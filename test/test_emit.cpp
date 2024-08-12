#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include <c4/fs/fs.hpp>

#include "./test_lib/test_case.hpp"

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
    _c4dbgpf("emit result: [{}]~~~{}~~~", result.size(), to_csubstr(result));
    return result;
    C4_SUPPRESS_WARNING_MSVC_POP
}

template<class Emit>
std::string emit2stream(Emit &&fn)
{
    std::ostringstream ss;
    fn(ss);
    std::string result = ss.str();
    _c4dbgpf("emit result: [{}]~~~{}~~~", result.size(), to_csubstr(result));
    return result;
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
    _c4dbgpf("emit result: [{}]~~~{}~~~", buf.size(), to_csubstr(buf));
    return buf;
}

template<class Emit>
std::string emitrs_append(csubstr first_part, Emit &&fn)
{
    std::string buf(first_part.begin(), first_part.end());
    fn(&buf);
    _c4dbgpf("emit result: [{}]~~~{}~~~", buf.size(), to_csubstr(buf));
    return buf;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(as_yaml, basic)
{
    Tree et;
    {
        as_yaml j(et);
        EXPECT_EQ(j.tree, &et);
    }
    Tree t = parse_in_arena("[foo, bar]");
    {
        as_yaml j(t);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, t.root_id());
        EXPECT_EQ(j.options, EmitOptions{});
    }
    {
        EmitOptions opts = EmitOptions{}.max_depth(10);
        as_yaml j(t, opts);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, t.root_id());
        EXPECT_EQ(j.options, opts);
    }
    {
        as_yaml j(t, 2u);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, 2u);
        EXPECT_EQ(j.options, EmitOptions{});
    }
    {
        EmitOptions opts = EmitOptions{}.max_depth(10);
        as_yaml j(t, 2u, opts);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, 2u);
        EXPECT_EQ(j.options, opts);
    }
    {
        as_yaml j(t[0]);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, 1u);
        EXPECT_EQ(j.options, EmitOptions{});
    }
    {
        EmitOptions opts = EmitOptions{}.max_depth(10);
        as_yaml j(t[0], opts);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, 1u);
        EXPECT_EQ(j.options, opts);
    }
}

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
        EXPECT_EQ(j.options, EmitOptions{});
    }
    {
        EmitOptions opts = EmitOptions{}.max_depth(10);
        as_json j(t, opts);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, t.root_id());
        EXPECT_EQ(j.options, opts);
    }
    {
        as_json j(t, 2u);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, 2u);
        EXPECT_EQ(j.options, EmitOptions{});
    }
    {
        EmitOptions opts = EmitOptions{}.max_depth(10);
        as_json j(t, 2u, opts);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, 2u);
        EXPECT_EQ(j.options, opts);
    }
    {
        as_json j(t[0]);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, 1u);
        EXPECT_EQ(j.options, EmitOptions{});
    }
    {
        EmitOptions opts = EmitOptions{}.max_depth(10);
        as_json j(t[0], opts);
        EXPECT_EQ(j.tree, &t);
        EXPECT_EQ(j.node, 1u);
        EXPECT_EQ(j.options, opts);
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


void test_emits(Tree const& t, id_type id, std::string const& expected, std::string const& expected_json)
{
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, id,                buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, id, EmitOptions{}, buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, id, EmitOptions{}, buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t, id,                buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t, id, EmitOptions{}, buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_YAML, t, id, /*error_on_excess*/true); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_JSON, t, id, /*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t, id,                f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t, id, EmitOptions{}, f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t, id,                f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t, id, EmitOptions{}, f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(f); return em.emit_as(EMIT_YAML, t, id, /*error_on_excess*/true).len; }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(f); return em.emit_as(EMIT_JSON, t, id, /*error_on_excess*/true).len; }), expected_json);
    if(id != NONE)
    {
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(t, id);                }), expected);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(t, id, EmitOptions{}); }), expected);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(t, id);                }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(t, id, EmitOptions{}); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em({}, oss); em.emit_as(EMIT_YAML, t, id, /*error_on_excess*/true); }), expected);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em({}, oss); em.emit_as(EMIT_JSON, t, id, /*error_on_excess*/true); }), expected_json);
    }
    EXPECT_EQ(emitrs_yaml<std::string>(t, id),                expected);
    EXPECT_EQ(emitrs_yaml<std::string>(t, id, EmitOptions{}), expected);
    EXPECT_EQ(emitrs_json<std::string>(t, id),                expected_json);
    EXPECT_EQ(emitrs_json<std::string>(t, id, EmitOptions{}), expected_json);
    std::string append_prefix = "#before\n";
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(t, id,                s, /*append*/true); } ), append_prefix + expected);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(t, id, EmitOptions{}, s, /*append*/true); } ), append_prefix + expected);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(t, id,                s, /*append*/true); } ), append_prefix + expected_json);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(t, id, EmitOptions{}, s, /*append*/true); } ), append_prefix + expected_json);
    // error on max depth
    if(id == NONE)
        return;
    id_type max_depth = t.depth_desc(id);
    if(max_depth > 1)
    {
        EmitOptions opts = EmitOptions{}.max_depth(0);
        ExpectError::check_error(&t, [&]{ return emit2buf([&](substr buf){ return emit_yaml(t, id, opts, buf); }); });
        ExpectError::check_error(&t, [&]{ return emit2buf([&](substr buf){ return emit_json(t, id, opts, buf); }); });
        ExpectError::check_error(&t, [&]{ return emit2file([&](FILE *f){ return emit_yaml(t, id, opts, f); }); });
        ExpectError::check_error(&t, [&]{ return emit2file([&](FILE *f){ return emit_json(t, id, opts, f); }); });
        ExpectError::check_error(&t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(t, id, opts); }); });
        ExpectError::check_error(&t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(t, id, opts); }); });
        ExpectError::check_error(&t, [&]{ return emitrs_yaml<std::string>(t, id, opts); });
        ExpectError::check_error(&t, [&]{ return emitrs_json<std::string>(t, id, opts); });
    }
}

void test_emits(Tree const& t, std::string const& expected, std::string const& expected_json)
{
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t,                buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, EmitOptions{}, buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t,                buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t, EmitOptions{}, buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_YAML, t, /*error_on_excess*/true); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_JSON, t, /*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t,                f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t, EmitOptions{}, f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t,                f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t, EmitOptions{}, f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(f); return em.emit_as(EMIT_YAML, t, /*error_on_excess*/true).len; }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(f); return em.emit_as(EMIT_JSON, t, /*error_on_excess*/true).len; }), expected_json);
    if(!t.empty())
    {
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(t);                }), expected);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(t, EmitOptions{}); }), expected);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(t);                }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(t, EmitOptions{}); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em({}, oss); em.emit_as(EMIT_YAML, t, /*error_on_excess*/true); }), expected);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em({}, oss); em.emit_as(EMIT_JSON, t, /*error_on_excess*/true); }), expected_json);
    }
    EXPECT_EQ(emitrs_yaml<std::string>(t),                expected);
    EXPECT_EQ(emitrs_yaml<std::string>(t, EmitOptions{}), expected);
    EXPECT_EQ(emitrs_json<std::string>(t),                expected_json);
    EXPECT_EQ(emitrs_json<std::string>(t, EmitOptions{}), expected_json);
    std::string append_prefix = "#before\n";
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(t,                s, /*append*/true); } ), append_prefix + expected);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(t, EmitOptions{}, s, /*append*/true); } ), append_prefix + expected);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(t,                s, /*append*/true); } ), append_prefix + expected_json);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(t, EmitOptions{}, s, /*append*/true); } ), append_prefix + expected_json);
    // error on max depth
    id_type max_depth = t.empty() ? 0 : t.depth_desc(t.root_id());
    if(max_depth > 1)
    {
        EmitOptions opts = EmitOptions{}.max_depth(0);
        ExpectError::check_error(&t, [&]{ return emit2buf([&](substr buf){ return emit_yaml(t, opts, buf); }); });
        ExpectError::check_error(&t, [&]{ return emit2buf([&](substr buf){ return emit_json(t, opts, buf); }); });
        ExpectError::check_error(&t, [&]{ return emit2file([&](FILE *f){ return emit_yaml(t, opts, f); }); });
        ExpectError::check_error(&t, [&]{ return emit2file([&](FILE *f){ return emit_json(t, opts, f); }); });
        ExpectError::check_error(&t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(ConstNodeRef(&t), opts); }); });
        ExpectError::check_error(&t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(ConstNodeRef(&t), opts); }); });
        ExpectError::check_error(&t, [&]{ return emitrs_yaml<std::string>(t, opts); });
        ExpectError::check_error(&t, [&]{ return emitrs_json<std::string>(t, opts); });
    }
}

void test_emits(ConstNodeRef n, std::string const& expected, std::string const& expected_json)
{
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(n,                buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(n, EmitOptions{}, buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(n,                buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(n, EmitOptions{}, buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_YAML, n, /*error_on_excess*/true); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_JSON, n, /*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(n,                f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(n, EmitOptions{}, f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(n,                f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(n, EmitOptions{}, f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(f); return em.emit_as(EMIT_YAML, n, /*error_on_excess*/true).len; }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(f); return em.emit_as(EMIT_JSON, n, /*error_on_excess*/true).len; }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss <<         n;                 }), expected);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(n);                }), expected);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(n, EmitOptions{}); }), expected);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(n);                }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(n, EmitOptions{}); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em({}, oss); em.emit_as(EMIT_YAML, n, /*error_on_excess*/true); }), expected);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em({}, oss); em.emit_as(EMIT_JSON, n, /*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(n),                expected);
    EXPECT_EQ(emitrs_yaml<std::string>(n, EmitOptions{}), expected);
    EXPECT_EQ(emitrs_json<std::string>(n),                expected_json);
    EXPECT_EQ(emitrs_json<std::string>(n, EmitOptions{}), expected_json);
    std::string append_prefix = "#before\n";
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(n,                s, /*append*/true); } ), append_prefix + expected);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(n, EmitOptions{}, s, /*append*/true); } ), append_prefix + expected);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(n,                s, /*append*/true); } ), append_prefix + expected_json);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(n, EmitOptions{}, s, /*append*/true); } ), append_prefix + expected_json);
    // error on max depth
    if(n.tree() && n.id() != NONE)
    {
        id_type max_depth = n.depth_desc();
        if(max_depth > 1)
        {
            EmitOptions opts = EmitOptions{}.max_depth(0);
            ExpectError::check_error(n.tree(), [&]{
                return emit2buf([&](substr buf){
                    return emit_yaml(n, opts, buf);
                });
            });
            ExpectError::check_error(n.tree(), [&]{ return emit2buf([&](substr buf){ return emit_json(n, opts, buf); }); });
            ExpectError::check_error(n.tree(), [&]{ return emit2file([&](FILE *f){ return emit_yaml(n, opts, f); }); });
            ExpectError::check_error(n.tree(), [&]{ return emit2file([&](FILE *f){ return emit_json(n, opts, f); }); });
            ExpectError::check_error(n.tree(), [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(n, opts); }); });
            ExpectError::check_error(n.tree(), [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(n, opts); }); });
            ExpectError::check_error(n.tree(), [&]{ return emitrs_yaml<std::string>(n, opts); });
            ExpectError::check_error(n.tree(), [&]{ return emitrs_json<std::string>(n, opts); });
        }
    }
}


TEST(emit, empty_tree)
{
    const Tree t; // must be const!
    std::string expected = R"()";
    test_emits(t, expected, expected);
}

TEST(emit, empty_node)
{
    std::string expected = R"()";
    test_emits(NodeRef{}, expected, expected);
    test_emits(ConstNodeRef{}, expected, expected);
}

TEST(emit, existing_tree)
{
    const Tree t = parse_in_arena("[foo, bar]");
    std::string expected = "[foo,bar]";
    std::string expected_json = R"(["foo","bar"])";
    test_emits(t, expected, expected_json);
}

TEST(emit, no_node)
{
    const Tree t = parse_in_arena("[foo, bar]");
    std::string expected = "[foo,bar]";
    std::string expected_json = R"(["foo","bar"])";
    test_emits(t, NONE, expected, expected_json);
}

TEST(emit, existing_seq_node_flow)
{
    Tree nct = parse_in_arena("[foo, bar, [nested, seq], {nested: map}]");
    Tree const& t = nct;
    {
        SCOPED_TRACE("full");
        std::string expected = "[foo,bar,[nested,seq],{nested: map}]";
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
        SCOPED_TRACE("t[0]");
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
        nct._add_flags(n.id(), FLOW_SL);
        expected = "foo";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[1]");
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
        nct._add_flags(n.id(), FLOW_SL);
        expected = "bar";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[2]");
        ConstNodeRef n = t[2];
        std::string expected = "[nested,seq]";
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
        nct._add_flags(n.id(), FLOW_SL);
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[3]");
        ConstNodeRef n = t[3];
        std::string expected = "{nested: map}";
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
        nct._add_flags(n.id(), FLOW_SL);
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
}

TEST(emit, existing_seq_node_block)
{
    Tree nct = parse_in_arena("- foo\n- bar\n- - nested\n  - seq\n- nested: map\n");
    Tree const& t = nct;
    {
        SCOPED_TRACE("full");
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
        SCOPED_TRACE("t[0]");
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
        nct._add_flags(n.id(), FLOW_SL);
        expected = "foo";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[1]");
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
        nct._add_flags(n.id(), FLOW_SL);
        expected = "bar";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[2]");
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
        nct._add_flags(n.id(), FLOW_SL);
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[3]");
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
        nct._add_flags(n.id(), FLOW_SL);
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
    }
}

TEST(emit, existing_map_node_flow)
{
    Tree nct = parse_in_arena("{0: foo, 1: bar, 2: [nested, seq], 3: {nested: map}}");
    Tree const& t = nct;
    {
        SCOPED_TRACE("root");
        std::string expected = "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}";
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
        SCOPED_TRACE("t[0]");
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
        nct._add_flags(n.id(), FLOW_SL);
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
        SCOPED_TRACE("t[1]");
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
        nct._add_flags(n.id(), FLOW_SL);
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
        SCOPED_TRACE("t[2]");
        ConstNodeRef n = t[2];
        std::string expected = "2: [nested,seq]";
        std::string expected_json = "\"2\": [\"nested\",\"seq\"]";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
        expected = "2:\n  - nested\n  - seq\n";
        nct._rem_flags(n.id(), CONTAINER_STYLE);
        nct._add_flags(n.id(), BLOCK);
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
        SCOPED_TRACE("t[3]");
        ConstNodeRef n = t[3];
        std::string expected = "3: {nested: map}";
        std::string expected_json = "\"3\": {\"nested\": \"map\"}";
        {
            SCOPED_TRACE("noderef");
            test_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("t, id");
            test_emits(t, n.id(), expected, expected_json);
        }
        expected = "3:\n  nested: map\n";
        nct._rem_flags(n.id(), CONTAINER_STYLE);
        nct._add_flags(n.id(), BLOCK);
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

TEST(emit, existing_map_node_block)
{
    Tree nct = parse_in_arena("0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n");
    Tree const& t = nct;
    {
        SCOPED_TRACE("root");
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
        SCOPED_TRACE("t[0]");
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
        nct._add_flags(n.id(), FLOW_SL);
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
        SCOPED_TRACE("t[1]");
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
        nct._add_flags(n.id(), FLOW_SL);
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
        SCOPED_TRACE("t[2]");
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
        expected = "2:\n  - nested\n  - seq\n";
        nct._rem_flags(n.id(), CONTAINER_STYLE);
        nct._add_flags(n.id(), BLOCK);
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
        SCOPED_TRACE("t[3]");
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
        expected = "3:\n  nested: map\n";
        nct._rem_flags(n.id(), CONTAINER_STYLE);
        nct._add_flags(n.id(), BLOCK);
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
    Tree ti = parse_in_arena("{at: [], backtick: []}");
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
        ASSERT_EQ(t["at"].num_children(), 4u);
        ASSERT_EQ(t["backtick"].num_children(), 4u);
        EXPECT_EQ(t["at"][0].val(), "@test");
        EXPECT_EQ(t["at"][1].val(), "@test2");
        EXPECT_EQ(t["at"][2].val(), "@");
        EXPECT_EQ(t["at"][3].val(), "@");
        EXPECT_TRUE(t["at"][0].is_val_quoted());
        EXPECT_TRUE(t["at"][1].is_val_quoted());
        EXPECT_TRUE(t["at"][2].is_val_quoted());
        EXPECT_TRUE(t["at"][3].is_val_quoted());
        EXPECT_EQ(t["backtick"][0].val(), "`test");
        EXPECT_EQ(t["backtick"][1].val(), "`test2");
        EXPECT_EQ(t["backtick"][2].val(), "`");
        EXPECT_EQ(t["backtick"][3].val(), "`");
        EXPECT_TRUE(t["backtick"][0].is_val_quoted());
        EXPECT_TRUE(t["backtick"][1].is_val_quoted());
        EXPECT_TRUE(t["backtick"][2].is_val_quoted());
        EXPECT_TRUE(t["backtick"][3].is_val_quoted());
    });
}

TEST(emit, at_is_quoted_only_in_the_beggining__issue_320)
{
    Tree ti = parse_in_arena("{at: [], backtick: []}");
    ti["at"].append_child() << "@test";
    ti["at"].append_child() << "t@est";
    ti["at"].append_child() << "test@";
    ti["at"].append_child() = "@test2";
    ti["at"].append_child() = "t@est2";
    ti["at"].append_child() = "test2@";
    ti["backtick"].append_child() << "`test";
    ti["backtick"].append_child() << "t`est";
    ti["backtick"].append_child() << "test`";
    ti["backtick"].append_child() = "`test2";
    ti["backtick"].append_child() = "t`est2";
    ti["backtick"].append_child() = "test2`";
    std::string yaml = emitrs_yaml<std::string>(ti);
    test_check_emit_check(to_csubstr(yaml), [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("at"));
        ASSERT_TRUE(t.rootref().has_child("backtick"));
        ASSERT_EQ(t["at"].num_children(), 6u);
        ASSERT_EQ(t["backtick"].num_children(), 6u);
        EXPECT_EQ(t["at"][0].val(), "@test");
        EXPECT_EQ(t["at"][1].val(), "t@est");
        EXPECT_EQ(t["at"][2].val(), "test@");
        EXPECT_EQ(t["at"][3].val(), "@test2");
        EXPECT_EQ(t["at"][4].val(), "t@est2");
        EXPECT_EQ(t["at"][5].val(), "test2@");
        EXPECT_TRUE(   t["at"][0].is_val_quoted());
        EXPECT_TRUE( ! t["at"][1].is_val_quoted());
        EXPECT_TRUE( ! t["at"][2].is_val_quoted());
        EXPECT_TRUE(   t["at"][3].is_val_quoted());
        EXPECT_TRUE( ! t["at"][4].is_val_quoted());
        EXPECT_TRUE( ! t["at"][5].is_val_quoted());
        EXPECT_EQ(t["backtick"][0].val(), "`test");
        EXPECT_EQ(t["backtick"][1].val(), "t`est");
        EXPECT_EQ(t["backtick"][2].val(), "test`");
        EXPECT_EQ(t["backtick"][3].val(), "`test2");
        EXPECT_EQ(t["backtick"][4].val(), "t`est2");
        EXPECT_EQ(t["backtick"][5].val(), "test2`");
        EXPECT_TRUE(   t["backtick"][0].is_val_quoted());
        EXPECT_TRUE( ! t["backtick"][1].is_val_quoted());
        EXPECT_TRUE( ! t["backtick"][2].is_val_quoted());
        EXPECT_TRUE(   t["backtick"][3].is_val_quoted());
        EXPECT_TRUE( ! t["backtick"][4].is_val_quoted());
        EXPECT_TRUE( ! t["backtick"][5].is_val_quoted());
    });
}


TEST(emit, error_on_emit_yaml_to_short_buffer)
{
    csubstr yaml = "this is: not empty\n";
    const Tree tree = parse_in_arena(yaml);
    {
        char too_small[2];
        ExpectError::check_error([&]{
            emit_yaml(tree, too_small);
        });
        ExpectError::check_error([&]{
            emit_yaml(tree, too_small, /*error_on_excess*/true);
        });
        substr required = emit_yaml(tree, too_small, /*error_on_excess*/false);
        EXPECT_EQ(required.str, nullptr);
        EXPECT_EQ(required.len, yaml.len);
    }
    {
        substr nothing;
        EXPECT_EQ(nothing.str, nullptr);
        EXPECT_EQ(nothing.len, 0u);
        ExpectError::check_error([&]{
            emit_yaml(tree, nothing);
        });
        ExpectError::check_error([&]{
            emit_yaml(tree, nothing, /*error_on_excess*/true);
        });
        substr required = emit_yaml(tree, nothing, /*error_on_excess*/false);
        EXPECT_EQ(required.str, nullptr);
        EXPECT_EQ(required.len, yaml.len);
    }
}

TEST(emit, error_on_emit_json_to_short_buffer)
{
    csubstr json = "{\"this is\": \"not empty\"}";
    const Tree tree = parse_in_arena(json);
    {
        char too_small[2];
        ExpectError::check_error([&]{
            emit_json(tree, too_small);
        });
        ExpectError::check_error([&]{
            emit_json(tree, too_small, /*error_on_excess*/true);
        });
        substr required = emit_json(tree, too_small, /*error_on_excess*/false);
        EXPECT_EQ(required.str, nullptr);
        EXPECT_EQ(required.len, json.len);
    }
    {
        substr nothing;
        EXPECT_EQ(nothing.str, nullptr);
        EXPECT_EQ(nothing.len, 0u);
        ExpectError::check_error([&]{
            emit_json(tree, nothing);
        });
        ExpectError::check_error([&]{
            emit_json(tree, nothing, /*error_on_excess*/true);
        });
        substr required = emit_json(tree, nothing, /*error_on_excess*/false);
        EXPECT_EQ(required.str, nullptr);
        EXPECT_EQ(required.len, json.len);
    }
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
