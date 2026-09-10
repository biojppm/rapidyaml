#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/writer.hpp>
#endif
#include <c4/fs/fs.hpp>

#include "./test_lib/test_case.hpp"
#include "./test_lib/tree_and_ints.hpp"

#include <gtest/gtest.h>

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

// NOLINTBEGIN(*-signed-bitwise)

template<class Emit>
std::string emit2file(Emit &&fn)
{
    SCOPED_TRACE("emit2file");
    C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4996) // fopen unsafe
    C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wdeprecated-declarations") // fopen is deprecated
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
    C4_SUPPRESS_WARNING_CLANG_POP
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
    buf.resize(out.len);
    if(out.len > buf.size())
    {
        out = fn(to_substr(buf));
    }
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

TEST(emitter, options)
{
    const EmitOptions defaults{};
    const EmitOptions maxcols = EmitOptions{}.max_cols(10);
    const EmitOptions maxdepth = EmitOptions{}.max_depth(2);
    const EmitOptions erronstream = EmitOptions{}.json_err_on_stream(true);
    EXPECT_FALSE(maxcols == defaults);
    EXPECT_FALSE(maxdepth == defaults);
    EXPECT_FALSE(erronstream == defaults);
    {
        EmitterFile em(defaults);
        EXPECT_EQ(em.options(), defaults);
    }
    {
        EmitterFile em(maxcols);
        EXPECT_EQ(em.options(), maxcols);
    }
    {
        EmitterFile em(maxdepth);
        EXPECT_EQ(em.options(), maxdepth);
    }
    {
        EmitterFile em(erronstream);
        EXPECT_EQ(em.options(), erronstream);
    }
}


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

#define test_emit_yaml_(...) { SCOPED_TRACE("here"); test_emit_yaml(__VA_ARGS__); }
#define test_emit_json_(...) { SCOPED_TRACE("here"); test_emit_json(__VA_ARGS__); }

static const EmitOptions without_dash = {};
static const EmitOptions with_dash = EmitOptions{}.emit_nonroot_dash(true);
static const EmitOptions with_key = {};
static const EmitOptions without_key = EmitOptions{}.emit_nonroot_key(false);
static const EmitOptions noindent = EmitOptions{}.indent_flow_ml(false);

TEST(emit_nested, preconditions)
{
    ASSERT_FALSE(without_dash.emit_nonroot_dash());
    ASSERT_TRUE(with_dash.emit_nonroot_dash());
    ASSERT_TRUE(with_key.emit_nonroot_key());
    ASSERT_FALSE(without_key.emit_nonroot_key());
    ASSERT_FALSE(noindent.indent_flow_ml());
    ASSERT_TRUE(noindent.emit_nonroot_key());
}

TEST(emit_nested, basic)
{
    const TreeAndInts ti = parse_tree_and_ints(R"(- a
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
    test_emit_yaml_(ti.tree[3]["beer"][0], ti.ints, 57, without_dash, "Rochefort 10");
    test_emit_yaml_(ti.tree[3]["beer"][0], ti.ints, 57, with_dash, "- Rochefort 10\n");
    test_emit_yaml_(ti.tree[3]["beer"][3], ti.ints, 66, without_dash, R"(- and so
- many other
- wonderful beers
)");
    test_emit_yaml_(ti.tree[3]["beer"][3], ti.ints, 66, with_dash, R"(- - and so
  - many other
  - wonderful beers
)");
    {
        std::string key = "beer";
        std::string val = R"(- Rochefort 10
- Busch
- Leffe Rituel
- - and so
  - many other
  - wonderful beers
)";
        std::string keyval = R"(beer:
  - Rochefort 10
  - Busch
  - Leffe Rituel
  - - and so
    - many other
    - wonderful beers
)";
        test_emit_yaml_(ti.tree[3]["beer"], with_key, keyval);
        test_emit_yaml_(ti.ints, 53,        with_key, keyval);
        test_emit_yaml_(ti.ints, 56,        with_key, val);
        test_emit_yaml_(ti.tree[3]["beer"], without_key, val);
        test_emit_yaml_(ti.ints, 53,        without_key, key);
        test_emit_yaml_(ti.ints, 56,        without_key, val);
    }
}

TEST(emit_nested, scalar_key)
{
    std::string yaml = ""
        "key: {c: d}\n"
        "e: [f]\n"
        "g: h\n"
        "bmap:\n"
        "  a: b\n"
        "bseq:\n"
        "  - a\n"
        "  - b\n"
        "";
    const TreeAndInts ti = parse_tree_and_ints(to_csubstr(yaml));
    test_emit_yaml_(ti.tree, ti.ints, 0, with_key,    yaml);//BSTR
    test_emit_yaml_(ti.tree, ti.ints, 0, without_key, yaml);//BSTR
    test_emit_yaml_(ti.tree, ti.ints, 1, with_key,    yaml);//BDOC
    test_emit_yaml_(ti.tree, ti.ints, 1, without_key, yaml);//BDOC
    test_emit_yaml_(ti.tree, ti.ints, 2, with_key,    yaml);//BMAP
    test_emit_yaml_(ti.tree, ti.ints, 2, without_key, yaml);//BMAP
    //
    test_emit_yaml_(ti.tree["key"], with_key,    "key: {c: d}\n");
    test_emit_yaml_(ti.ints, 3,     with_key,    "key: {c: d}\n");
    test_emit_yaml_(ti.ints, 6,     with_key,    "{c: d}");
    test_emit_yaml_(ti.tree["key"], without_key, "{c: d}");
    test_emit_yaml_(ti.ints, 3,     without_key, "key");
    test_emit_yaml_(ti.ints, 6,     without_key, "{c: d}");
    //
    test_emit_yaml_(ti.tree["key"]["c"], with_key   , "c: d\n");
    test_emit_yaml_(ti.ints, 7,          with_key   , "c: d\n");
    test_emit_yaml_(ti.ints, 10,         with_key   , "d");
    test_emit_yaml_(ti.tree["key"]["c"], without_key, "d");
    test_emit_yaml_(ti.ints, 7,          without_key, "c");
    test_emit_yaml_(ti.ints, 10,         without_key, "d");
    //
    test_emit_yaml_(ti.tree["e"], with_key   , "e: [f]\n");
    test_emit_yaml_(ti.ints, 14,  with_key   , "e: [f]\n");
    test_emit_yaml_(ti.ints, 17,  with_key   , "[f]");
    test_emit_yaml_(ti.tree["e"], without_key, "[f]");
    test_emit_yaml_(ti.ints, 14,  without_key, "e");
    test_emit_yaml_(ti.ints, 17,  without_key, "[f]");
    //
    test_emit_yaml_(ti.tree["g"], with_key   , "g: h\n");
    test_emit_yaml_(ti.ints, 22,  with_key   , "g: h\n");
    test_emit_yaml_(ti.ints, 25,  with_key   , "h");
    test_emit_yaml_(ti.tree["g"], without_key, "h");
    test_emit_yaml_(ti.ints, 22,  without_key, "g");
    test_emit_yaml_(ti.ints, 25,  without_key, "h");
    //
    test_emit_yaml_(ti.tree["bmap"], with_key   , "bmap:\n  a: b\n");
    test_emit_yaml_(ti.ints, 28,     with_key   , "bmap:\n  a: b\n");
    test_emit_yaml_(ti.ints, 31,     with_key   , "a: b\n");
    test_emit_yaml_(ti.tree["bmap"], without_key, "a: b\n");
    test_emit_yaml_(ti.ints, 28,     without_key, "bmap");
    test_emit_yaml_(ti.ints, 31,     without_key, "a: b\n");
    //
    test_emit_yaml_(ti.tree["bseq"], with_key   , "bseq:\n  - a\n  - b\n");
    test_emit_yaml_(ti.ints, 39,     with_key   , "bseq:\n  - a\n  - b\n");
    test_emit_yaml_(ti.ints, 42,     with_key   , "- a\n- b\n");
    test_emit_yaml_(ti.tree["bseq"], without_key, "- a\n- b\n");
    test_emit_yaml_(ti.ints, 39,     without_key, "bseq");
    test_emit_yaml_(ti.ints, 42,     without_key, "- a\n- b\n");
}

