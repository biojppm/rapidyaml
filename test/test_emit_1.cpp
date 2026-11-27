#ifndef RYML_SINGLE_HEADER
#include "c4/yml/emit.hpp"
#include <c4/yml/parse.hpp>
#endif

#include "./test_emits/test_emits_common.hpp"

#include <gtest/gtest.h>

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

// NOLINTBEGIN(*-signed-bitwise)


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

TEST(emit, emitrs_null_tree)
{
    std::string s = {};
    ConstNodeRef node = {};
    EmitOptions opts = {};
    EXPECT_EQ(s, emitrs_yaml<std::string>(node));
    EXPECT_EQ(s, emitrs_yaml<std::string>(node, opts));
    EXPECT_EQ(s, emitrs_json<std::string>(node));
    EXPECT_EQ(s, emitrs_json<std::string>(node, opts));
    EXPECT_EQ(0, emitrs_yaml(node, &s).len);
    EXPECT_EQ(0, emitrs_yaml(node, opts, &s).len);
    EXPECT_EQ(0, emitrs_json(node, &s).len);
    EXPECT_EQ(0, emitrs_json(node, opts, &s).len);
    EXPECT_EQ(nullptr, emitrs_yaml(node, &s).str);
    EXPECT_EQ(nullptr, emitrs_yaml(node, opts, &s).str);
    EXPECT_EQ(nullptr, emitrs_json(node, &s).str);
    EXPECT_EQ(nullptr, emitrs_json(node, opts, &s).str);
}


//-----------------------------------------------------------------------------

TEST(emit_nested, preconditions)
{
    ASSERT_FALSE(without_dash.emit_nonroot_dash());
    ASSERT_TRUE(with_dash.emit_nonroot_dash());
    ASSERT_TRUE(with_key.emit_nonroot_key());
    ASSERT_FALSE(without_key.emit_nonroot_key());
    ASSERT_FALSE(noindent.indent_flow_ml());
    ASSERT_TRUE(noindent.emit_nonroot_key());
    ASSERT_TRUE(with_flow_spc.force_flow_spc());
}

