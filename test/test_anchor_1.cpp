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
    EXPECT_EQ(r.m_known_keys.m_known_keys.m_arr.m_callbacks, tree1.callbacks());
    EXPECT_EQ(r.m_known_keys.m_unique_keys.m_callbacks, tree1.callbacks());
    RYML_EXPECT_ERROR(check_success([&]{
        Tree tree2;
        ASSERT_NE(tree2.callbacks(), tree1.callbacks());
        r.resolve(&tree2);
        EXPECT_EQ(r.m_anchors.m_arr.m_callbacks, tree1.callbacks());
        EXPECT_EQ(r.m_branch_size.m_callbacks, tree1.callbacks());
        EXPECT_EQ(r.m_known_keys.m_known_keys.m_arr.m_callbacks, tree1.callbacks());
        EXPECT_EQ(r.m_known_keys.m_unique_keys.m_callbacks, tree1.callbacks());
    }));
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
#ifdef TO_BE_FIXED
TEST(anchors, parse_err_on_single_ampersand)
{
    test_parse_err_yaml_("&");
}
TEST(anchors, parse_err_on_single_star)
{
    test_parse_err_yaml_("*");
}

TEST(anchors, resolve_err_on_empty_ampersand_tree)
{
    Tree tree;
    NodeRef r = tree;
    r.set_val_anchor("");
    r.set_val("val");
    test_resolve_err_tree_(tree);
}
TEST(anchors, resolve_err_on_empty_ampersand_ints)
{
    using namespace extra::ievt;
    evt_bits evts[] = {
        BSTR,
        BDOC,
        VAL_|ANCH, 0, 0,
        VAL_|SCLR|PLAI|PEX2, 0, 3,
        EDOC,
        ESTR,
    };
    char intsrc[] = "val";
    IntBufs ints;
    ints.evts.ptr = evts;
    ints.evts.len = 10;
    ints.evts.cap = 10;
    ints.src = intsrc;
    ints.arena = {};
    test_resolve_err_ints_(ints);
}

TEST(anchors, resolve_err_on_empty_star_tree)
{
    Tree tree;
    NodeRef r = tree;
    csubstr ref = "*";
    r.set_val_ref(ref.sub(1));
    r.set_val(ref);
    test_resolve_err_tree_(tree);
}
TEST(anchors, resolve_err_on_empty_star_ints)
{
    using namespace extra::ievt;
    evt_bits evts[] = {
        BSTR,
        BDOC,
        VAL_|ALIA, 0, 0,
        EDOC,
        ESTR,
    };
    char intsrc[] = "*";
    IntBufs ints;
    ints.evts.ptr = evts;
    ints.evts.len = 7;
    ints.evts.cap = 7;
    ints.src = intsrc;
    ints.arena = {};
    test_resolve_err_ints_(ints);
}
#endif // TO_BE_FIXED


//-----------------------------------------------------------------------------

TEST(anchors, err_on_cyclic_reference_1)
{
    const std::string yaml = "&x\n- *x\n";
    TreeAndInts ti(to_csubstr(yaml));
    ASSERT_TRUE(ti.tree.rootref().has_val_anchor());
    ASSERT_TRUE(ti.tree[0].is_val_ref());
    EXPECT_EQ(ti.tree.rootref().val_anchor(), "x");
    EXPECT_EQ(ti.tree[0].val_ref(), "x");
    SCOPED_CALL(test_emit_resolve_err, (yaml, "[\n  \"*x\"\n]\n"));
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

static ResolveParams always_binary_search()
{
    auto params = ResolveParams{};
    params.anchor_map_linear_threshold = 0; // always binary
    params.known_keys_linear_threshold = 0; // always binary
    return params;
}

TEST(anchors, merge0)
{
    auto dotest = [](ResolveParams params){
        test_resolve_(ResolveOptions{}, params,
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
        "");
    };
    SCOPED_CALL(dotest, (ResolveParams{}));
    SCOPED_CALL(dotest, (always_binary_search()));
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

} // namespace yml
} // namespace c4