TEST(emit_nested, container_key)
{
    char src[] = "[a, b]: {c: d}";
    IntBufs ints;
    parse_ints(src, &ints);
    test_emit_yaml_(ints, 0, with_key,    "? [a,b]\n: {c: d}\n");//BSTR
    test_emit_yaml_(ints, 0, without_key, "? [a,b]\n: {c: d}\n");//BSTR
    test_emit_yaml_(ints, 1, with_key,    "? [a,b]\n: {c: d}\n");//BDOC
    test_emit_yaml_(ints, 1, without_key, "? [a,b]\n: {c: d}\n");//BDOC
    test_emit_yaml_(ints, 2, with_key,    "? [a,b]\n: {c: d}\n");//BMAP
    test_emit_yaml_(ints, 2, without_key, "? [a,b]\n: {c: d}\n");//BMAP
    //FIXME test_emit_yaml_(ints, 3, with_key,    "? [a,b]\n: {c: d}\n");//KEY|BSEQ
    test_emit_yaml_(ints, 3, without_key, "[a,b]");//KEY|BSEQ
    test_emit_yaml_(ints, 4, with_key,    "a");
    test_emit_yaml_(ints, 4, without_key, "a");
    test_emit_yaml_(ints, 7, with_key,    "b");
    test_emit_yaml_(ints, 7, without_key, "b");
    test_emit_yaml_(ints, 11, with_key,    "{c: d}");
    test_emit_yaml_(ints, 11, without_key, "{c: d}");
    test_emit_yaml_(ints, 12, with_key,    "c: d\n");
    test_emit_yaml_(ints, 12, without_key, "c");
    test_emit_yaml_(ints, 15, with_key,    "d");
    test_emit_yaml_(ints, 15, without_key, "d");
}


//-----------------------------------------------------------------------------

TEST(emit_block_seq, ambiguous_plain_emitted_as_squo)
{
    EXPECT_EQ(scalar_style_query_plain_flow(": odd"), false);
    EXPECT_EQ(scalar_style_query_plain_flow(":\todd"), false);
    EXPECT_EQ(scalar_style_choose_flow(": odd"), SCALAR_SQUO);
    EXPECT_EQ(scalar_style_choose_flow(":\todd"), SCALAR_SQUO);
    EXPECT_EQ(scalar_style_query_plain_block(": odd"), false);
    EXPECT_EQ(scalar_style_query_plain_block(":\todd"), false);
    EXPECT_EQ(scalar_style_choose_block(": odd"), SCALAR_SQUO);
    EXPECT_EQ(scalar_style_choose_block(":\todd"), SCALAR_SQUO);
    {
        SCOPED_TRACE("1");
        Tree t;
        NodeRef r = t.rootref();
        r.set_seq(BLOCK);
        r[0].set_val(": odd", VAL_PLAIN);
        r[1].set_val(":\todd", VAL_PLAIN);
        test_emit_yaml_same_ints(r, "- : odd\n- :\todd\n");
    }
    {
        Tree t;
        NodeRef r = t.rootref();
        r.set_seq(BLOCK);
        r[0].set_val(": odd");
        r[1].set_val(":\todd");
        EXPECT_FALSE(r[0].is_val_plain());
        EXPECT_FALSE(r[1].is_val_plain());
        test_emit_yaml_same_ints(r, "- ': odd'\n- ':\todd'\n");
    }
}

