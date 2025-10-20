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
    SCOPED_TRACE("emit2file");
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
    SCOPED_TRACE("emit2stream");
    std::ostringstream ss;
    fn(ss);
    std::string result = ss.str();
    _c4dbgpf("emit result: [{}]~~~{}~~~", result.size(), to_csubstr(result));
    return result;
}

template<class Emit>
std::string emit2buf(Emit &&fn)
{
    SCOPED_TRACE("emit2buf");
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
    SCOPED_TRACE("emitrs_append");
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

TEST(emit_nested, basic)
{
    const Tree tree = parse_in_arena(R"(- a
- b
- x0: 1
  x1: 2
- champagne: Dom Perignon
  coffee: Arabica
  more:
    vinho verde: Soalheiro
    vinho tinto: Redoma 2017
  beer:
    - Rochefort 10
    - Busch
    - Leffe Rituel
    - - and so
      - many other
      - wonderful beers
- more
- seq
- members
- here
)");
    EmitOptions without_dash = {};
    EmitOptions with_dash = EmitOptions{}.emit_nonroot_dash(true);
    EmitOptions with_key = {};
    EmitOptions without_key = EmitOptions{}.emit_nonroot_key(false);
    ASSERT_FALSE(without_dash.emit_nonroot_dash());
    ASSERT_TRUE(with_dash.emit_nonroot_dash());
    ASSERT_TRUE(with_key.emit_nonroot_key());
    ASSERT_FALSE(without_key.emit_nonroot_key());
    EXPECT_EQ(emitrs_yaml<std::string>(tree[3]["beer"][0], without_dash), "Rochefort 10");
    EXPECT_EQ(emitrs_yaml<std::string>(tree[3]["beer"][0], with_dash), "- Rochefort 10\n");
    EXPECT_EQ(emitrs_yaml<std::string>(tree[3]["beer"][3], without_dash), R"(- and so
- many other
- wonderful beers
)");
    EXPECT_EQ(emitrs_yaml<std::string>(tree[3]["beer"][3], with_dash), R"(- - and so
  - many other
  - wonderful beers
)");
    EXPECT_EQ(emitrs_yaml<std::string>(tree[3]["beer"], with_key), R"(beer:
  - Rochefort 10
  - Busch
  - Leffe Rituel
  - - and so
    - many other
    - wonderful beers
)");
    EXPECT_EQ(emitrs_yaml<std::string>(tree[3]["beer"], without_key), R"(- Rochefort 10
- Busch
- Leffe Rituel
- - and so
  - many other
  - wonderful beers
)");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct TmpContainerStyle
{
    Tree &t;
    id_type id;
    NodeType prev;
    TmpContainerStyle(NodeRef n, NodeType tmp) : TmpContainerStyle(*n.tree(), n.id(), tmp) {}
    TmpContainerStyle(Tree &t_, id_type id_, NodeType tmp) : t(t_), id(id_)
    {
        prev = t.type(id);
        t._p(id)->m_type &= ~CONTAINER_STYLE;
        t._p(id)->m_type |= tmp;
    }
    ~TmpContainerStyle()
    {
        t._p(id)->m_type = prev;
    }
};

void test_emits(Tree const& t, id_type id, std::string const& expected, std::string const& expected_json, EmitOptions const& opts={})
{
    RYML_TRACE_FMT("id={}", id);
    std::string append_prefix = "#before\n";
    if(opts == EmitOptions{})
    {
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, id, buf); }), expected);
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t, id, buf); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t, id, f); }), expected);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t, id, f); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(t, id); }), expected);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(t, id); }), expected_json);
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_YAML, t, id, /*error_on_excess*/true); }), expected);
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_JSON, t, id, /*error_on_excess*/true); }), expected_json);
        EXPECT_EQ(emitrs_yaml<std::string>(t, id), expected);
        EXPECT_EQ(emitrs_json<std::string>(t, id), expected_json);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(t, id, s, /*append*/true); } ), append_prefix + expected);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(t, id, s, /*append*/true); } ), append_prefix + expected_json);
    }
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, id, opts, buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t, id, opts, buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); return em.emit_as(EMIT_YAML, t, id, /*error_on_excess*/true); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); return em.emit_as(EMIT_JSON, t, id, /*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t, id, opts, f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t, id, opts, f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); return em.emit_as(EMIT_YAML, t, id, /*error_on_excess*/true).len; }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); return em.emit_as(EMIT_JSON, t, id, /*error_on_excess*/true).len; }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(t, id, opts); }), expected);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(t, id, opts); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, oss); em.emit_as(EMIT_YAML, t, id, /*error_on_excess*/true); }), expected);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, oss); em.emit_as(EMIT_JSON, t, id, /*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(t, id, opts), expected);
    EXPECT_EQ(emitrs_json<std::string>(t, id, opts), expected_json);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(t, id, opts, s, /*append*/true); } ), append_prefix + expected);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(t, id, opts, s, /*append*/true); } ), append_prefix + expected_json);
    // error on max depth
    if(id == NONE)
        return;
    id_type max_depth = t.depth_desc(id);
    if(max_depth > 1)
    {
        EmitOptions optsd = opts;
        optsd = optsd.max_depth(0);
        ExpectError::check_error_basic(&t, [&]{ return emit2buf([&](substr buf){ return emit_yaml(t, id, optsd, buf); }); });
        ExpectError::check_error_basic(&t, [&]{ return emit2buf([&](substr buf){ return emit_json(t, id, optsd, buf); }); });
        ExpectError::check_error_basic(&t, [&]{ return emit2file([&](FILE *f){ return emit_yaml(t, id, optsd, f); }); });
        ExpectError::check_error_basic(&t, [&]{ return emit2file([&](FILE *f){ return emit_json(t, id, optsd, f); }); });
        ExpectError::check_error_basic(&t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(t, id, optsd); }); });
        ExpectError::check_error_basic(&t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(t, id, optsd); }); });
        ExpectError::check_error_basic(&t, [&]{ return emitrs_yaml<std::string>(t, id, optsd); });
        ExpectError::check_error_basic(&t, [&]{ return emitrs_json<std::string>(t, id, optsd); });
    }
}