TEST(emit_nested, basic)
{
    TreeAndInts ti(R"(- a
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
    test_emit_yaml_(ti.tree[2], ti.ints, 9, without_dash, "x0: 1\nx1: 2\n");
    test_emit_yaml_(ti.tree[2], ti.ints, 9, with_dash, "- x0: 1\n  x1: 2\n");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree[2], 9);
        test_emit_yaml_(ti.tree[2], ti.ints, 9, without_dash, "x0: 1\nx1: 2\n");
        test_emit_yaml_(ti.tree[2], ti.ints, 9, with_dash, "- x0: 1\n  x1: 2\n");
    }
    {
        SCOPED_TRACE("flowsl");
        TMPSTY(flowsl, ti, ti.tree[2], 9);
        test_emit_yaml_(ti.tree[2], ti.ints, 9, without_dash, "{x0: 1,x1: 2}");
        test_emit_yaml_(ti.tree[2], ti.ints, 9, with_dash, "- {x0: 1,x1: 2}\n");
    }
    test_emit_yaml_(ti.tree[3]["beer"][0], ti.ints, 57, without_dash, "Rochefort 10");
    test_emit_yaml_(ti.tree[3]["beer"][0], ti.ints, 57, with_dash, "- Rochefort 10\n");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree[3]["beer"], 57);
        test_emit_yaml_(ti.tree[3]["beer"][0], ti.ints, 57, without_dash, "Rochefort 10");
        test_emit_yaml_(ti.tree[3]["beer"][0], ti.ints, 57, with_dash, "- Rochefort 10\n");
    }
    test_emit_yaml_(ti.tree[3]["beer"][3], ti.ints, 66, without_dash,
                    "- and so\n"
                    "- many other\n"
                    "- wonderful beers\n");
    test_emit_yaml_(ti.tree[3]["beer"][3], ti.ints, 66, with_dash,
                    "- - and so\n"
                    "  - many other\n"
                    "  - wonderful beers\n");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree[3]["beer"], 66);
        test_emit_yaml_(ti.tree[3]["beer"][3], ti.ints, 66, without_dash,
                        "- and so\n"
                        "- many other\n"
                        "- wonderful beers\n");
        test_emit_yaml_(ti.tree[3]["beer"][3], ti.ints, 66, with_dash,
                        "- - and so\n"
                        "  - many other\n"
                        "  - wonderful beers\n");
    }
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
        test_emit_yaml_tree_(ti.tree[3]["beer"], with_key, keyval);
        test_emit_yaml_ints_(ti.ints, 53,        with_key, keyval);
        test_emit_yaml_ints_(ti.ints, 56,        with_key, val);
        test_emit_yaml_tree_(ti.tree[3]["beer"], without_key, val);
        test_emit_yaml_ints_(ti.ints, 53,        without_key, key);
        test_emit_yaml_ints_(ti.ints, 56,        without_key, val);
        {
            SCOPED_TRACE("nosty");
            TMPSTY(nostykv, ti, ti.tree[3]["beer"], 53, 56);
            test_emit_yaml_tree_(ti.tree[3]["beer"], with_key, keyval);
            test_emit_yaml_ints_(ti.ints, 53,        with_key, keyval);
            test_emit_yaml_ints_(ti.ints, 56,        with_key, val);
            test_emit_yaml_tree_(ti.tree[3]["beer"], without_key, val);
            test_emit_yaml_ints_(ti.ints, 53,        without_key, key);
            test_emit_yaml_ints_(ti.ints, 56,        without_key, val);
        }
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
    TreeAndInts ti(to_csubstr(yaml));
    test_emit_yaml_(ti.tree, ti.ints, 0, with_key,    yaml);//BSTR
    test_emit_yaml_(ti.tree, ti.ints, 0, without_key, yaml);//BSTR
    test_emit_yaml_(ti.tree, ti.ints, 1, with_key,    yaml);//BDOC
    test_emit_yaml_(ti.tree, ti.ints, 1, without_key, yaml);//BDOC
    test_emit_yaml_(ti.tree, ti.ints, 2, with_key,    yaml);//BMAP
    test_emit_yaml_(ti.tree, ti.ints, 2, without_key, yaml);//BMAP
    //
    test_emit_yaml_tree_(ti.tree["key"], with_key,    "key: {c: d}\n");
    test_emit_yaml_ints_(ti.ints, 3,     with_key,    "key: {c: d}\n");
    test_emit_yaml_ints_(ti.ints, 6,     with_key,    "{c: d}");
    test_emit_yaml_tree_(ti.tree["key"], without_key, "{c: d}");
    test_emit_yaml_ints_(ti.ints, 3,     without_key, "key");
    test_emit_yaml_ints_(ti.ints, 6,     without_key, "{c: d}");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nostykv, ti, ti.tree["key"], 3, 6);
        test_emit_yaml_tree_(ti.tree["key"], with_key,    "key:\n  c: d\n");
        test_emit_yaml_ints_(ti.ints, 3,     with_key,    "key:\n  c: d\n");
        test_emit_yaml_ints_(ti.ints, 6,     with_key,    "c: d\n");
        test_emit_yaml_tree_(ti.tree["key"], without_key, "c: d\n");
        test_emit_yaml_ints_(ti.ints, 3,     without_key, "key");
        test_emit_yaml_ints_(ti.ints, 6,     without_key, "c: d\n");
    }
    //
    test_emit_yaml_tree_(ti.tree["key"]["c"], with_key   , "c: d\n");
    test_emit_yaml_ints_(ti.ints, 7,          with_key   , "c: d\n");
    test_emit_yaml_ints_(ti.ints, 10,         with_key   , "d");
    test_emit_yaml_tree_(ti.tree["key"]["c"], without_key, "d");
    test_emit_yaml_ints_(ti.ints, 7,          without_key, "c");
    test_emit_yaml_ints_(ti.ints, 10,         without_key, "d");
    //
    test_emit_yaml_tree_(ti.tree["e"], with_key   , "e: [f]\n");
    test_emit_yaml_ints_(ti.ints, 14,  with_key   , "e: [f]\n");
    test_emit_yaml_ints_(ti.ints, 17,  with_key   , "[f]");
    test_emit_yaml_tree_(ti.tree["e"], without_key, "[f]");
    test_emit_yaml_ints_(ti.ints, 14,  without_key, "e");
    test_emit_yaml_ints_(ti.ints, 17,  without_key, "[f]");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nostykv, ti, ti.tree["e"], 14, 17);
        test_emit_yaml_tree_(ti.tree["e"], with_key   , "e:\n  - f\n");
        test_emit_yaml_ints_(ti.ints, 14,  with_key   , "e:\n  - f\n");
        test_emit_yaml_ints_(ti.ints, 17,  with_key   , "- f\n");
        test_emit_yaml_tree_(ti.tree["e"], without_key, "- f\n");
        test_emit_yaml_ints_(ti.ints, 14,  without_key, "e");
        test_emit_yaml_ints_(ti.ints, 17,  without_key, "- f\n");
    }
    //
    test_emit_yaml_tree_(ti.tree["g"], with_key   , "g: h\n");
    test_emit_yaml_ints_(ti.ints, 22,  with_key   , "g: h\n");
    test_emit_yaml_ints_(ti.ints, 25,  with_key   , "h");
    test_emit_yaml_tree_(ti.tree["g"], without_key, "h");
    test_emit_yaml_ints_(ti.ints, 22,  without_key, "g");
    test_emit_yaml_ints_(ti.ints, 25,  without_key, "h");
    //
    test_emit_yaml_tree_(ti.tree["bmap"], with_key   , "bmap:\n  a: b\n");
    test_emit_yaml_ints_(ti.ints, 28,     with_key   , "bmap:\n  a: b\n");
    test_emit_yaml_ints_(ti.ints, 31,     with_key   , "a: b\n");
    test_emit_yaml_tree_(ti.tree["bmap"], without_key, "a: b\n");
    test_emit_yaml_ints_(ti.ints, 28,     without_key, "bmap");
    test_emit_yaml_ints_(ti.ints, 31,     without_key, "a: b\n");
    //
    test_emit_yaml_tree_(ti.tree["bseq"], with_key   , "bseq:\n  - a\n  - b\n");
    test_emit_yaml_ints_(ti.ints, 39,     with_key   , "bseq:\n  - a\n  - b\n");
    test_emit_yaml_ints_(ti.ints, 42,     with_key   , "- a\n- b\n");
    test_emit_yaml_tree_(ti.tree["bseq"], without_key, "- a\n- b\n");
    test_emit_yaml_ints_(ti.ints, 39,     without_key, "bseq");
    test_emit_yaml_ints_(ti.ints, 42,     without_key, "- a\n- b\n");
}