TEST(emit_block_map, ambiguous_plain_emitted_as_squo)
{
    {
        Tree t;
        NodeRef r = t.rootref();
        r.set_map(BLOCK);
        r[0].set_key(": odd");
        r[0].set_val(": odd");
        r[1].set_key(":\todd");
        r[1].set_val(":\todd");
        test_emit_yaml_same_ints(r, "': odd': ': odd'\n':\todd': ':\todd'\n");
    }
    {
        Tree t;
        NodeRef r = t.rootref();
        r.set_map(BLOCK);
        r[0].set_key(": odd", KEY_PLAIN);
        r[0].set_val(": odd", VAL_PLAIN);
        r[1].set_key(":\todd", KEY_PLAIN);
        r[1].set_val(":\todd", VAL_PLAIN);
        EXPECT_EQ(emitrs_yaml<std::string>(t), ": odd: : odd\n:\todd: :\todd\n");
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define retonfail() do { if(testing::Test::HasFailure()) return; } while(0)
#define bailonfail() do { if(testing::Test::HasFailure()) goto failure; /* NOLINT */ } while(0)
void test_emits(Tree const* t, id_type id, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    retonfail();
    RYML_TRACE_FMT("id={}", id);
    std::string append_prefix = "#before\n";
    ConstNodeRef node{t, id};
    ASSERT_EQ(node.id(), id);
    ASSERT_EQ(node.tree(), t);
    bool isroot = id == NONE || (!t->empty() && id == t->root_id());
    if(!t)
    {
        ASSERT_EQ(id, NONE);
        if(opts == EmitOptions{})
        {
            EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(EmitOptions{}, buf); em.emit_as(EMIT_YAML, t); return em.get_result(/*error_on_excess*/true); }), expected_yaml);
            bailonfail();
            EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(EmitOptions{}, buf); em.emit_as(EMIT_JSON, t); return em.get_result(/*error_on_excess*/true); }), expected_json);
            EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(EmitOptions{}, f); em.emit_as(EMIT_YAML, t); }), expected_yaml);
            EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(EmitOptions{}, f); em.emit_as(EMIT_JSON, t); }), expected_json);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(EmitOptions{}, &oss); em.emit_as(EMIT_YAML, t); }), expected_yaml);;
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(EmitOptions{}, &oss); em.emit_as(EMIT_JSON, t); }), expected_json);;
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node); }), expected_yaml);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(node); }), expected_json);
        }
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_YAML, t); return em.get_result(/*error_on_excess*/true); }), expected_yaml);
        bailonfail();
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_JSON, t); return em.get_result(/*error_on_excess*/true); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); em.emit_as(EMIT_YAML, t); }), expected_yaml);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); em.emit_as(EMIT_JSON, t); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_YAML, t); }), expected_yaml);;
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_JSON, t); }), expected_json);;
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node, opts); }), expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(node, opts); }), expected_json);
        return;
    }
    if(opts == EmitOptions{})
    {
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(*t, id, buf); }), expected_yaml);
        bailonfail();
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(*t, id, buf); }), expected_json);
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(node, buf); }), expected_yaml);
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(node, buf); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(*t, id, f); }), expected_yaml);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(*t, id, f); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(node, f); }), expected_yaml);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(node, f); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(*t, id); }), expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(*t, id); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss <<         node; }),  expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node); }), expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(node); }), expected_json);
        EXPECT_EQ(emitrs_yaml<std::string>(*t, id), expected_yaml);
        EXPECT_EQ(emitrs_json<std::string>(*t, id), expected_json);
        EXPECT_EQ(emitrs_yaml<std::string>(node), expected_yaml);
        EXPECT_EQ(emitrs_json<std::string>(node), expected_json);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(*t, id, s, /*append*/true); } ), append_prefix + expected_yaml);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(*t, id, s, /*append*/true); } ), append_prefix + expected_json);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(node, s, /*append*/true); } ), append_prefix + expected_yaml);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(node, s, /*append*/true); } ), append_prefix + expected_json);
        bailonfail();
        if(isroot)
        {
            EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(*t, buf); }), expected_yaml);
            bailonfail();
            EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(*t, buf); }), expected_json);
            EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(EmitOptions{}, buf); em.emit_as(EMIT_YAML, t); return em.get_result(/*error_on_excess*/true); }), expected_yaml);
            EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(EmitOptions{}, buf); em.emit_as(EMIT_JSON, t); return em.get_result(/*error_on_excess*/true); }), expected_json);
            EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(*t, f); }), expected_yaml);
            EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(*t, f); }), expected_json);
            EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(EmitOptions{}, f); em.emit_as(EMIT_YAML, t); }), expected_yaml);
            EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(EmitOptions{}, f); em.emit_as(EMIT_JSON, t); }), expected_json);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << *t; }), expected_yaml);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(*t); }), expected_yaml);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(*t); }), expected_json);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node); }), expected_yaml);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(node); }), expected_json);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(EmitOptions{}, &oss); em.emit_as(EMIT_YAML, t); }), expected_yaml);;
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(EmitOptions{}, &oss); em.emit_as(EMIT_JSON, t); }), expected_json);;
            EXPECT_EQ(emitrs_yaml<std::string>(*t), expected_yaml);
            EXPECT_EQ(emitrs_json<std::string>(*t), expected_json);
            EXPECT_EQ(emitrs_yaml<std::string>(node), expected_yaml);
            EXPECT_EQ(emitrs_json<std::string>(node), expected_json);
            EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(*t, s, /*append*/true); } ), append_prefix + expected_yaml);
            EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(*t, s, /*append*/true); } ), append_prefix + expected_json);
            bailonfail();
        }
    }
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(*t, id, opts, buf); }), expected_yaml);
    bailonfail();
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(*t, id, opts, buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(node, opts, buf); }), expected_yaml);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(node, opts, buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_YAML, t, id); return em.get_result(/*error_on_excess*/true); }), expected_yaml);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_JSON, t, id); return em.get_result(/*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(*t, id, opts, f); }), expected_yaml);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(*t, id, opts, f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(node, opts, f); }), expected_yaml);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(node, opts, f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); em.emit_as(EMIT_YAML, t, id); }), expected_yaml);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); em.emit_as(EMIT_JSON, t, id); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(*t, id, opts); }), expected_yaml);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(*t, id, opts); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node, opts); }), expected_yaml);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(node, opts); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_YAML, t, id); }), expected_yaml);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_JSON, t, id); }), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(*t, id, opts), expected_yaml);
    EXPECT_EQ(emitrs_json<std::string>(*t, id, opts), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(node, opts), expected_yaml);
    EXPECT_EQ(emitrs_json<std::string>(node, opts), expected_json);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(*t, id, opts, s, /*append*/true); } ), append_prefix + expected_yaml);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(*t, id, opts, s, /*append*/true); } ), append_prefix + expected_json);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(node, opts, s, /*append*/true); } ), append_prefix + expected_yaml);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(node, opts, s, /*append*/true); } ), append_prefix + expected_json);
    bailonfail();
    if(isroot)
    {
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(*t, opts, buf); }), expected_yaml);
        bailonfail();
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(*t, opts, buf); }), expected_json);
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_YAML, t); return em.get_result(/*error_on_excess*/true); }), expected_yaml);
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_JSON, t); return em.get_result(/*error_on_excess*/true); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(*t, opts, f); }), expected_yaml);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(*t, opts, f); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); return em.emit_as(EMIT_YAML, t); }), expected_yaml);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); return em.emit_as(EMIT_JSON, t); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(*t, opts); }), expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(*t, opts); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_YAML, t); }), expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_JSON, t); }), expected_json);
        EXPECT_EQ(emitrs_yaml<std::string>(*t, opts), expected_yaml);
        EXPECT_EQ(emitrs_json<std::string>(*t, opts), expected_json);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(*t, opts, s, /*append*/true); } ), append_prefix + expected_yaml);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(*t, opts, s, /*append*/true); } ), append_prefix + expected_json);
        bailonfail();
    }
    if(!t->empty() && t->depth_desc(id != NONE ? id : t->root_id()) > 1)
    {
        // error on max depth
        EmitOptions optsd = opts;
        optsd = optsd.max_depth(0);
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2buf([&](substr buf){ return emit_yaml(*t, id, optsd, buf); }); }));
        bailonfail();
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2buf([&](substr buf){ return emit_json(*t, id, optsd, buf); }); }));
        bailonfail();
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2buf([&](substr buf){ return emit_yaml(node, optsd, buf); }); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2buf([&](substr buf){ return emit_json(node, optsd, buf); }); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2file([&](FILE *f){ emit_yaml(*t, id, optsd, f); }); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2file([&](FILE *f){ emit_json(*t, id, optsd, f); }); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2file([&](FILE *f){ emit_yaml(node, optsd, f); }); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2file([&](FILE *f){ emit_json(node, optsd, f); }); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(*t, id, optsd); }); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(*t, id, optsd); }); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node, optsd); }); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(node, optsd); }); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emitrs_yaml<std::string>(*t, id, optsd); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emitrs_json<std::string>(*t, id, optsd); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emitrs_yaml<std::string>(node, optsd); }));
        RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emitrs_json<std::string>(node, optsd); }));
        bailonfail();
        if(id == NONE || id == t->root_id())
        {
            RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2buf([&](substr buf){ return emit_yaml(*t, optsd, buf); }); }));
            bailonfail();
            RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2buf([&](substr buf){ return emit_json(*t, optsd, buf); }); }));
            bailonfail();
            RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2file([&](FILE *f){ emit_yaml(*t, optsd, f); }); }));
            RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2file([&](FILE *f){ emit_json(*t, optsd, f); }); }));
            RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(ConstNodeRef(t), optsd); }); }));
            RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(ConstNodeRef(t), optsd); }); }));
            RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emitrs_yaml<std::string>(*t, optsd); }));
            RYML_EXPECT_ERROR(check_error_visit(t, [&]{ return emitrs_json<std::string>(*t, optsd); }));
            bailonfail();
        }
    }
    return;
failure:
    print_tree(t);
}

using extra::ievt::evt_size;
void test_emits_ints(IntBufsCR ints, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    retonfail();
    RYML_TRACE_FMT("pos={}", pos);
    EXPECT_EQ(ints.emit_yaml<std::string>(opts, pos), expected_yaml);
    bailonfail();
    EXPECT_EQ(ints.emit_json<std::string>(opts, pos), expected_json);
    bailonfail();
    EXPECT_EQ(emit2buf([&](substr buf){ size_t sz = ints.emit_yaml(buf, opts, pos); buf.str = nullptr; buf.len = sz ; return buf; }), expected_yaml);
    EXPECT_EQ(emit2buf([&](substr buf){ size_t sz = ints.emit_json(buf, opts, pos); buf.str = nullptr; buf.len = sz ; return buf; }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return ints.emit_yaml(f, opts, pos); }), expected_yaml);
    EXPECT_EQ(emit2file([&](FILE *f){ return ints.emit_json(f, opts, pos); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ ints.emit_yaml_stream(oss, opts, pos); }), expected_yaml);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ ints.emit_json_stream(oss, opts, pos); }), expected_json);
    bailonfail();
    return;