void test_emits(Tree const& t, std::string const& expected, std::string const& expected_json, EmitOptions const& opts={})
{
    std::string append_prefix = "#before\n";
    if(opts == EmitOptions{})
    {
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, buf); }), expected);
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t, buf); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t, f); }), expected);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t, f); }), expected_json);
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_YAML, t, /*error_on_excess*/true); }), expected);
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_JSON, t, /*error_on_excess*/true); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(f); return em.emit_as(EMIT_YAML, t, /*error_on_excess*/true).len; }), expected);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(f); return em.emit_as(EMIT_JSON, t, /*error_on_excess*/true).len; }), expected_json);
        if(!t.empty())
        {
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(t); }), expected);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(t); }), expected_json);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(oss); em.emit_as(EMIT_YAML, t, /*error_on_excess*/true); }), expected);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(oss); em.emit_as(EMIT_JSON, t, /*error_on_excess*/true); }), expected_json);
        }
        EXPECT_EQ(emitrs_yaml<std::string>(t), expected);
        EXPECT_EQ(emitrs_json<std::string>(t), expected_json);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(t, s, /*append*/true); } ), append_prefix + expected);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(t, s, /*append*/true); } ), append_prefix + expected_json);
    }
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(t, opts, buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(t, opts, buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); return em.emit_as(EMIT_YAML, t, /*error_on_excess*/true); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); return em.emit_as(EMIT_JSON, t, /*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(t, opts, f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(t, opts, f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); return em.emit_as(EMIT_YAML, t, /*error_on_excess*/true).len; }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); return em.emit_as(EMIT_JSON, t, /*error_on_excess*/true).len; }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(t, opts); }), expected);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(t, opts); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, oss); em.emit_as(EMIT_YAML, t, /*error_on_excess*/true); }), expected);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, oss); em.emit_as(EMIT_JSON, t, /*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(t, opts), expected);
    EXPECT_EQ(emitrs_json<std::string>(t, opts), expected_json);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(t, opts, s, /*append*/true); } ), append_prefix + expected);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(t, opts, s, /*append*/true); } ), append_prefix + expected_json);
    // error on max depth
    id_type max_depth = t.empty() ? 0 : t.depth_desc(t.root_id());
    if(max_depth > 1)
    {
        EmitOptions optsd = opts;
        optsd = optsd.max_depth(0);
        ExpectError::check_error_basic(&t, [&]{ return emit2buf([&](substr buf){ return emit_yaml(t, optsd, buf); }); });
        ExpectError::check_error_basic(&t, [&]{ return emit2buf([&](substr buf){ return emit_json(t, optsd, buf); }); });
        ExpectError::check_error_basic(&t, [&]{ return emit2file([&](FILE *f){ return emit_yaml(t, optsd, f); }); });
        ExpectError::check_error_basic(&t, [&]{ return emit2file([&](FILE *f){ return emit_json(t, optsd, f); }); });
        ExpectError::check_error_basic(&t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(ConstNodeRef(&t), optsd); }); });
        ExpectError::check_error_basic(&t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(ConstNodeRef(&t), optsd); }); });
        ExpectError::check_error_basic(&t, [&]{ return emitrs_yaml<std::string>(t, optsd); });
        ExpectError::check_error_basic(&t, [&]{ return emitrs_json<std::string>(t, optsd); });
    }
}


void test_emits(ConstNodeRef n, std::string const& expected, std::string const& expected_json, EmitOptions const& opts={})
{
    std::string append_prefix = "#before\n";
    if(opts == EmitOptions{})
    {
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(n, buf); }), expected);
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(n, buf); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(n, f); }), expected);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(n, f); }), expected_json);
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_YAML, n, /*error_on_excess*/true); }), expected);
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(buf); return em.emit_as(EMIT_JSON, n, /*error_on_excess*/true); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(f); return em.emit_as(EMIT_YAML, n, /*error_on_excess*/true).len; }), expected);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(f); return em.emit_as(EMIT_JSON, n, /*error_on_excess*/true).len; }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss <<         n; }), expected);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(n); }), expected);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(n); }), expected_json);
        EXPECT_EQ(emitrs_yaml<std::string>(n), expected);
        EXPECT_EQ(emitrs_json<std::string>(n), expected_json);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(n, s, /*append*/true); } ), append_prefix + expected);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(n, s, /*append*/true); } ), append_prefix + expected_json);
    }
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(n, opts, buf); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(n, opts, buf); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(n, opts, f); }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(n, opts, f); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); return em.emit_as(EMIT_YAML, n, /*error_on_excess*/true); }), expected);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); return em.emit_as(EMIT_JSON, n, /*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); return em.emit_as(EMIT_YAML, n, /*error_on_excess*/true).len; }), expected);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); return em.emit_as(EMIT_JSON, n, /*error_on_excess*/true).len; }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(n, opts); }), expected);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(n, opts); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, oss); em.emit_as(EMIT_YAML, n, /*error_on_excess*/true); }), expected);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, oss); em.emit_as(EMIT_JSON, n, /*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(n, opts), expected);
    EXPECT_EQ(emitrs_json<std::string>(n, opts), expected_json);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(n, opts, s, /*append*/true); } ), append_prefix + expected);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(n, opts, s, /*append*/true); } ), append_prefix + expected_json);
    // error on max depth
    if(n.tree() && n.id() != NONE)
    {
        id_type max_depth = n.depth_desc();
        if(max_depth > 1)
        {
            EmitOptions optsd = opts;
            optsd = optsd.max_depth(0);
            ExpectError::check_error_basic(n.tree(), [&]{ return emit2buf([&](substr buf){ return emit_yaml(n, optsd, buf); }); });
            ExpectError::check_error_basic(n.tree(), [&]{ return emit2buf([&](substr buf){ return emit_json(n, optsd, buf); }); });
            ExpectError::check_error_basic(n.tree(), [&]{ return emit2file([&](FILE *f){ return emit_yaml(n, optsd, f); }); });
            ExpectError::check_error_basic(n.tree(), [&]{ return emit2file([&](FILE *f){ return emit_json(n, optsd, f); }); });
            ExpectError::check_error_basic(n.tree(), [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(n, optsd); }); });
            ExpectError::check_error_basic(n.tree(), [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(n, optsd); }); });
            ExpectError::check_error_basic(n.tree(), [&]{ return emitrs_yaml<std::string>(n, optsd); });
            ExpectError::check_error_basic(n.tree(), [&]{ return emitrs_json<std::string>(n, optsd); });
        }
    }
}


