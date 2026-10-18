#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"
#include "./test_lib/tree_and_ints.hpp"
#ifndef C4_YML_REFERENCE_RESOLVER_HPP_
#include "c4/yml/reference_resolver.hpp"
#endif

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


TEST(anchors, ReferenceResolver)
{
    // verify the resolver receives each tree's callbacks
    Tree tree1;
    ReferenceResolver r;
    r.resolve(&tree1);
    EXPECT_EQ(r.m_anchors.m_arr.m_callbacks, tree1.callbacks());
    EXPECT_EQ(r.m_branch_size.m_callbacks, tree1.callbacks());
    EXPECT_EQ(r.m_known_keys.known_keys.m_arr.m_callbacks, tree1.callbacks());
    EXPECT_EQ(r.m_known_keys.unique_keys.m_arr.m_callbacks, tree1.callbacks());
    RYML_EXPECT_ERROR(check_success([&]{
        Tree tree2;
        ASSERT_NE(tree2.callbacks(), tree1.callbacks());
        r.resolve(&tree2);
        EXPECT_EQ(r.m_anchors.m_arr.m_callbacks, tree1.callbacks());
        EXPECT_EQ(r.m_branch_size.m_callbacks, tree1.callbacks());
        EXPECT_EQ(r.m_known_keys.known_keys.m_arr.m_callbacks, tree1.callbacks());
        EXPECT_EQ(r.m_known_keys.unique_keys.m_arr.m_callbacks, tree1.callbacks());
    }));
}


TEST(anchors, errs_on_missing_anchors)
{
    SCOPED_CALL(test_resolve_err, ("a: *b"));
    SCOPED_CALL(test_resolve, (ResolveOptions{}.ignore_missing_anchors(true),
                               "a: *b\n"));
}

static const ResolveOptions opts_ws = ResolveOptions{}.whole_scope(true);
TEST(anchors, whole_scope_1_seq)
{
    SCOPED_CALL(test_resolve_err, (     "[*a,&a b]"));
    SCOPED_CALL(test_resolve, (opts_ws, "[*a,&a b]", "[b,b]"));
}

TEST(anchors, whole_scope_2_map_1)
{
    SCOPED_CALL(test_resolve_err, (     "{*a : c,&a b: d}"));
    SCOPED_CALL(test_resolve, (opts_ws, "{*a : c,&a b: d}", "{b: c,b: d}"));
}

TEST(anchors, whole_scope_2_map_2)
{
    {
        TreeAndInts ti("{*a : &a b}");
        // there is a difference in behavior between tree and ints due
        // to how the visiting is counted:
        //
        // key and val have different visit id (the position in the array)
        test_resolve_err_ints_(ti.ints);
        // key and val have same visit id (the node id)
        SCOPED_CALL(test_resolve_tree, (ti.tree));
        SCOPED_CALL(test_emit_yaml_tree, (ti.tree, "{b: b}"));
    }
    SCOPED_CALL(test_resolve, (opts_ws, "{*a : &a b}", "{b: b}"));
}


//-----------------------------------------------------------------------------

static const ResolveOptions opts_norep_merg = ResolveOptions{}.err_on_repeated_keys_mergedin(true);
static const ResolveOptions opts_norep_orig = ResolveOptions{}.err_on_repeated_keys_original(true);
static const ResolveOptions opts_norep_any = ResolveOptions{}.err_on_repeated_keys_mergedin(true)
                                                             .err_on_repeated_keys_original(true);
TEST(anchors, err_on_repeated_keys_0)
{
    test_resolve_("{a: 0,b: 1}");
    test_resolve_(opts_norep_orig, "{a: 0,b: 1}");
    test_resolve_(opts_norep_any,  "{a: 0,b: 1}");
    test_resolve_(opts_norep_merg, "{a: 0,b: 1}");
}
TEST(anchors, err_on_repeated_keys_1)
{
    test_resolve_("{a: 0,a: 1}");
    test_resolve_err_(opts_norep_orig, "{a: 0,a: 1}");
    test_resolve_err_(opts_norep_any, "{a: 0,a: 1}");
    test_resolve_(opts_norep_merg, "{a: 0,a: 1}");
}
TEST(anchors, err_on_repeated_keys_2)
{
    test_resolve_(                 "{_: &m {a: 1,b: 1},orig: {c: 2,<<: *m}}", "{_: {a: 1,b: 1},orig: {c: 2,a: 1,b: 1}}");
    test_resolve_(opts_norep_orig, "{_: &m {a: 1,b: 1},orig: {c: 2,<<: *m}}", "{_: {a: 1,b: 1},orig: {c: 2,a: 1,b: 1}}");
    test_resolve_(opts_norep_merg, "{_: &m {a: 1,b: 1},orig: {c: 2,<<: *m}}", "{_: {a: 1,b: 1},orig: {c: 2,a: 1,b: 1}}");
    test_resolve_(opts_norep_any,  "{_: &m {a: 1,b: 1},orig: {c: 2,<<: *m}}", "{_: {a: 1,b: 1},orig: {c: 2,a: 1,b: 1}}");
}
TEST(anchors, err_on_repeated_keys_3)
{
    test_resolve_(                     "{_: &m {a: 1,b: 1},orig: {a: 2,<<: *m}}", "{_: {a: 1,b: 1},orig: {a: 2,b: 1}}");
    test_resolve_(opts_norep_orig,     "{_: &m {a: 1,b: 1},orig: {a: 2,<<: *m}}", "{_: {a: 1,b: 1},orig: {a: 2,b: 1}}");
    test_resolve_err_(opts_norep_merg, "{_: &m {a: 1,b: 1},orig: {a: 2,<<: *m}}");
    test_resolve_err_(opts_norep_any,  "{_: &m {a: 1,b: 1},orig: {a: 2,<<: *m}}");
}
TEST(anchors, err_on_repeated_keys_4)
{
    test_resolve_(                     "{_: &m {a: 1,a: 2},orig: {a: 3,<<: *m}}", "{_: {a: 1,a: 2},orig: {a: 3}}");
    test_resolve_err_(opts_norep_orig, "{_: &m {a: 1,a: 2},orig: {a: 3,<<: *m}}");
    test_resolve_err_(opts_norep_merg, "{_: &m {a: 1,a: 2},orig: {a: 3,<<: *m}}");
    test_resolve_err_(opts_norep_any,  "{_: &m {a: 1,a: 2},orig: {a: 3,<<: *m}}");
}
TEST(anchors, err_on_repeated_keys_5)
{
    test_resolve_(                     "{_: &m {a: 1},orig: {a: 2,a: 3,<<: *m}}", "{_: {a: 1},orig: {a: 2}}");
    test_resolve_err_(opts_norep_orig, "{_: &m {a: 1},orig: {a: 2,a: 3,<<: *m}}");
    test_resolve_err_(opts_norep_merg, "{_: &m {a: 1},orig: {a: 2,a: 3,<<: *m}}");
    test_resolve_err_(opts_norep_any,  "{_: &m {a: 1},orig: {a: 2,a: 3,<<: *m}}");
}


//-----------------------------------------------------------------------------

static const ResolveOptions opts_multianch = ResolveOptions{}.accept_multiple_anchors(true);
static const ResolveOptions opts_multianch_ws = ResolveOptions{}.accept_multiple_anchors(true).whole_scope(true);
TEST(anchors, err_on_repeated_anchors)
{
    test_resolve_err_(               "[&a 0,*a,&a 1,*a,&a 2,*a,*a,*a,&a 3]");
    test_resolve_(opts_multianch,    "[&a 0,*a,&a 1,*a,&a 2,*a,*a,*a,&a 3]", "[0,0,1,1,2,2,2,2,3]");
    test_resolve_(opts_multianch_ws, "[&a 0,*a,&a 1,*a,&a 2,*a,*a,*a,&a 3,*b,&b 4,&b 5]", "[0,0,1,1,2,2,2,2,3,4,4,5]");
}


//-----------------------------------------------------------------------------

TEST(anchors, errs_on_inheritance_0_scalar)
{
    SCOPED_CALL(test_resolve, ("<<: \n"));
    SCOPED_CALL(test_resolve, ("<<: scalar notref\n"));
}
TEST(anchors, errs_on_inheritance_1_map)
{
    SCOPED_CALL(test_resolve, ("<<: {}\n"));
    SCOPED_CALL(test_resolve, ("<<: {map: notref}\n"));
}
TEST(anchors, errs_on_inheritance_2_seq)
{
    SCOPED_CALL(test_resolve, ("<<: []\n"));
    SCOPED_CALL(test_resolve, ("<<: [seq,notref]\n"));
}
TEST(anchors, errs_on_inheritance_3_ok)
{
    SCOPED_CALL(test_resolve, (
        "scalar: &scalar a\n"
        "seq: &seq [b]\n"
        "map1: &map1\n"
        "  c: d\n"
        "map2: &map2\n"
        "  e: f\n"
        "inherit:\n"
        "  <<: [*map1,*map2]\n"
        ""
        ,
        ""
        "scalar: a\n"
        "seq: [b]\n"
        "map1:\n"
        "  c: d\n"
        "map2:\n"
        "  e: f\n"
        "inherit:\n"
        "  c: d\n"
        "  e: f\n"
        ""));
}
TEST(anchors, errs_on_inheritance_3_ignore_missing_0)
{
    SCOPED_CALL(test_resolve, (ResolveOptions{}.ignore_missing_anchors(true),
        "scalar: &scalar a\n"
        "seq: &seq [b]\n"
        "map1: &map1\n"
        "  c: d\n"
        "map2: &map2\n"
        "  e: f\n"
        "inherit:\n"
        "  <<: [*map1,*map2]\n"
        ""
        ,
        ""
        "scalar: a\n"
        "seq: [b]\n"
        "map1:\n"
        "  c: d\n"
        "map2:\n"
        "  e: f\n"
        "inherit:\n"
        "  c: d\n"
        "  e: f\n"
        ""));
}
TEST(anchors, errs_on_inheritance_3_ignore_missing_1)
{
    SCOPED_CALL(test_resolve, (ResolveOptions{}.ignore_missing_anchors(true),
        "scalar: a\n"
        "seq: [b]\n"
        "map1:\n  c: d\n"
        "map2:\n  e: f\n"
        "inherit:\n  <<: *map1\n"
        ""
        ,
        ""
        "scalar: a\n"
        "seq: [b]\n"
        "map1:\n  c: d\n"
        "map2:\n  e: f\n"
        "inherit:\n  <<: *map1\n"
        ""));
}
TEST(anchors, errs_on_inheritance_3_ignore_missing_2)
{
    SCOPED_CALL(test_resolve, (ResolveOptions{}.ignore_missing_anchors(true),
        "scalar: a\n"
        "seq: [b]\n"
        "map1:\n  c: d\n"
        "map2:\n  e: f\n"
        "inherit:\n  <<: [*map1,*map2]\n"
        ""
        ,
        ""
        "scalar: a\n"
        "seq: [b]\n"
        "map1:\n  c: d\n"
        "map2:\n  e: f\n"
        "inherit:\n  <<: [*map1,*map2]\n"
        ""));
}
TEST(anchors, errs_on_inheritance_4_err_on_map_then_scalar)
{
    SCOPED_CALL(test_resolve_err, (
        ""
        "scalar: &scalar a\n"
        "seq: &seq [b]\n"
        "map1: &map1\n"
        "  c: d\n"
        "map2: &map2\n"
        "  d: e\n"
        "inherit:\n"
        "  <<: [*map1,*scalar]\n"
        ""));
}
TEST(anchors, errs_on_inheritance_4_err_on_map_then_seq)
{
    SCOPED_CALL(test_resolve_err, (
        ""
        "scalar: &scalar a\n"
        "seq: &seq [b]\n"
        "map1: &map1\n"
        "  c: d\n"
        "map2: &map2\n"
        "  d: e\n"
        "inherit:\n"
        "  <<: [*map1,*seq]\n"
        ""));
}
TEST(anchors, errs_on_inheritance_4_err_on_map_then_notref)
{
    SCOPED_CALL(test_resolve_err, (
        ""
        "scalar: &scalar a\n"
        "seq: &seq [b]\n"
        "map1: &map1\n"
        "  c: d\n"
        "map2: &map2\n"
        "  d: e\n"
        "inherit:\n"
        "  <<: [*map1,notref]\n"
        ""));
}
TEST(anchors, errs_on_inheritance_5_err_on_ref_then_scalar)
{
    SCOPED_CALL(test_resolve_err, (
        "scalar: &scalar a\n"
        "seq: &seq [b]\n"
        "map1: &map1\n"
        "  c: d\n"
        "map2: &map2\n"
        "  c: d\n"
        "inherit:\n"
        "  <<: [*map1,0]\n"
        ""));
    SCOPED_CALL(test_resolve_err, (
        ""
        "scalar: &scalar a\n"
        "seq: &seq [b]\n"
        "map1: &map1\n"
        "  c: d\n"
        "map2: &map2\n"
        "  c: d\n"
        "inherit:\n"
        "  <<: [*map1,*map2,0]\n"
        ""));
}
TEST(anchors, errs_on_inheritance_5_err_on_ref_then_seq)
{
    SCOPED_CALL(test_resolve_err, (
                    "scalar: &scalar a\n"
                    "seq: &seq [b]\n"
                    "map1: &map1\n"
                    "  c: d\n"
                    "map2: &map2\n"
                    "  c: d\n"
                    "inherit:\n"
                    "  <<: [*map1,[seq]]\n"
                    ""));
    SCOPED_CALL(test_resolve_err, (
                    "scalar: &scalar a\n"
                    "seq: &seq [b]\n"
                    "map1: &map1\n"
                    "  c: d\n"
                    "map2: &map2\n"
                    "  c: d\n"
                    "inherit:\n"
                    "  <<: [*map1,*map2,[seq]]\n"
                    ""));
}
TEST(anchors, errs_on_inheritance_5_err_on_ref_then_map)
{
    SCOPED_CALL(test_resolve_err, (
                    "scalar: &scalar a\n"
                    "seq: &seq [b]\n"
                    "map1: &map1\n"
                    "  c: d\n"
                    "map2: &map2\n"
                    "  c: d\n"
                    "inherit:\n"
                    "  <<: [*map1,{map:}]\n"
                    ""));
    SCOPED_CALL(test_resolve_err, (
                    "scalar: &scalar a\n"
                    "seq: &seq [b]\n"
                    "map1: &map1\n"
                    "  c: d\n"
                    "map2: &map2\n"
                    "  c: d\n"
                    "inherit:\n"
                    "  <<: [*map1,*map2,{map:}]\n"
                    ""));
}


//-----------------------------------------------------------------------------

TEST(anchors, cyclic_roundtrips_ok)
{
    std::string yaml = R"(&x
- *x
)";
    TreeAndInts ti(to_csubstr(yaml));
    ASSERT_TRUE(ti.tree.rootref().has_val_anchor());
    ASSERT_TRUE(ti.tree[0].is_val_ref());
    EXPECT_EQ(ti.tree.rootref().val_anchor(), "x");
    EXPECT_EQ(ti.tree[0].val_ref(), "x");
    SCOPED_CALL(test_emit_resolve_err, (yaml, "[\n  \"*x\"\n]\n"));
}



TEST(anchors, err_on_cyclic_reference_1)
{
    SCOPED_CALL(test_emit_resolve_err, (
        "&x\n"
        "- *x\n"
        ,
        "[\n"
        "  \"*x\"\n"
        "]\n"
        ""));
}

TEST(anchors, err_on_cyclic_reference_2)
{
    SCOPED_CALL(test_emit_resolve_err, ("&x {a: *x}", "{\"a\": \"*x\"}"));
}

TEST(anchors, err_on_cyclic_reference_2_1)
{
    // this is legal (and not cyclic):
    SCOPED_CALL(test_emit_resolve, (
                    "{&x a: *x}",
                    "{a: a}",
                    "{\"a\": \"a\"}"
                    ""));
}

TEST(anchors, err_on_cyclic_reference_3)
{
    SCOPED_CALL(test_emit_resolve_err, (
        ""
        "&x\n"
        "- - - - a\n"
        "      - *x\n"
        ,
        ""
        "[\n"
        "  [\n"
        "    [\n"
        "      [\n"
        "        \"a\",\n"
        "        \"*x\"\n"
        "      ]\n"
        "    ]\n"
        "  ]\n"
        "]\n"
        ""));
}

TEST(anchors, err_on_cyclic_reference_4)
{
    SCOPED_CALL(test_emit_resolve_err, (
        ""
        "&x\n"
        "a:\n"
        "  <<: *x\n"
        ,
        "{\n"
        "  \"a\": {\n"
        "    \"<<\": \"*x\"\n"
        "  }\n"
        "}\n"
        ""));
}

TEST(anchors, err_on_cyclic_reference_5)
{
    SCOPED_CALL(test_emit_resolve_err, (
        ""
        "&x\n"
        "a:\n"
        "  <<: [*x,*x,*x]\n"
        ,
        "{\n"
        "  \"a\": {\n"
        "    \"<<\": [\"*x\",\"*x\",\"*x\"]\n"
        "  }\n"
        "}\n"
        ""));
}

TEST(anchors, err_on_cyclic_reference_6)
{
    SCOPED_CALL(test_emit_resolve_err, (
        ""
        "map0: &map0\n"
        "  k0: v0\n"
        "map1: &map1\n"
        "  k1: v1\n"
        "  <<: [*map0,*map1]\n"
        ,
        ""
        "{\n"
        "  \"map0\": {\n"
        "    \"k0\": \"v0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"k1\": \"v1\",\n"
        "    \"<<\": [\"*map0\",\"*map1\"]\n"
        "  }\n"
        "}\n"
        ""));
}

TEST(anchors, err_on_keyref_to_seq)
{
    TreeAndInts ti("a: &a [0, 1]\n"
                   "*a : b\n");
    SCOPED_CALL(test_resolve_err_tree, (ti.tree));
    SCOPED_CALL(test_resolve_ints, (ti.ints));
    SCOPED_CALL(test_emit_yaml_ints, (ti.ints, 0, "a: [0,1]\n? [0,1]\n: b\n"));
}

TEST(anchors, err_on_keyref_to_map)
{
    TreeAndInts ti("a: &a {0: 1}\n"
                   "*a : b\n");
    SCOPED_CALL(test_resolve_err_tree, (ti.tree));
    SCOPED_CALL(test_resolve_ints, (ti.ints));
    SCOPED_CALL(test_emit_yaml_ints, (ti.ints, 0, "a: {0: 1}\n? {0: 1}\n: b\n"));
}


//-----------------------------------------------------------------------------