failure:
    ints.print();
}

void test_emits_ints(evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    retonfail();
    IntBufs ints;
    std::string yaml = expected_yaml;
    parse_ints(to_substr(yaml), &ints);
    test_emits_ints(ints, pos, expected_yaml, expected_json, opts);
}

void test_emits(Tree const& t, id_type id, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    test_emits(&t, id, expected_yaml, expected_json, opts);
    test_emits_ints(pos, expected_yaml, expected_json, opts);
}
void test_emits(TreeAndInts const& ti, id_type id, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    test_emits(&ti.tree, id, expected_yaml, expected_json, opts);
    test_emits_ints(ti.ints, pos, expected_yaml, expected_json, opts);
}
void test_emits(Tree const& t, id_type id, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    test_emits(&t, id, expected_yaml, expected_json, opts);
    if(id == NONE || id == 0)
        test_emits_ints(0, expected_yaml, expected_json, opts);
}
void test_emits(TreeAndInts const& ti, id_type id, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    test_emits(&ti.tree, id, expected_yaml, expected_json, opts);
    if(id == NONE || id == 0)
        test_emits_ints(ti.ints, 0, expected_yaml, expected_json, opts);
}

void test_emits(Tree const& t, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    if(t.empty())
        test_emits(&t, NONE, expected_yaml, expected_json, opts);
    else
        test_emits(&t, t.root_id(), expected_yaml, expected_json, opts);
    test_emits_ints(0, expected_yaml, expected_json, opts);
}
void test_emits(TreeAndInts const& ti, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    if(ti.tree.empty())
        test_emits(&ti.tree, NONE, expected_yaml, expected_json, opts);
    else
        test_emits(&ti.tree, ti.tree.root_id(), expected_yaml, expected_json, opts);
    test_emits_ints(ti.ints, 0, expected_yaml, expected_json, opts);
}

void test_emits(ConstNodeRef node, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    test_emits(node.tree(), node.id(), expected_yaml, expected_json, opts);
    test_emits_ints(pos, expected_yaml, expected_json, opts);
}
void test_emits(TreeAndInts const& ti, ConstNodeRef node, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    ASSERT_EQ(node.tree(), &ti.tree);
    test_emits(node.tree(), node.id(), expected_yaml, expected_json, opts);
    test_emits_ints(ti.ints, pos, expected_yaml, expected_json, opts);
}
void test_emits(ConstNodeRef node, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    test_emits(node.tree(), node.id(), expected_yaml, expected_json, opts);
    if(node.id() == NONE || (node.tree() && node.id() == node.tree()->root_id_maybe()))
        test_emits_ints(0, expected_yaml, expected_json, opts);
}
void test_emits(TreeAndInts const& ti, ConstNodeRef node, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={})
{
    ASSERT_EQ(node.tree(), &ti.tree);
    test_emits(node.tree(), node.id(), expected_yaml, expected_json, opts);
    if(node.id() == NONE || (node.tree() && node.id() == node.tree()->root_id_maybe()))
        test_emits_ints(ti.ints, 0, expected_yaml, expected_json, opts);
}

#define test_emits_(...) { SCOPED_TRACE("test_emits"); test_emits(__VA_ARGS__); }


//-----------------------------------------------------------------------------

TEST(emit, empty_tree)
{
    const Tree t(/*capacity*/0); // must be const!
    ASSERT_TRUE(t.empty());
    std::string expected = R"()";
    test_emits_(t, expected, expected);
}

TEST(emit, empty_node)
{
    std::string expected = R"()";
    test_emits_(NodeRef{}, 0, expected, expected);
    test_emits_(ConstNodeRef{}, 0, expected, expected);
}

TEST(emit, existing_tree)
{
    const Tree t = parse_in_arena("[foo, bar]");
    std::string expected = "[foo,bar]";
    std::string expected_json = R"(["foo","bar"])";
    test_emits_(t, expected, expected_json);
}

TEST(emit, no_node)
{
    const Tree t = parse_in_arena("[foo, bar]");
    std::string expected = "[foo,bar]";
    std::string expected_json = R"(["foo","bar"])";
    test_emits_(t, NONE, expected, expected_json);
}