static void check_container_key_json_errs_(IntBufs & ints)
{
    auto check = [&]{
        RYML_EXPECT_ERROR(check_error_basic([&]{ ints.emitrs_json<std::string>({}, 0); }));
        RYML_EXPECT_ERROR(check_error_basic([&]{ ints.emitrs_json<std::string>({}, 1); }));
        RYML_EXPECT_ERROR(check_error_basic([&]{ ints.emitrs_json<std::string>({}, 2); }));
        RYML_EXPECT_ERROR(check_error_basic([&]{ ints.emitrs_json<std::string>({}, 3); }));
        RYML_EXPECT_ERROR(check_error_basic([&]{ ints.emitrs_json<std::string>(with_key, 3); }));
        RYML_EXPECT_ERROR(check_success(    [&]{ ints.emitrs_json<std::string>(without_key, 3); }));
        RYML_EXPECT_ERROR(check_success(    [&]{ ints.emitrs_json<std::string>({}, 11); }));
    };
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ints, 2);
        check();
    }
    {
        SCOPED_TRACE("block");
        TMPSTY(blk, ints, 2);
        check();
    }
    {
        SCOPED_TRACE("flowsl");
        TMPSTY(flowsl, ints, 2);
        check();
    }
    {
        SCOPED_TRACE("flowml1");
        TMPSTY(flowml1, ints, 2);
        check();
    }
    {
        SCOPED_TRACE("flowmln");
        TMPSTY(flowmln, ints, 2);
        check();
    }
}