csubstr billion_laughs = R"(
l0: &l0 [x,x]     # 3
l1: &l1 [*l0,*l0] # 7
l2: &l2 [*l1,*l1] # 15
)";
TEST(anchors, billion_laughs_small_limit)
{
    SCOPED_TRACE("small limit should cause an error");
    TreeAndInts ti(billion_laughs);
    ResolveOptions opts{};
    uint32_t mult = 2;
    EXPECT_NE(opts.max_size_multiplier(), mult);
    opts.max_size_multiplier(mult);
    EXPECT_EQ(opts.max_size_multiplier(), mult);
    SCOPED_CALL(test_resolve_err, (ti, opts));
}
TEST(anchors, billion_laughs_large_limit)
{
    SCOPED_TRACE("large limit should accept resolve");
    TreeAndInts ti(billion_laughs);
    ResolveOptions opts{};
    uint32_t mult = 10000;
    EXPECT_NE(opts.max_size_multiplier(), mult);
    opts.max_size_multiplier(mult);
    EXPECT_EQ(opts.max_size_multiplier(), mult);
    SCOPED_CALL(test_resolve, (ti, opts));
}
TEST(anchors, billion_laughs_no_limit)
{
    SCOPED_TRACE("no limit should accept resolve");
    TreeAndInts ti(billion_laughs);
    ResolveOptions opts{};
    uint32_t mult = 0;
    EXPECT_NE(opts.max_size_multiplier(), mult);
    EXPECT_NE(opts.max_size(ti.tree.size()), 0);
    opts.max_size_multiplier(mult);
    EXPECT_EQ(opts.max_size_multiplier(), mult);
    EXPECT_EQ(opts.max_size(ti.tree.size()), 0);
    SCOPED_CALL(test_resolve, (ti, opts));
}


// https://en.wikipedia.org/wiki/Billion_laughs_attack
static void test_billion_laughs_roundtrip(
    std::string const& yaml,
    std::string const& roundtrip,
    std::string const& roundtrip_no_clear_anchors)
{
    {
        TreeAndInts ti(to_csubstr(yaml));
        {
            SCOPED_TRACE("no resolve");
            SCOPED_CALL(test_emit_yaml, (ti, yaml));
        }
        {
            SCOPED_TRACE("resolve clear anchors (defaults)");
            ResolveOptions opts = {};
            ASSERT_TRUE(opts.clear_anchors());
            SCOPED_CALL(test_resolve, (ti, opts));
            SCOPED_CALL(test_emit_yaml, (ti, roundtrip));
        }
    }
    {
        SCOPED_TRACE("resolve keep anchors");
        ResolveOptions opts = ResolveOptions{}.clear_anchors(false);
        ASSERT_FALSE(opts.clear_anchors());
        TreeAndInts ti(to_csubstr(yaml));
        SCOPED_CALL(test_resolve, (ti, opts));
        test_resolve(ti, opts);
        SCOPED_CALL(test_emit_yaml, (ti, roundtrip_no_clear_anchors));
    }
}

TEST(anchors, billion_laughs_roundtrip_seq_flow_sl)
{
    SCOPED_CALL(test_billion_laughs_roundtrip, (
        "a: &a [0,0]\n"
        "b: &b [*a,*a]\n"
        "c: &c [*b,*b]\n"
        "",
        "a: [0,0]\n"
        "b: [[0,0],[0,0]]\n"
        "c: [[[0,0],[0,0]],[[0,0],[0,0]]]\n"
        "",
        "a: &a [0,0]\n"
        "b: &b [[0,0],[0,0]]\n"
        "c: &c [[[0,0],[0,0]],[[0,0],[0,0]]]\n"
        ""));
}

TEST(anchors, billion_laughs_roundtrip_map_flow_sl)
{
    SCOPED_CALL(test_billion_laughs_roundtrip, (
        "a: &a {0: 0}\n"
        "b: &b {1: *a,2: *a}\n"
        "c: &c {1: *b,2: *b}\n"
        "",
        "a: {0: 0}\n"
        "b: {1: {0: 0},2: {0: 0}}\n"
        "c: {1: {1: {0: 0},2: {0: 0}},2: {1: {0: 0},2: {0: 0}}}\n"
        "",
        "a: &a {0: 0}\n"
        "b: &b {1: {0: 0},2: {0: 0}}\n"
        "c: &c {1: {1: {0: 0},2: {0: 0}},2: {1: {0: 0},2: {0: 0}}}\n"
        ""));
}

TEST(anchors, billion_laughs_roundtrip_seq_flow_ml)
{
    SCOPED_CALL(test_billion_laughs_roundtrip, (
        "a: &a [\n"
        "    0,\n"
        "    0\n"
        "  ]\n"
        "b: &b [\n"
        "    *a,\n"
        "    *a\n"
        "  ]\n"
        "c: &c [\n"
        "    *b,\n"
        "    *b\n"
        "  ]\n"
        "",
        "a: [\n"
        "    0,\n"
        "    0\n"
        "  ]\n"
        "b: [\n"
        "    [\n"
        "      0,\n"
        "      0\n"
        "    ],\n"
        "    [\n"
        "      0,\n"
        "      0\n"
        "    ]\n"
        "  ]\n"
        "c: [\n"
        "    [\n"
        "      [\n"
        "        0,\n"
        "        0\n"
        "      ],\n"
        "      [\n"
        "        0,\n"
        "        0\n"
        "      ]\n"
        "    ],\n"
        "    [\n"
        "      [\n"
        "        0,\n"
        "        0\n"
        "      ],\n"
        "      [\n"
        "        0,\n"
        "        0\n"
        "      ]\n"
        "    ]\n"
        "  ]\n"
        "",
        "a: &a [\n"
        "    0,\n"
        "    0\n"
        "  ]\n"
        "b: &b [\n"
        "    [\n"
        "      0,\n"
        "      0\n"
        "    ],\n"
        "    [\n"
        "      0,\n"
        "      0\n"
        "    ]\n"
        "  ]\n"
        "c: &c [\n"
        "    [\n"
        "      [\n"
        "        0,\n"
        "        0\n"
        "      ],\n"
        "      [\n"
        "        0,\n"
        "        0\n"
        "      ]\n"
        "    ],\n"
        "    [\n"
        "      [\n"
        "        0,\n"
        "        0\n"
        "      ],\n"
        "      [\n"
        "        0,\n"
        "        0\n"
        "      ]\n"
        "    ]\n"
        "  ]\n"
        ""));
}

TEST(anchors, billion_laughs_roundtrip_map_flow_ml)
{
    SCOPED_CALL(test_billion_laughs_roundtrip, (
        "a: &a {\n"
        "    0: 0\n"
        "  }\n"
        "b: &b {\n"
        "    1: *a,\n"
        "    2: *a\n"
        "  }\n"
        "c: &c {\n"
        "    1: *b,\n"
        "    2: *b\n"
        "  }\n"
        "",
        "a: {\n"
        "    0: 0\n"
        "  }\n"
        "b: {\n"
        "    1: {\n"
        "      0: 0\n"
        "    },\n"
        "    2: {\n"
        "      0: 0\n"
        "    }\n"
        "  }\n"
        "c: {\n"
        "    1: {\n"
        "      1: {\n"
        "        0: 0\n"
        "      },\n"
        "      2: {\n"
        "        0: 0\n"
        "      }\n"
        "    },\n"
        "    2: {\n"
        "      1: {\n"
        "        0: 0\n"
        "      },\n"
        "      2: {\n"
        "        0: 0\n"
        "      }\n"
        "    }\n"
        "  }\n"
        "",
        "a: &a {\n"
        "    0: 0\n"
        "  }\n"
        "b: &b {\n"
        "    1: {\n"
        "      0: 0\n"
        "    },\n"
        "    2: {\n"
        "      0: 0\n"
        "    }\n"
        "  }\n"
        "c: &c {\n"
        "    1: {\n"
        "      1: {\n"
        "        0: 0\n"
        "      },\n"
        "      2: {\n"
        "        0: 0\n"
        "      }\n"
        "    },\n"
        "    2: {\n"
        "      1: {\n"
        "        0: 0\n"
        "      },\n"
        "      2: {\n"
        "        0: 0\n"
        "      }\n"
        "    }\n"
        "  }\n"
        ""));
}

TEST(anchors, billion_laughs_roundtrip_seq_block)
{
    SCOPED_CALL(test_billion_laughs_roundtrip, (
        "a: &a\n"
        "  - 0\n"
        "  - 0\n"
        "b: &b\n"
        "  - *a\n"
        "  - *a\n"
        "c: &c\n"
        "  - *b\n"
        "  - *b\n"
        "",
        "a:\n"
        "  - 0\n"
        "  - 0\n"
        "b:\n"
        "  - - 0\n"
        "    - 0\n"
        "  - - 0\n"
        "    - 0\n"
        "c:\n"
        "  - - - 0\n"
        "      - 0\n"
        "    - - 0\n"
        "      - 0\n"
        "  - - - 0\n"
        "      - 0\n"
        "    - - 0\n"
        "      - 0\n"
        "",
        "a: &a\n"
        "  - 0\n"
        "  - 0\n"
        "b: &b\n"
        "  - - 0\n"
        "    - 0\n"
        "  - - 0\n"
        "    - 0\n"
        "c: &c\n"
        "  - - - 0\n"
        "      - 0\n"
        "    - - 0\n"
        "      - 0\n"
        "  - - - 0\n"
        "      - 0\n"
        "    - - 0\n"
        "      - 0\n"
        ""));
}

TEST(anchors, billion_laughs_roundtrip_map_block)
{
    SCOPED_CALL(test_billion_laughs_roundtrip, (
        "a: &a\n"
        "  0: 0\n"
        "b: &b\n"
        "  1: *a\n"
        "  2: *a\n"
        "c: &c\n"
        "  1: *b\n"
        "  2: *b\n"
        "",
        "a:\n"
        "  0: 0\n"
        "b:\n"
        "  1:\n"
        "    0: 0\n"
        "  2:\n"
        "    0: 0\n"
        "c:\n"
        "  1:\n"
        "    1:\n"
        "      0: 0\n"
        "    2:\n"
        "      0: 0\n"
        "  2:\n"
        "    1:\n"
        "      0: 0\n"
        "    2:\n"
        "      0: 0\n"
        "",
        "a: &a\n"
        "  0: 0\n"
        "b: &b\n"
        "  1:\n"
        "    0: 0\n"
        "  2:\n"
        "    0: 0\n"
        "c: &c\n"
        "  1:\n"
        "    1:\n"
        "      0: 0\n"
        "    2:\n"
        "      0: 0\n"
        "  2:\n"
        "    1:\n"
        "      0: 0\n"
        "    2:\n"
        "      0: 0\n"
        ""));
}


//-----------------------------------------------------------------------------

TEST(anchors, merge0)
{
    SCOPED_CALL(test_resolve, (
        "- &LEFT {x: 0,y: 2}\n- &BIG {r: 10}\n- &SMALL {r: 1}\n"
        "- m0:\n    <<: [*BIG,*LEFT,*SMALL]\n    x: 1\n"
        "- m1:\n    <<: [*BIG,*SMALL,*LEFT]\n    x: 1\n"
        "- m2:\n    <<: [*LEFT,*BIG,*SMALL]\n    x: 1\n"
        "- m3:\n    x: 1\n    <<: [*BIG,*LEFT,*SMALL]\n"
        "- m4:\n    x: 1\n    <<: [*LEFT,*BIG,*SMALL]\n"
        "- m5:\n    x: 1\n    <<: [*LEFT,*BIG,*SMALL]\n"
        ,
        "- {x: 0,y: 2}\n- {r: 10}\n- {r: 1}\n"
        "- m0:\n    r: 10\n    x: 1\n    y: 2\n"
        "- m1:\n    r: 10\n    x: 1\n    y: 2\n"
        "- m2:\n    x: 1\n    y: 2\n    r: 10\n"
        "- m3:\n    x: 1\n    r: 10\n    y: 2\n"
        "- m4:\n    x: 1\n    y: 2\n    r: 10\n"
        "- m5:\n    x: 1\n    y: 2\n    r: 10\n"
        ""));
}

TEST(anchors, merge1_conflict_in_one_only)
{
    SCOPED_CALL(test_resolve, (
        "- &conflict {a: 1,c: 3}\n- &noproblem {d: 4}\n"
        "- {a: 10,b: 20,<<: [*conflict,*noproblem]}\n"
        ,
        "- {a: 1,c: 3}\n- {d: 4}\n"
        "- {a: 10,b: 20,c: 3,d: 4}\n"
        ""));
}

TEST(anchors, exit_early1)
{
    SCOPED_CALL(test_resolve, (ResolveOptions{}.keep_repeated_keys(true),
        "no: anchors\n"
        "no: refs\n"
        ,
        "no: anchors\n"
        "no: refs\n"
        ""));
}

TEST(anchors, exit_early2)
{
    SCOPED_CALL(test_resolve, (ResolveOptions{}.merge_always(true),
        "no: anchors\n"
        "no: refs\n"
        ,
        "no: anchors\n"
        ""));
}

TEST(anchors, alia_at_doc_level)
{
    SCOPED_CALL(test_resolve, (
        "--- &a a\n"
        "--- *a\n"
        ,
        "--- a\n"
        "--- a\n"
        ""));
}

TEST(anchors, alia_with_keep_repeated)
{
    SCOPED_CALL(test_resolve, (ResolveOptions{}.keep_repeated_keys(true),
        "key: &val val\n"
        "key: *val\n"
        "*val : key\n"
        ,
        "key: val\n"
        "key: val\n"
        "val: key\n"
        ""));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(anchors, node_scalar_set_ref_when_empty)
{
    {
        NodeScalar ns;
        ns.set_ref_maybe_replacing_scalar("foo", /*has_scalar*/false);
        EXPECT_EQ(ns.scalar, "foo");
        EXPECT_EQ(ns.anchor, "foo");
    }
    {
        NodeScalar ns;
        ns.set_ref_maybe_replacing_scalar("*foo", /*has_scalar*/false);
        EXPECT_EQ(ns.scalar, "*foo");
        EXPECT_EQ(ns.anchor, "foo");
    }
}

TEST(anchors, node_scalar_set_ref_when_non_empty)
{
    {
        NodeScalar ns;
        ns.scalar = "whatever";
        ns.set_ref_maybe_replacing_scalar("foo", /*has_scalar*/true);
        EXPECT_EQ(ns.scalar, "foo");
        EXPECT_EQ(ns.anchor, "foo");
    }
    {
        NodeScalar ns;
        ns.scalar = "whatever";
        ns.set_ref_maybe_replacing_scalar("*foo", /*has_scalar*/true);
        EXPECT_EQ(ns.scalar, "*foo");
        EXPECT_EQ(ns.anchor, "foo");
        ns.set_ref_maybe_replacing_scalar("foo", /*has_scalar*/true);
        EXPECT_EQ(ns.scalar, "*foo"); // keep the previous as it is well formed
        EXPECT_EQ(ns.anchor, "foo");
        ns.set_ref_maybe_replacing_scalar("bar", /*has_scalar*/true);
        EXPECT_EQ(ns.scalar, "bar"); // replace previous as it is not well formed
        EXPECT_EQ(ns.anchor, "bar");
    }
}

TEST(anchors, no_ambiguity_when_key_scalars_begin_with_star)
{
    TreeAndInts ti("{foo: &foo: 1, *foo: : 2, '*foo:': 3}");
    Tree &t = ti.tree;
    EXPECT_TRUE(t[1].is_key_ref());
    EXPECT_FALSE(t[2].is_key_ref());
    EXPECT_FALSE(t[1].is_key_quoted());
    EXPECT_TRUE(t[2].is_key_quoted());
    EXPECT_EQ(t[1].key(), "*foo:");
    EXPECT_EQ(t[1].key_ref(), "foo:");
    EXPECT_EQ(t[2].key(), "*foo:");
    SCOPED_CALL(test_emit_yaml, (ti, R"({foo: &foo: 1,*foo: : 2,'*foo:': 3})"));
    SCOPED_CALL(test_resolve, (ti));
    SCOPED_CALL(test_emit_yaml, (ti, R"({foo: 1,1: 2,'*foo:': 3})"));
}

TEST(anchors, no_ambiguity_when_val_scalars_begin_with_star)
{
    TreeAndInts ti("{foo: &foo: 1, ref: *foo:, quo: '*foo:'}");
    Tree &t = ti.tree;
    EXPECT_TRUE(t["ref"].is_val_ref());
    EXPECT_FALSE(t["quo"].is_val_ref());
    EXPECT_FALSE(t["ref"].is_val_quoted());
    EXPECT_TRUE(t["quo"].is_val_quoted());
    EXPECT_EQ(t["ref"].val_ref(), "foo:");
    EXPECT_EQ(t["ref"].val(), "*foo:");
    EXPECT_EQ(t["quo"].val(), "*foo:");
    SCOPED_CALL(test_emit_yaml, (ti, R"({foo: &foo: 1,ref: *foo:,quo: '*foo:'})"));
    SCOPED_CALL(test_resolve, (ti));
    SCOPED_CALL(test_emit_yaml, (ti, R"({foo: 1,ref: 1,quo: '*foo:'})"));
}

TEST(anchors, no_ambiguity_with_inheritance)
{
    TreeAndInts ti(R"({
foo: &foo {a: 1, b: 2},
bar: {<<: *foo},
 sq: {'<<': haha},
 dq: {"<<": hehe}
})");
    Tree const& t = ti.tree;
    _c4dbg_tree("parsed tree", t);
    EXPECT_TRUE(t["bar"].has_child("<<"));
    EXPECT_FALSE(t["bar"]["<<"].is_key_ref());
    EXPECT_TRUE(t["bar"]["<<"].is_val_ref());
    EXPECT_TRUE(t["sq"]["<<"].is_key_quoted());
    EXPECT_TRUE(t["dq"]["<<"].is_key_quoted());
    EXPECT_FALSE(t["sq"]["<<"].is_key_ref());
    EXPECT_FALSE(t["dq"]["<<"].is_key_ref());
    EXPECT_EQ(t["sq"]["<<"].key(), "<<");
    EXPECT_EQ(t["dq"]["<<"].key(), "<<");
    EXPECT_EQ(t["bar"]["<<"].key(), "<<");
    EXPECT_EQ(t["bar"]["<<"].val(), "*foo");
    //EXPECT_EQ(t["bar"]["<<"].key_ref(), "<<"); // not a ref!
    EXPECT_EQ(t["bar"]["<<"].val_ref(), "foo");
    SCOPED_CALL(test_emit_yaml, (ti, ""
                                 "{\n"
                                 "  foo: &foo {a: 1,b: 2},\n"
                                 "  bar: {<<: *foo},\n"
                                 "  sq: {'<<': haha},\n"
                                 "  dq: {\"<<\": hehe}\n"
                                 "}\n"
                                 ""));
    SCOPED_CALL(test_resolve, (ti));
    SCOPED_CALL(test_emit_yaml, (ti,
                                 "{\n"
                                 "  foo: {a: 1,b: 2},\n"
                                 "  bar: {a: 1,b: 2},\n"
                                 "  sq: {'<<': haha},\n"
                                 "  dq: {\"<<\": hehe}\n"
                                 "}\n"
                                 ""));
}