TEST(emit, empty_key_squo)
{
    {
        SCOPED_TRACE("one only");
        const TreeAndInts ti = parse_tree_and_ints(R"(
? ''
: literal
)");
        {
            SCOPED_TRACE("indent");
            std::string expected = "'': literal\n";
            std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
            test_emits_(ti, expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "'': literal\n";
            std::string expected_json = "{\n\"\": \"literal\"\n}\n";
            test_emits_(ti, expected, expected_json, noindent);
        }
    }
    {
        SCOPED_TRACE("nested");
        const TreeAndInts ti = parse_tree_and_ints(R"(
level1:
  ? ''
  : literal
)");
        {
            SCOPED_TRACE("indent");
            std::string expected = "level1:\n  '': literal\n";
            std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
            test_emits_(ti, expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "level1:\n  '': literal\n";
            std::string expected_json = "{\n\"level1\": {\n\"\": \"literal\"\n}\n}\n";
            test_emits_(ti, expected, expected_json, noindent);
        }
    }
    const TreeAndInts ti = parse_tree_and_ints(R"(
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
            test_emits_(ti, ti.tree["level1"]["level2"]["level3"], 29, expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "level3:\n  '': literal\n";
            std::string expected_json = "\"level3\": {\n\"\": \"literal\"\n}\n";
            test_emits_(ti, ti.tree["level1"]["level2"]["level3"], 29, expected, expected_json, noindent);
        }
    }
    {
        SCOPED_TRACE("level2");
        {
            SCOPED_TRACE("indent");
            std::string expected = "level2:\n  '': literal\n  level3:\n    '': literal\n";
            std::string expected_json = "\"level2\": {\n  \"\": \"literal\",\n  \"level3\": {\n    \"\": \"literal\"\n  }\n}\n";
            test_emits_(ti, ti.tree["level1"]["level2"], 19, expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "level2:\n  '': literal\n  level3:\n    '': literal\n";
            std::string expected_json = "\"level2\": {\n\"\": \"literal\",\n\"level3\": {\n\"\": \"literal\"\n}\n}\n";
            test_emits_(ti, ti.tree["level1"]["level2"].id(), 19, expected, expected_json, noindent);
        }
    }
    {
        SCOPED_TRACE("level1");
        {
            SCOPED_TRACE("indent");
            std::string expected = "level1:\n  '': literal\n  level2:\n    '': literal\n    level3:\n      '': literal\n";
            std::string expected_json = "\"level1\": {\n  \"\": \"literal\",\n  \"level2\": {\n    \"\": \"literal\",\n    \"level3\": {\n      \"\": \"literal\"\n    }\n  }\n}\n";
            test_emits_(ti, ti.tree["level1"], 9, expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "level1:\n  '': literal\n  level2:\n    '': literal\n    level3:\n      '': literal\n";
            std::string expected_json = "\"level1\": {\n\"\": \"literal\",\n\"level2\": {\n\"\": \"literal\",\n\"level3\": {\n\"\": \"literal\"\n}\n}\n}\n";
            test_emits_(ti, ti.tree["level1"], 9, expected, expected_json, noindent);
        }
    }
    {
        SCOPED_TRACE("level0");
        {
            SCOPED_TRACE("indent");
            std::string expected = "'': literal\nlevel1:\n  '': literal\n  level2:\n    '': literal\n    level3:\n      '': literal\n";
            std::string expected_json = "{\n  \"\": \"literal\",\n  \"level1\": {\n    \"\": \"literal\",\n    \"level2\": {\n      \"\": \"literal\",\n      \"level3\": {\n        \"\": \"literal\"\n      }\n    }\n  }\n}\n";
            test_emits_(ti, NONE, expected, expected_json);
        }
        {
            SCOPED_TRACE("no indent");
            std::string expected = "'': literal\nlevel1:\n  '': literal\n  level2:\n    '': literal\n    level3:\n      '': literal\n";
            std::string expected_json = "{\n\"\": \"literal\",\n\"level1\": {\n\"\": \"literal\",\n\"level2\": {\n\"\": \"literal\",\n\"level3\": {\n\"\": \"literal\"\n}\n}\n}\n}\n";
            test_emits_(ti, NONE, expected, expected_json, noindent);
        }
    }
}

TEST(emit, empty_key_dquo)
{
    {
        SCOPED_TRACE("one only");
        const TreeAndInts ti = parse_tree_and_ints(R"(
? ""
: literal
)");
        std::string expected = "\"\": literal\n";
        std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    {
        SCOPED_TRACE("nested");
        const TreeAndInts ti = parse_tree_and_ints(R"(
level1:
  ? ""
  : literal
)");
        std::string expected = "level1:\n  \"\": literal\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    const TreeAndInts ti = parse_tree_and_ints(R"(
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
        test_emits_(ti, ti.tree["level1"]["level2"]["level3"], 29, expected, expected_json);
    }
    {
        SCOPED_TRACE("level2");
        std::string expected = "level2:\n  \"\": literal\n  level3:\n    \"\": literal\n";
        std::string expected_json = "\"level2\": {\n  \"\": \"literal\",\n  \"level3\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits_(ti, ti.tree["level1"]["level2"], 19, expected, expected_json);
    }
    {
        SCOPED_TRACE("level1");
        std::string expected = "level1:\n  \"\": literal\n  level2:\n    \"\": literal\n    level3:\n      \"\": literal\n";
        std::string expected_json = "\"level1\": {\n  \"\": \"literal\",\n  \"level2\": {\n    \"\": \"literal\",\n    \"level3\": {\n      \"\": \"literal\"\n    }\n  }\n}\n";
        test_emits_(ti, ti.tree["level1"], 9, expected, expected_json);
    }
    {
        SCOPED_TRACE("level0");
        std::string expected = "\"\": literal\nlevel1:\n  \"\": literal\n  level2:\n    \"\": literal\n    level3:\n      \"\": literal\n";
        std::string expected_json = "{\n  \"\": \"literal\",\n  \"level1\": {\n    \"\": \"literal\",\n    \"level2\": {\n      \"\": \"literal\",\n      \"level3\": {\n        \"\": \"literal\"\n      }\n    }\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
}

TEST(emit, empty_key_plain)
{
    {
        SCOPED_TRACE("one only");
        const TreeAndInts ti = parse_tree_and_ints(R"(
?
: literal
)");
        std::string expected = ": literal\n";
        std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    {
        SCOPED_TRACE("nested");
        const TreeAndInts ti = parse_tree_and_ints(R"(
level1:
  ?
  : literal
)");
        std::string expected = "level1:\n  : literal\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    const TreeAndInts ti = parse_tree_and_ints(R"(
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
        test_emits_(ti, ti.tree["level1"]["level2"]["level3"], 29, expected, expected_json);
    }
    {
        SCOPED_TRACE("level2");
        std::string expected = "level2:\n  : literal\n  level3:\n    : literal\n";
        std::string expected_json = "\"level2\": {\n  \"\": \"literal\",\n  \"level3\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits_(ti, ti.tree["level1"]["level2"], 19, expected, expected_json);
    }
    {
        SCOPED_TRACE("level1");
        std::string expected = "level1:\n  : literal\n  level2:\n    : literal\n    level3:\n      : literal\n";
        std::string expected_json = "\"level1\": {\n  \"\": \"literal\",\n  \"level2\": {\n    \"\": \"literal\",\n    \"level3\": {\n      \"\": \"literal\"\n    }\n  }\n}\n";
        test_emits_(ti, ti.tree["level1"], 9, expected, expected_json);
    }
    {
        SCOPED_TRACE("level0");
        std::string expected = ": literal\nlevel1:\n  : literal\n  level2:\n    : literal\n    level3:\n      : literal\n";
        std::string expected_json = "{\n  \"\": \"literal\",\n  \"level1\": {\n    \"\": \"literal\",\n    \"level2\": {\n      \"\": \"literal\",\n      \"level3\": {\n        \"\": \"literal\"\n      }\n    }\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
}

TEST(emit, empty_key_literal)
{
    {
        SCOPED_TRACE("one only");
        const TreeAndInts ti = parse_tree_and_ints(R"(
? |-
: literal
)");
        std::string expected = "? |-\n: literal\n";
        std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    {
        SCOPED_TRACE("nested1");
        const TreeAndInts ti = parse_tree_and_ints("level1:\n  ? |-\n  : literal\n");
        std::string expected = "level1:\n  ? |-\n  : literal\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    const TreeAndInts ti = parse_tree_and_ints(R"(
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
        test_emits_(ti, ti.tree["level1"]["level2"]["level3"], 29, expected, expected_json);
    }
    {
        SCOPED_TRACE("level2");
        std::string expected = "level2:\n  ? |-\n  : literal\n  level3:\n    ? |-\n    : literal\n";
        std::string expected_json = "\"level2\": {\n  \"\": \"literal\",\n  \"level3\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits_(ti, ti.tree["level1"]["level2"], 19, expected, expected_json);
    }
    {
        SCOPED_TRACE("level1");
        std::string expected = "level1:\n  ? |-\n  : literal\n  level2:\n    ? |-\n    : literal\n    level3:\n      ? |-\n      : literal\n";
        std::string expected_json = "\"level1\": {\n  \"\": \"literal\",\n  \"level2\": {\n    \"\": \"literal\",\n    \"level3\": {\n      \"\": \"literal\"\n    }\n  }\n}\n";
        test_emits_(ti, ti.tree["level1"], 9, expected, expected_json);
    }
    {
        SCOPED_TRACE("level0");
        std::string expected = "? |-\n: literal\nlevel1:\n  ? |-\n  : literal\n  level2:\n    ? |-\n    : literal\n    level3:\n      ? |-\n      : literal\n";
        std::string expected_json = "{\n  \"\": \"literal\",\n  \"level1\": {\n    \"\": \"literal\",\n    \"level2\": {\n      \"\": \"literal\",\n      \"level3\": {\n        \"\": \"literal\"\n      }\n    }\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
}

TEST(emit, empty_key_folded)
{
    {
        SCOPED_TRACE("one only");
        const TreeAndInts ti = parse_tree_and_ints(R"(
? >-
: folded
)");
        std::string expected = "? >-\n: folded\n";
        std::string expected_json = "{\n  \"\": \"folded\"\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    {
        SCOPED_TRACE("nested");
        const TreeAndInts ti = parse_tree_and_ints(R"(
level1:
  ? >-
  : folded
)");
        std::string expected = "level1:\n  ? >-\n  : folded\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"folded\"\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    const TreeAndInts ti = parse_tree_and_ints(R"(
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
        test_emits_(ti, ti.tree["level1"]["level2"]["level3"], 29, expected, expected_json);
    }
    {
        SCOPED_TRACE("level2");
        std::string expected = "level2:\n  ? >-\n  : folded\n  level3:\n    ? >-\n    : folded\n";
        std::string expected_json = "\"level2\": {\n  \"\": \"folded\",\n  \"level3\": {\n    \"\": \"folded\"\n  }\n}\n";
        test_emits_(ti, ti.tree["level1"]["level2"], 19, expected, expected_json);
    }
    {
        SCOPED_TRACE("level1");
        std::string expected = "level1:\n  ? >-\n  : folded\n  level2:\n    ? >-\n    : folded\n    level3:\n      ? >-\n      : folded\n";
        std::string expected_json = "\"level1\": {\n  \"\": \"folded\",\n  \"level2\": {\n    \"\": \"folded\",\n    \"level3\": {\n      \"\": \"folded\"\n    }\n  }\n}\n";
        test_emits_(ti, ti.tree["level1"], 9, expected, expected_json);
    }
    {
        SCOPED_TRACE("level0");
        std::string expected = "? >-\n: folded\nlevel1:\n  ? >-\n  : folded\n  level2:\n    ? >-\n    : folded\n    level3:\n      ? >-\n      : folded\n";
        std::string expected_json = "{\n  \"\": \"folded\",\n  \"level1\": {\n    \"\": \"folded\",\n    \"level2\": {\n      \"\": \"folded\",\n      \"level3\": {\n        \"\": \"folded\"\n      }\n    }\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
}


//-----------------------------------------------------------------------------

using extra::ievt::evt_bits;
namespace xievt = extra::ievt;

struct TmpContainerStyle
{
    Tree *tree;
    id_type id;
    NodeType prev;
    IntBufs *ints;
    evt_size evt_pos;
    evt_bits evt_prev;
    extra::ievt::evt_bits prev_ints;
    TmpContainerStyle(NodeRef n, NodeType tmp) : TmpContainerStyle(n.tree(), nullptr, n.id(), tmp, {}, {}) {}
    TmpContainerStyle(Tree &t_, id_type id_, NodeType tmp) : TmpContainerStyle(&t_, nullptr, id_, tmp, {}, {}) {}
    TmpContainerStyle(TreeAndInts &ti, NodeRef const& n, NodeType tmp, evt_size evt_pos_, evt_bits evt) : TmpContainerStyle(&ti.tree, &ti.ints, n.id(), tmp, evt_pos_, evt) {}
    TmpContainerStyle(TreeAndInts &ti, NodeType tmp, evt_bits evt) : TmpContainerStyle(&ti.tree, &ti.ints, ti.tree.root_id(), tmp, 0, evt) {}
    TmpContainerStyle(Tree *tree_, IntBufs *ints_, id_type id_, NodeType tmp, evt_size evt_pos_, evt_bits evt)
        : tree(tree_)
        , id(id_)
        , prev(tree_->type(id))
        , ints(ints_)
        , evt_pos(evt_pos_)
        , evt_prev()
    {
        (tree->_p(id)->m_type &= ~CONTAINER_STYLE) |= tmp;
        if(ints)
        {
            RYML_ASSERT_BASIC_(evt_pos < ints_->evts.len);
            evt_prev = ints->evts.ptr[evt_pos];
            RYML_ASSERT_BASIC_(evt_prev & (xievt::detail::styles_ievt_cont|xievt::BSEQ|xievt::BMAP)); // NOLINT
            (ints->evts.ptr[evt_pos] &= ~xievt::detail::styles_ievt_cont) |= evt; // NOLINT
        }
    }
    ~TmpContainerStyle()
    {
        tree->_p(id)->m_type = prev;
        if(ints)
        {
            ints->evts.ptr[evt_pos] = evt_prev;
        }
    }
};

TmpContainerStyle mkblk(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, BLOCK,  pos, xievt::BLCK}; };
TmpContainerStyle mkflowsl(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, FLOW_SL,  pos, xievt::FLOW|xievt::FSL_}; };
TmpContainerStyle mkflowml1(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, FLOW_ML1, pos, xievt::FLOW|xievt::FML1}; };
TmpContainerStyle mkflowmln(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, FLOW_MLN, pos, xievt::FLOW|xievt::FMLN}; };
TmpContainerStyle mkflowslspc(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, FLOW_SL|FLOW_SPC,  pos, xievt::FLOW|xievt::FSL_|xievt::FSPC}; };
TmpContainerStyle mkflowml1spc(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, FLOW_ML1|FLOW_SPC, pos, xievt::FLOW|xievt::FML1|xievt::FSPC}; };
TmpContainerStyle mkflowmlnspc(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, FLOW_MLN|FLOW_SPC, pos, xievt::FLOW|xievt::FMLN|xievt::FSPC}; };
#define TMPSTY(sty, ...) TmpContainerStyle C4_XCAT(tmpsty_, __LINE__) = mk##sty(__VA_ARGS__)

static void test_emit_seq_node(TreeAndInts & ti, std::string const& yaml)
{
    RYML_TRACE_FMT("yaml:\n~~~{}\n~~~\n", yaml);
    Tree & t = ti.tree;
    {
        SCOPED_TRACE("t[0]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            test_emits_(ti, t[0], 3, "foo", "\"foo\"");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            test_emits_(ti, t[0], 3, "foo", "\"foo\"");
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t, 2);
            test_emits_(ti, t[0], 3, "foo", "\"foo\"");
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t, 2);
            test_emits_(ti, t[0], 3, "foo", "\"foo\"");
        }
    }
    {
        SCOPED_TRACE("t[1]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            test_emits_(ti, t[1], 6, "bar", "\"bar\"");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            test_emits_(ti, t[1], 6, "bar", "\"bar\"");
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t, 2);
            test_emits_(ti, t[1], 6, "bar", "\"bar\"");
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t, 2);
            test_emits_(ti, t[1], 6, "bar", "\"bar\"");
        }
    }
    {
        SCOPED_TRACE("t[2]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t[2], 9);
            test_emits_(ti, t[2], 9,
                        "- nested\n- seq\n",
                        "[\n  \"nested\",\n  \"seq\"\n]\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t[2], 9);
            test_emits_(ti, t[2], 9,
                        "[nested,seq]",
                        "[\"nested\",\"seq\"]");
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t[2], 9);
            test_emits_(ti, t[2], 9,
                        "[\n  nested,\n  seq\n]\n",
                        "[\n  \"nested\",\n  \"seq\"\n]\n");
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t[2], 9);
            test_emits_(ti, t[2], 9,
                        "[\n  nested,seq\n]\n",
                        "[\n  \"nested\",\"seq\"\n]\n");
        }
    }
    {
        SCOPED_TRACE("t[3]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t[3], 17);
            test_emits_(ti, t[3], 17,
                        "nested: map\n",
                        "{\n  \"nested\": \"map\"\n}\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t[3], 17);
            test_emits_(ti, t[3], 17,
                        "{nested: map}",
                        "{\"nested\": \"map\"}");
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t[3], 17);
            test_emits_(ti, t[3], 17,
                        "{\n  nested: map\n}\n",
                        "{\n  \"nested\": \"map\"\n}\n");
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t[3], 17);
            test_emits_(ti, t[3], 17,
                        "{\n  nested: map\n}\n",
                        "{\n  \"nested\": \"map\"\n}\n");
        }
    }
    {
        SCOPED_TRACE("root");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            TMPSTY(blk, ti, t[2], 9);
            TMPSTY(blk, ti, t[3], 17);
            test_emits_(ti, t, 0,
                        "- foo\n- bar\n- - nested\n  - seq\n- nested: map\n",
                        "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 9);
            TMPSTY(flowsl, ti, t[3], 17);
            test_emits_(ti, t, 0,
                        "[foo,bar,[nested,seq],{nested: map}]",
                        "[\"foo\",\"bar\",[\"nested\",\"seq\"],{\"nested\": \"map\"}]");
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t, 2);
            TMPSTY(flowml1, ti, t[2], 9);
            TMPSTY(flowml1, ti, t[3], 17);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "[\n  foo,\n  bar,\n  [\n    nested,\n    seq\n  ],\n  {\n    nested: map\n  }\n]\n",
                            "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 2,
                            "[\nfoo,\nbar,\n[\nnested,\nseq\n],\n{\nnested: map\n}\n]\n",
                            "[\n\"foo\",\n\"bar\",\n[\n\"nested\",\n\"seq\"\n],\n{\n\"nested\": \"map\"\n}\n]\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t, 2);
            TMPSTY(flowmln, ti, t[2], 9);
            TMPSTY(flowmln, ti, t[3], 17);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "[\n  foo,bar,[\n    nested,seq\n  ],{\n    nested: map\n  }\n]\n",
                            "[\n  \"foo\",\"bar\",[\n    \"nested\",\"seq\"\n  ],{\n    \"nested\": \"map\"\n  }\n]\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 0,
                            "[\nfoo,bar,[\nnested,seq\n],{\nnested: map\n}\n]\n",
                            "[\n\"foo\",\"bar\",[\n\"nested\",\"seq\"\n],{\n\"nested\": \"map\"\n}\n]\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("block-flow-ml");
            {
                TMPSTY(blk, ti, t, 2);
                TMPSTY(flowml1, ti, t[2], 9);
                TMPSTY(flowml1, ti, t[3], 17);
                {
                    SCOPED_TRACE("indent");
                    test_emits_(ti, t, 0,
                                "- foo\n- bar\n- [\n    nested,\n    seq\n  ]\n- {\n    nested: map\n  }\n",
                                "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
                }
                {
                    SCOPED_TRACE("no indent");
                    test_emits_(ti, t, 0,
                                "- foo\n- bar\n- [\n  nested,\n  seq\n  ]\n- {\n  nested: map\n  }\n",
                                "[\n\"foo\",\n\"bar\",\n[\n\"nested\",\n\"seq\"\n],\n{\n\"nested\": \"map\"\n}\n]\n",
                                noindent);
                }
            }
        }
    }
    {
        SCOPED_TRACE("full-all");
        {
            SCOPED_TRACE("flow_ml");
            TMPSTY(flowml1, ti, t, 2);
            TMPSTY(flowml1, ti, t[2], 9);
            TMPSTY(flowml1, ti, t[3], 17);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "[\n  foo,\n  bar,\n  [\n    nested,\n    seq\n  ],\n  {\n    nested: map\n  }\n]\n",
                            "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
            }
            {
                SCOPED_TRACE("no indent ml");
                test_emits_(ti, t, 0,
                            "[\nfoo,\nbar,\n[\nnested,\nseq\n],\n{\nnested: map\n}\n]\n",
                            "[\n\"foo\",\n\"bar\",\n[\n\"nested\",\n\"seq\"\n],\n{\n\"nested\": \"map\"\n}\n]\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 9);
            TMPSTY(flowsl, ti, t[3], 17);
            test_emits_(ti, t, 0,
                        "[foo,bar,[nested,seq],{nested: map}]",
                        "[\"foo\",\"bar\",[\"nested\",\"seq\"],{\"nested\": \"map\"}]");
        }
    }
}

TEST(emit, existing_seq_node_flow_sl)
{
    const std::string yaml = "[foo,bar,[nested,seq],{nested: map}]";
    SCOPED_TRACE(yaml);
    TreeAndInts ti = parse_tree_and_ints(to_csubstr(yaml));
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, ti.tree, 0,
                    "[foo,bar,[nested,seq],{nested: map}]",
                    "[\"foo\",\"bar\",[\"nested\",\"seq\"],{\"nested\": \"map\"}]");
    }
    test_emit_seq_node(ti, yaml);
}