TEST(emit_nested, container_key_1)
{
    char src[] = "[a, b]: {c: d}";
    IntBufs ints;
    parse_ints(src, &ints);
    test_emit_yaml_ints_(ints, 0, with_key,    "? [a,b]\n: {c: d}\n");//BSTR
    test_emit_yaml_ints_(ints, 0, without_key, "? [a,b]\n: {c: d}\n");//BSTR
    test_emit_yaml_ints_(ints, 1, with_key,    "? [a,b]\n: {c: d}\n");//BDOC
    test_emit_yaml_ints_(ints, 1, without_key, "? [a,b]\n: {c: d}\n");//BDOC
    test_emit_yaml_ints_(ints, 2, with_key,    "? [a,b]\n: {c: d}\n");//BMAP
    test_emit_yaml_ints_(ints, 2, without_key, "? [a,b]\n: {c: d}\n");//BMAP
    test_emit_yaml_ints_(ints, 3, with_key,    "? [a,b]\n: {c: d}\n");//KEY|BSEQ
    test_emit_yaml_ints_(ints, 3, without_key, "[a,b]");//KEY|BSEQ
    test_emit_yaml_ints_(ints, 4, with_key,    "a");
    test_emit_yaml_ints_(ints, 4, without_key, "a");
    test_emit_yaml_ints_(ints, 7, with_key,    "b");
    test_emit_yaml_ints_(ints, 7, without_key, "b");
    test_emit_yaml_ints_(ints, 11, with_key,    "{c: d}");
    test_emit_yaml_ints_(ints, 11, without_key, "{c: d}");
    test_emit_yaml_ints_(ints, 12, with_key,    "c: d\n");
    test_emit_yaml_ints_(ints, 12, without_key, "c");
    test_emit_yaml_ints_(ints, 15, with_key,    "d");
    test_emit_yaml_ints_(ints, 15, without_key, "d");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ints, 2);
        TMPSTY(nosty, ints, 3);
        TMPSTY(nosty, ints, 11);
        test_emit_yaml_ints_(ints, 0, with_key,    "?\n  - a\n  - b\n:\n  c: d\n");//BSTR
        test_emit_yaml_ints_(ints, 0, without_key, "?\n  - a\n  - b\n:\n  c: d\n");//BSTR
        test_emit_yaml_ints_(ints, 1, with_key,    "?\n  - a\n  - b\n:\n  c: d\n");//BDOC
        test_emit_yaml_ints_(ints, 1, without_key, "?\n  - a\n  - b\n:\n  c: d\n");//BDOC
        test_emit_yaml_ints_(ints, 2, with_key,    "?\n  - a\n  - b\n:\n  c: d\n");//BMAP
        test_emit_yaml_ints_(ints, 2, without_key, "?\n  - a\n  - b\n:\n  c: d\n");//BMAP
        test_emit_yaml_ints_(ints, 3, with_key,    "?\n  - a\n  - b\n:\n  c: d\n");//BMAP
        test_emit_yaml_ints_(ints, 3, without_key, "- a\n- b\n");//KEY|BSEQ
        test_emit_yaml_ints_(ints, 4, with_key,    "a");
        test_emit_yaml_ints_(ints, 4, without_key, "a");
        test_emit_yaml_ints_(ints, 7, with_key,    "b");
        test_emit_yaml_ints_(ints, 7, without_key, "b");
        test_emit_yaml_ints_(ints, 11, with_key,    "c: d\n");
        test_emit_yaml_ints_(ints, 11, without_key, "c: d\n");
        test_emit_yaml_ints_(ints, 12, with_key,    "c: d\n");
        test_emit_yaml_ints_(ints, 12, without_key, "c");
        test_emit_yaml_ints_(ints, 15, with_key,    "d");
        test_emit_yaml_ints_(ints, 15, without_key, "d");
    }
    {
        SCOPED_TRACE("key err");
        check_container_key_json_errs_(ints);
    }
}