TEST(anchors, programatic_key_ref)
{
    Tree t = parse_in_arena("{}");
    t._rem_flags(t.root_id(), CONTAINER_STYLE);
    t._add_flags(t.root_id(), BLOCK);
    NodeRef r = t.rootref();
    r["kanchor"].set_val("2");
    r["kanchor"].set_key_anchor("kanchor");
    r["vanchor"].set_val("3");
    r["vanchor"].set_val_anchor("vanchor");
    r["*kanchor"].set_val("4");
    r["*vanchor"].set_val("5");
    NodeRef ch = r.append_child();
    ch.set_key_ref("kanchor");
    ch.set_val("6");
    ch = r.append_child();
    ch.set_key_ref("vanchor");
    ch.set_val("7");
    _c4dbg_tree(t);
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"(&kanchor kanchor: 2
vanchor: &vanchor 3
'*kanchor': 4
'*vanchor': 5
*kanchor : 6
*vanchor : 7
)");
    t.resolve();
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"(kanchor: 2
vanchor: 3
'*kanchor': 4
'*vanchor': 5
kanchor: 6
3: 7
)");
}

TEST(anchors, programatic_val_ref)
{
    Tree t = parse_in_arena("{}");
    t._rem_flags(t.root_id(), CONTAINER_STYLE);
    t._add_flags(t.root_id(), BLOCK);
    t["kanchor"].set_val("2");
    t["kanchor"].set_key_anchor("kanchor");
    t["vanchor"].set_val("3");
    t["vanchor"].set_val_anchor("vanchor");
    t["kref"].set_val_ref("kanchor");
    t["vref"].set_val_ref("vanchor");
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"(&kanchor kanchor: 2
vanchor: &vanchor 3
kref: *kanchor
vref: *vanchor
)");
    t.resolve();
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"(kanchor: 2
vanchor: 3
kref: kanchor
vref: 3
)");
}

TEST(anchors, programatic_inheritance)
{
    Tree t = parse_in_arena(R"(orig: &orig {foo: bar, baz: bat}
copy: {}
notcopy: {}
notref: {}
)");
    _c4dbg_tree(t);
    t["copy"]["<<"].set_val_ref("orig");
    t["notcopy"]["test"].set_val_ref("orig");
    t["notcopy"]["<<"].set_val_ref("orig");
    t["notref"]["<<"].set_val("*orig");
    _c4dbg_tree(t);
    test_emit_yaml_tree_(
        t,
        "orig: &orig {foo: bar,baz: bat}\n"
        "copy: {<<: *orig}\n"
        "notcopy: {test: *orig,<<: *orig}\n"
        "notref: {<<: '*orig'}\n"
        "");
    test_resolve_tree_(t);
    test_emit_yaml_tree_(
        t,
        "orig: {foo: bar,baz: bat}\n"
        "copy: {foo: bar,baz: bat}\n"
        "notcopy: {test: {foo: bar,baz: bat},foo: bar,baz: bat}\n"
        "notref: {<<: '*orig'}\n"
        "");
}

TEST(anchors, programatic_multiple_inheritance)
{
    Tree t = parse_in_arena(R"(orig1: &orig1 {foo: bar}
orig2: &orig2 {baz: bat}
orig3: &orig3 {and: more}
copy: {}
)");
    NodeRef seq = t["copy"]["<<"];
    seq.set_seq();
    seq.append_child().set_val_ref("orig1");
    seq.append_child().set_val_ref("orig2");
    seq.append_child().set_val_ref("orig3");
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"(orig1: &orig1 {foo: bar}
orig2: &orig2 {baz: bat}
orig3: &orig3 {and: more}
copy: {<<: [*orig1,*orig2,*orig3]}
)");
    t.resolve();
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"(orig1: {foo: bar}
orig2: {baz: bat}
orig3: {and: more}
copy: {foo: bar,baz: bat,and: more}
)");
}

TEST(anchors, set_anchor_leading_ampersand_is_optional)
{
    Tree t = parse_in_arena("{without: 0, with: 1}");

    t["without"].set_key_anchor("without");
    t["with"].set_key_anchor("&with");
    EXPECT_EQ(t["without"].key_anchor(), "without");
    EXPECT_EQ(t["with"].key_anchor(), "with");
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"({&without without: 0,&with with: 1})");

    t["without"].set_val_anchor("without");
    t["with"].set_val_anchor("&with");
    EXPECT_EQ(t["without"].val_anchor(), "without");
    EXPECT_EQ(t["with"].val_anchor(), "with");
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"({&without without: &without 0,&with with: &with 1})");
}

TEST(anchors, set_ref_leading_star_is_optional)
{
    Tree t = parse_in_arena("{}");

    t["*without"].set_val("0");
    t["*with"].set_val("1");
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"({'*without': 0,'*with': 1})");

    t["*without"].set_key_ref("without");
    t["*with"].set_key_ref("*with");
    EXPECT_EQ(t["*without"].key_ref(), "without");
    EXPECT_EQ(t["*with"].key_ref(), "with");
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"({*without : 0,*with : 1})");

    t["*without"].set_val_ref("without");
    t["*with"].set_val_ref("*with");
    EXPECT_EQ(t["*without"].val_ref(), "without");
    EXPECT_EQ(t["*with"].val_ref(), "with");
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"({*without : *without,*with : *with})");
}

TEST(anchors, set_key_ref_also_sets_the_key_when_none_exists)
{
    Tree t = parse_in_arena("{}");
    NodeRef root = t.rootref();
    NodeRef without = root.append_child();
    NodeRef with = root.append_child();
    ASSERT_FALSE(without.has_key());
    ASSERT_FALSE(with.has_key());
    without.set_key_ref("without");
    with.set_key_ref("*with");
    without.set_val("0");
    with.set_val("1");
    ASSERT_TRUE(without.has_key());
    ASSERT_TRUE(with.has_key());
    EXPECT_EQ(without.key(), "without");
    EXPECT_EQ(with.key(), "*with");
    EXPECT_EQ(without.key_ref(), "without");
    EXPECT_EQ(with.key_ref(), "with");
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"({*without : 0,*with : 1})");
}

TEST(anchors, set_val_ref_also_sets_the_val_when_none_exists)
{
    Tree t = parse_in_arena("{}");
    NodeRef root = t.rootref();
    NodeRef without = root.append_child();
    NodeRef with = root.append_child();
    without.set_key("without");
    with.set_key("with");
    ASSERT_FALSE(without.has_val());
    ASSERT_FALSE(with.has_val());
    without.set_val_ref("without");
    with.set_val_ref("*with");
    ASSERT_TRUE(without.has_val());
    ASSERT_TRUE(with.has_val());
    EXPECT_EQ(without.val(), "without");
    EXPECT_EQ(with.val(), "*with");
    EXPECT_EQ(without.val_ref(), "without");
    EXPECT_EQ(with.val_ref(), "with");
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"({without: *without,with: *with})");
}

TEST(anchors, set_key_ref_replaces_existing_key)
{
    Tree t = parse_in_arena("{*foo : bar}");
    NodeRef root = t.rootref();
    EXPECT_TRUE(root.has_child("*foo"));
    root["*foo"].set_key_ref("notfoo");
    EXPECT_FALSE(root.has_child("*foo"));
    EXPECT_FALSE(root.has_child("*notfoo"));
    EXPECT_TRUE(root.has_child("notfoo"));
    EXPECT_EQ(emitrs_yaml<std::string>(t), "{*notfoo : bar}");
}

TEST(anchors, set_val_ref_replaces_existing_key)
{
    Tree t = parse_in_arena("{foo : *bar}");
    NodeRef root = t.rootref();
    root["foo"].set_val_ref("notbar");
    EXPECT_EQ(root["foo"].val(), "notbar");
    root["foo"].set_val_ref("*notfoo");
    EXPECT_EQ(root["foo"].val(), "*notfoo");
    EXPECT_EQ(emitrs_yaml<std::string>(t), "{foo: *notfoo}");
}


TEST(anchors, github529)
{
    std::string unresolved = R"(
dimensions: 3
type: EXPRESSION
variables:
  "<<":
    - biomes/abstract/carving/carving_land.yml:carving.sampler.variables
  carvingMaxHeight: 40
expression: $biomes/abstract/carving/carving_land.yml:carving.sampler.expression
samplers: $biomes/abstract/carving/carving_land.yml:carving.sampler.samplers
<<: not a reference
tpl: &anchor
  it: works
<<: *anchor
 )";
    TreeAndInts ti(to_csubstr(unresolved));
    Tree const& tree = ti.tree;
    //print_tree(tree);
    EXPECT_FALSE(tree["variables"]["<<"].is_val_ref());
    EXPECT_TRUE(tree["variables"]["<<"].is_key_quoted());
    EXPECT_EQ(tree[5].key(), "<<");
    EXPECT_FALSE(tree[5].is_val_ref());
    EXPECT_FALSE(tree[5].is_key_quoted());
    EXPECT_EQ(tree[7].key(), "<<");
    EXPECT_TRUE(tree[7].is_val_ref());
    EXPECT_FALSE(tree[7].is_key_quoted());
    SCOPED_CALL(test_resolve, (ti));
    EXPECT_EQ(tree["variables"]["<<"][0].val(), "biomes/abstract/carving/carving_land.yml:carving.sampler.variables");
    EXPECT_EQ(ti.ints.getstr(23), "biomes/abstract/carving/carving_land.yml:carving.sampler.variables");
    EXPECT_EQ(tree["<<"].val(), "not a reference");
    EXPECT_EQ(ti.ints.getstr(49), "not a reference");
    EXPECT_EQ(tree["it"].val(), "works");
}


//-----------------------------------------------------------------------------

Tree github566_make_map(NodeType root_style)
{
    Tree tree;
    NodeRef root = tree.rootref();
    root.set_map(root_style);
    root.set_val_anchor("ref0");
    root["a"].set_val("1");
    NodeRef self = root["self"];
    self.set_val("*ref0");
    self.set_val_ref("ref0");
    return tree;
}

Tree github566_make_seq(NodeType root_style)
{
    Tree tree;
    NodeRef root = tree.rootref();
    root.set_seq(root_style);
    root.set_val_anchor("ref0");
    root[0].set_val("1");
    root[1].set_val("*ref0");
    root[1].set_val_ref("ref0");
    return tree;
}

void github566_cmp(type_bits mask, Tree const& orig, std::string const& expected)
{
    const Tree parsed = parse_in_arena(to_csubstr(expected));
    test_compare(orig, parsed, "orig", "parsed", mask);
    EXPECT_EQ(expected, emitrs_yaml<std::string>(parsed));
    EXPECT_EQ(expected, emitrs_yaml<std::string>(orig));
}

TEST(anchors, github566_map_NOSTYLE)
{
    const Tree orig = github566_make_map(NOTYPE);
    github566_cmp(TYMASK_, orig, R"(&ref0
a: 1
self: *ref0
)");
}
TEST(anchors, github566_map_BLOCK)
{
    const Tree orig = github566_make_map(BLOCK);
    github566_cmp(0xffffffff, orig, R"(&ref0
a: 1
self: *ref0
)");
}
TEST(anchors, github566_map_FLOW_ML)
{
    const Tree orig = github566_make_map(FLOW_ML1);
    github566_cmp(0xffffffff, orig, R"(&ref0 {
  a: 1,
  self: *ref0
}
)");
}
TEST(anchors, github566_map_FLOW_SL)
{
    const Tree orig = github566_make_map(FLOW_SL);
    github566_cmp(0xffffffff, orig, R"(&ref0 {a: 1,self: *ref0})");
}


TEST(anchors, github566_seq_NOSTYLE)
{
    const Tree orig = github566_make_seq(NOTYPE);
    github566_cmp(TYMASK_, orig, R"(&ref0
- 1
- *ref0
)");
}
TEST(anchors, github566_seq_BLOCK)
{
    const Tree orig = github566_make_seq(BLOCK);
    github566_cmp(0xffffffff, orig, R"(&ref0
- 1
- *ref0
)");
}
TEST(anchors, github566_seq_FLOW_ML1)
{
    const Tree orig = github566_make_seq(FLOW_ML1);
    github566_cmp(0xffffffff, orig, R"(&ref0 [
  1,
  *ref0
]
)");
}
TEST(anchors, github566_seq_FLOW_SL)
{
    const Tree orig = github566_make_seq(FLOW_SL);
    github566_cmp(0xffffffff, orig, R"(&ref0 [1,*ref0])");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(anchors, weird_anchor_cases_from_suite_2SXE)
{
    TreeAndInts ti(R"(&a: key: &a value
foo:
  *a:
)");
    test_resolve_(ti);
    test_emit_yaml(ti, R"(key: value
foo: key
)");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//     SIMPLE_ANCHOR/YmlTestCase.parse_using_ryml/0

C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")

/** verify that the reference class is working correctly (yay, testing the tests) */
TEST(CaseNode, anchors)
{
    const NodeType mask = KEYREF|KEYANCH|VALREF|VALANCH;

    {
        auto n = N("*vref", AR(KEYREF, "vref"), "c");
        EXPECT_EQ(n.key, "*vref");
        EXPECT_EQ(n.val, "c");
        EXPECT_EQ((type_bits)(n.type & mask), (type_bits)KEYREF);
        EXPECT_EQ((type_bits)n.key_anchor.type, (type_bits)KEYREF);
        EXPECT_EQ((type_bits)n.val_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ(n.key_anchor.str, "vref");
        EXPECT_EQ(n.val_anchor.str, "");
    }

    {
        TestCaseNode n("<<", "*base", AR(VALANCH, "base"));
        EXPECT_EQ(n.key, "<<");
        EXPECT_EQ(n.val, "*base");
        EXPECT_EQ((type_bits)(n.type & mask), (type_bits)VALANCH);
        EXPECT_EQ((type_bits)n.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)n.val_anchor.type, (type_bits)VALANCH);
        EXPECT_EQ(n.key_anchor.str, "");
        EXPECT_EQ(n.val_anchor.str, "base");
    }

    {
        TestCaseNode n("base", L{N("name", "Everyone has same name")}, AR(VALANCH, "base"));
        EXPECT_EQ(n.key, "base");
        EXPECT_EQ(n.val, "");
        EXPECT_NE(n.type.is_seq(), true);
        EXPECT_EQ((type_bits)(n.type & mask), (type_bits)VALANCH);
        EXPECT_EQ((type_bits)n.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)n.val_anchor.type, (type_bits)VALANCH);
        EXPECT_EQ(n.key_anchor.str, "");
        EXPECT_EQ(n.val_anchor.str, "base");
    }

    {
        L l{N("<<", "*base", AR(VALREF, "base"))};
        TestCaseNode const& base = *l.begin();
        EXPECT_EQ(base.key, "<<");
        EXPECT_EQ(base.val, "*base");
        EXPECT_EQ(base.type.is_keyval(), true);
        EXPECT_EQ((type_bits)(base.type & mask), (type_bits)VALREF);
        EXPECT_EQ((type_bits)base.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)base.val_anchor.type, (type_bits)VALREF);
        EXPECT_EQ(base.key_anchor.str, "");
        EXPECT_EQ(base.val_anchor.str, "base");
    }

    {
        L l{N("<<", "*base", AR(VALREF, "base")), N("age", "10")};
        TestCaseNode const& base = *l.begin();
        TestCaseNode const& age = *(&base + 1);
        EXPECT_EQ(base.key, "<<");
        EXPECT_EQ(base.val, "*base");
        EXPECT_EQ(base.type.is_keyval(), true);
        EXPECT_EQ((type_bits)(base.type & mask), (type_bits)VALREF);
        EXPECT_EQ((type_bits)base.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)base.val_anchor.type, (type_bits)VALREF);
        EXPECT_EQ(base.key_anchor.str, "");
        EXPECT_EQ(base.val_anchor.str, "base");

        EXPECT_EQ(age.key, "age");
        EXPECT_EQ(age.val, "10");
        EXPECT_EQ(age.type.is_keyval(), true);
        EXPECT_EQ((type_bits)(age.type & mask), (type_bits)0);
        EXPECT_EQ((type_bits)age.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)age.val_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ(age.key_anchor.str, "");
        EXPECT_EQ(age.val_anchor.str, "");
    }

    {
        TestCaseNode n("foo", L{N("<<", "*base", AR(VALREF, "base")), N("age", "10")}, AR(VALANCH, "foo"));
        EXPECT_EQ(n.key, "foo");
        EXPECT_EQ(n.val, "");
        EXPECT_EQ(n.type.has_key(), true);
        EXPECT_EQ(n.type.is_map(), true);
        EXPECT_EQ((type_bits)(n.type & mask), (type_bits)VALANCH);
        EXPECT_EQ((type_bits)n.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)n.val_anchor.type, (type_bits)VALANCH);
        EXPECT_EQ(n.key_anchor.str, "");
        EXPECT_EQ(n.val_anchor.str, "foo");

        TestCaseNode const& base = n.children[0];
        EXPECT_EQ(base.key, "<<");
        EXPECT_EQ(base.val, "*base");
        EXPECT_EQ(base.type.has_key() && base.type.has_val(), true);
        EXPECT_EQ((type_bits)(base.type & mask), (type_bits)VALREF);
        EXPECT_EQ((type_bits)base.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)base.val_anchor.type, (type_bits)VALREF);
        EXPECT_EQ(base.key_anchor.str, "");
        EXPECT_EQ(base.val_anchor.str, "base");
    }

}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(anchors, resolve_works_on_an_empty_tree)
{
    TreeAndInts ti;
    ti.tree = Tree(0);
    ASSERT_EQ(ti.tree.capacity(), 0);
    test_resolve_(ti);
    EXPECT_TRUE(ti.tree.empty());
    ASSERT_EQ(ti.tree.capacity(), 0);
}

TEST(anchors, resolve_works_on_a_tree_without_refs)
{
    TreeAndInts ti("[a, b, c, d, e, f]");
    id_type size_before_tree = ti.tree.size();
    extra::evt_size size_before_ints = ti.ints.evts.len;
    test_resolve_(ti);
    EXPECT_EQ(ti.tree.size(), size_before_tree);
    EXPECT_EQ(ti.ints.evts.len, size_before_ints);
    test_emit_yaml_(ti, "[a,b,c,d,e,f]");
}