TEST(emit, existing_seq_node_flow_ml)
{
    const std::string yaml = "[\n  foo,\n  bar,\n  [\n    nested,\n    seq\n  ],\n  {\n    nested: map\n  }\n]\n";
    SCOPED_TRACE(yaml);
    TreeAndInts ti = parse_tree_and_ints(to_csubstr(yaml));
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, ti.tree, 0,
                    "[\n  foo,\n  bar,\n  [\n    nested,\n    seq\n  ],\n  {\n    nested: map\n  }\n]\n",
                    "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
    }
    test_emit_seq_node(ti, yaml);
}

TEST(emit, existing_seq_node_block)
{
    const std::string yaml = "- foo\n- bar\n- - nested\n  - seq\n- nested: map\n";
    SCOPED_TRACE(yaml);
    TreeAndInts ti = parse_tree_and_ints(to_csubstr(yaml));
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, ti.tree, 0,
                    "- foo\n- bar\n- - nested\n  - seq\n- nested: map\n",
                    "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
    }
    test_emit_seq_node(ti, yaml);
}


//-----------------------------------------------------------------------------


static void test_emit_map_node(TreeAndInts & ti, std::string const& yaml)
{
    SCOPED_TRACE(yaml);
    Tree & t = ti.tree;
    {
        SCOPED_TRACE("root");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            TMPSTY(blk, ti, t[2], 18);
            TMPSTY(blk, ti, t[3], 29);
            test_emits_(ti, t, 0,
                        "0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n",
                        "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 18);
            TMPSTY(flowsl, ti, t[3], 29);
            test_emits_(ti, t, 0,
                        "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}",
                        R"({"0": "foo","1": "bar","2": ["nested","seq"],"3": {"nested": "map"}})");
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t, 2);
            TMPSTY(flowml1, ti, t[2], 18);
            TMPSTY(flowml1, ti, t[3], 29);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "{\n  0: foo,\n  1: bar,\n  2: [\n    nested,\n    seq\n  ],\n  3: {\n    nested: map\n  }\n}\n",
                            "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 0,
                            "{\n0: foo,\n1: bar,\n2: [\nnested,\nseq\n],\n3: {\nnested: map\n}\n}\n",
                            "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\n\"nested\",\n\"seq\"\n],\n\"3\": {\n\"nested\": \"map\"\n}\n}\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("flow_ml1-flowsl");
            TMPSTY(flowml1, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 18);
            TMPSTY(flowsl, ti, t[3], 29);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "{\n  0: foo,\n  1: bar,\n  2: [nested,seq],\n  3: {nested: map}\n}\n",
                            "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\"nested\",\"seq\"],\n  \"3\": {\"nested\": \"map\"}\n}\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 0,
                            "{\n0: foo,\n1: bar,\n2: [nested,seq],\n3: {nested: map}\n}\n",
                            "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\"nested\",\"seq\"],\n\"3\": {\"nested\": \"map\"}\n}\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("flow_mln-flowsl");
            TMPSTY(flowmln, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 18);
            TMPSTY(flowsl, ti, t[3], 29);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "{\n  0: foo,1: bar,2: [nested,seq],3: {nested: map}\n}\n",
                            "{\n  \"0\": \"foo\",\"1\": \"bar\",\"2\": [\"nested\",\"seq\"],\"3\": {\"nested\": \"map\"}\n}\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 0,
                            "{\n0: foo,1: bar,2: [nested,seq],3: {nested: map}\n}\n",
                            "{\n\"0\": \"foo\",\"1\": \"bar\",\"2\": [\"nested\",\"seq\"],\"3\": {\"nested\": \"map\"}\n}\n",
                            noindent);
            }
        }
    }
    {
        SCOPED_TRACE("full-all");
        {
        }
        {
            SCOPED_TRACE("flow_ml");
            TMPSTY(flowml1, ti, t, 2);
            TMPSTY(flowml1, ti, t[2], 18);
            TMPSTY(flowml1, ti, t[3], 29);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "{\n  0: foo,\n  1: bar,\n  2: [\n    nested,\n    seq\n  ],\n  3: {\n    nested: map\n  }\n}\n",
                            "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 0,
                            "{\n0: foo,\n1: bar,\n2: [\nnested,\nseq\n],\n3: {\nnested: map\n}\n}\n",
                            "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\n\"nested\",\n\"seq\"\n],\n\"3\": {\n\"nested\": \"map\"\n}\n}\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 18);
            TMPSTY(flowsl, ti, t[3], 29);
            test_emits_(ti, t, 0,
                        "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}",
                        R"({"0": "foo","1": "bar","2": ["nested","seq"],"3": {"nested": "map"}})");
        }
    }
    {
        SCOPED_TRACE("t[0]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            test_emits_(ti, t[0], 3,
                        "0: foo\n",
                        "\"0\": \"foo\"\n");
        }
        {
            SCOPED_TRACE("flow_ml");
            TMPSTY(flowml1, ti, t, 2);
            test_emits_(ti, t[0], 3,
                        "0: foo\n",
                        "\"0\": \"foo\"\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            test_emits_(ti, t[0], 3,
                        "0: foo\n",
                        "\"0\": \"foo\"\n");
        }
    }
    {
        SCOPED_TRACE("t[1]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            test_emits_(ti, t[1], 9,
                        "1: bar\n",
                        "\"1\": \"bar\"\n");
        }
        {
            SCOPED_TRACE("flow_ml");
            TMPSTY(flowml1, ti, t, 2);
            test_emits_(ti, t[1], 9,
                        "1: bar\n",
                        "\"1\": \"bar\"\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            test_emits_(ti, t[1], 9,
                        "1: bar\n",
                        "\"1\": \"bar\"\n");
        }
    }
    {
        SCOPED_TRACE("t[2]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t[2], 18);
            test_emits_(ti, t[2], 15,
                        "2:\n  - nested\n  - seq\n",
                        "\"2\": [\n  \"nested\",\n  \"seq\"\n]\n");
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t[2], 18);
            test_emits_(ti, t[2], 15,
                        "2: [\n    nested,\n    seq\n  ]\n", // FIXME should be indented one level, not two
                        "\"2\": [\n  \"nested\",\n  \"seq\"\n]\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t[2], 18);
            test_emits_(ti, t[2], 15,
                        "2: [nested,seq]\n",
                        "\"2\": [\"nested\",\"seq\"]"); // FIXME should have trailing newline
        }
    }
    {
        SCOPED_TRACE("t[3]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t[3], 29);
            test_emits_(ti, t[3], 26,
                        "3:\n  nested: map\n",
                        "\"3\": {\n  \"nested\": \"map\"\n}\n");
        }
        {
            SCOPED_TRACE("flow_ml");
            TMPSTY(flowml1, ti, t[3], 29);
            test_emits_(ti, t[3], 26,
                        "3: {\n    nested: map\n  }\n", // FIXME should be indented one level, not two
                        "\"3\": {\n  \"nested\": \"map\"\n}\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t[3], 29);
            test_emits_(ti, t[3], 26,
                        "3: {nested: map}\n",
                        "\"3\": {\"nested\": \"map\"}"); // FIXME should have trailing newline
        }
    }
}