TEST(emit_nested, container_key_2)
{
    char src[] = "{a: b}: [c, d]";
    IntBufs ints;
    parse_ints(src, &ints);
    test_emit_yaml_ints_(ints, 0, with_key,    "? {a: b}\n: [c,d]\n");
    test_emit_yaml_ints_(ints, 0, without_key, "? {a: b}\n: [c,d]\n");
    test_emit_yaml_ints_(ints, 1, with_key,    "? {a: b}\n: [c,d]\n");
    test_emit_yaml_ints_(ints, 1, without_key, "? {a: b}\n: [c,d]\n");
    test_emit_yaml_ints_(ints, 2, with_key,    "? {a: b}\n: [c,d]\n");
    test_emit_yaml_ints_(ints, 2, without_key, "? {a: b}\n: [c,d]\n");
    test_emit_yaml_ints_(ints, 3, with_key,    "? {a: b}\n: [c,d]\n");
    test_emit_yaml_ints_(ints, 3, without_key, "{a: b}");
    test_emit_yaml_ints_(ints, 4, with_key,    "a: b\n");
    test_emit_yaml_ints_(ints, 4, without_key, "a");
    test_emit_yaml_ints_(ints, 7, with_key,    "b");
    test_emit_yaml_ints_(ints, 7, without_key, "b");
    test_emit_yaml_ints_(ints, 11, with_key,    "[c,d]");
    test_emit_yaml_ints_(ints, 11, without_key, "[c,d]");
    test_emit_yaml_ints_(ints, 12, with_key,    "c");
    test_emit_yaml_ints_(ints, 12, without_key, "c");
    test_emit_yaml_ints_(ints, 15, with_key,    "d");
    test_emit_yaml_ints_(ints, 15, without_key, "d");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ints, 2);
        TMPSTY(nosty, ints, 3);
        TMPSTY(nosty, ints, 11);
        test_emit_yaml_ints_(ints, 0, with_key,    "?\n  a: b\n:\n  - c\n  - d\n");
        test_emit_yaml_ints_(ints, 0, without_key, "?\n  a: b\n:\n  - c\n  - d\n");
        test_emit_yaml_ints_(ints, 1, with_key,    "?\n  a: b\n:\n  - c\n  - d\n");
        test_emit_yaml_ints_(ints, 1, without_key, "?\n  a: b\n:\n  - c\n  - d\n");
        test_emit_yaml_ints_(ints, 2, with_key,    "?\n  a: b\n:\n  - c\n  - d\n");
        test_emit_yaml_ints_(ints, 2, without_key, "?\n  a: b\n:\n  - c\n  - d\n");
        test_emit_yaml_ints_(ints, 3, with_key,    "?\n  a: b\n:\n  - c\n  - d\n");
        test_emit_yaml_ints_(ints, 3, without_key, "a: b\n");
        test_emit_yaml_ints_(ints, 4, with_key,    "a: b\n");
        test_emit_yaml_ints_(ints, 4, without_key, "a");
        test_emit_yaml_ints_(ints, 7, with_key,    "b");
        test_emit_yaml_ints_(ints, 7, without_key, "b");
        test_emit_yaml_ints_(ints, 11, with_key,    "- c\n- d\n");
        test_emit_yaml_ints_(ints, 11, without_key, "- c\n- d\n");
        test_emit_yaml_ints_(ints, 12, with_key,    "c");
        test_emit_yaml_ints_(ints, 12, without_key, "c");
        test_emit_yaml_ints_(ints, 15, with_key,    "d");
        test_emit_yaml_ints_(ints, 15, without_key, "d");
    }
    {
        SCOPED_TRACE("key err");
        check_container_key_json_errs_(ints);
    }
}