TEST(anchors, resolve_works_on_keyrefvalref)
{
    TreeAndInts ti("{&a a: &b b, *b : *a}");
    Tree const& t = ti.tree;
    EXPECT_EQ(t["a"].has_key_anchor(), true);
    EXPECT_EQ(t["a"].has_val_anchor(), true);
    EXPECT_EQ(t["a"].key_anchor(), "a");
    EXPECT_EQ(t["a"].val_anchor(), "b");
    EXPECT_EQ(t["*b"].is_key_ref(), true);
    EXPECT_EQ(t["*b"].is_val_ref(), true);
    EXPECT_EQ(t["*b"].key_ref(), "b");
    EXPECT_EQ(t["*b"].val_ref(), "a");
    test_emit_yaml_(ti, "{&a a: &b b,*b : *a}");
    test_resolve_(ti);
    EXPECT_EQ(t["a"].val(), "b");
    EXPECT_EQ(t["b"].val(), "a");
    test_emit_yaml_(ti, "{a: b,b: a}");
}

TEST(anchors, anchors_of_first_child_key_implicit)
{
    csubstr yaml = R"(&anchor0
&anchor4 top4:
  key4: scalar4
top5: &anchor5
  key5: scalar5
top6:
  &anchor6 key6: scalar6
top61:
  &anchor61 key61:
      scalar61
top62:
  &anchor62
  key62:
      scalar62
)";
    Tree t = parse_in_arena(yaml);
    EXPECT_EQ(t.rootref().has_val_anchor(), true);
    EXPECT_EQ(t.rootref().val_anchor(), "anchor0");
    EXPECT_EQ(t["top4"].has_key_anchor(), true);
    EXPECT_EQ(t["top4"].has_val_anchor(), false);
    EXPECT_EQ(t["top4"].key_anchor(), "anchor4");
    EXPECT_EQ(t["top4"]["key4"].val(), "scalar4");
    EXPECT_EQ(t["top4"]["key4"].has_key_anchor(), false);
    EXPECT_EQ(t["top5"].has_key_anchor(), false);
    EXPECT_EQ(t["top5"].has_val_anchor(), true);
    EXPECT_EQ(t["top5"].val_anchor(), "anchor5");
    EXPECT_EQ(t["top5"]["key5"].val(), "scalar5");
    EXPECT_EQ(t["top5"]["key5"].has_key_anchor(), false);
    EXPECT_EQ(t["top6"].has_key_anchor(), false);
    EXPECT_EQ(t["top6"].has_val_anchor(), false);
    EXPECT_EQ(t["top6"]["key6"].val(), "scalar6");
    ASSERT_EQ(t["top6"]["key6"].has_key_anchor(), true);
    EXPECT_EQ(t["top6"]["key6"].key_anchor(), "anchor6");
    EXPECT_EQ(t["top61"].has_key_anchor(), false);
    EXPECT_EQ(t["top61"].has_val_anchor(), false);
    EXPECT_EQ(t["top61"]["key61"].val(), "scalar61");
    ASSERT_EQ(t["top61"]["key61"].has_key_anchor(), true);
    EXPECT_EQ(t["top61"]["key61"].key_anchor(), "anchor61");
    EXPECT_EQ(t["top62"].has_key_anchor(), false);
    EXPECT_EQ(t["top62"].has_val_anchor(), true);
    EXPECT_EQ(t["top62"].val_anchor(), "anchor62");
    EXPECT_EQ(t["top62"]["key62"].val(), "scalar62");
    ASSERT_EQ(t["top62"]["key62"].has_key_anchor(), false);
}

TEST(anchors, merge_seqs)
{
    const Tree src = parse_in_arena(R"([0, 1])");
    {
        Tree dst = parse_in_arena(R"([2, 3])");
        dst.duplicate_children_no_rep(&src, 0, 0, NONE);
        EXPECT_EQ(emitrs_yaml<std::string>(dst), "[0,1,2,3]");
    }
    {
        Tree dst = parse_in_arena(R"([2, 3])");
        dst.duplicate_children_no_rep(&src, 0, 0, 1);
        EXPECT_EQ(emitrs_yaml<std::string>(dst), "[2,0,1,3]");
    }
    {
        Tree dst = parse_in_arena(R"([2, 3])");
        dst.duplicate_children_no_rep(&src, 0, 0, 2);
        EXPECT_EQ(emitrs_yaml<std::string>(dst), "[2,3,0,1]");
    }
}

TEST(anchors, issue_400)
{
    TreeAndInts ti(R"(
a: &a
  x: 1
b: &b
  ref: *a
c:
  ref: *b
)");
    test_resolve_(ti);
    test_emit_yaml_(ti,
                    "a:\n"
                    "  x: 1\n"
                    "b:\n"
                    "  ref:\n"
                    "    x: 1\n"
                    "c:\n"
                    "  ref:\n"
                    "    ref:\n"
                    "      x: 1\n"
                    "");
}

TEST(anchors, resolve_nested)
{
    std::string yaml = R"(a: &a
  b:
    <<: *a
)";
    {
        TreeAndInts ti(to_csubstr(yaml));
        test_emit_yaml_(ti, yaml);
    }
    {
        TreeAndInts ti(to_csubstr(yaml));
        test_resolve_err_(ti, ResolveOptions{}.clear_anchors(true));
    }
    {
        TreeAndInts ti(to_csubstr(yaml));
        test_resolve_err_(ti, ResolveOptions{}.clear_anchors(false));
    }
}

TEST(anchors, resolve_nested_2)
{
    std::string yaml = R"(a0: &a0
  b0: &b0
    c0: &c0 v0
a1: &a1
  ref: *a0
a2: &a2
  ref: *a1
a3: &a3
  ref: *a2
a4: &a4
  ref: *a3
a5: &a5
  ref: *a4
)";
    {
        TreeAndInts ti(to_csubstr(yaml));
        test_emit_yaml_(ti, yaml);
    }
    {
        TreeAndInts ti(to_csubstr(yaml));
        test_resolve(ti, ResolveOptions{}.clear_anchors(true));
        test_emit_yaml_(ti,
                        "a0:\n"
                        "  b0:\n"
                        "    c0: v0\n"
                        "a1:\n"
                        "  ref:\n"
                        "    b0:\n"
                        "      c0: v0\n"
                        "a2:\n"
                        "  ref:\n"
                        "    ref:\n"
                        "      b0:\n"
                        "        c0: v0\n"
                        "a3:\n"
                        "  ref:\n"
                        "    ref:\n"
                        "      ref:\n"
                        "        b0:\n"
                        "          c0: v0\n"
                        "a4:\n"
                        "  ref:\n"
                        "    ref:\n"
                        "      ref:\n"
                        "        ref:\n"
                        "          b0:\n"
                        "            c0: v0\n"
                        "a5:\n"
                        "  ref:\n"
                        "    ref:\n"
                        "      ref:\n"
                        "        ref:\n"
                        "          ref:\n"
                        "            b0:\n"
                        "              c0: v0\n"
                        "");
    }
    {
        TreeAndInts ti(to_csubstr(yaml));
        test_resolve(ti, ResolveOptions{}.clear_anchors(false));
        test_emit_yaml_(ti,
                        "a0: &a0\n"
                        "  b0: &b0\n"
                        "    c0: &c0 v0\n"
                        "a1: &a1\n"
                        "  ref:\n"
                        "    b0: &b0\n"
                        "      c0: &c0 v0\n"
                        "a2: &a2\n"
                        "  ref:\n"
                        "    ref:\n"
                        "      b0: &b0\n"
                        "        c0: &c0 v0\n"
                        "a3: &a3\n"
                        "  ref:\n"
                        "    ref:\n"
                        "      ref:\n"
                        "        b0: &b0\n"
                        "          c0: &c0 v0\n"
                        "a4: &a4\n"
                        "  ref:\n"
                        "    ref:\n"
                        "      ref:\n"
                        "        ref:\n"
                        "          b0: &b0\n"
                        "            c0: &c0 v0\n"
                        "a5: &a5\n"
                        "  ref:\n"
                        "    ref:\n"
                        "      ref:\n"
                        "        ref:\n"
                        "          ref:\n"
                        "            b0: &b0\n"
                        "              c0: &c0 v0\n"
                        "");
    }
}


static const ResolveOptions opts_pri_orig = {};
static const ResolveOptions opts_keep = ResolveOptions{}.keep_repeated_keys(true);
static const ResolveOptions opts_pri_merg = ResolveOptions{}.priority_original(false).priority_mergedin(true);
static const ResolveOptions opts_pri_none = ResolveOptions{}.priority_original(false);
static const ResolveOptions opts_pri_both = ResolveOptions{}.priority_mergedin(true);
static const ResolveOptions opts_pri_orig_all = ResolveOptions{opts_pri_orig}.merge_always(true);
static const ResolveOptions opts_pri_merg_all = ResolveOptions{opts_pri_merg}.merge_always(true);
static const ResolveOptions opts_pri_none_all = ResolveOptions{opts_pri_none}.merge_always(true);
static const ResolveOptions opts_pri_both_all = ResolveOptions{opts_pri_both}.merge_always(true);

static const ResolveOptions opts_pri_orig_last = ResolveOptions{opts_pri_orig}.pick_last(true);
static const ResolveOptions opts_pri_merg_last = ResolveOptions{opts_pri_merg}.pick_last(true);
static const ResolveOptions opts_pri_none_last = ResolveOptions{opts_pri_none}.pick_last(true);
static const ResolveOptions opts_pri_both_last = ResolveOptions{opts_pri_both}.pick_last(true);
static const ResolveOptions opts_pri_orig_all_last = ResolveOptions{opts_pri_orig_all}.pick_last(true);
static const ResolveOptions opts_pri_merg_all_last = ResolveOptions{opts_pri_merg_all}.pick_last(true);
static const ResolveOptions opts_pri_none_all_last = ResolveOptions{opts_pri_none_all}.pick_last(true);
static const ResolveOptions opts_pri_both_all_last = ResolveOptions{opts_pri_both_all}.pick_last(true);


TEST(anchors, resolve_options)
{
    #define TEST_RESOLVE_OPTIONS(opts, priority_original_, priority_mergedin_, pick_last_, keep_repeated_keys_, merge_always_) \
        EXPECT_##priority_original_((opts).priority_original());        \
        EXPECT_##priority_mergedin_((opts).priority_mergedin());        \
        EXPECT_##pick_last_((opts).pick_last());                        \
        EXPECT_##keep_repeated_keys_((opts).keep_repeated_keys());      \
        EXPECT_##merge_always_((opts).merge_always())

    TEST_RESOLVE_OPTIONS(opts_keep, TRUE,  FALSE, FALSE, TRUE, FALSE);

    TEST_RESOLVE_OPTIONS(opts_pri_orig, TRUE,  FALSE, FALSE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_merg, FALSE, TRUE,  FALSE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_none, FALSE, FALSE, FALSE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_both, TRUE,  TRUE,  FALSE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_orig_all, TRUE,  FALSE, FALSE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_merg_all, FALSE, TRUE,  FALSE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_none_all, FALSE, FALSE, FALSE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_both_all, TRUE,  TRUE,  FALSE, FALSE, TRUE);

    TEST_RESOLVE_OPTIONS(opts_pri_orig_last, TRUE,  FALSE, TRUE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_merg_last, FALSE, TRUE,  TRUE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_none_last, FALSE, FALSE, TRUE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_both_last, TRUE,  TRUE,  TRUE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_orig_all_last, TRUE,  FALSE, TRUE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_merg_all_last, FALSE, TRUE,  TRUE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_none_all_last, FALSE, FALSE, TRUE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_both_all_last, TRUE,  TRUE,  TRUE, FALSE, TRUE);


    #undef TEST_RESOLVE_OPTIONS
}

#define test_resolve_with_options0(yaml, resolved)              \
    {                                                           \
        TreeAndInts ti(to_csubstr(yaml));                       \
        test_resolve_(ti);                                      \
        test_emit_yaml_(ti, resolved);                          \
    }
#define test_resolve_with_options_(yaml, optsname, resolved)    \
    {                                                           \
        TreeAndInts ti(to_csubstr(yaml));                       \
        test_resolve_(ti, optsname);                            \
        test_emit_yaml_(ti, resolved);                          \
    }
#define test_resolve_with_options2(yaml, optsname1, optsname2, resolved) \
    {                                                           \
        test_resolve_with_options_(yaml, optsname1, resolved);  \
        test_resolve_with_options_(yaml, optsname2, resolved);  \
    }
#define test_resolve_with_options4(yaml, optsname1, optsname2, optsname3, optsname4, resolved) \
    {                                                           \
        test_resolve_with_options_(yaml, optsname1, resolved);  \
        test_resolve_with_options_(yaml, optsname2, resolved);  \
        test_resolve_with_options_(yaml, optsname3, resolved);  \
        test_resolve_with_options_(yaml, optsname4, resolved);  \
    }

TEST(anchors, cyclic2)
{
    test_resolve_err_("- &x [a,b]\n- &y [*x,*y]\n");
}
TEST(anchors, cyclic3)
{
    test_resolve_err_("- &x [a,b]\n- &y [*y,*y]\n");
}
TEST(anchors, cyclic4)
{
    test_resolve_with_options0("- &x [a,b]\n- &y [*x,*x]\n",
                               "- [a,b]\n- [[a,b],[a,b]]\n");
}
TEST(anchors, cyclic5)
{
    test_resolve_with_options0("{&z x: *z}",
                               "{x: x}");
}
TEST(anchors, cyclic6)
{
    test_resolve_err_("&z {&x x: a, *x : *z}\n");
}
TEST(anchors, cyclic7)
{
    test_resolve_err_("&z {&x x: a, *z : *x}\n");
}
TEST(anchors, cyclic8)
{
    test_resolve_with_options0("- &a0\n- &a1\n  - *a0\n",
                               "- \n- - \n");
}
TEST(anchors, cyclic9)
{
    test_resolve_err("- &a0\n- &a1\n  - *a1\n");
}
TEST(anchors, cyclic10)
{
    test_resolve_with_options0("- &a0\n"
                               "  - - - - &a1\n"
                               "        - *a1\n"
                               ,
                               "- - - - - \n"
                               "        - \n"
                               "");
}
TEST(anchors, cyclic11)
{
    test_resolve_err("- &a0\n"
                     "  - - - - &a1\n"
                     "        - *a0\n");
}