void test_all_emits(ConstNodeRef n, std::string const &expected, std::string const& expected_json, EmitOptions const& opts={})
{
    SCOPED_TRACE("test_all_emits");
    {
        SCOPED_TRACE("rootref");
        test_emits(n, expected, expected_json, opts);
    }
    if(n.is_root())
    {
        SCOPED_TRACE("t");
        test_emits(*n.tree(), expected, expected_json, opts);
    }
    {
        SCOPED_TRACE("t, id");
        test_emits(*n.tree(), n.id(), expected, expected_json, opts);
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

TEST(emit, empty_key_squo)
{
    {
        SCOPED_TRACE("one only");
        const Tree t = parse_in_arena(R"(
? ''
: literal
)");
        {
            SCOPED_TRACE("indent");
            std::string expected = "'': literal\n";
            std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
            test_emits(t, t.root_id(), expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "'': literal\n";
            std::string expected_json = "{\n\"\": \"literal\"\n}\n";
            test_emits(t, t.root_id(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
        }
    }
    {
        SCOPED_TRACE("nested");
        const Tree t = parse_in_arena(R"(
level1:
  ? ''
  : literal
)");
        {
            SCOPED_TRACE("indent");
            std::string expected = "level1:\n  '': literal\n";
            std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
            test_emits(t, t.root_id(), expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "level1:\n  '': literal\n";
            std::string expected_json = "{\n\"level1\": {\n\"\": \"literal\"\n}\n}\n";
            test_emits(t, t.root_id(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
        }
    }
    const Tree t = parse_in_arena(R"(
? ''
: literal
level1:
  ? ''
  : literal
  level2:
    ? ''
    : literal
    level3:
      ? ''
      : literal
)");
    {
        SCOPED_TRACE("level3");
        {
            SCOPED_TRACE("indent");
            std::string expected = "level3:\n  '': literal\n";
            std::string expected_json = "\"level3\": {\n  \"\": \"literal\"\n}\n";
            test_emits(t, t["level1"]["level2"]["level3"].id(), expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "level3:\n  '': literal\n";
            std::string expected_json = "\"level3\": {\n\"\": \"literal\"\n}\n";
            test_emits(t, t["level1"]["level2"]["level3"].id(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
        }
    }
    {
        SCOPED_TRACE("level2");
        {
            SCOPED_TRACE("indent");
            std::string expected = "level2:\n  '': literal\n  level3:\n    '': literal\n";
            std::string expected_json = "\"level2\": {\n  \"\": \"literal\",\n  \"level3\": {\n    \"\": \"literal\"\n  }\n}\n";
            test_emits(t, t["level1"]["level2"].id(), expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "level2:\n  '': literal\n  level3:\n    '': literal\n";
            std::string expected_json = "\"level2\": {\n\"\": \"literal\",\n\"level3\": {\n\"\": \"literal\"\n}\n}\n";
            test_emits(t, t["level1"]["level2"].id(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
        }
    }
    {
        SCOPED_TRACE("level1");
        {
            SCOPED_TRACE("indent");
            std::string expected = "level1:\n  '': literal\n  level2:\n    '': literal\n    level3:\n      '': literal\n";
            std::string expected_json = "\"level1\": {\n  \"\": \"literal\",\n  \"level2\": {\n    \"\": \"literal\",\n    \"level3\": {\n      \"\": \"literal\"\n    }\n  }\n}\n";
            test_emits(t, t["level1"].id(), expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "level1:\n  '': literal\n  level2:\n    '': literal\n    level3:\n      '': literal\n";
            std::string expected_json = "\"level1\": {\n\"\": \"literal\",\n\"level2\": {\n\"\": \"literal\",\n\"level3\": {\n\"\": \"literal\"\n}\n}\n}\n";
            test_emits(t, t["level1"].id(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
        }
    }
    {
        SCOPED_TRACE("level0");
        {
            SCOPED_TRACE("indent");
            std::string expected = "'': literal\nlevel1:\n  '': literal\n  level2:\n    '': literal\n    level3:\n      '': literal\n";
            std::string expected_json = "{\n  \"\": \"literal\",\n  \"level1\": {\n    \"\": \"literal\",\n    \"level2\": {\n      \"\": \"literal\",\n      \"level3\": {\n        \"\": \"literal\"\n      }\n    }\n  }\n}\n";
            test_emits(t, NONE, expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "'': literal\nlevel1:\n  '': literal\n  level2:\n    '': literal\n    level3:\n      '': literal\n";
            std::string expected_json = "{\n\"\": \"literal\",\n\"level1\": {\n\"\": \"literal\",\n\"level2\": {\n\"\": \"literal\",\n\"level3\": {\n\"\": \"literal\"\n}\n}\n}\n}\n";
            test_emits(t, NONE, expected, expected_json, EmitOptions{}.indent_flow_ml(false));
        }
    }
}

TEST(emit, empty_key_dquo)
{
    {
        SCOPED_TRACE("one only");
        const Tree t = parse_in_arena(R"(
? ""
: literal
)");
        std::string expected = "\"\": literal\n";
        std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
        test_emits(t, t.root_id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("nested");
        const Tree t = parse_in_arena(R"(
level1:
  ? ""
  : literal
)");
        std::string expected = "level1:\n  \"\": literal\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits(t, t.root_id(), expected, expected_json);
    }
    const Tree t = parse_in_arena(R"(
? ""
: literal
level1:
  ? ""
  : literal
  level2:
    ? ""
    : literal
    level3:
      ? ""
      : literal
)");
    {
        SCOPED_TRACE("level3");
        std::string expected = "level3:\n  \"\": literal\n";
        std::string expected_json = "\"level3\": {\n  \"\": \"literal\"\n}\n";
        test_emits(t, t["level1"]["level2"]["level3"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level2");
        std::string expected = "level2:\n  \"\": literal\n  level3:\n    \"\": literal\n";
        std::string expected_json = "\"level2\": {\n  \"\": \"literal\",\n  \"level3\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits(t, t["level1"]["level2"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level1");
        std::string expected = "level1:\n  \"\": literal\n  level2:\n    \"\": literal\n    level3:\n      \"\": literal\n";
        std::string expected_json = "\"level1\": {\n  \"\": \"literal\",\n  \"level2\": {\n    \"\": \"literal\",\n    \"level3\": {\n      \"\": \"literal\"\n    }\n  }\n}\n";
        test_emits(t, t["level1"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level0");
        std::string expected = "\"\": literal\nlevel1:\n  \"\": literal\n  level2:\n    \"\": literal\n    level3:\n      \"\": literal\n";
        std::string expected_json = "{\n  \"\": \"literal\",\n  \"level1\": {\n    \"\": \"literal\",\n    \"level2\": {\n      \"\": \"literal\",\n      \"level3\": {\n        \"\": \"literal\"\n      }\n    }\n  }\n}\n";
        test_emits(t, NONE, expected, expected_json);
    }
}

TEST(emit, empty_key_plain)
{
    {
        SCOPED_TRACE("one only");
        const Tree t = parse_in_arena(R"(
?
: literal
)");
        std::string expected = ": literal\n";
        std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
        test_emits(t, t.root_id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("nested");
        const Tree t = parse_in_arena(R"(
level1:
  ?
  : literal
)");
        std::string expected = "level1:\n  : literal\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits(t, t.root_id(), expected, expected_json);
    }
    const Tree t = parse_in_arena(R"(
?
: literal
level1:
  ?
  : literal
  level2:
    ?
    : literal
    level3:
      ?
      : literal
)");
    {
        SCOPED_TRACE("level3");
        std::string expected = "level3:\n  : literal\n";
        std::string expected_json = "\"level3\": {\n  \"\": \"literal\"\n}\n";
        test_emits(t, t["level1"]["level2"]["level3"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level2");
        std::string expected = "level2:\n  : literal\n  level3:\n    : literal\n";
        std::string expected_json = "\"level2\": {\n  \"\": \"literal\",\n  \"level3\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits(t, t["level1"]["level2"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level1");
        std::string expected = "level1:\n  : literal\n  level2:\n    : literal\n    level3:\n      : literal\n";
        std::string expected_json = "\"level1\": {\n  \"\": \"literal\",\n  \"level2\": {\n    \"\": \"literal\",\n    \"level3\": {\n      \"\": \"literal\"\n    }\n  }\n}\n";
        test_emits(t, t["level1"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level0");
        std::string expected = ": literal\nlevel1:\n  : literal\n  level2:\n    : literal\n    level3:\n      : literal\n";
        std::string expected_json = "{\n  \"\": \"literal\",\n  \"level1\": {\n    \"\": \"literal\",\n    \"level2\": {\n      \"\": \"literal\",\n      \"level3\": {\n        \"\": \"literal\"\n      }\n    }\n  }\n}\n";
        test_emits(t, NONE, expected, expected_json);
    }
}

TEST(emit, empty_key_literal)
{
    {
        SCOPED_TRACE("one only");
        const Tree t = parse_in_arena(R"(
? |-
: literal
)");
        std::string expected = "? |-\n: literal\n";
        std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
        test_emits(t, t.root_id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("nested");
        const Tree t = parse_in_arena(R"(
level1:
  ? |-
  : literal
)");
        std::string expected = "level1:\n  ? |-\n  : literal\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits(t, t.root_id(), expected, expected_json);
    }
    const Tree t = parse_in_arena(R"(
? |-
: literal
level1:
  ? |-
  : literal
  level2:
    ? |-
    : literal
    level3:
      ? |-
      : literal
)");
    {
        SCOPED_TRACE("level3");
        std::string expected = "level3:\n  ? |-\n  : literal\n";
        std::string expected_json = "\"level3\": {\n  \"\": \"literal\"\n}\n";
        test_emits(t, t["level1"]["level2"]["level3"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level2");
        std::string expected = "level2:\n  ? |-\n  : literal\n  level3:\n    ? |-\n    : literal\n";
        std::string expected_json = "\"level2\": {\n  \"\": \"literal\",\n  \"level3\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits(t, t["level1"]["level2"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level1");
        std::string expected = "level1:\n  ? |-\n  : literal\n  level2:\n    ? |-\n    : literal\n    level3:\n      ? |-\n      : literal\n";
        std::string expected_json = "\"level1\": {\n  \"\": \"literal\",\n  \"level2\": {\n    \"\": \"literal\",\n    \"level3\": {\n      \"\": \"literal\"\n    }\n  }\n}\n";
        test_emits(t, t["level1"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level0");
        std::string expected = "? |-\n: literal\nlevel1:\n  ? |-\n  : literal\n  level2:\n    ? |-\n    : literal\n    level3:\n      ? |-\n      : literal\n";
        std::string expected_json = "{\n  \"\": \"literal\",\n  \"level1\": {\n    \"\": \"literal\",\n    \"level2\": {\n      \"\": \"literal\",\n      \"level3\": {\n        \"\": \"literal\"\n      }\n    }\n  }\n}\n";
        test_emits(t, NONE, expected, expected_json);
    }
}

TEST(emit, empty_key_folded)
{
    {
        SCOPED_TRACE("one only");
        const Tree t = parse_in_arena(R"(
? >-
: folded
)");
        std::string expected = "? >-\n: folded\n";
        std::string expected_json = "{\n  \"\": \"folded\"\n}\n";
        test_emits(t, t.root_id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("nested");
        const Tree t = parse_in_arena(R"(
level1:
  ? >-
  : folded
)");
        std::string expected = "level1:\n  ? >-\n  : folded\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"folded\"\n  }\n}\n";
        test_emits(t, t.root_id(), expected, expected_json);
    }
    const Tree t = parse_in_arena(R"(
? >-
: folded
level1:
  ? >-
  : folded
  level2:
    ? >-
    : folded
    level3:
      ? >-
      : folded
)");
    {
        SCOPED_TRACE("level3");
        std::string expected = "level3:\n  ? >-\n  : folded\n";
        std::string expected_json = "\"level3\": {\n  \"\": \"folded\"\n}\n";
        test_emits(t, t["level1"]["level2"]["level3"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level2");
        std::string expected = "level2:\n  ? >-\n  : folded\n  level3:\n    ? >-\n    : folded\n";
        std::string expected_json = "\"level2\": {\n  \"\": \"folded\",\n  \"level3\": {\n    \"\": \"folded\"\n  }\n}\n";
        test_emits(t, t["level1"]["level2"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level1");
        std::string expected = "level1:\n  ? >-\n  : folded\n  level2:\n    ? >-\n    : folded\n    level3:\n      ? >-\n      : folded\n";
        std::string expected_json = "\"level1\": {\n  \"\": \"folded\",\n  \"level2\": {\n    \"\": \"folded\",\n    \"level3\": {\n      \"\": \"folded\"\n    }\n  }\n}\n";
        test_emits(t, t["level1"].id(), expected, expected_json);
    }
    {
        SCOPED_TRACE("level0");
        std::string expected = "? >-\n: folded\nlevel1:\n  ? >-\n  : folded\n  level2:\n    ? >-\n    : folded\n    level3:\n      ? >-\n      : folded\n";
        std::string expected_json = "{\n  \"\": \"folded\",\n  \"level1\": {\n    \"\": \"folded\",\n    \"level2\": {\n      \"\": \"folded\",\n      \"level3\": {\n        \"\": \"folded\"\n      }\n    }\n  }\n}\n";
        test_emits(t, NONE, expected, expected_json);
    }
}

TEST(emit, existing_seq_node_flow)
{
    Tree nct = parse_in_arena("[foo, bar, [nested, seq], {nested: map}]");
    Tree const& t = nct;
    {
        SCOPED_TRACE("full");
        std::string expected = "[foo,bar,[nested,seq],{nested: map}]";
        std::string expected_ml = "[\n  foo,\n  bar,\n  [nested,seq],\n  {nested: map}\n]\n";
        std::string expected_ml_all = "[\n  foo,\n  bar,\n  [\n    nested,\n    seq\n  ],\n  {\n    nested: map\n  }\n]\n";
        std::string expected_json = "[\"foo\",\"bar\",[\"nested\",\"seq\"],{\"nested\": \"map\"}]";
        std::string expected_json_ml = "[\n  \"foo\",\n  \"bar\",\n  [\"nested\",\"seq\"],\n  {\"nested\": \"map\"}\n]\n";
        std::string expected_json_ml_all = "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n";
        {
            SCOPED_TRACE("here");
            test_all_emits(t.crootref(), expected, expected_json);
        }
        {
            SCOPED_TRACE("ml");
            TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
            test_all_emits(t.crootref(), expected_ml, expected_json_ml);
        }
        {
            SCOPED_TRACE("rootref-ml-all");
            TmpContainerStyle tmp1(nct.rootref(), FLOW_ML);
            TmpContainerStyle tmp2(nct[2], FLOW_ML);
            TmpContainerStyle tmp3(nct[3], FLOW_ML);
            test_all_emits(t.crootref(), expected_ml_all, expected_json_ml_all);
        }
    }
    {
        SCOPED_TRACE("t[0]");
        ConstNodeRef n = t[0];
        std::string expected = "foo";
        std::string expected_json = "\"foo\"";
        test_all_emits(n, expected, expected_json);
        nct._add_flags(n.id(), FLOW_SL);
        expected = "foo";
        expected_json = "\"foo\"";
        test_all_emits(n, expected, expected_json);
    }
    {
        SCOPED_TRACE("t[1]");
        ConstNodeRef n = t[1];
        std::string expected = "bar";
        std::string expected_json = "\"bar\"";
        test_all_emits(n, expected, expected_json);
        nct._add_flags(n.id(), FLOW_SL);
        expected = "bar";
        expected_json = "\"bar\"";
        test_all_emits(n, expected, expected_json);
    }
    {
        SCOPED_TRACE("t[2]");
        NodeRef n = nct[2];
        {
            SCOPED_TRACE("FLOW_SL");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "[nested,seq]";
            std::string expected_json = "[\"nested\",\"seq\"]";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("FLOW_ML");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "[\n  nested,\n  seq\n]\n";
            std::string expected_json = "[\n  \"nested\",\n  \"seq\"\n]\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("BLOCK");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "- nested\n- seq\n";
            std::string expected_json = "[\n  \"nested\",\n  \"seq\"\n]\n";
            test_all_emits(n, expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[3]");
        NodeRef n = nct[3];
        {
            SCOPED_TRACE("FLOW_SL");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "{nested: map}";
            std::string expected_json = "{\"nested\": \"map\"}";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("FLOW_ML");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "{\n  nested: map\n}\n";
            std::string expected_json = "{\n  \"nested\": \"map\"\n}\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("BLOCK");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "nested: map\n";
            std::string expected_json = "{\n  \"nested\": \"map\"\n}\n";
            test_all_emits(n, expected, expected_json);
        }
    }
}

TEST(emit, existing_seq_node_block)
{
    Tree nct = parse_in_arena("- foo\n- bar\n- - nested\n  - seq\n- nested: map\n");
    Tree const& t = nct;
    {
        SCOPED_TRACE("full-top");
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(nct.rootref(), BLOCK);
            std::string expected = "- foo\n- bar\n- - nested\n  - seq\n- nested: map\n";
            std::string expected_json = "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n";
            test_all_emits(t.crootref(), expected, expected_json);
        }
        {
            SCOPED_TRACE("block-flow-ml");
            {
                SCOPED_TRACE("indent");
                TmpContainerStyle tmp(nct.rootref(), BLOCK);
                TmpContainerStyle tmp2(nct[2], FLOW_ML);
                TmpContainerStyle tmp3(nct[3], FLOW_ML);
                std::string expected = "- foo\n- bar\n- [\n    nested,\n    seq\n  ]\n- {\n    nested: map\n  }\n";
                std::string expected_json = "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n";
                test_all_emits(t.crootref(), expected, expected_json);
            }
            {
                SCOPED_TRACE("no indent");
                TmpContainerStyle tmp(nct.rootref(), BLOCK);
                TmpContainerStyle tmp2(nct[2], FLOW_ML);
                TmpContainerStyle tmp3(nct[3], FLOW_ML);
                std::string expected = "- foo\n- bar\n- [\n  nested,\n  seq\n  ]\n- {\n  nested: map\n  }\n";
                std::string expected_json = "[\n\"foo\",\n\"bar\",\n[\n\"nested\",\n\"seq\"\n],\n{\n\"nested\": \"map\"\n}\n]\n";
                test_all_emits(t.crootref(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
            }
        }
        {
            SCOPED_TRACE("flow_ml");
            {
                SCOPED_TRACE("indent");
                TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
                std::string expected = "[\n  foo,\n  bar,\n  [nested,seq],\n  {nested: map}\n]\n";
                std::string expected_json = "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n";
                test_all_emits(t.crootref(), expected, expected_json);
            }
            {
                SCOPED_TRACE("no indent");
                TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
                std::string expected = "[\nfoo,\nbar,\n[nested,seq],\n{nested: map}\n]\n";
                std::string expected_json = "[\n\"foo\",\n\"bar\",\n[\n\"nested\",\n\"seq\"\n],\n{\n\"nested\": \"map\"\n}\n]\n";
                test_all_emits(t.crootref(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
            }
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(nct.rootref(), FLOW_SL);
            std::string expected = "[foo,bar,[nested,seq],{nested: map}]";
            std::string expected_json = "[\"foo\",\"bar\",[\"nested\",\"seq\"],{\"nested\": \"map\"}]";
            test_all_emits(t.crootref(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("full-all");
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp0(nct.rootref(), BLOCK);
            TmpContainerStyle tmp2(nct[2], BLOCK);
            TmpContainerStyle tmp3(nct[3], BLOCK);
            std::string expected = "- foo\n- bar\n- - nested\n  - seq\n- nested: map\n";
            std::string expected_json = "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n";
            test_all_emits(t.crootref(), expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            {
                SCOPED_TRACE("indent");
                TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
                TmpContainerStyle tmp2(nct[2], FLOW_ML);
                TmpContainerStyle tmp3(nct[3], FLOW_ML);
                std::string expected = "[\n  foo,\n  bar,\n  [\n    nested,\n    seq\n  ],\n  {\n    nested: map\n  }\n]\n";
                std::string expected_json = "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n";
                test_all_emits(t.crootref(), expected, expected_json);
            }
            {
                SCOPED_TRACE("no indent ml");
                TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
                TmpContainerStyle tmp2(nct[2], FLOW_ML);
                TmpContainerStyle tmp3(nct[3], FLOW_ML);
                std::string expected = "[\nfoo,\nbar,\n[\nnested,\nseq\n],\n{\nnested: map\n}\n]\n";
                std::string expected_json = "[\n\"foo\",\n\"bar\",\n[\n\"nested\",\n\"seq\"\n],\n{\n\"nested\": \"map\"\n}\n]\n";
                test_all_emits(t.crootref(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
            }
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(nct.rootref(), FLOW_SL);
            TmpContainerStyle tmp2(nct[2], FLOW_SL);
            TmpContainerStyle tmp3(nct[3], FLOW_SL);
            std::string expected = "[foo,bar,[nested,seq],{nested: map}]";
            std::string expected_json = "[\"foo\",\"bar\",[\"nested\",\"seq\"],{\"nested\": \"map\"}]";
            test_all_emits(t.crootref(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[0]");
        NodeRef n = nct[0];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "foo";
            std::string expected_json = "\"foo\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "foo\n";
            std::string expected_json = "\"foo\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "foo";
            std::string expected_json = "\"foo\"";
            test_all_emits(n, expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[1]");
        NodeRef n = nct[1];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "bar";
            std::string expected_json = "\"bar\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "bar\n";
            std::string expected_json = "\"bar\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "bar";
            std::string expected_json = "\"bar\"";
            test_all_emits(n, expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[2]");
        NodeRef n = nct[2];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "- nested\n- seq\n";
            std::string expected_json = "[\n  \"nested\",\n  \"seq\"\n]\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "[\n  nested,\n  seq\n]\n";
            std::string expected_json = "[\n  \"nested\",\n  \"seq\"\n]\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "[nested,seq]";
            std::string expected_json = "[\"nested\",\"seq\"]";
            test_all_emits(n, expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[3]");
        NodeRef n = nct[3];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "nested: map\n";
            std::string expected_json = "{\n  \"nested\": \"map\"\n}\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "{\n  nested: map\n}\n";
            std::string expected_json = "{\n  \"nested\": \"map\"\n}\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "{nested: map}";
            std::string expected_json = "{\"nested\": \"map\"}";
            test_all_emits(n, expected, expected_json);
        }
    }
}

TEST(emit, existing_map_node_flow)
{
    Tree nct = parse_in_arena("{0: foo, 1: bar, 2: [nested, seq], 3: {nested: map}}");
    Tree const& t = nct;
    {
        SCOPED_TRACE("full-top");
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(nct.rootref(), BLOCK);
            std::string expected = "0: foo\n1: bar\n2: [nested,seq]\n3: {nested: map}\n";
            std::string expected_json = "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\"nested\",\"seq\"],\n  \"3\": {\"nested\": \"map\"}\n}\n";
            test_all_emits(t.crootref(), expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            {
                SCOPED_TRACE("indent");
                TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
                std::string expected = "{\n  0: foo,\n  1: bar,\n  2: [nested,seq],\n  3: {nested: map}\n}\n";
                std::string expected_json = "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\"nested\",\"seq\"],\n  \"3\": {\"nested\": \"map\"}\n}\n";
                test_all_emits(t.crootref(), expected, expected_json);
            }
            {
                SCOPED_TRACE("no indent");
                TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
                std::string expected = "{\n0: foo,\n1: bar,\n2: [nested,seq],\n3: {nested: map}\n}\n";
                std::string expected_json = "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\"nested\",\"seq\"],\n\"3\": {\"nested\": \"map\"}\n}\n";
                test_all_emits(t.crootref(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
            }
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(nct.rootref(), FLOW_SL);
            std::string expected = "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}";
            std::string expected_json = "{\"0\": \"foo\",\"1\": \"bar\",\"2\": [\"nested\",\"seq\"],\"3\": {\"nested\": \"map\"}}";
            test_all_emits(t.crootref(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("full-all");
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp0(nct.rootref(), BLOCK);
            TmpContainerStyle tmp2(nct[2], BLOCK);
            TmpContainerStyle tmp3(nct[3], BLOCK);
            std::string expected = "0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n";
            std::string expected_json = "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n";
            test_all_emits(t.crootref(), expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            {
                SCOPED_TRACE("indent");
                TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
                TmpContainerStyle tmp2(nct[2], FLOW_ML);
                TmpContainerStyle tmp3(nct[3], FLOW_ML);
                std::string expected = "{\n  0: foo,\n  1: bar,\n  2: [\n    nested,\n    seq\n  ],\n  3: {\n    nested: map\n  }\n}\n";
                std::string expected_json = "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n";
                test_all_emits(t.crootref(), expected, expected_json);
            }
            {
                SCOPED_TRACE("no indent");
                TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
                TmpContainerStyle tmp2(nct[2], FLOW_ML);
                TmpContainerStyle tmp3(nct[3], FLOW_ML);
                std::string expected = "{\n0: foo,\n1: bar,\n2: [\nnested,\nseq\n],\n3: {\nnested: map\n}\n}\n";
                std::string expected_json = "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\n\"nested\",\n\"seq\"\n],\n\"3\": {\n\"nested\": \"map\"\n}\n}\n";
                test_all_emits(t.crootref(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
            }
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(nct.rootref(), FLOW_SL);
            TmpContainerStyle tmp2(nct[2], FLOW_SL);
            TmpContainerStyle tmp3(nct[3], FLOW_SL);
            std::string expected = "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}";
            std::string expected_json = "{\"0\": \"foo\",\"1\": \"bar\",\"2\": [\"nested\",\"seq\"],\"3\": {\"nested\": \"map\"}}";
            test_all_emits(t.crootref(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[0]");
        NodeRef n = nct[0];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "0: foo\n";
            std::string expected_json = "\"0\": \"foo\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "0: foo\n";
            std::string expected_json = "\"0\": \"foo\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "0: foo\n";
            std::string expected_json = "\"0\": \"foo\""; // FIXME should have trailing newline
            test_all_emits(n, expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[1]");
        NodeRef n = nct[1];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "1: bar\n";
            std::string expected_json = "\"1\": \"bar\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "1: bar\n";
            std::string expected_json = "\"1\": \"bar\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "1: bar\n";
            std::string expected_json = "\"1\": \"bar\""; // FIXME should have trailing newline
            test_all_emits(n, expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[2]");
        NodeRef n = nct[2];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "2:\n  - nested\n  - seq\n";
            std::string expected_json = "\"2\": [\n  \"nested\",\n  \"seq\"\n]\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "2: [\n    nested,\n    seq\n  ]\n"; // FIXME should be indented one level, not two
            std::string expected_json = "\"2\": [\n  \"nested\",\n  \"seq\"\n]\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "2: [nested,seq]\n";
            std::string expected_json = "\"2\": [\"nested\",\"seq\"]"; // FIXME should have trailing newline
            test_all_emits(n, expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[3]");
        NodeRef n = nct[3];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "3:\n  nested: map\n";
            std::string expected_json = "\"3\": {\n  \"nested\": \"map\"\n}\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "3: {\n    nested: map\n  }\n"; // FIXME should be indented one level, not two
            std::string expected_json = "\"3\": {\n  \"nested\": \"map\"\n}\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "3: {nested: map}\n";
            std::string expected_json = "\"3\": {\"nested\": \"map\"}"; // FIXME should have trailing newline
            test_all_emits(n, expected, expected_json);
        }
    }
}

TEST(emit, existing_map_node_block)
{
    Tree nct = parse_in_arena("0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n");
    Tree const& t = nct;
    {
        SCOPED_TRACE("full-top");
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(nct.rootref(), BLOCK);
            std::string expected = "0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n";
            std::string expected_json = "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n";
            test_all_emits(t.crootref(), expected, expected_json);
        }
        {
            SCOPED_TRACE("block-flow-ml");
            {
                SCOPED_TRACE("indent");
                TmpContainerStyle tmp(nct.rootref(), BLOCK);
                TmpContainerStyle tmp2(nct[2], FLOW_ML);
                TmpContainerStyle tmp3(nct[3], FLOW_ML);
                std::string expected = "0: foo\n1: bar\n2: [\n    nested,\n    seq\n  ]\n3: {\n    nested: map\n  }\n";
                std::string expected_json = "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n";
                test_all_emits(t.crootref(), expected, expected_json);
            }
            {
                SCOPED_TRACE("no indent");
                TmpContainerStyle tmp(nct.rootref(), BLOCK);
                TmpContainerStyle tmp2(nct[2], FLOW_ML);
                TmpContainerStyle tmp3(nct[3], FLOW_ML);
                std::string expected = "0: foo\n1: bar\n2: [\n  nested,\n  seq\n  ]\n3: {\n  nested: map\n  }\n";
                std::string expected_json = "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\n\"nested\",\n\"seq\"\n],\n\"3\": {\n\"nested\": \"map\"\n}\n}\n";
                test_all_emits(t.crootref(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
            }
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
            {
                SCOPED_TRACE("indent");
                std::string expected = "{\n  0: foo,\n  1: bar,\n  2: [nested,seq],\n  3: {nested: map}\n}\n";
                std::string expected_json = "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n";
                test_all_emits(t.crootref(), expected, expected_json);
            }
            {
                SCOPED_TRACE("no indent");
                std::string expected = "{\n0: foo,\n1: bar,\n2: [nested,seq],\n3: {nested: map}\n}\n";
                std::string expected_json = "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\n\"nested\",\n\"seq\"\n],\n\"3\": {\n\"nested\": \"map\"\n}\n}\n";
                test_all_emits(t.crootref(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
            }
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(nct.rootref(), FLOW_SL);
            std::string expected = "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}";
            std::string expected_json = R"({"0": "foo","1": "bar","2": ["nested","seq"],"3": {"nested": "map"}})";
            test_all_emits(t.crootref(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("full-all");
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp0(nct.rootref(), BLOCK);
            TmpContainerStyle tmp2(nct[2], BLOCK);
            TmpContainerStyle tmp3(nct[3], BLOCK);
            std::string expected = "0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n";
            std::string expected_json = "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n";
            test_all_emits(t.crootref(), expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(nct.rootref(), FLOW_ML);
            TmpContainerStyle tmp2(nct[2], FLOW_ML);
            TmpContainerStyle tmp3(nct[3], FLOW_ML);
            {
                SCOPED_TRACE("indent");
                std::string expected = "{\n  0: foo,\n  1: bar,\n  2: [\n    nested,\n    seq\n  ],\n  3: {\n    nested: map\n  }\n}\n";
                std::string expected_json = "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n";
                test_all_emits(t.crootref(), expected, expected_json);
            }
            {
                SCOPED_TRACE("no indent");
                std::string expected = "{\n0: foo,\n1: bar,\n2: [\nnested,\nseq\n],\n3: {\nnested: map\n}\n}\n";
                std::string expected_json = "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\n\"nested\",\n\"seq\"\n],\n\"3\": {\n\"nested\": \"map\"\n}\n}\n";
                test_all_emits(t.crootref(), expected, expected_json, EmitOptions{}.indent_flow_ml(false));
            }
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(nct.rootref(), FLOW_SL);
            TmpContainerStyle tmp2(nct[2], FLOW_SL);
            TmpContainerStyle tmp3(nct[3], FLOW_SL);
            std::string expected = "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}";
            std::string expected_json = R"({"0": "foo","1": "bar","2": ["nested","seq"],"3": {"nested": "map"}})";
            test_all_emits(t.crootref(), expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[0]");
        NodeRef n = nct[0];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "0: foo\n";
            std::string expected_json = "\"0\": \"foo\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "0: foo\n";
            std::string expected_json = "\"0\": \"foo\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "0: foo\n";
            std::string expected_json = "\"0\": \"foo\""; // FIXME should have trailing newline
            test_all_emits(n, expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[1]");
        NodeRef n = nct[1];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "1: bar\n";
            std::string expected_json = "\"1\": \"bar\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "1: bar\n";
            std::string expected_json = "\"1\": \"bar\"\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "1: bar\n";
            std::string expected_json = "\"1\": \"bar\""; // FIXME should have trailing newline
            test_all_emits(n, expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[2]");
        NodeRef n = nct[2];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "2:\n  - nested\n  - seq\n";
            std::string expected_json = "\"2\": [\n  \"nested\",\n  \"seq\"\n]\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "2: [\n    nested,\n    seq\n  ]\n"; // FIXME should be indented one level, not two
            std::string expected_json = "\"2\": [\n  \"nested\",\n  \"seq\"\n]\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "2: [nested,seq]\n";
            std::string expected_json = "\"2\": [\"nested\",\"seq\"]"; // FIXME should have trailing newline
            test_all_emits(n, expected, expected_json);
        }
    }
    {
        SCOPED_TRACE("t[3]");
        NodeRef n = nct[3];
        {
            SCOPED_TRACE("block");
            TmpContainerStyle tmp(n, BLOCK);
            std::string expected = "3:\n  nested: map\n";
            std::string expected_json = "\"3\": {\n  \"nested\": \"map\"\n}\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_ml");
            TmpContainerStyle tmp(n, FLOW_ML);
            std::string expected = "3: {\n    nested: map\n  }\n"; // FIXME should be indented one level, not two
            std::string expected_json = "\"3\": {\n  \"nested\": \"map\"\n}\n";
            test_all_emits(n, expected, expected_json);
        }
        {
            SCOPED_TRACE("flow_sl");
            TmpContainerStyle tmp(n, FLOW_SL);
            std::string expected = "3: {nested: map}\n";
            std::string expected_json = "\"3\": {\"nested\": \"map\"}"; // FIXME should have trailing newline
            test_all_emits(n, expected, expected_json);
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
        ExpectError::check_error_basic([&]{
            emit_yaml(tree, too_small);
        });
        ExpectError::check_error_basic([&]{
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
        ExpectError::check_error_basic([&]{
            emit_yaml(tree, nothing);
        });
        ExpectError::check_error_basic([&]{
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
        ExpectError::check_error_basic([&]{
            emit_json(tree, too_small);
        });
        ExpectError::check_error_basic([&]{
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
        ExpectError::check_error_basic([&]{
            emit_json(tree, nothing);
        });
        ExpectError::check_error_basic([&]{
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