TEST(emit, existing_map_node_flow_sl)
{
    std::string yaml = "{0: foo, 1: bar, 2: [nested, seq], 3: {nested: map}}";
    TreeAndInts ti = parse_tree_and_ints(to_csubstr(yaml));
    Tree & t = ti.tree;
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, t, 0,
                    "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}",
                    R"({"0": "foo","1": "bar","2": ["nested","seq"],"3": {"nested": "map"}})");
    }
    {
        SCOPED_TRACE("here");
        test_emit_map_node(ti, yaml);
    }
}

TEST(emit, existing_map_node_flow_ml)
{
    std::string yaml = "{\n 0: foo, 1: bar, 2: [nested, seq], 3: {nested: map}\n}\n";
    TreeAndInts ti = parse_tree_and_ints(to_csubstr(yaml));
    Tree & t = ti.tree;
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, t, 0,
                    "{\n  0: foo,\n  1: bar,\n  2: [nested,seq],\n  3: {nested: map}\n}\n",
                    "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\"nested\",\"seq\"],\n  \"3\": {\"nested\": \"map\"}\n}\n");
    }
    {
        SCOPED_TRACE("here");
        test_emit_map_node(ti, yaml);
    }
}

TEST(emit, existing_map_node_block)
{
    std::string yaml = "0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n";
    TreeAndInts ti = parse_tree_and_ints(to_csubstr(yaml));
    Tree & t = ti.tree;
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, t, 0,
                    "0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n",
                    "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n");
    }
    {
        SCOPED_TRACE("here");
        test_emit_map_node(ti, yaml);
    }
}