TEST(anchors, repeated_doc)
{
    std::string yaml = ""
        "--- &merg1\nrep: m11\nrep: m12\n"
        "--- &merg2\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\n<<: *merg1\nrep: o1\nbar: a00\nrep: o2\nrep: o3\n<<: *merg2\n"
        "---\nrep: o1\n<<: *merg1\nbar: a01\nrep: o2\n<<: *merg2\nrep: o3\n"
        "---\nrep: o1\nbar: a02\n<<: *merg1\n<<: *merg2\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a03\n<<: *merg2\nrep: o2\n<<: *merg1\nrep: o3\n"
        "---\nrep: o1\n<<: *merg2\nbar: a04\nrep: o2\nrep: o3\n<<: *merg1\n"
        "---\n<<: [*merg1,*merg2]\nrep: o1\nbar: a05\nrep: o2\nrep: o3\n"
        "---\n<<: [*merg2,*merg1]\nrep: o1\nbar: a06\nrep: o2\nrep: o3\n"
        "---\n<<: *merg1\n<<: *merg2\nrep: o1\nbar: a07\nrep: o2\nrep: o3\n"
        "---\n<<: *merg2\n<<: *merg1\nrep: o1\nbar: a08\nrep: o2\nrep: o3\n"
        "---\nrep: o1\n<<: [*merg1,*merg2]\nbar: a09\nrep: o2\nrep: o3\n"
        "---\nrep: o1\n<<: [*merg2,*merg1]\nbar: a10\nrep: o2\nrep: o3\n"
        "---\nrep: o1\n<<: *merg1\n<<: *merg2\nbar: a11\nrep: o2\nrep: o3\n"
        "---\nrep: o1\n<<: *merg2\n<<: *merg1\nbar: a12\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a13\n<<: [*merg1,*merg2]\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a14\n<<: [*merg2,*merg1]\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a15\n<<: *merg1\n<<: *merg2\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a16\n<<: *merg2\n<<: *merg1\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a17\nrep: o2\n<<: [*merg1,*merg2]\nrep: o3\n"
        "---\nrep: o1\nbar: a18\nrep: o2\n<<: [*merg2,*merg1]\nrep: o3\n"
        "---\nrep: o1\nbar: a19\nrep: o2\n<<: *merg1\n<<: *merg2\nrep: o3\n"
        "---\nrep: o1\nbar: a20\nrep: o2\n<<: *merg2\n<<: *merg1\nrep: o3\n"
        "---\nrep: o1\nbar: a21\nrep: o2\nrep: o3\n<<: [*merg1,*merg2]\n"
        "---\nrep: o1\nbar: a22\nrep: o2\nrep: o3\n<<: [*merg2,*merg1]\n"
        "---\nrep: o1\nbar: a23\nrep: o2\nrep: o3\n<<: *merg1\n<<: *merg2\n"
        "---\nrep: o1\nbar: a24\nrep: o2\nrep: o3\n<<: *merg2\n<<: *merg1\n"
        "";
    test_resolve_with_options_(yaml, opts_keep,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: m11\nrep: m12\nrep: o1\nbar: a00\nrep: o2\nrep: o3\nrep: m21\nrep: m22\n"
        "---\nrep: o1\nrep: m11\nrep: m12\nbar: a01\nrep: o2\nrep: m21\nrep: m22\nrep: o3\n"
        "---\nrep: o1\nbar: a02\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a03\nrep: m21\nrep: m22\nrep: o2\nrep: m11\nrep: m12\nrep: o3\n"
        "---\nrep: o1\nrep: m21\nrep: m22\nbar: a04\nrep: o2\nrep: o3\nrep: m11\nrep: m12\n"
        "---\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o1\nbar: a05\nrep: o2\nrep: o3\n"
        "---\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o1\nbar: a06\nrep: o2\nrep: o3\n"
        "---\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o1\nbar: a07\nrep: o2\nrep: o3\n"
        "---\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o1\nbar: a08\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nbar: a09\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nbar: a10\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nbar: a11\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nbar: a12\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a13\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a14\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a15\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a16\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a17\nrep: o2\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o3\n"
        "---\nrep: o1\nbar: a18\nrep: o2\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o3\n"
        "---\nrep: o1\nbar: a19\nrep: o2\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o3\n"
        "---\nrep: o1\nbar: a20\nrep: o2\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o3\n"
        "---\nrep: o1\nbar: a21\nrep: o2\nrep: o3\nrep: m11\nrep: m12\nrep: m21\nrep: m22\n"
        "---\nrep: o1\nbar: a22\nrep: o2\nrep: o3\nrep: m21\nrep: m22\nrep: m11\nrep: m12\n"
        "---\nrep: o1\nbar: a23\nrep: o2\nrep: o3\nrep: m11\nrep: m12\nrep: m21\nrep: m22\n"
        "---\nrep: o1\nbar: a24\nrep: o2\nrep: o3\nrep: m21\nrep: m22\nrep: m11\nrep: m12\n"
        "");

    {
        std::string expected =
            ""
            "---\nrep: m11\nrep: m12\n"
            "---\nrep: m21\nrep: m22\n"
            "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
            "---\nrep: o1\nbar: a00\n"
            "---\nrep: o1\nbar: a01\n"
            "---\nrep: o1\nbar: a02\n"
            "---\nrep: o1\nbar: a03\n"
            "---\nrep: o1\nbar: a04\n"
            "---\nrep: o1\nbar: a05\n"
            "---\nrep: o1\nbar: a06\n"
            "---\nrep: o1\nbar: a07\n"
            "---\nrep: o1\nbar: a08\n"
            "---\nrep: o1\nbar: a09\n"
            "---\nrep: o1\nbar: a10\n"
            "---\nrep: o1\nbar: a11\n"
            "---\nrep: o1\nbar: a12\n"
            "---\nrep: o1\nbar: a13\n"
            "---\nrep: o1\nbar: a14\n"
            "---\nrep: o1\nbar: a15\n"
            "---\nrep: o1\nbar: a16\n"
            "---\nrep: o1\nbar: a17\n"
            "---\nrep: o1\nbar: a18\n"
            "---\nrep: o1\nbar: a19\n"
            "---\nrep: o1\nbar: a20\n"
            "---\nrep: o1\nbar: a21\n"
            "---\nrep: o1\nbar: a22\n"
            "---\nrep: o1\nbar: a23\n"
            "---\nrep: o1\nbar: a24\n"
            "";
        test_resolve_with_options0(yaml, expected);
        test_resolve_with_options_(yaml, opts_pri_orig, expected);
    }

    test_resolve_with_options_(yaml, opts_pri_merg,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: m11\nbar: a00\n"
        "---\nrep: m11\nbar: a01\n"
        "---\nrep: m11\nbar: a02\n"
        "---\nrep: m21\nbar: a03\n"
        "---\nrep: m21\nbar: a04\n"
        "---\nrep: m11\nbar: a05\n"
        "---\nrep: m21\nbar: a06\n"
        "---\nrep: m11\nbar: a07\n"
        "---\nrep: m21\nbar: a08\n"
        "---\nrep: m11\nbar: a09\n"
        "---\nrep: m21\nbar: a10\n"
        "---\nrep: m11\nbar: a11\n"
        "---\nrep: m21\nbar: a12\n"
        "---\nrep: m11\nbar: a13\n"
        "---\nrep: m21\nbar: a14\n"
        "---\nrep: m11\nbar: a15\n"
        "---\nrep: m21\nbar: a16\n"
        "---\nrep: m11\nbar: a17\n"
        "---\nrep: m21\nbar: a18\n"
        "---\nrep: m11\nbar: a19\n"
        "---\nrep: m21\nbar: a20\n"
        "---\nrep: m11\nbar: a21\n"
        "---\nrep: m21\nbar: a22\n"
        "---\nrep: m11\nbar: a23\n"
        "---\nrep: m21\nbar: a24\n"
        "");

    test_resolve_with_options2(
        yaml, opts_pri_none, opts_pri_both,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: m11\nbar: a00\n"
        "---\nrep: o1\nbar: a01\n"
        "---\nrep: o1\nbar: a02\n"
        "---\nrep: o1\nbar: a03\n"
        "---\nrep: o1\nbar: a04\n"
        "---\nrep: m11\nbar: a05\n"
        "---\nrep: m21\nbar: a06\n"
        "---\nrep: m11\nbar: a07\n"
        "---\nrep: m21\nbar: a08\n"
        "---\nrep: o1\nbar: a09\n"
        "---\nrep: o1\nbar: a10\n"
        "---\nrep: o1\nbar: a11\n"
        "---\nrep: o1\nbar: a12\n"
        "---\nrep: o1\nbar: a13\n"
        "---\nrep: o1\nbar: a14\n"
        "---\nrep: o1\nbar: a15\n"
        "---\nrep: o1\nbar: a16\n"
        "---\nrep: o1\nbar: a17\n"
        "---\nrep: o1\nbar: a18\n"
        "---\nrep: o1\nbar: a19\n"
        "---\nrep: o1\nbar: a20\n"
        "---\nrep: o1\nbar: a21\n"
        "---\nrep: o1\nbar: a22\n"
        "---\nrep: o1\nbar: a23\n"
        "---\nrep: o1\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_orig_all,
        "---\nrep: m11\n"
        "---\nrep: m21\n"
        "---\nrep: O1\nbar: a\n"
        "---\nrep: o1\nbar: a00\n"
        "---\nrep: o1\nbar: a01\n"
        "---\nrep: o1\nbar: a02\n"
        "---\nrep: o1\nbar: a03\n"
        "---\nrep: o1\nbar: a04\n"
        "---\nrep: o1\nbar: a05\n"
        "---\nrep: o1\nbar: a06\n"
        "---\nrep: o1\nbar: a07\n"
        "---\nrep: o1\nbar: a08\n"
        "---\nrep: o1\nbar: a09\n"
        "---\nrep: o1\nbar: a10\n"
        "---\nrep: o1\nbar: a11\n"
        "---\nrep: o1\nbar: a12\n"
        "---\nrep: o1\nbar: a13\n"
        "---\nrep: o1\nbar: a14\n"
        "---\nrep: o1\nbar: a15\n"
        "---\nrep: o1\nbar: a16\n"
        "---\nrep: o1\nbar: a17\n"
        "---\nrep: o1\nbar: a18\n"
        "---\nrep: o1\nbar: a19\n"
        "---\nrep: o1\nbar: a20\n"
        "---\nrep: o1\nbar: a21\n"
        "---\nrep: o1\nbar: a22\n"
        "---\nrep: o1\nbar: a23\n"
        "---\nrep: o1\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_merg_all,
        "---\nrep: m11\n"
        "---\nrep: m21\n"
        "---\nrep: O1\nbar: a\n"
        "---\nrep: m11\nbar: a00\n"
        "---\nrep: m11\nbar: a01\n"
        "---\nrep: m11\nbar: a02\n"
        "---\nrep: m21\nbar: a03\n"
        "---\nrep: m21\nbar: a04\n"
        "---\nrep: m11\nbar: a05\n"
        "---\nrep: m21\nbar: a06\n"
        "---\nrep: m11\nbar: a07\n"
        "---\nrep: m21\nbar: a08\n"
        "---\nrep: m11\nbar: a09\n"
        "---\nrep: m21\nbar: a10\n"
        "---\nrep: m11\nbar: a11\n"
        "---\nrep: m21\nbar: a12\n"
        "---\nrep: m11\nbar: a13\n"
        "---\nrep: m21\nbar: a14\n"
        "---\nrep: m11\nbar: a15\n"
        "---\nrep: m21\nbar: a16\n"
        "---\nrep: m11\nbar: a17\n"
        "---\nrep: m21\nbar: a18\n"
        "---\nrep: m11\nbar: a19\n"
        "---\nrep: m21\nbar: a20\n"
        "---\nrep: m11\nbar: a21\n"
        "---\nrep: m21\nbar: a22\n"
        "---\nrep: m11\nbar: a23\n"
        "---\nrep: m21\nbar: a24\n"
        "");

    test_resolve_with_options2(
        yaml, opts_pri_none_all, opts_pri_both_all,
        "---\nrep: m11\n"
        "---\nrep: m21\n"
        "---\nrep: O1\nbar: a\n"
        "---\nrep: m11\nbar: a00\n"
        "---\nrep: o1\nbar: a01\n"
        "---\nrep: o1\nbar: a02\n"
        "---\nrep: o1\nbar: a03\n"
        "---\nrep: o1\nbar: a04\n"
        "---\nrep: m11\nbar: a05\n"
        "---\nrep: m21\nbar: a06\n"
        "---\nrep: m11\nbar: a07\n"
        "---\nrep: m21\nbar: a08\n"
        "---\nrep: o1\nbar: a09\n"
        "---\nrep: o1\nbar: a10\n"
        "---\nrep: o1\nbar: a11\n"
        "---\nrep: o1\nbar: a12\n"
        "---\nrep: o1\nbar: a13\n"
        "---\nrep: o1\nbar: a14\n"
        "---\nrep: o1\nbar: a15\n"
        "---\nrep: o1\nbar: a16\n"
        "---\nrep: o1\nbar: a17\n"
        "---\nrep: o1\nbar: a18\n"
        "---\nrep: o1\nbar: a19\n"
        "---\nrep: o1\nbar: a20\n"
        "---\nrep: o1\nbar: a21\n"
        "---\nrep: o1\nbar: a22\n"
        "---\nrep: o1\nbar: a23\n"
        "---\nrep: o1\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_orig_last,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: o3\nbar: a00\n"
        "---\nrep: o3\nbar: a01\n"
        "---\nrep: o3\nbar: a02\n"
        "---\nrep: o3\nbar: a03\n"
        "---\nrep: o3\nbar: a04\n"
        "---\nrep: o3\nbar: a05\n"
        "---\nrep: o3\nbar: a06\n"
        "---\nrep: o3\nbar: a07\n"
        "---\nrep: o3\nbar: a08\n"
        "---\nrep: o3\nbar: a09\n"
        "---\nrep: o3\nbar: a10\n"
        "---\nrep: o3\nbar: a11\n"
        "---\nrep: o3\nbar: a12\n"
        "---\nrep: o3\nbar: a13\n"
        "---\nrep: o3\nbar: a14\n"
        "---\nrep: o3\nbar: a15\n"
        "---\nrep: o3\nbar: a16\n"
        "---\nrep: o3\nbar: a17\n"
        "---\nrep: o3\nbar: a18\n"
        "---\nrep: o3\nbar: a19\n"
        "---\nrep: o3\nbar: a20\n"
        "---\nrep: o3\nbar: a21\n"
        "---\nrep: o3\nbar: a22\n"
        "---\nrep: o3\nbar: a23\n"
        "---\nrep: o3\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_merg_last,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: m22\nbar: a00\n"
        "---\nrep: m22\nbar: a01\n"
        "---\nrep: m22\nbar: a02\n"
        "---\nrep: m12\nbar: a03\n"
        "---\nrep: m12\nbar: a04\n"
        "---\nrep: m22\nbar: a05\n"
        "---\nrep: m12\nbar: a06\n"
        "---\nrep: m22\nbar: a07\n"
        "---\nrep: m12\nbar: a08\n"
        "---\nrep: m22\nbar: a09\n"
        "---\nrep: m12\nbar: a10\n"
        "---\nrep: m22\nbar: a11\n"
        "---\nrep: m12\nbar: a12\n"
        "---\nrep: m22\nbar: a13\n"
        "---\nrep: m12\nbar: a14\n"
        "---\nrep: m22\nbar: a15\n"
        "---\nrep: m12\nbar: a16\n"
        "---\nrep: m22\nbar: a17\n"
        "---\nrep: m12\nbar: a18\n"
        "---\nrep: m22\nbar: a19\n"
        "---\nrep: m12\nbar: a20\n"
        "---\nrep: m22\nbar: a21\n"
        "---\nrep: m12\nbar: a22\n"
        "---\nrep: m22\nbar: a23\n"
        "---\nrep: m12\nbar: a24\n"
        "");

    test_resolve_with_options2(
        yaml, opts_pri_none_last, opts_pri_both_last,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: m22\nbar: a00\n"
        "---\nrep: o3\nbar: a01\n"
        "---\nrep: o3\nbar: a02\n"
        "---\nrep: o3\nbar: a03\n"
        "---\nrep: m12\nbar: a04\n"
        "---\nrep: o3\nbar: a05\n"
        "---\nrep: o3\nbar: a06\n"
        "---\nrep: o3\nbar: a07\n"
        "---\nrep: o3\nbar: a08\n"
        "---\nrep: o3\nbar: a09\n"
        "---\nrep: o3\nbar: a10\n"
        "---\nrep: o3\nbar: a11\n"
        "---\nrep: o3\nbar: a12\n"
        "---\nrep: o3\nbar: a13\n"
        "---\nrep: o3\nbar: a14\n"
        "---\nrep: o3\nbar: a15\n"
        "---\nrep: o3\nbar: a16\n"
        "---\nrep: o3\nbar: a17\n"
        "---\nrep: o3\nbar: a18\n"
        "---\nrep: o3\nbar: a19\n"
        "---\nrep: o3\nbar: a20\n"
        "---\nrep: m22\nbar: a21\n"
        "---\nrep: m12\nbar: a22\n"
        "---\nrep: m22\nbar: a23\n"
        "---\nrep: m12\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_orig_all_last,
        "---\nrep: m12\n"
        "---\nrep: m22\n"
        "---\nrep: O3\nbar: a\n"
        "---\nrep: o3\nbar: a00\n"
        "---\nrep: o3\nbar: a01\n"
        "---\nrep: o3\nbar: a02\n"
        "---\nrep: o3\nbar: a03\n"
        "---\nrep: o3\nbar: a04\n"
        "---\nrep: o3\nbar: a05\n"
        "---\nrep: o3\nbar: a06\n"
        "---\nrep: o3\nbar: a07\n"
        "---\nrep: o3\nbar: a08\n"
        "---\nrep: o3\nbar: a09\n"
        "---\nrep: o3\nbar: a10\n"
        "---\nrep: o3\nbar: a11\n"
        "---\nrep: o3\nbar: a12\n"
        "---\nrep: o3\nbar: a13\n"
        "---\nrep: o3\nbar: a14\n"
        "---\nrep: o3\nbar: a15\n"
        "---\nrep: o3\nbar: a16\n"
        "---\nrep: o3\nbar: a17\n"
        "---\nrep: o3\nbar: a18\n"
        "---\nrep: o3\nbar: a19\n"
        "---\nrep: o3\nbar: a20\n"
        "---\nrep: o3\nbar: a21\n"
        "---\nrep: o3\nbar: a22\n"
        "---\nrep: o3\nbar: a23\n"
        "---\nrep: o3\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_merg_all_last,
        "---\nrep: m12\n"
        "---\nrep: m22\n"
        "---\nrep: O3\nbar: a\n"
        "---\nrep: m22\nbar: a00\n"
        "---\nrep: m22\nbar: a01\n"
        "---\nrep: m22\nbar: a02\n"
        "---\nrep: m12\nbar: a03\n"
        "---\nrep: m12\nbar: a04\n"
        "---\nrep: m22\nbar: a05\n"
        "---\nrep: m12\nbar: a06\n"
        "---\nrep: m22\nbar: a07\n"
        "---\nrep: m12\nbar: a08\n"
        "---\nrep: m22\nbar: a09\n"
        "---\nrep: m12\nbar: a10\n"
        "---\nrep: m22\nbar: a11\n"
        "---\nrep: m12\nbar: a12\n"
        "---\nrep: m22\nbar: a13\n"
        "---\nrep: m12\nbar: a14\n"
        "---\nrep: m22\nbar: a15\n"
        "---\nrep: m12\nbar: a16\n"
        "---\nrep: m22\nbar: a17\n"
        "---\nrep: m12\nbar: a18\n"
        "---\nrep: m22\nbar: a19\n"
        "---\nrep: m12\nbar: a20\n"
        "---\nrep: m22\nbar: a21\n"
        "---\nrep: m12\nbar: a22\n"
        "---\nrep: m22\nbar: a23\n"
        "---\nrep: m12\nbar: a24\n"
        "");

    test_resolve_with_options2(
        yaml, opts_pri_none_all_last, opts_pri_both_all_last,
        "---\nrep: m12\n"
        "---\nrep: m22\n"
        "---\nrep: O3\nbar: a\n"
        "---\nrep: m22\nbar: a00\n"
        "---\nrep: o3\nbar: a01\n"
        "---\nrep: o3\nbar: a02\n"
        "---\nrep: o3\nbar: a03\n"
        "---\nrep: m12\nbar: a04\n"
        "---\nrep: o3\nbar: a05\n"
        "---\nrep: o3\nbar: a06\n"
        "---\nrep: o3\nbar: a07\n"
        "---\nrep: o3\nbar: a08\n"
        "---\nrep: o3\nbar: a09\n"
        "---\nrep: o3\nbar: a10\n"
        "---\nrep: o3\nbar: a11\n"
        "---\nrep: o3\nbar: a12\n"
        "---\nrep: o3\nbar: a13\n"
        "---\nrep: o3\nbar: a14\n"
        "---\nrep: o3\nbar: a15\n"
        "---\nrep: o3\nbar: a16\n"
        "---\nrep: o3\nbar: a17\n"
        "---\nrep: o3\nbar: a18\n"
        "---\nrep: o3\nbar: a19\n"
        "---\nrep: o3\nbar: a20\n"
        "---\nrep: m22\nbar: a21\n"
        "---\nrep: m12\nbar: a22\n"
        "---\nrep: m22\nbar: a23\n"
        "---\nrep: m12\nbar: a24\n"
        "");
}

TEST(anchors, repeated_map)
{
    std::string yaml = R"(map:
  foo: 0
  foo: 1
)";
    test_resolve_with_options0(yaml,                "map:\n  foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_orig, "map:\n  foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_merg, "map:\n  foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_none, "map:\n  foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_both, "map:\n  foo: 0\n  foo: 1\n");
}

TEST(anchors, repeated_seq)
{
    std::string yaml = R"(-
  foo: 0
  foo: 1
)";
    test_resolve_with_options0(yaml,                "- foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_orig, "- foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_merg, "- foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_none, "- foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_both, "- foo: 0\n  foo: 1\n");
}


//-----------------------------------------------------------------------------