//-----------------------------------------------------------------------------

TEST(emit, null_ints)
{
    IntBufs ints = {};
    ASSERT_EQ(ints.evts.len, 0);
    ASSERT_EQ(ints.evts.ptr, nullptr);
    ASSERT_EQ(ints.src.len, 0);
    ASSERT_EQ(ints.src.str, nullptr);
    ASSERT_EQ(ints.arena.len, 0);
    ASSERT_EQ(ints.arena.str, nullptr);
    std::string ret;
    RYML_EXPECT_ERROR(check_success([&]{ ret = ints.emitrs_yaml<std::string>(); }));
    EXPECT_TRUE(ret.empty());
    RYML_EXPECT_ERROR(check_success([&]{ ret = ints.emitrs_json<std::string>(); }));
}


//-----------------------------------------------------------------------------

TEST(emit_block_seq, ambiguous_plain_emitted_as_squo)
{
    EXPECT_EQ(scalar_style_query_plain_flow(": odd"), false);
    EXPECT_EQ(scalar_style_query_plain_flow(":\todd"), false);
    EXPECT_EQ(scalar_style_choose_flow(": odd"), SCALAR_SQUO);
    EXPECT_EQ(scalar_style_choose_flow(":\todd"), SCALAR_SQUO);
    EXPECT_EQ(xievt::detail::scalar_style_choose_flow_ievt(": odd"), xievt::SQUO);
    EXPECT_EQ(xievt::detail::scalar_style_choose_flow_ievt(":\todd"), xievt::SQUO);
    EXPECT_EQ(xievt::detail::scalar_style_choose_flow_ievt("\n \t"), xievt::DQUO);
    EXPECT_EQ(xievt::detail::scalar_style_choose_flow_ievt(csubstr{}), xievt::PLAI);
    EXPECT_EQ(xievt::detail::scalar_style_choose_flow_ievt(csubstr("foo").first(0)), xievt::SQUO);
    EXPECT_EQ(scalar_style_query_plain_block(": odd"), false);
    EXPECT_EQ(scalar_style_query_plain_block(":\todd"), false);
    EXPECT_EQ(scalar_style_choose_block(": odd"), SCALAR_SQUO);
    EXPECT_EQ(scalar_style_choose_block(":\todd"), SCALAR_SQUO);
    EXPECT_EQ(xievt::detail::scalar_style_choose_block_ievt(": odd"), xievt::SQUO);
    EXPECT_EQ(xievt::detail::scalar_style_choose_block_ievt(":\todd"), xievt::SQUO);
    EXPECT_EQ(xievt::detail::scalar_style_choose_block_ievt(csubstr{}), xievt::PLAI);
    EXPECT_EQ(xievt::detail::scalar_style_choose_block_ievt(csubstr("foo").first(0)), xievt::SQUO);
    {
        SCOPED_TRACE("1");
        Tree t;
        NodeRef r = t.rootref();
        r.set_seq(BLOCK);
        r[0].set_val(": odd", VAL_PLAIN);
        r[1].set_val(":\todd", VAL_PLAIN);
        test_emit_yaml_tree(r, "- : odd\n- :\todd\n");
    }
    {
        Tree t;
        NodeRef r = t.rootref();
        r.set_seq(BLOCK);
        r[0].set_val(": odd");
        r[1].set_val(":\todd");
        EXPECT_FALSE(r[0].is_val_plain());
        EXPECT_FALSE(r[1].is_val_plain());
        test_emit_yaml_tree(r, "- ': odd'\n- ':\todd'\n");
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

TEST(emit, empty_map)
{
    const TreeAndInts ti("{}");
    test_emits_(ti, "{}", "{}");
}

TEST(emit, empty_seq)
{
    const TreeAndInts ti("[]");
    test_emits_(ti, "[]", "[]");
}

TEST(emit, empty_map_ml)
{
    const TreeAndInts ti("{\n\n}");
    test_emits_(ti, "{\n}\n", "{\n}\n");
}

TEST(emit, empty_seq_ml)
{
    const TreeAndInts ti("[\n\n]");
    test_emits_(ti, "[\n]\n", "[\n]\n");
}

TEST(emit, empty_key_squo)
{
    {
        SCOPED_TRACE("one only");
        const TreeAndInts ti(R"(
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
        const TreeAndInts ti(R"(
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
    const TreeAndInts ti(R"(
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
        const TreeAndInts ti(R"(
? ""
: literal
)");
        std::string expected = "\"\": literal\n";
        std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    {
        SCOPED_TRACE("nested");
        const TreeAndInts ti(R"(
level1:
  ? ""
  : literal
)");
        std::string expected = "level1:\n  \"\": literal\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    const TreeAndInts ti(R"(
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
        const TreeAndInts ti(R"(
?
: literal
)");
        std::string expected = ": literal\n";
        std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    {
        SCOPED_TRACE("nested");
        const TreeAndInts ti(R"(
level1:
  ?
  : literal
)");
        std::string expected = "level1:\n  : literal\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    const TreeAndInts ti(R"(
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
        const TreeAndInts ti(R"(
? |-
: literal
)");
        std::string expected = "? |-\n: literal\n";
        std::string expected_json = "{\n  \"\": \"literal\"\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    {
        SCOPED_TRACE("nested1");
        const TreeAndInts ti("level1:\n  ? |-\n  : literal\n");
        std::string expected = "level1:\n  ? |-\n  : literal\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"literal\"\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    const TreeAndInts ti(R"(
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
        const TreeAndInts ti(R"(
? >-
: folded
)");
        std::string expected = "? >-\n: folded\n";
        std::string expected_json = "{\n  \"\": \"folded\"\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    {
        SCOPED_TRACE("nested");
        const TreeAndInts ti(R"(
level1:
  ? >-
  : folded
)");
        std::string expected = "level1:\n  ? >-\n  : folded\n";
        std::string expected_json = "{\n  \"level1\": {\n    \"\": \"folded\"\n  }\n}\n";
        test_emits_(ti, expected, expected_json);
    }
    const TreeAndInts ti(R"(
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


TEST(emit, docval)
{
    SCOPED_TRACE("docval");
    TreeAndInts ti("a");
    ConstNodeRef root = ti.tree;
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree, 2);
        test_emits_(ti, "a\n", "\"a\"");
        test_emits_(ti, root, 0, "a\n", "\"a\"");
        test_emits_(ti, root, 1, "a\n", "\"a\"");
        test_emits_tree_(root, "a\n", "\"a\"");
        test_emits_ints_(ti.ints, 2, "a", "\"a\"");
    }
    {
        SCOPED_TRACE("plain");
        TMPSTY(plai, ti, ti.tree, 2);
        test_emits_(ti, "a\n", "\"a\"");
        test_emits_(ti, root, 0, "a\n", "\"a\"");
        test_emits_(ti, root, 1, "a\n", "\"a\"");
        test_emits_tree_(root, "a\n", "\"a\"");
        test_emits_ints_(ti.ints, 2, "a", "\"a\"");
    }
    {
        SCOPED_TRACE("squo");
        TMPSTY(squo, ti, ti.tree, 2);
        test_emits_(ti, "'a'\n", "\"a\"");
        test_emits_(ti, root, 0, "'a'\n", "\"a\"");
        test_emits_(ti, root, 1, "'a'\n", "\"a\"");
        test_emits_tree_(root, "'a'\n", "\"a\"");
        test_emits_ints_(ti.ints, 2, "'a'", "\"a\"");
    }
    {
        SCOPED_TRACE("dquo");
        TMPSTY(dquo, ti, ti.tree, 2);
        test_emits_(ti, "\"a\"\n", "\"a\"");
        test_emits_(ti, root, 0, "\"a\"\n", "\"a\"");
        test_emits_(ti, root, 1, "\"a\"\n", "\"a\"");
        test_emits_tree_(root, "\"a\"\n", "\"a\"");
        test_emits_ints_(ti.ints, 2, "\"a\"", "\"a\"");
    }
    {
        SCOPED_TRACE("literal");
        TMPSTY(litl, ti, ti.tree, 2);
        test_emits_(ti, "|-\n  a\n", "\"a\"");
        test_emits_(ti, root, 0, "|-\n  a\n", "\"a\"");
        test_emits_(ti, root, 1, "|-\n  a\n", "\"a\"");
        test_emits_tree_(root, "|-\n  a\n", "\"a\"");
        test_emits_ints_(ti.ints, 2, "|-\n  a", "\"a\"");
    }
    {
        SCOPED_TRACE("fold");
        TMPSTY(fold, ti, ti.tree, 2);
        test_emits_(ti, ">-\n  a\n", "\"a\"");
        test_emits_(ti, root, 0, ">-\n  a\n", "\"a\"");
        test_emits_(ti, root, 1, ">-\n  a\n", "\"a\"");
        test_emits_tree_(root, ">-\n  a\n", "\"a\"");
        test_emits_ints_(ti.ints, 2, ">-\n  a", "\"a\"");
    }
}

TEST(emit, docval_empty)
{
    SCOPED_TRACE("docval_empty");
    TreeAndInts ti("|-");
    ConstNodeRef root = ti.tree;
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree, 2);
        test_emits_(ti, "''\n", "\"\"");
        test_emits_(ti, root, 0, "''\n", "\"\"");
        test_emits_(ti, root, 1, "''\n", "\"\"");
        test_emits_tree_(root, "''\n", "\"\"");
        test_emits_ints_(ti.ints, 2, "''", "\"\"");
    }
    {
        SCOPED_TRACE("plain");
        TMPSTY(plai, ti, ti.tree, 2);
        test_emits_(ti, "\n", "\"\"");
        test_emits_(ti, root, 0, "\n", "\"\"");
        test_emits_(ti, root, 1, "\n", "\"\"");
        test_emits_tree_(root, "\n", "\"\"");
        test_emits_ints_(ti.ints, 2, "", "\"\"");
    }
    {
        SCOPED_TRACE("squo");
        TMPSTY(squo, ti, ti.tree, 2);
        test_emits_(ti, "''\n", "\"\"");
        test_emits_(ti, root, 0, "''\n", "\"\"");
        test_emits_(ti, root, 1, "''\n", "\"\"");
        test_emits_tree_(root, "''\n", "\"\"");
        test_emits_ints_(ti.ints, 2, "''", "\"\"");
    }
    {
        SCOPED_TRACE("dquo");
        TMPSTY(dquo, ti, ti.tree, 2);
        test_emits_(ti, "\"\"\n", "\"\"");
        test_emits_(ti, root, 0, "\"\"\n", "\"\"");
        test_emits_(ti, root, 1, "\"\"\n", "\"\"");
        test_emits_tree_(root, "\"\"\n", "\"\"");
        test_emits_ints_(ti.ints, 2, "\"\"", "\"\"");
    }
    {
        SCOPED_TRACE("literal");
        TMPSTY(litl, ti, ti.tree, 2);
        test_emits_(ti, "|-\n", "\"\"");
        test_emits_(ti, root, 0, "|-\n", "\"\"");
        test_emits_(ti, root, 1, "|-\n", "\"\"");
        test_emits_tree_(root, "|-\n", "\"\"");
        test_emits_ints_(ti.ints, 2, "|-", "\"\"");
    }
    {
        SCOPED_TRACE("fold");
        TMPSTY(fold, ti, ti.tree, 2);
        test_emits_(ti, ">-\n", "\"\"");
        test_emits_(ti, root, 0, ">-\n", "\"\"");
        test_emits_(ti, root, 1, ">-\n", "\"\"");
        test_emits_tree_(root, ">-\n", "\"\"");
        test_emits_ints_(ti.ints, 2, ">-", "\"\"");
    }
}


// NOLINTEND(*-signed-bitwise)

} // namespace yml
} // namespace c4
