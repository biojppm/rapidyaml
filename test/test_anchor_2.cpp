#include "./test_lib/tree_and_ints.hpp"
#ifndef C4_YML_REFERENCE_RESOLVER_HPP_
#include "c4/yml/reference_resolver.hpp"
#endif
#ifndef C4_YML_PARSE_HPP_
#include "c4/yml/parse.hpp"
#endif
#ifndef C4_YML_EMIT_HPP_
#include "c4/yml/emit.hpp"
#endif

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

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
    extra::ievt::evtbuf evt = ti.ints.evts;
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
    EXPECT_EQ(evt.ptr[69], extra::ievt::RREF); // NOLINT
    EXPECT_EQ(evt.ptr[70], 59);
    EXPECT_EQ(evt.ptr[71], 65);
    EXPECT_EQ(ti.ints.getstr(59), "it");
    EXPECT_EQ(ti.ints.getstr(62), "works");
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
    test_emit_yaml_(ti, "key: value\nfoo: key\n");
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
    extra::ievt::evt_size size_before_ints = ti.ints.evts.len;
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

} // namespace yml
} // namespace c4