TEST(anchors, issue_484_0)
{
    csubstr cases[] = {
        csubstr("base_1: &base_1\n  a: 10\n  b: 10\n"
                "base_2: &base_2\n  b: 20\n"
                "k1:\n  <<: [*base_1,*base_2]\n"
                "k2:\n  <<: [*base_2,*base_1]\n"
                "k3:\n  a: 100\n"
                "  <<: [*base_1,*base_2]\n"
                "k4:\n  <<: [*base_1,*base_2]\n  a: 100\n"
                "k5:\n  a: 100\n  <<: [*base_2,*base_1]\n"
                "k6:\n  <<: [*base_2,*base_1]\n  a: 100\n"
                ""),
        csubstr(""
                "base_1: &base_1\n  a: 10\n  b: 10\n"
                "base_2: &base_2\n  b: 20\nk1:\n"
                "  <<: *base_1\n  <<: *base_2\n"
                "k2:\n  <<: *base_2\n  <<: *base_1\n"
                "k3:\n  a: 100\n  <<: *base_1\n  <<: *base_2\n"
                "k4:\n  <<: *base_1\n  <<: *base_2\n  a: 100\n"
                "k5:\n  a: 100\n  <<: *base_2\n  <<: *base_1\n"
                "k6:\n  <<: *base_2\n  <<: *base_1\n  a: 100\n"
                ""),
        csubstr(""
                "base_1: &base_1\n  a: 10\n  b: 10\n"
                "base_2: &base_2\n  b: 20\n"
                "k1:\n  !!merge <<: [*base_1,*base_2]\n"
                "k2:\n  !!merge <<: [*base_2,*base_1]\n"
                "k3:\n  a: 100\n  !!merge <<: [*base_1,*base_2]\n"
                "k4:\n  !!merge <<: [*base_1,*base_2]\n  a: 100\n"
                "k5:\n  a: 100\n  !!merge <<: [*base_2,*base_1]\n"
                "k6:\n  !!merge <<: [*base_2,*base_1]\n  a: 100\n"
                ""),
        csubstr(""
                "base_1: &base_1\n  a: 10\n  b: 10\n"
                "base_2: &base_2\n  b: 20\n"
                "k1:\n  !!merge <<: *base_1\n  !!merge <<: *base_2\n"
                "k2:\n  !!merge <<: *base_2\n  !!merge <<: *base_1\n"
                "k3:\n  a: 100\n  !!merge <<: *base_1\n  !!merge <<: *base_2\n"
                "k4:\n  !!merge <<: *base_1\n  !!merge <<: *base_2\n  a: 100\n"
                "k5:\n  a: 100\n  !!merge <<: *base_2\n  !!merge <<: *base_1\n"
                "k6:\n  !!merge <<: *base_2\n  !!merge <<: *base_1\n  a: 100\n"
                ""),
    };
    size_t count = 0;
    for(csubstr yaml : cases)
    {
        RYML_TRACE_FMT("case #{}", count++);
        TreeAndInts ti(to_csubstr(yaml));
        test_resolve_(ti);
        if(testing::Test::HasFailure()) break;
        test_emit_yaml_(ti,
                        "base_1:\n  a: 10\n  b: 10\n"
                        "base_2:\n  b: 20\n"
                        "k1:\n  a: 10\n  b: 10\n"
                        "k2:\n  b: 20\n  a: 10\n"
                        "k3:\n  a: 100\n  b: 10\n"
                        "k4:\n  a: 100\n  b: 10\n"
                        "k5:\n  a: 100\n  b: 20\n"
                        "k6:\n  b: 20\n  a: 100\n"
                        "");
        if(testing::Test::HasFailure()) break;
    }
}

TEST(anchors, issue_484_0_flow_sl)
{
    csubstr cases[] = {
        csubstr(""
                "[\n"
                "  {base_1: &base_1 {a: 10,b: 10}},\n"
                "  {base_2: &base_2 {b: 20}},\n"
                "  {k1: {<<: [*base_1,*base_2]}},\n"
                "  {k2: {<<: [*base_2,*base_1]}},\n"
                "  {k3: {a: 100,<<: [*base_1,*base_2]}},\n"
                "  {k4: {<<: [*base_1,*base_2],a: 100}},\n"
                "  {k5: {a: 100,<<: [*base_2,*base_1]}},\n"
                "  {k6: {<<: [*base_2,*base_1],a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {base_1: &base_1 {a: 10,b: 10}},\n"
                "  {base_2: &base_2 {b: 20}},\n"
                "  {k1: {<<: *base_1,<<: *base_2}},\n"
                "  {k2: {<<: *base_2,<<: *base_1}},\n"
                "  {k3: {a: 100,<<: *base_1,<<: *base_2}},\n"
                "  {k4: {<<: *base_1,<<: *base_2,a: 100}},\n"
                "  {k5: {a: 100,<<: *base_2,<<: *base_1}},\n"
                "  {k6: {<<: *base_2,<<: *base_1,a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {base_1: &base_1 {a: 10,b: 10}},\n"
                "  {base_2: &base_2 {b: 20}},\n"
                "  {k1: {!!merge <<: [*base_1,*base_2]}},\n"
                "  {k2: {!!merge <<: [*base_2,*base_1]}},\n"
                "  {k3: {a: 100,!!merge <<: [*base_1,*base_2]}},\n"
                "  {k4: {!!merge <<: [*base_1,*base_2],a: 100}},\n"
                "  {k5: {a: 100,!!merge <<: [*base_2,*base_1]}},\n"
                "  {k6: {!!merge <<: [*base_2,*base_1],a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {base_1: &base_1 {a: 10,b: 10}},\n"
                "  {base_2: &base_2 {b: 20}},\n"
                "  {k1: {!!merge <<: *base_1,!!merge <<: *base_2}},\n"
                "  {k2: {!!merge <<: *base_2,!!merge <<: *base_1}},\n"
                "  {k3: {a: 100,!!merge <<: *base_1,!!merge <<: *base_2}},\n"
                "  {k4: {!!merge <<: *base_1,!!merge <<: *base_2,a: 100}},\n"
                "  {k5: {a: 100,!!merge <<: *base_2,!!merge <<: *base_1}},\n"
                "  {k6: {!!merge <<: *base_2,!!merge <<: *base_1,a: 100}}\n"
                "]"
                ""),
    };
    size_t count = 0;
    for(csubstr yaml : cases)
    {
        RYML_TRACE_FMT("case #{}", count++);
        TreeAndInts ti(to_csubstr(yaml));
        test_resolve_(ti);
        if(testing::Test::HasFailure()) break;
        test_emit_yaml_(ti,
                        ""
                        "[\n"
                        "  {base_1: {a: 10,b: 10}},\n"
                        "  {base_2: {b: 20}},\n"
                        "  {k1: {a: 10,b: 10}},\n"
                        "  {k2: {b: 20,a: 10}},\n"
                        "  {k3: {a: 100,b: 10}},\n"
                        "  {k4: {a: 100,b: 10}},\n"
                        "  {k5: {a: 100,b: 20}},\n"
                        "  {k6: {b: 20,a: 100}}\n"
                        "]\n"
                        "");
        if(testing::Test::HasFailure()) break;
    }
}

TEST(anchors, issue_484_0_flow_ml)
{
    csubstr cases[] = {
        csubstr(""
                "[\n"
                "  {\nbase_1: &base_1 {\na: 10,b: 10}},\n"
                "  {\nbase_2: &base_2 {\nb: 20}},\n"
                "  {\nk1: {\n<<: [*base_1,*base_2]}},\n"
                "  {\nk2: {\n<<: [*base_2,*base_1]}},\n"
                "  {\nk3: {\na: 100,<<: [*base_1,*base_2]}},\n"
                "  {\nk4: {\n<<: [*base_1,*base_2],a: 100}},\n"
                "  {\nk5: {\na: 100,<<: [*base_2,*base_1]}},\n"
                "  {\nk6: {\n<<: [*base_2,*base_1],a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {\nbase_1: &base_1 {\na: 10,b: 10}},\n"
                "  {\nbase_2: &base_2 {\nb: 20}},\n"
                "  {\nk1: {\n<<: *base_1,<<: *base_2}},\n"
                "  {\nk2: {\n<<: *base_2,<<: *base_1}},\n"
                "  {\nk3: {\na: 100,<<: *base_1,<<: *base_2}},\n"
                "  {\nk4: {\n<<: *base_1,<<: *base_2,a: 100}},\n"
                "  {\nk5: {\na: 100,<<: *base_2,<<: *base_1}},\n"
                "  {\nk6: {\n<<: *base_2,<<: *base_1,a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {\nbase_1: &base_1 {\na: 10,b: 10}},\n"
                "  {\nbase_2: &base_2 {\nb: 20}},\n"
                "  {\nk1: {\n!!merge <<: [*base_1,*base_2]}},\n"
                "  {\nk2: {\n!!merge <<: [*base_2,*base_1]}},\n"
                "  {\nk3: {\na: 100,!!merge <<: [*base_1,*base_2]}},\n"
                "  {\nk4: {\n!!merge <<: [*base_1,*base_2],a: 100}},\n"
                "  {\nk5: {\na: 100,!!merge <<: [*base_2,*base_1]}},\n"
                "  {\nk6: {\n!!merge <<: [*base_2,*base_1],a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {\nbase_1: &base_1 {\na: 10,b: 10}},\n"
                "  {\nbase_2: &base_2 {\nb: 20}},\n"
                "  {\nk1: {\n!!merge <<: *base_1,!!merge <<: *base_2}},\n"
                "  {\nk2: {\n!!merge <<: *base_2,!!merge <<: *base_1}},\n"
                "  {\nk3: {\na: 100,!!merge <<: *base_1,!!merge <<: *base_2}},\n"
                "  {\nk4: {\n!!merge <<: *base_1,!!merge <<: *base_2,a: 100}},\n"
                "  {\nk5: {\na: 100,!!merge <<: *base_2,!!merge <<: *base_1}},\n"
                "  {\nk6: {\n!!merge <<: *base_2,!!merge <<: *base_1,a: 100}}\n"
                "]"
                ""),
    };
    size_t count = 0;
    for(csubstr yaml : cases)
    {
        RYML_TRACE_FMT("case #{}", count++);
        TreeAndInts ti(to_csubstr(yaml));
        test_resolve_(ti);
        if(testing::Test::HasFailure()) break;
        test_emit_yaml_(ti,
                        ""
                        "[\n"
                        "  {\n    base_1: {\n      a: 10,\n      b: 10\n    }\n  },\n"
                        "  {\n    base_2: {\n      b: 20\n    }\n  },\n"
                        "  {\n    k1: {\n      a: 10,\n      b: 10\n    }\n  },\n"
                        "  {\n    k2: {\n      b: 20,\n      a: 10\n    }\n  },\n"
                        "  {\n    k3: {\n      a: 100,\n      b: 10\n    }\n  },\n"
                        "  {\n    k4: {\n      a: 100,\n      b: 10\n    }\n  },\n"
                        "  {\n    k5: {\n      a: 100,\n      b: 20\n    }\n  },\n"
                        "  {\n    k6: {\n      b: 20,\n      a: 100\n    }\n  }\n"
                        "]\n"
                        "");
        if(testing::Test::HasFailure()) break;
    }
}


TEST(anchors, issue_484_1)
{
    std::string yaml = ""
        "base_1: &base_1\n  a: a1\n"
        "base_2: &base_2\n  b: b2\n"
        "k1:\n  <<: *base_1\n  <<: *base_2\n"
        "k2:\n  <<: *base_2\n  <<: *base_1\n"
        "k3:\n  a: ak3\n  <<: *base_1\n  <<: *base_2\n"
        "k4:\n  <<: *base_1\n  <<: *base_2\n  a: ak4\n"
        "";
    test_resolve_with_options0(yaml,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: ak3\n  b: b2\n"
                               "k4:\n  a: ak4\n  b: b2\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: ak3\n  b: b2\n"
                               "k4:\n  a: ak4\n  b: b2\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: a1\n  b: b2\n"
                               "k4:\n  a: a1\n  b: b2\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: ak3\n  b: b2\n"
                               "k4:\n  a: a1\n  b: b2\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: ak3\n  b: b2\n"
                               "k4:\n  a: ak4\n  b: b2\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: a1\n  b: b2\n"
                               "k4:\n  a: a1\n  b: b2\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: a1\n  b: b2\n"
                               "k4:\n  a: ak4\n  b: b2\n"
                               "");
}

TEST(anchors, issue_484_1_flow_sl)
{
    std::string yaml = ""
        "[\n"
        "  {base_1: &base_1 {a: a1}},\n"
        "  {base_2: &base_2 {b: b2}},\n"
        "  {k1: {<<: *base_1,<<: *base_2}},\n"
        "  {k2: {<<: *base_2,<<: *base_1}},\n"
        "  {k3: {a: ak3,<<: *base_1,<<: *base_2}},\n"
        "  {k4: {<<: *base_1,<<: *base_2,a: ak4}}\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: ak3,b: b2}},\n"
                               "  {k4: {a: ak4,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: ak3,b: b2}},\n"
                               "  {k4: {a: ak4,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: a1,b: b2}},\n"
                               "  {k4: {a: a1,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: ak3,b: b2}},\n"
                               "  {k4: {a: a1,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: ak3,b: b2}},\n"
                               "  {k4: {a: ak4,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: a1,b: b2}},\n"
                               "  {k4: {a: a1,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: a1,b: b2}},\n"
                               "  {k4: {a: ak4,b: b2}}\n"
                               "]\n"
                               "");
}

TEST(anchors, issue_484_1_flow_ml)
{
    std::string yaml = ""
        "[\n"
        "  {\nbase_1: &base_1 {\na: a1}},\n"
        "  {\nbase_2: &base_2 {\nb: b2}},\n"
        "  {\nk1: {\n<<: *base_1,<<: *base_2}},\n"
        "  {\nk2: {\n<<: *base_2,<<: *base_1}},\n"
        "  {\nk3: {\na: ak3,<<: *base_1,<<: *base_2}},\n"
        "  {\nk4: {\n<<: *base_1,<<: *base_2,a: ak4}}\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
}

TEST(anchors, issue_484_2)
{
    std::string yaml = ""
        "base_1: &base_1\n  a: a1\n  b: b1\n"
        "base_2: &base_2\n  a: a2\n"
        "k1:\n  <<: *base_1\n  <<: *base_2\n"
        "k2:\n  <<: *base_2\n  <<: *base_1\n"
        "k3:\n  a: ak3\n  <<: *base_1\n  <<: *base_2\n"
        "k4:\n  <<: *base_1\n  <<: *base_2\n  a: ak4\n"
        "";
    test_resolve_with_options0(yaml,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  a: a2\n  b: b1\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  a: a2\n  b: b1\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  a: a2\n  b: b1\n"
                               "k3:\n  a: a1\n  b: b1\n"
                               "k4:\n  a: a1\n  b: b1\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  a: a2\n  b: b1\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: a1\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a2\n  b: b1\n"
                               "k2:\n  a: a1\n  b: b1\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a2\n  b: b1\n"
                               "k2:\n  a: a1\n  b: b1\n"
                               "k3:\n  a: a2\n  b: b1\n"
                               "k4:\n  a: a2\n  b: b1\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a2\n  b: b1\n"
                               "k2:\n  a: a1\n  b: b1\n"
                               "k3:\n  a: a2\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "");
}

TEST(anchors, issue_484_2_flow_sl)
{
    std::string yaml = ""
        "[\n"
        "  {base_1: &base_1 {a: a1,b: b1}},\n"
        "  {base_2: &base_2 {a: a2}},\n"
        "  {k1: {<<: *base_1,<<: *base_2}},\n"
        "  {k2: {<<: *base_2,<<: *base_1}},\n"
        "  {k3: {a: ak3,<<: *base_1,<<: *base_2}},\n"
        "  {k4: {<<: *base_1,<<: *base_2,a: ak4}},\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {a: a2,b: b1}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {a: a2,b: b1}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {a: a2,b: b1}},\n"
                               "  {k3: {a: a1,b: b1}},\n"
                               "  {k4: {a: a1,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {a: a2,b: b1}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: a1,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a2,b: b1}},\n"
                               "  {k2: {a: a1,b: b1}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a2,b: b1}},\n"
                               "  {k2: {a: a1,b: b1}},\n"
                               "  {k3: {a: a2,b: b1}},\n"
                               "  {k4: {a: a2,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a2,b: b1}},\n"
                               "  {k2: {a: a1,b: b1}},\n"
                               "  {k3: {a: a2,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}}\n"
                               "]\n"
                               "");
}

TEST(anchors, issue_484_2_flow_ml)
{
    std::string yaml = ""
        "[\n"
        "  {\nbase_1: &base_1 {\na: a1,b: b1}},\n"
        "  {\nbase_2: &base_2 {\na: a2}},\n"
        "  {\nk1: {\n<<: *base_1,<<: *base_2}},\n"
        "  {\nk2: {\n<<: *base_2,<<: *base_1}},\n"
        "  {\nk3: {\na: ak3,<<: *base_1,<<: *base_2}},\n"
        "  {\nk4: {\n<<: *base_1,<<: *base_2,a: ak4}},\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a2,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
}

TEST(anchors, issue_484_3)
{
    std::string yaml = ""
        "base_1: &base_1\n" "  a: a1\n" "  b: b1\n"
        "base_2: &base_2\n" "  a: a2\n"
        "base_3: &base_3\n" "  b: b3\n"
        "k1:\n" "  <<: *base_1\n" "  <<: *base_2\n" "  <<: *base_3\n"
        "k2:\n" "  <<: *base_3\n" "  <<: *base_2\n" "  <<: *base_1\n"
        "k3:\n" "  a: ak3\n" "  <<: *base_1\n" "  <<: *base_2\n" "  <<: *base_3\n"
        "k4:\n" "  <<: *base_1\n" "  a: ak4\n" "  <<: *base_2\n" "  <<: *base_3\n"
        "k5:\n" "  <<: *base_1\n" "  <<: *base_2\n" "  <<: *base_3\n" "  a: ak5\n"
        "";
    test_resolve_with_options0(yaml,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  b: b3\n  a: a2\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "k5:\n  a: ak5\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  b: b3\n  a: a2\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "k5:\n  a: ak5\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  b: b3\n  a: a2\n"
                               "k3:\n  a: a1\n  b: b1\n"
                               "k4:\n  a: a1\n  b: b1\n"
                               "k5:\n  a: a1\n  b: b1\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  b: b3\n  a: a2\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: a1\n  b: b1\n"
                               "k5:\n  a: a1\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a2\n  b: b3\n"
                               "k2:\n  b: b1\n  a: a1\n"
                               "k3:\n  a: ak3\n  b: b3\n"
                               "k4:\n  a: ak4\n  b: b3\n"
                               "k5:\n  a: ak5\n  b: b3\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a2\n  b: b3\n"
                               "k2:\n  b: b1\n  a: a1\n"
                               "k3:\n  a: a2\n  b: b3\n"
                               "k4:\n  a: a2\n  b: b3\n"
                               "k5:\n  a: a2\n  b: b3\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a2\n  b: b3\n"
                               "k2:\n  b: b1\n  a: a1\n"
                               "k3:\n  a: a2\n  b: b3\n"
                               "k4:\n  a: a2\n  b: b3\n"
                               "k5:\n  a: ak5\n  b: b3\n"
                               "");
}