//-----------------------------------------------------------------------------

TEST(emit, percent_is_quoted)
{
    Tree ti = parse_in_arena("{}");
    ASSERT_TRUE(ti.rootref().is_map());
    ti["%ROOT"].set_val("%VAL");
    ti["%ROOT2"].set_seq();
    ti["%ROOT2"][0].set_val("%VAL");
    ti["%ROOT2"][1].set_val("%VAL");
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
    auto check = [](Tree const &t){
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
    };
    {
        Tree ti = parse_in_arena("{at: [], backtick: []}");
        ti["at"][0].save("@test");
        ti["at"][1].set_val("@test2");
        ti["at"][2].save("@");
        ti["at"][3].set_val("@");
        ti["backtick"][0].save("`test");
        ti["backtick"][1].set_val("`test2");
        ti["backtick"][2].save("`");
        ti["backtick"][3].set_val("`");
        std::string yaml = emitrs_yaml<std::string>(ti);
        test_check_emit_check(to_csubstr(yaml), check);
    }
    {
        Tree ti = parse_in_arena("{at: [], backtick: []}");
        ti.set_serialized(ti["at"].append_child().id(),  "@test");
        ti.set_val       (ti["at"].append_child().id(), "@test2");
        ti.set_serialized(ti["at"].append_child().id(),  "@");
        ti.set_val       (ti["at"].append_child().id(), "@");
        ti.set_serialized(ti["backtick"].append_child().id(),  "`test");
        ti.set_val       (ti["backtick"].append_child().id(), "`test2");
        ti.set_serialized(ti["backtick"].append_child().id(),  "`");
        ti.set_val       (ti["backtick"].append_child().id(), "`");
        std::string yaml = emitrs_yaml<std::string>(ti);
        test_check_emit_check(to_csubstr(yaml), check);
    }
}

TEST(emit, at_is_quoted_only_in_the_beggining__issue_320)
{
    Tree ti = parse_in_arena("{at: [], backtick: []}");
    ti["at"].append_child().save("@test");
    ti["at"].append_child().save("t@est");
    ti["at"].append_child().save("test@");
    ti["at"].append_child().set_val("@test2");
    ti["at"].append_child().set_val("t@est2");
    ti["at"].append_child().set_val("test2@");
    ti["backtick"].append_child().save("`test");
    ti["backtick"].append_child().save("t`est");
    ti["backtick"].append_child().save("test`");
    ti["backtick"].append_child().set_val("`test2");
    ti["backtick"].append_child().set_val("t`est2");
    ti["backtick"].append_child().set_val("test2`");
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

// NOLINTEND(*-signed-bitwise)

} // namespace yml
} // namespace c4