TEST(anchors, issue_484_3_flow_sl)
{
    std::string yaml = ""
        "[\n"
        "  {base_1: &base_1 {a: a1,b: b1}},\n"
        "  {base_2: &base_2 {a: a2}},\n"
        "  {base_3: &base_3 {b: b3}},\n"
        "  {k1: {<<: *base_1,<<: *base_2,<<: *base_3}},\n"
        "  {k2: {<<: *base_3,<<: *base_2,<<: *base_1}},\n"
        "  {k3: {a: ak3,<<: *base_1,<<: *base_2,<<: *base_3}},\n"
        "  {k4: {<<: *base_1,a: ak4,<<: *base_2,<<: *base_3}},\n"
        "  {k5: {<<: *base_1,<<: *base_2,<<: *base_3,a: ak5}}\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {b: b3,a: a2}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}},\n"
                               "  {k5: {a: ak5,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {b: b3,a: a2}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}},\n"
                               "  {k5: {a: ak5,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {b: b3,a: a2}},\n"
                               "  {k3: {a: a1,b: b1}},\n"
                               "  {k4: {a: a1,b: b1}},\n"
                               "  {k5: {a: a1,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {b: b3,a: a2}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: a1,b: b1}},\n"
                               "  {k5: {a: a1,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a2,b: b3}},\n"
                               "  {k2: {b: b1,a: a1}},\n"
                               "  {k3: {a: ak3,b: b3}},\n"
                               "  {k4: {a: ak4,b: b3}},\n"
                               "  {k5: {a: ak5,b: b3}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a2,b: b3}},\n"
                               "  {k2: {b: b1,a: a1}},\n"
                               "  {k3: {a: a2,b: b3}},\n"
                               "  {k4: {a: a2,b: b3}},\n"
                               "  {k5: {a: a2,b: b3}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a2,b: b3}},\n"
                               "  {k2: {b: b1,a: a1}},\n"
                               "  {k3: {a: a2,b: b3}},\n"
                               "  {k4: {a: a2,b: b3}},\n"
                               "  {k5: {a: ak5,b: b3}}\n"
                               "]\n"
                               "");
}

TEST(anchors, issue_484_3_flow_ml)
{
    std::string yaml = ""
        "[\n"
        "  {\n    base_1: &base_1 {\na: a1,b: b1}},\n"
        "  {\n    base_2: &base_2 {\na: a2}},\n"
        "  {\n    base_3: &base_3 {\nb: b3}},\n"
        "  {\n    k1: {\n<<: *base_1,<<: *base_2,<<: *base_3}},\n"
        "  {\n    k2: {\n<<: *base_3,<<: *base_2,<<: *base_1}},\n"
        "  {\n    k3: {\na: ak3,<<: *base_1,<<: *base_2,<<: *base_3}},\n"
        "  {\n    k4: {\n<<: *base_1,a: ak4,<<: *base_2,<<: *base_3}},\n"
        "  {\n    k5: {\n<<: *base_1,<<: *base_2,<<: *base_3,a: ak5}}\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b3,\n      a: a2\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  },\n"
                               "  {\n    k5: {\n      a: ak5,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b3,\n      a: a2\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  },\n"
                               "  {\n    k5: {\n      a: ak5,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b3,\n      a: a2\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k5: {\n      a: a1,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b3,\n      a: a2\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k5: {\n      a: a1,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b1,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b3\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b3\n    }\n  },\n"
                               "  {\n    k5: {\n      a: ak5,\n      b: b3\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b1,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k5: {\n      a: a2,\n      b: b3\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b1,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k5: {\n      a: ak5,\n      b: b3\n    }\n  }\n"
                               "]\n"
                               "");
}


TEST(anchors, key_anchor_error_json)
{
    TreeAndInts ti("{&k key: val}");
    test_emit_json_(ti, "{\"key\": \"val\"}");
    test_emit_error_json_(ti, EmitOptions{}.json_err_on_anchor(true));
}

TEST(anchors, val_anchor_error_json)
{
    TreeAndInts ti("{key: &v val}");
    test_emit_json_(ti, "{\"key\": \"val\"}");
    test_emit_error_json_(ti, EmitOptions{}.json_err_on_anchor(true));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(SIMPLE_ANCHOR)
{

ADD_CASE_TO_GROUP("anchor colon ambiguity 1",
R"({*foo : bar})",
N(MFS, L{N(KEY|VP, "*foo", AR(KEYREF, "foo"), "bar")})
);

ADD_CASE_TO_GROUP("anchor colon ambiguity 2", EXPECT_PARSE_ERROR,
R"({*foo: bar})",
   Location(1, 8)
);

ADD_CASE_TO_GROUP("merge example, unresolved",
R"(# https://yaml.org/type/merge.html
- &CENTER { x: 1, y: 2 }
- &LEFT { x: 0, y: 2 }
- &BIG { r: 10 }
- &SMALL { r: 1 }

# All the following maps are equal:

- # Explicit keys
  x: 1
  y: 2
  r: 10
  label: center/big

- # Merge one map
  << : *CENTER
  r: 10
  label: center/big

- # Merge multiple maps
  << : [ *CENTER, *BIG ]
  label: center/big

- # Override
  << : [ *BIG, *LEFT, *SMALL ]
  x: 1
  label: center/big
)",
N(SB, L{
    N(MFS, L{N(KP|VP, "x", "1" ), N(KP|VP, "y", "2")}, AR(VALANCH, "CENTER")),
    N(MFS, L{N(KP|VP, "x", "0" ), N(KP|VP, "y", "2")}, AR(VALANCH, "LEFT"  )),
    N(MFS, L{N(KP|VP, "r", "10")}, AR(VALANCH, "BIG"   )),
    N(MFS, L{N(KP|VP, "r", "1" )}, AR(VALANCH, "SMALL" )),
    N(MB, L{N(KP|VP, "x", "1" ), N(KP|VP, "y", "2"), N(KP|VP, "r", "10"), N(KP|VP, "label", "center/big")}),
    N(MB, L{N(KP|VAL, "<<", "*CENTER", AR(VALREF, "*CENTER")), N(KP|VP, "r", "10"), N(KP|VP, "label", "center/big")}),
    N(MB, L{N(KP|SFS, "<<", L{N(VAL, "*CENTER", AR(VALREF, "*CENTER")), N(VAL, "*BIG", AR(VALREF, "*BIG"))}), N(KP|VP, "label", "center/big")}),
    N(MB, L{N(KP|SFS, "<<", L{N(VAL, "*BIG", AR(VALREF, "*BIG")), N(VAL, "*LEFT", AR(VALREF, "*LEFT")), N(VAL, "*SMALL", AR(VALREF, "*SMALL"))}), N(KP|VP, "x", "1"), N(KP|VP, "label", "center/big")}),
})
);

ADD_CASE_TO_GROUP("merge example, resolved", RESOLVE_REFS,
R"(# https://yaml.org/type/merge.html
- &CENTER { x: 1, y: 2 }
- &LEFT { x: 0, y: 2 }
- &BIG { r: 10 }
- &SMALL { r: 1 }

# All the following maps are equal:

- # Explicit keys
  x: 1
  y: 2
  r: 10
  label: center/big0

- # Merge one map
  << : *CENTER
  r: 10
  label: center/big1

- # Merge multiple maps
  << : [ *CENTER, *BIG ]
  label: center/big2

- # Override
  << : [ *SMALL, *LEFT, *BIG ]
  x: 1
  label: center/big3
)",
N(SB, L{
    N(MFS, L{N(KP|VP, "x", "1" ), N(KP|VP, "y", "2")}),
    N(MFS, L{N(KP|VP, "x", "0" ), N(KP|VP, "y", "2")}),
    N(MFS, L{N(KP|VP, "r", "10")}),
    N(MFS, L{N(KP|VP, "r", "1" )}),
    N(MB, L{N(KP|VP, "x", "1"), N(KP|VP, "y", "2"), N(KP|VP, "r", "10"), N(KP|VP, "label", "center/big0")}),
    N(MB, L{N(KP|VP, "x", "1"), N(KP|VP, "y", "2"), N(KP|VP, "r", "10"), N(KP|VP, "label", "center/big1")}),
    N(MB, L{N(KP|VP, "x", "1"), N(KP|VP, "y", "2"), N(KP|VP, "r", "10"), N(KP|VP, "label", "center/big2")}),
    N(MB, L{N(KP|VP, "r", "1"), N(KP|VP, "x", "1"), N(KP|VP, "y", "2" ), N(KP|VP, "label", "center/big3")}),
})
);

ADD_CASE_TO_GROUP("simple anchor 1, implicit, unresolved",
R"(
anchored_content: &anchor_name This string will appear as the value of two keys.
other_anchor: *anchor_name
anchors_in_seqs:
  - &anchor_in_seq this value appears in both elements of the sequence
  - *anchor_in_seq
base: &base
    name: Everyone has same name
foo: &foo
    <<: *base
    age: 10
bar: &bar
    <<: *base
    age: 20
)",
N(MB, L{
      N(KP|VP, "anchored_content", "This string will appear as the value of two keys.", AR(VALANCH, "anchor_name")),
      N(KP|VAL, "other_anchor", "*anchor_name", AR(VALREF, "anchor_name")),
      N(KP|SB, "anchors_in_seqs", L{
              N(VP, "this value appears in both elements of the sequence", AR(VALANCH, "anchor_in_seq")),
              N(VAL, "*anchor_in_seq", AR(VALREF, "anchor_in_seq")),
          }),
      N(KP|MB, "base", L{N(KP|VP, "name", "Everyone has same name")}, AR(VALANCH, "base")),
      N(KP|MB, "foo", L{N(KP|VAL, "<<", "*base", AR(VALREF, "base")), N(KP|VP, "age", "10")}, AR(VALANCH, "foo")),
      N(KP|MB, "bar", L{N(KP|VAL, "<<", "*base", AR(VALREF, "base")), N(KP|VP, "age", "20")}, AR(VALANCH, "bar")),
  })
);

ADD_CASE_TO_GROUP("simple anchor 1, explicit, unresolved",
R"({
anchored_content: &anchor_name This string will appear as the value of two keys.,
other_anchor: *anchor_name,
anchors_in_seqs: [
  &anchor_in_seq this value appears in both elements of the sequence,
  *anchor_in_seq
  ],
base: &base {
    name: Everyone has same name
  },
foo: &foo {
    <<: *base,
    age: 10
  },
bar: &bar {
    <<: *base,
    age: 20
  }
})",
N(MFM,  L{
      N(KP|VP, "anchored_content", "This string will appear as the value of two keys.", AR(VALANCH, "anchor_name")),
      N(KP|VAL, "other_anchor", "*anchor_name", AR(VALREF, "anchor_name")),
      N(KP|SFM, "anchors_in_seqs", L{
              N(VP, "this value appears in both elements of the sequence", AR(VALANCH, "anchor_in_seq")),
              N(VAL, "*anchor_in_seq", AR(VALREF, "anchor_in_seq")),
          }),
      N(KP|MFM, "base", L{N(KP|VP, "name", "Everyone has same name")}, AR(VALANCH, "base")),
      N(KP|MFM, "foo", L{N(KP|VAL, "<<", "*base", AR(VALREF, "base")), N(KP|VP, "age", "10")}, AR(VALANCH, "foo")),
      N(KP|MFM, "bar", L{N(KP|VAL, "<<", "*base", AR(VALREF, "base")), N(KP|VP, "age", "20")}, AR(VALANCH, "bar")),
  })
);

ADD_CASE_TO_GROUP("simple anchor 1, implicit, resolved", RESOLVE_REFS,
R"(
anchored_content: &anchor_name This string will appear as the value of two keys.
other_anchor: *anchor_name
anchors_in_seqs:
  - &anchor_in_seq this value appears in both elements of the sequence
  - *anchor_in_seq
base: &base
    name: Everyone has same name
foo: &foo
    <<: *base
    age: 10
bar: &bar
    <<: *base
    age: 20
)",
N(MB, L{
      N(KP|VP, "anchored_content", "This string will appear as the value of two keys."),
      N(KP|VP, "other_anchor", "This string will appear as the value of two keys."),
      N(KP|SB, "anchors_in_seqs", L{
              N(VP, "this value appears in both elements of the sequence"),
              N(VP, "this value appears in both elements of the sequence"),
          }),
      N(KP|MB, "base", L{N(KP|VP, "name", "Everyone has same name")}),
      N(KP|MB, "foo", L{N(KP|VP, "name", "Everyone has same name"), N(KP|VP, "age", "10")}),
      N(KP|MB, "bar", L{N(KP|VP, "name", "Everyone has same name"), N(KP|VP, "age", "20")}),
  })
);

ADD_CASE_TO_GROUP("simple anchor 1, explicit, resolved", RESOLVE_REFS,
R"({
anchored_content: &anchor_name This string will appear as the value of two keys.,
other_anchor: *anchor_name,
anchors_in_seqs: [
  &anchor_in_seq this value appears in both elements of the sequence,
  *anchor_in_seq
  ],
base: &base {
    name: Everyone has same name
  },
foo: &foo {
    <<: *base,
    age: 10
  },
bar: &bar {
    <<: *base,
    age: 20
  }
})",
N(MFM, L{
      N(KP|VP, "anchored_content", "This string will appear as the value of two keys."),
      N(KP|VP, "other_anchor", "This string will appear as the value of two keys."),
      N(KP|SFM, "anchors_in_seqs", L{
              N(VP, "this value appears in both elements of the sequence"),
              N(VP, "this value appears in both elements of the sequence"),
          }),
      N(KP|MFM, "base", L{N(KP|VP, "name", "Everyone has same name")}),
      N(KP|MFM, "foo", L{N(KP|VP, "name", "Everyone has same name"), N(KP|VP, "age", "10")}),
      N(KP|MFM, "bar", L{N(KP|VP, "name", "Everyone has same name"), N(KP|VP, "age", "20")}),
  })
);


ADD_CASE_TO_GROUP("anchor example 2, unresolved",
R"(
receipt:     Oz-Ware Purchase Invoice
date:        2012-08-06
customer:
    first_name:   Dorothy
    family_name:  Gale
items:
    - part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4
    - part_no:   E1628
      descrip:   High Heeled "Ruby" Slippers
      size:      8
      price:     133.7
      quantity:  1
bill-to:  &id001
    street: |
            123 Tornado Alley
            Suite 16
    city:   East Centerville
    state:  KS
ship-to:  *id001
specialDelivery:  >
    Follow the Yellow Brick
    Road to the Emerald City.
    Pay no attention to the
    man behind the curtain.
)",
N(MB, L{
  N{KP|VP, "receipt", "Oz-Ware Purchase Invoice"},
  N{KP|VP, "date",    "2012-08-06"},
  N{KP|MB, "customer", L{N{KP|VP, "first_name", "Dorothy"}, N{KP|VP, "family_name", "Gale"}}},
  N{KP|SB, "items", L{
    N{MB, L{N{KP|VP, "part_no",   "A4786"},
        N{KP|VP, "descrip",   "Water Bucket (Filled)"},
        N{KP|VP, "price",     "1.47"},
        N{KP|VP, "quantity",  "4"},}},
    N{MB, L{N{KP|VP, "part_no", "E1628"},
        N{KP|VP, "descrip",   "High Heeled \"Ruby\" Slippers"},
        N{KP|VP, "size",      "8"},
        N{KP|VP, "price",     "133.7"},
        N{KP|VP, "quantity",  "1"},}}}},
   N{KP|MB, "bill-to", L{
        N{KP|VL, "street", "123 Tornado Alley\nSuite 16\n"},
        N{KP|VP, "city", "East Centerville"},
        N{KP|VP, "state", "KS"},}, AR(VALANCH, "id001")},
   N{KP|VAL, "ship-to", "*id001", AR(VALREF, "id001")},
   N{KP|VF, "specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
  })
);


ADD_CASE_TO_GROUP("anchor example 2, resolved", RESOLVE_REFS,
R"(
receipt:     Oz-Ware Purchase Invoice
date:        2012-08-06
customer:
    first_name:   Dorothy
    family_name:  Gale
items:
    - part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4
    - part_no:   E1628
      descrip:   High Heeled "Ruby" Slippers
      size:      8
      price:     133.7
      quantity:  1
bill-to:  &id001
    street: |
            123 Tornado Alley
            Suite 16
    city:   East Centerville
    state:  KS
ship-to:  *id001
specialDelivery:  >
    Follow the Yellow Brick
    Road to the Emerald City.
    Pay no attention to the
    man behind the curtain.
)",
N(MB, L{
  N{KP|VP, "receipt", "Oz-Ware Purchase Invoice"},
  N{KP|VP, "date",    "2012-08-06"},
  N{KP|MB, "customer", L{
    N{KP|VP, "first_name", "Dorothy"},
    N{KP|VP, "family_name", "Gale"}
  }},
  N{KP|SB, "items", L{
    N{MB, L{
      N{KP|VP, "part_no",   "A4786"},
      N{KP|VP, "descrip",   "Water Bucket (Filled)"},
      N{KP|VP, "price",     "1.47"},
      N{KP|VP, "quantity",  "4"},
    }},
    N{MB, L{
      N{KP|VP, "part_no", "E1628"},
      N{KP|VP, "descrip",   "High Heeled \"Ruby\" Slippers"},
      N{KP|VP, "size",      "8"},
      N{KP|VP, "price",     "133.7"},
      N{KP|VP, "quantity",  "1"},
    }}
  }},
  N{KP|MB, "bill-to", L{
    N{KP|VL, "street", "123 Tornado Alley\nSuite 16\n"},
    N{KP|VP, "city", "East Centerville"},
    N{KP|VP, "state", "KS"},
  }},
  N{KP|MB, "ship-to", L{
    N{KP|VL, "street", "123 Tornado Alley\nSuite 16\n"},
    N{KP|VP, "city", "East Centerville"},
    N{KP|VP, "state", "KS"},
  }},
  N{KP|VF, "specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
})
);

ADD_CASE_TO_GROUP("anchor example 3, unresolved",
R"(
- step:  &id001                  # defines anchor label &id001
    instrument:      Lasik 2000
    pulseEnergy:     5.4
    pulseDuration:   12
    repetition:      1000
    spotSize:        1mm
- step: &id002
    instrument:      Lasik 2000
    pulseEnergy:     5.0
    pulseDuration:   10
    repetition:      500
    spotSize:        2mm
- step: *id001                   # refers to the first step (with anchor &id001)
- step: *id002                   # refers to the second step
- step:
    <<: *id001
    spotSize: 2mm                # redefines just this key, refers rest from &id001
- step: *id002
)",
N(SB, L{
  N(MB, L{
    N(KP|MB, "step", L{
      N{KP|VP, "instrument",      "Lasik 2000"},
      N{KP|VP, "pulseEnergy",     "5.4"},
      N{KP|VP, "pulseDuration",   "12"},
      N{KP|VP, "repetition",      "1000"},
      N{KP|VP, "spotSize",        "1mm"},
    }, AR(VALANCH, "id001")),
  }),
  N(MB, L{
    N(KP|MB, "step", L{
      N{KP|VP, "instrument",      "Lasik 2000"},
      N{KP|VP, "pulseEnergy",     "5.0"},
      N{KP|VP, "pulseDuration",   "10"},
      N{KP|VP, "repetition",      "500"},
      N{KP|VP, "spotSize",        "2mm"},
    }, AR(VALANCH, "id002")),
  }),
  N(MB, L{
    N{KP|VAL, "step", "*id001", AR(VALREF, "id001")},
  }),
  N(MB, L{
    N{KP|VAL, "step", "*id002", AR(VALREF, "id002")},
  }),
  N(MB, L{
    N{KP|MB, "step", L{
      N{KP|VAL, "<<", "*id001", AR(VALREF, "id002")},
      N{KP|VP, "spotSize", "2mm"},
    }},
  }),
  N(MB, L{
    N{KP|VAL, "step", "*id002", AR(VALREF, "id002")},
  }),
})
);

ADD_CASE_TO_GROUP("anchor example 3, resolved", RESOLVE_REFS,
R"(
- step:  &id001                  # defines anchor label &id001
    instrument:      Lasik 2000
    pulseEnergy:     5.4
    pulseDuration:   12
    repetition:      1000
    spotSize:        1mm
- step: &id002
    instrument:      Lasik 2000
    pulseEnergy:     5.0
    pulseDuration:   10
    repetition:      500
    spotSize:        2mm
- step: *id001                   # refers to the first step (with anchor &id001)
- step: *id002                   # refers to the second step
- step:
    <<: *id001
    spotSize: 2mm                # redefines just this key, refers rest from &id001
- step: *id002
)",
N(SB, L{
        N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.4"},
    N{KP|VP, "pulseDuration",   "12"},
    N{KP|VP, "repetition",      "1000"},
    N{KP|VP, "spotSize",        "1mm"},
        }},
    }), N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.0"},
    N{KP|VP, "pulseDuration",   "10"},
    N{KP|VP, "repetition",      "500"},
    N{KP|VP, "spotSize",        "2mm"},
        }},
    }), N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.4"},
    N{KP|VP, "pulseDuration",   "12"},
    N{KP|VP, "repetition",      "1000"},
    N{KP|VP, "spotSize",        "1mm"},
        }},
    }), N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.0"},
    N{KP|VP, "pulseDuration",   "10"},
    N{KP|VP, "repetition",      "500"},
    N{KP|VP, "spotSize",        "2mm"},
        }},
    }), N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.4"},
    N{KP|VP, "pulseDuration",   "12"},
    N{KP|VP, "repetition",      "1000"},
    N{KP|VP, "spotSize",        "2mm"},
        }},
    }), N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.0"},
    N{KP|VP, "pulseDuration",   "10"},
    N{KP|VP, "repetition",      "500"},
    N{KP|VP, "spotSize",        "2mm"},
        }},
    }),
})
);

ADD_CASE_TO_GROUP("tagged doc with anchors 9KAX",
R"(
---
&a1
!!str
scalar1
--- &a1 !!str scalar1
---
!!str
&a1
scalar1
--- !!str &a1 scalar1
---
!!str
&a2
scalar2
--- &a2 !!str scalar2
---
&a3
!!str scalar3
--- &a3 !!str scalar3
---
&a4 !!map
&a5 !!str key5: value4
--- &a4 !!map
&a5 !!str key5: value4
---
a6: 1
&anchor6 b6: 2
---
!!map
&a8 !!str key8: value7
--- !!map
&a8 !!str key8: value7
---
!!map
!!str &a10 key10: value9
--- !!map
&a10 !!str key10: value9
---
!!str &a11
value11
--- &a11 !!str value11
)",
N(STREAM, L{
    N(DOC|VP, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOC|VP, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOC|VP, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOC|VP, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOC|VP, TS("!!str", "scalar2"), AR(VALANCH, "a2")),
    N(DOC|VP, TS("!!str", "scalar2"), AR(VALANCH, "a2")),
    N(DOC|VP, TS("!!str", "scalar3"), AR(VALANCH, "a3")),
    N(DOC|VP, TS("!!str", "scalar3"), AR(VALANCH, "a3")),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key5"), AR(KEYANCH, "a5"), "value4")}), AR(VALANCH, "a4")),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key5"), AR(KEYANCH, "a5"), "value4")}), AR(VALANCH, "a4")),
    N(DOC|MB, L{N(KP|VP, "a6", "1"), N(KP|VP, "b6", AR(KEYANCH, "anchor6"), "2")}),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key8"), AR(KEYANCH, "a8"), "value7")})),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key8"), AR(KEYANCH, "a8"), "value7")})),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key10"), AR(KEYANCH, "a10"), "value9")})),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key10"), AR(KEYANCH, "a10"), "value9")})),
    N(DOC|VP, TS("!!str", "value11"), AR(VALANCH, "a11")),
    N(DOC|VP, TS("!!str", "value11"), AR(VALANCH, "a11")),
})
);

ADD_CASE_TO_GROUP("github131 1, unresolved",
R"(
a: &vref b
*vref : c
&kref aa: bb
aaa: &kvref bbb
foo:
  *kref : cc
  *kvref : dd
)",
N(MB, L{
    N(KP|VP, "a", "b", AR(VALANCH, "vref")),
    N(KEY|VP, "*vref", AR(KEYREF, "vref"), "c"),
    N(KP|VP, "aa", AR(KEYANCH, "kref"), "bb"),
    N(KP|VP, "aaa", "bbb", AR(VALANCH, "kvref")),
    N(KP|MB, "foo", L{
         N(KEY|VP, "*kref", AR(KEYREF, "kref"), "cc"),
         N(KEY|VP, "*kvref", AR(KEYREF, "kvref"), "dd"),
    })
})
);

ADD_CASE_TO_GROUP("github131 1, resolved", RESOLVE_REFS,
R"(
a: &vref b
*vref : c
&kref aa: bb
aaa: &kvref bbb
foo:
  *kref : cc
  *kvref : dd
)",
N(MB, L{
    N(KP|VP, "a", "b"),
    N(KP|VP, "b", "c"),
    N(KP|VP, "aa", "bb"),
    N(KP|VP, "aaa", "bbb"),
    N(KP|MB, "foo", L{N(KP|VP, "aa", "cc"), N(KP|VP, "bbb", "dd")})
})
);


ADD_CASE_TO_GROUP("anchors+refs on key+val, unresolved",
R"({&a0 a0: &b0 b0, *b0 : *a0})",
N(MFS, L{
    N(KP|VP, "a0", AR(KEYANCH, "a0"), "b0", AR(VALANCH, "b0")),
    N(AR(KEYREF, "*b0"), AR(VALREF, "*a0")),
})
);

ADD_CASE_TO_GROUP("anchors+refs on key+val, resolved", RESOLVE_REFS,
R"({&a0 a0: &b0 b0, *b0 : *a0})",
N(MFS, L{
    N(KP|VP, "a0", "b0"),
    N(KP|VP, "b0", "a0"),
})
);


ADD_CASE_TO_GROUP("ambiguous anchor, unresolved", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(&rootanchor
&a0 a0: &b0 b0
*b0 : *a0
map1:
  &a1 a1: &b1 b1  # &a1 must be a KEY anchor on a1, not a VAL anchor on map1
  *b1 : *a1
map2:
  *b0 : *a0  # ensure the anchor is enough to establish the indentation
  &a2 a2: &b2 b2
  *b2 : *a2
map3: &a3   # &a3 must be a VAL anchor on map3, not a KEY anchor on a3
  a3: &b3 b3
  *b3 : *b0
map4: *a0
map5:
  &map5
  &a5 a5: &b5 b5
  *b5 : *a5
map6:
  &map6
  a6: &b6 b6
  *b6 : *b6
)",
N(MB, L{
    N(KP|VP, "a0", AR(KEYANCH, "a0"), "b0", AR(VALANCH, "b0")),
    N(AR(KEYREF, "*b0"), AR(VALREF, "*a0")),
    N(KP|MB, "map1", L{N(KP|VP, "a1", AR(KEYANCH, "a1"), "b1", AR(VALANCH, "b1")), N(AR(KEYREF, "*b1"), AR(VALREF, "*a1")),}),
    N(KP|MB, "map2", L{N(AR(KEYREF, "*b0"), AR(VALREF, "*a0")), N(KP|VP, "a2", AR(KEYANCH, "a2"), "b2", AR(VALANCH, "b2")), N(AR(KEYREF, "*b2"), AR(VALREF, "*a2")),}),
    N(KP|MB, "map3", L{N(KP|VP, "a3", "b3", AR(VALANCH, "b3")), N(AR(KEYREF, "*b3"), AR(VALREF, "*b0")),}, AR(VALANCH, "a3")),
    N(KP|VAL, "map4", "*a0", AR(VALREF, "a0")),
    N(KP|MB, "map5", L{N(KP|VP, "a5", AR(KEYANCH, "a5"), "b5", AR(VALANCH, "b5")), N(AR(KEYREF, "*b5"), AR(VALREF, "*a5")),}, AR(VALANCH, "map5")),
    N(KP|MB, "map6", L{N(KP|VP, "a6", "b6", AR(VALANCH, "b6")), N(AR(KEYREF, "*b6"), AR(VALREF, "*b6")),}, AR(VALANCH, "map6")),
}, AR(VALANCH, "rootanchor"))
);

ADD_CASE_TO_GROUP("ambiguous anchor, resolved", RESOLVE_REFS,
R"(
&a0 a0: &b0 b0
*b0 : *a0
map1:
  &a1 a1: &b1 b1  # &a1 must be a KEY anchor on a1, not a VAL anchor on map1
  *b1 : *a1
map2:
  *b0 : *a0  # ensure the anchor is enough to establish the indentation
  &a2 a2: &b2 b2
  *b2 : *a2
map3: &a3   # &a3 must be a VAL anchor on map3, not a KEY anchor on a3
  a3: &b3 b3
  *b3 : *b0
map4: *a0
map5:
  &map5
  &a5 a5: &b5 b5
  *b5 : *a5
map6:
  &map6
  a6: &b6 b6
  *b6 : *b6
)",
N(MB, L{
    N(KP|VP, "a0", "b0"), N(KP|VP, "b0", "a0"),
    N(KP|MB, "map1", L{N(KP|VP, "a1", "b1"), N(KP|VP, "b1", "a1"),}),
    N(KP|MB, "map2", L{N(KP|VP, "b0", "a0"), N(KP|VP, "a2", "b2"), N(KP|VP, "b2", "a2"),}),
    N(KP|MB, "map3", L{N(KP|VP, "a3", "b3"), N(KP|VP, "b3", "b0"),}),
    N(KP|VP, "map4", "a0"),
    N(KP|MB, "map5", L{N(KP|VP, "a5", "b5"), N(KP|VP, "b5", "a5"),}),
    N(KP|MB, "map6", L{N(KP|VP, "a6", "b6"), N(KP|VP, "b6", "b6"),}),
})
);


ADD_CASE_TO_GROUP("ambiguous anchor in seq, unresolved", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
&seq
- &a0
  &a1 k1: v1
  &a2 k2: v2
  &a3 k3: v3
- &a4 k4: v4
  &a5 k5: v5
  &a6 k6: v6
- &a7
  &a8 k8: v8
- &a9
  k10: v10
- *a1 : w1
  *a2 : w2
  *a3 : w3
  *a4 : w4
  *a5 : w5
  *a6 : w6
  *a8 : w8
- *a0
- *a7
- *a8
- *a9
)",
N(SB, L{
  N(MB, L{N(KP|VP, "k1", AR(KEYANCH, "a1"), "v1"), N(KP|VP, "k2", AR(KEYANCH, "a2"), "v2"), N(KP|VP, "k3", AR(KEYANCH, "a3"), "v3")}, AR(VALANCH, "a0")),
  N(MB, L{N(KP|VP, "k4", AR(KEYANCH, "a4"), "v4"), N(KP|VP, "k5", AR(KEYANCH, "a5"), "v5"), N(KP|VP, "k6", AR(KEYANCH, "a6"), "v6")}),
  N(MB, L{N(KP|VP, "k8", AR(KEYANCH, "a8"), "v8")}, AR(VALANCH, "a7")),
  N(MB, L{N(KP|VP, "k10", "v10")}, AR(VALANCH, "a9")),
  N(MB, L{
      N(KEY|VP, "*a1", AR(KEYREF, "*a1"), "w1"),
      N(KEY|VP, "*a2", AR(KEYREF, "*a2"), "w2"),
      N(KEY|VP, "*a3", AR(KEYREF, "*a3"), "w3"),
      N(KEY|VP, "*a4", AR(KEYREF, "*a4"), "w4"),
      N(KEY|VP, "*a5", AR(KEYREF, "*a5"), "w5"),
      N(KEY|VP, "*a6", AR(KEYREF, "*a6"), "w6"),
      N(KEY|VP, "*a8", AR(KEYREF, "*a8"), "w8"),
  }),
  N(VAL, "*a0", AR(VALREF, "*a0")),
  N(VAL, "*a7", AR(VALREF, "*a7")),
  N(VAL, "*a8", AR(VALREF, "*a8")),
  N(VAL, "*a9", AR(VALREF, "*a9")),
}, AR(VALANCH, "seq"))
);

ADD_CASE_TO_GROUP("ambiguous anchor in seq, resolved", RESOLVE_REFS,
R"(
&seq
- &a0
  &a1 k1: v1
  &a2 k2: v2
  &a3 k3: v3
- &a4 k4: v4
  &a5 k5: v5
  &a6 k6: v6
- &a7
  &a8 k8: v8
- &a9
  k10 : v10
- *a1 : w1
  *a2 : w2
  *a3 : w3
  *a4 : w4
  *a5 : w5
  *a6 : w6
  *a8 : w8
- *a0
- *a7
- *a8
- *a9
)",
N(SB, L{
  N(MB, L{N(KP|VP, "k1", "v1"), N(KP|VP, "k2", "v2"), N(KP|VP, "k3", "v3")}),
  N(MB, L{N(KP|VP, "k4", "v4"), N(KP|VP, "k5", "v5"), N(KP|VP, "k6", "v6")}),
  N(MB, L{N(KP|VP, "k8", "v8")}),
  N(MB, L{N(KP|VP, "k10", "v10")}),
  N(MB, L{
      N(KP|VP, "k1", "w1"),
      N(KP|VP, "k2", "w2"),
      N(KP|VP, "k3", "w3"),
      N(KP|VP, "k4", "w4"),
      N(KP|VP, "k5", "w5"),
      N(KP|VP, "k6", "w6"),
      N(KP|VP, "k8", "w8"),
  }),
  N(MB, L{N(KP|VP, "k1", "v1"), N(KP|VP, "k2", "v2"), N(KP|VP, "k3", "v3")}),
  N(MB, L{N(KP|VP, "k8", "v8")}),
  N(VP, "k8"),
  N(MB, L{N(KP|VP, "k10", "v10")}),
})
);

ADD_CASE_TO_GROUP("anchor after complex key without value ZWK4", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
a: 1
? b
&anchor c: 3
)",
N(MB, L{
    N(KP|VP, "a", "1"), N(KP|VN, "b", {}), N(KP|VP, "c", AR(KEYANCH, "anchor"), "3")
  })
);

ADD_CASE_TO_GROUP("anchor mixed with tag HMQ5, unresolved",
R"(
!!str &a1 "foo":
  !!str bar
&a2 baz : *a1
)",
N(MB, L{
      N(KD|VP, TS("!!str", "foo"), AR(KEYANCH, "a1"), TS("!!str", "bar")),
      N(KP|VAL, "baz", AR(KEYANCH, "a2"), "*a1", AR(VALREF, "*a1")),
  })
);

ADD_CASE_TO_GROUP("anchor mixed with tag HMQ5, resolved", RESOLVE_REFS,
R"(
!!str &a1 "foo":
  !!str bar
&a2 baz : *a1
)",
N(MB, L{
      N(KD|VP, TS("!!str", "foo"), TS("!!str", "bar")),
      N(KP|VD, "baz", "foo"),
  })
);

ADD_CASE_TO_GROUP("github 484, resolved", RESOLVE_REFS,
R"(
base_1: &base_1
  a: 10
  b: 10
base_2: &base_2
  b: 20
k1:
  !!merge <<: *base_1
  !!merge <<: *base_2
k2:
  !!merge <<: *base_2
  !!merge <<: *base_1
)",
N(MB, L{
      N(KP|MB, "base_1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "base_2", L{/*                 */N(KP|VP, "b", "20")}),
      N(KP|MB, "k1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "k2", L{N(KP|VP, "b", "20"), N(KP|VP, "a", "10")}),
  })
);

ADD_CASE_TO_GROUP("github 484, resolved, anch", RESOLVE_REFS,
R"(
base_1: &base_1
  a: 10
  b: 10
base_2: &base_2
  b: 20
k1:
  &k1a <<: *base_1
  &k2b <<: *base_2
k2:
  &k2a <<: *base_2
  &k2b <<: *base_1
)",
N(MB, L{
      N(KP|MB, "base_1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "base_2", L{/*                 */N(KP|VP, "b", "20")}),
      N(KP|MB, "k1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "k2", L{N(KP|VP, "b", "20"), N(KP|VP, "a", "10")}),
  })
);

ADD_CASE_TO_GROUP("github 484, resolved, anch + tag", RESOLVE_REFS,
R"(
base_1: &base_1
  a: 10
  b: 10
base_2: &base_2
  b: 20
k1:
  &k1a !!merge <<: *base_1
  &k2b !!merge <<: *base_2
k2:
  &k2a !!merge <<: *base_2
  &k2b !!merge <<: *base_1
)",
N(MB, L{
      N(KP|MB, "base_1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "base_2", L{/*                 */N(KP|VP, "b", "20")}),
      N(KP|MB, "k1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "k2", L{N(KP|VP, "b", "20"), N(KP|VP, "a", "10")}),
  })
);

}


C4_SUPPRESS_WARNING_GCC_POP

} // namespace yml
} // namespace c4
