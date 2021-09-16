#include "./test_group.hpp"

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
    Tree t = parse("{foo: &foo 1, *foo: 2, '*foo': 3}");

    EXPECT_TRUE(t[1].is_key_ref());
    EXPECT_FALSE(t[2].is_key_ref());

    EXPECT_FALSE(t[1].is_key_quoted());
    EXPECT_TRUE(t[2].is_key_quoted());

    EXPECT_EQ(t[1].key(), "*foo");
    EXPECT_EQ(t[1].key_ref(), "foo");
    EXPECT_EQ(t[2].key(), "*foo");

    EXPECT_EQ(emitrs<std::string>(t), R"(foo: &foo 1
*foo: 2
'*foo': 3
)");

    t.resolve();

    EXPECT_EQ(emitrs<std::string>(t), R"(foo: 1
1: 2
'*foo': 3
)");
}

TEST(anchors, no_ambiguity_when_val_scalars_begin_with_star)
{
    Tree t = parse("{foo: &foo 1, ref: *foo, quo: '*foo'}");

    EXPECT_TRUE(t["ref"].is_val_ref());
    EXPECT_FALSE(t["quo"].is_val_ref());

    EXPECT_FALSE(t["ref"].is_val_quoted());
    EXPECT_TRUE(t["quo"].is_val_quoted());

    EXPECT_EQ(t["ref"].val_ref(), "foo");
    EXPECT_EQ(t["ref"].val(), "*foo");
    EXPECT_EQ(t["quo"].val(), "*foo");

    EXPECT_EQ(emitrs<std::string>(t), R"(foo: &foo 1
ref: *foo
quo: '*foo'
)");

    t.resolve();

    EXPECT_EQ(emitrs<std::string>(t), R"(foo: 1
ref: 1
quo: '*foo'
)");
}

TEST(anchors, no_ambiguity_with_inheritance)
{
    Tree t = parse("{foo: &foo {a: 1, b: 2}, bar: {<<: *foo}, sq: {'<<': haha}, dq: {\"<<\": hehe}}");

    EXPECT_TRUE(t["bar"].has_child("<<"));
    EXPECT_TRUE(t["bar"]["<<"].is_key_ref());
    EXPECT_TRUE(t["bar"]["<<"].is_val_ref());
    EXPECT_TRUE(t["sq"]["<<"].is_key_quoted());
    EXPECT_TRUE(t["dq"]["<<"].is_key_quoted());
    EXPECT_FALSE(t["sq"]["<<"].is_key_ref());
    EXPECT_FALSE(t["dq"]["<<"].is_key_ref());
    EXPECT_EQ(t["sq"]["<<"].key(), "<<");
    EXPECT_EQ(t["dq"]["<<"].key(), "<<");
    EXPECT_EQ(t["bar"]["<<"].key(), "<<");
    EXPECT_EQ(t["bar"]["<<"].val(), "*foo");
    EXPECT_EQ(t["bar"]["<<"].key_ref(), "<<");
    EXPECT_EQ(t["bar"]["<<"].val_ref(), "foo");

    EXPECT_EQ(emitrs<std::string>(t), R"(foo: &foo
  a: 1
  b: 2
bar:
  <<: *foo
sq:
  '<<': haha
dq:
  '<<': hehe
)");
    t.resolve();
    EXPECT_EQ(emitrs<std::string>(t), R"(foo:
  a: 1
  b: 2
bar:
  a: 1
  b: 2
sq:
  '<<': haha
dq:
  '<<': hehe
)");
}

TEST(anchors, programatic_key_ref)
{
    Tree t = parse("{}");
    NodeRef r = t.rootref();
    r["kanchor"] = "2";
    r["kanchor"].set_key_anchor("kanchor");
    r["vanchor"] = "3";
    r["vanchor"].set_val_anchor("vanchor");
    r["*kanchor"] = "4";
    r["*vanchor"] = "5";
    NodeRef ch = r.append_child();
    ch.set_key_ref("kanchor");
    ch.set_val("6");
    ch = r.append_child();
    ch.set_key_ref("vanchor");
    ch.set_val("7");
    EXPECT_EQ(emitrs<std::string>(t), R"(&kanchor kanchor: 2
vanchor: &vanchor 3
'*kanchor': 4
'*vanchor': 5
*kanchor: 6
*vanchor: 7
)");
    t.resolve();
    EXPECT_EQ(emitrs<std::string>(t), R"(kanchor: 2
vanchor: 3
'*kanchor': 4
'*vanchor': 5
kanchor: 6
3: 7
)");
}

TEST(anchors, programatic_val_ref)
{
    Tree t = parse("{}");
    t["kanchor"] = "2";
    t["kanchor"].set_key_anchor("kanchor");
    t["vanchor"] = "3";
    t["vanchor"].set_val_anchor("vanchor");

    t["kref"].create();
    t["vref"].create();
    t["kref"].set_val_ref("kanchor");
    t["vref"].set_val_ref("vanchor");

    EXPECT_EQ(emitrs<std::string>(t), R"(&kanchor kanchor: 2
vanchor: &vanchor 3
kref: *kanchor
vref: *vanchor
)");
    t.resolve();
    EXPECT_EQ(emitrs<std::string>(t), R"(kanchor: 2
vanchor: 3
kref: kanchor
vref: 3
)");
}

TEST(anchors, programatic_inheritance)
{
    Tree t = parse("{orig: &orig {foo: bar, baz: bat}, copy: {}, notcopy: {}, notref: {}}");

    t["copy"]["<<"] = "*orig";
    t["copy"]["<<"].set_key_ref("<<");
    t["copy"]["<<"].set_val_ref("orig");

    t["notcopy"]["test"] = "*orig";
    t["notcopy"]["test"].set_val_ref("orig");
    t["notcopy"]["<<"] = "*orig";
    t["notcopy"]["<<"].set_val_ref("orig");

    t["notref"]["<<"] = "*orig";

    EXPECT_EQ(emitrs<std::string>(t), R"(orig: &orig
  foo: bar
  baz: bat
copy:
  <<: *orig
notcopy:
  test: *orig
  '<<': *orig
notref:
  '<<': '*orig'
)");
    t.resolve();
    EXPECT_EQ(emitrs<std::string>(t), R"(orig:
  foo: bar
  baz: bat
copy:
  foo: bar
  baz: bat
notcopy:
  test:
    foo: bar
    baz: bat
  '<<':
    foo: bar
    baz: bat
notref:
  '<<': '*orig'
)");
}

TEST(anchors, programatic_multiple_inheritance)
{
    Tree t = parse("{orig1: &orig1 {foo: bar}, orig2: &orig2 {baz: bat}, orig3: &orig3 {and: more}, copy: {}}");

    t["copy"]["<<"] |= SEQ;
    t["copy"]["<<"].set_key_ref("<<");
    NodeRef ref1 = t["copy"]["<<"].append_child();
    NodeRef ref2 = t["copy"]["<<"].append_child();
    NodeRef ref3 = t["copy"]["<<"].append_child();
    ref1 = "*orig1";
    ref2 = "*orig2";
    ref3 = "*orig3";
    ref1.set_val_ref("orig1");
    ref2.set_val_ref("orig2");
    ref3.set_val_ref("orig3");

    EXPECT_EQ(emitrs<std::string>(t), R"(orig1: &orig1
  foo: bar
orig2: &orig2
  baz: bat
orig3: &orig3
  and: more
copy:
  <<:
    - *orig1
    - *orig2
    - *orig3
)");
    t.resolve();
    EXPECT_EQ(emitrs<std::string>(t), R"(orig1:
  foo: bar
orig2:
  baz: bat
orig3:
  and: more
copy:
  foo: bar
  baz: bat
  and: more
)");
}

TEST(anchors, set_anchor_leading_ampersand_is_optional)
{
    Tree t = parse("{without: 0, with: 1}");

    t["without"].set_key_anchor("without");
    t["with"].set_key_anchor("&with");
    EXPECT_EQ(t["without"].key_anchor(), "without");
    EXPECT_EQ(t["with"].key_anchor(), "with");
    EXPECT_EQ(emitrs<std::string>(t), R"(&without without: 0
&with with: 1
)");

    t["without"].set_val_anchor("without");
    t["with"].set_val_anchor("&with");
    EXPECT_EQ(t["without"].val_anchor(), "without");
    EXPECT_EQ(t["with"].val_anchor(), "with");
    EXPECT_EQ(emitrs<std::string>(t), R"(&without without: &without 0
&with with: &with 1
)");
}

TEST(anchors, set_ref_leading_star_is_optional)
{
    Tree t = parse("{}");

    t["*without"] = "0";
    t["*with"] = "1";
    EXPECT_EQ(emitrs<std::string>(t), R"('*without': 0
'*with': 1
)");

    t["*without"].set_key_ref("without");
    t["*with"].set_key_ref("*with");
    EXPECT_EQ(t["*without"].key_ref(), "without");
    EXPECT_EQ(t["*with"].key_ref(), "with");
    EXPECT_EQ(emitrs<std::string>(t), R"(*without: 0
*with: 1
)");

    t["*without"].set_val_ref("without");
    t["*with"].set_val_ref("*with");
    EXPECT_EQ(t["*without"].val_ref(), "without");
    EXPECT_EQ(t["*with"].val_ref(), "with");
    EXPECT_EQ(emitrs<std::string>(t), R"(*without: *without
*with: *with
)");
}

TEST(anchors, set_key_ref_also_sets_the_key_when_none_exists)
{
    Tree t = parse("{}");
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
    EXPECT_EQ(emitrs<std::string>(t), R"(*without: 0
*with: 1
)");
}

TEST(anchors, set_val_ref_also_sets_the_val_when_none_exists)
{
    Tree t = parse("{}");
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
    EXPECT_EQ(emitrs<std::string>(t), R"(without: *without
with: *with
)");
}

TEST(anchors, set_key_ref_replaces_existing_key)
{
    Tree t = parse("{*foo: bar}");
    NodeRef root = t.rootref();
    EXPECT_TRUE(root.has_child("*foo"));
    root["*foo"].set_key_ref("notfoo");
    EXPECT_FALSE(root.has_child("*foo"));
    EXPECT_FALSE(root.has_child("*notfoo"));
    EXPECT_TRUE(root.has_child("notfoo"));
    EXPECT_EQ(emitrs<std::string>(t), "*notfoo: bar\n");
}

TEST(anchors, set_val_ref_replaces_existing_key)
{
    Tree t = parse("{foo: *bar}");
    NodeRef root = t.rootref();
    root["foo"].set_val_ref("notbar");
    EXPECT_EQ(root["foo"].val(), "notbar");
    root["foo"].set_val_ref("*notfoo");
    EXPECT_EQ(root["foo"].val(), "*notfoo");
    EXPECT_EQ(emitrs<std::string>(t), "foo: *notfoo\n");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(weird_anchor_cases_from_suite, 2SXE)
{
    Tree t = parse(R"(&a: key: &a value
foo:
  *a:
)");
    t.resolve();
    #ifdef THIS_IS_A_KNOWN_LIMITATION // since we do not allow colon in anchors, this would fail:
    EXPECT_EQ(emitrs<std::string>(t), R"(key: value
foo: key
)");
    #endif
    // so we get this instead:
    EXPECT_EQ(emitrs<std::string>(t), R"(key: value
foo:
  value: ~
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
        CaseNode n("<<", "*base", AR(VALANCH, "base"));
        EXPECT_EQ(n.key, "<<");
        EXPECT_EQ(n.val, "*base");
        EXPECT_EQ((type_bits)(n.type & mask), (type_bits)VALANCH);
        EXPECT_EQ((type_bits)n.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)n.val_anchor.type, (type_bits)VALANCH);
        EXPECT_EQ(n.key_anchor.str, "");
        EXPECT_EQ(n.val_anchor.str, "base");
    }

    {
        CaseNode n("base", L{N("name", "Everyone has same name")}, AR(VALANCH, "base"));
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
        CaseNode const& base = *l.begin();
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
        CaseNode const& base = *l.begin();
        CaseNode const& age = *(&base + 1);
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
        CaseNode n("foo", L{N("<<", "*base", AR(VALREF, "base")), N("age", "10")}, AR(VALANCH, "foo"));
        EXPECT_EQ(n.key, "foo");
        EXPECT_EQ(n.val, "");
        EXPECT_EQ(n.type.has_key(), true);
        EXPECT_EQ(n.type.is_map(), true);
        EXPECT_EQ((type_bits)(n.type & mask), (type_bits)VALANCH);
        EXPECT_EQ((type_bits)n.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)n.val_anchor.type, (type_bits)VALANCH);
        EXPECT_EQ(n.key_anchor.str, "");
        EXPECT_EQ(n.val_anchor.str, "foo");

        CaseNode const& base = n.children[0];
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

TEST(simple_anchor, resolve_works_on_an_empty_tree)
{
    Tree t;
    t.resolve();
    EXPECT_TRUE(t.empty());
}

TEST(simple_anchor, resolve_works_on_a_tree_without_refs)
{
    Tree t = parse("[a, b, c, d, e, f]");
    size_t size_before = t.size();
    t.resolve();
    EXPECT_EQ(t.size(), size_before);
}

TEST(simple_anchor, resolve_works_on_keyrefvalref)
{
    Tree t = parse("{&a a: &b b, *b: *a}");
    EXPECT_EQ(t["a"].has_key_anchor(), true);
    EXPECT_EQ(t["a"].has_val_anchor(), true);
    EXPECT_EQ(t["a"].key_anchor(), "a");
    EXPECT_EQ(t["a"].val_anchor(), "b");
    EXPECT_EQ(t["*b"].is_key_ref(), true);
    EXPECT_EQ(t["*b"].is_val_ref(), true);
    EXPECT_EQ(t["*b"].key_ref(), "b");
    EXPECT_EQ(t["*b"].val_ref(), "a");
    EXPECT_EQ(emitrs<std::string>(t), R"(&a a: &b b
*b: *a
)");
    t.resolve();
    EXPECT_EQ(t["a"].val(), "b");
    EXPECT_EQ(t["b"].val(), "a");
    EXPECT_EQ(emitrs<std::string>(t), R"(a: b
b: a
)");
}

TEST(simple_anchor, anchors_of_first_child_key_implicit)
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
    Tree t = parse(yaml);
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


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define SIMPLE_ANCHOR_CASES                            \
    "simple anchor 1, implicit, unresolved",\
    "simple anchor 1, implicit, resolved",\
    "simple anchor 1, explicit, unresolved",\
    "simple anchor 1, explicit, resolved",\
    "anchor example 2, unresolved",\
    "anchor example 2, resolved",\
    "anchor example 3, unresolved",\
    "anchor example 3, resolved",\
    "merge example, unresolved",\
    "merge example, resolved",\
    "tagged doc with anchors 9KAX",\
    "github131 1, unresolved",\
    "github131 1, resolved",\
    "anchors+refs on key+val, unresolved",\
    "anchors+refs on key+val, resolved",\
    "ambiguous anchor, unresolved",\
    "ambiguous anchor, resolved",\
    "ambiguous anchor in seq, unresolved",\
    "ambiguous anchor in seq, resolved",\
    "anchor after complex key without value ZWK4",\
    "anchor mixed with tag HMQ5, unresolved",\
    "anchor mixed with tag HMQ5, resolved"


CASE_GROUP(SIMPLE_ANCHOR)
{
    APPEND_CASES(

C("merge example, unresolved",
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
L{
    N(L{N("x", "1" ), N("y", "2")}, AR(VALANCH, "CENTER")),
    N(L{N("x", "0" ), N("y", "2")}, AR(VALANCH, "LEFT"  )),
    N(L{N("r", "10")             }, AR(VALANCH, "BIG"   )),
    N(L{N("r", "1" )             }, AR(VALANCH, "SMALL" )),
    N(L{N("x", "1" ), N("y", "2"), N("r", "10"), N("label", "center/big")}),
    N(L{N("<<", AR(KEYREF, "<<"), "*CENTER", AR(VALREF, "*CENTER")), N("r", "10"), N("label", "center/big")}),
    N(L{N("<<", AR(KEYREF, "<<"), L{N("*CENTER", AR(VALREF, "*CENTER")), N("*BIG", AR(VALREF, "*BIG"))}), N("label", "center/big")}),
    N(L{N("<<", AR(KEYREF, "<<"), L{N("*BIG", AR(VALREF, "*BIG")), N("*LEFT", AR(VALREF, "*LEFT")), N("*SMALL", AR(VALREF, "*SMALL"))}), N("x", "1"), N("label", "center/big")}),
}),

C("merge example, resolved", RESOLVE_REFS,
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
  << : [ *SMALL, *LEFT, *BIG ]
  x: 1
  label: center/big
)",
L{
    N(L{N("x", "1" ), N("y", "2")}),
    N(L{N("x", "0" ), N("y", "2")}),
    N(L{N("r", "10")             }),
    N(L{N("r", "1" )             }),
    N(L{N("x", "1" ), N("y", "2"), N("r", "10"), N("label", "center/big")}),
    N(L{N("x", "1" ), N("y", "2"), N("r", "10"), N("label", "center/big")}),
    N(L{N("x", "1" ), N("y", "2"), N("r", "10"), N("label", "center/big")}),
    N(L{N("x", "1" ), N("y", "2"), N("r", "10"), N("label", "center/big")}),
}),

C("simple anchor 1, implicit, unresolved",
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
  L{
      N("anchored_content", "This string will appear as the value of two keys.", AR(VALANCH, "anchor_name")),
      N("other_anchor", "*anchor_name", AR(VALREF, "anchor_name")),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence", AR(VALANCH, "anchor_in_seq")),
              N("*anchor_in_seq", AR(VALREF, "anchor_in_seq")),
          }),
      N("base", L{N("name", "Everyone has same name")}, AR(VALANCH, "base")),
      N("foo", L{N("<<", AR(KEYREF, "<<"), "*base", AR(VALREF, "base")), N("age", "10")}, AR(VALANCH, "foo")),
      N("bar", L{N("<<", AR(KEYREF, "<<"), "*base", AR(VALREF, "base")), N("age", "20")}, AR(VALANCH, "bar")),
  }
),

C("simple anchor 1, explicit, unresolved",
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
  L{
      N("anchored_content", "This string will appear as the value of two keys.", AR(VALANCH, "anchor_name")),
      N("other_anchor", "*anchor_name", AR(VALREF, "anchor_name")),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence", AR(VALANCH, "anchor_in_seq")),
              N("*anchor_in_seq", AR(VALREF, "anchor_in_seq")),
          }),
      N("base", L{N("name", "Everyone has same name")}, AR(VALANCH, "base")),
      N("foo", L{N("<<", AR(KEYREF, "<<"), "*base", AR(VALREF, "base")), N("age", "10")}, AR(VALANCH, "foo")),
      N("bar", L{N("<<", AR(KEYREF, "<<"), "*base", AR(VALREF, "base")), N("age", "20")}, AR(VALANCH, "bar")),
  }
),

C("simple anchor 1, implicit, resolved", RESOLVE_REFS,
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
  L{
      N("anchored_content", "This string will appear as the value of two keys."),
      N("other_anchor", "This string will appear as the value of two keys."),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence"),
              N("this value appears in both elements of the sequence"),
          }),
      N("base", L{N("name", "Everyone has same name")}),
      N("foo", L{N("name", "Everyone has same name"), N("age", "10")}),
      N("bar", L{N("name", "Everyone has same name"), N("age", "20")}),
  }
),

C("simple anchor 1, explicit, resolved", RESOLVE_REFS,
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
  L{
      N("anchored_content", "This string will appear as the value of two keys."),
      N("other_anchor", "This string will appear as the value of two keys."),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence"),
              N("this value appears in both elements of the sequence"),
          }),
      N("base", L{N("name", "Everyone has same name")}),
      N("foo", L{N("name", "Everyone has same name"), N("age", "10")}),
      N("bar", L{N("name", "Everyone has same name"), N("age", "20")}),
  }
),


C("anchor example 2, unresolved",
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
L{
  N{"receipt", "Oz-Ware Purchase Invoice"},
  N{"date",    "2012-08-06"},
  N{"customer", L{N{"first_name", "Dorothy"}, N{"family_name", "Gale"}}},
  N{"items", L{
    N{L{N{"part_no",   "A4786"},
        N{"descrip",   "Water Bucket (Filled)"},
        N{"price",     "1.47"},
        N{"quantity",  "4"},}},
    N{L{N{"part_no", "E1628"},
        N{"descrip",   "High Heeled \"Ruby\" Slippers"},
        N{"size",      "8"},
        N{"price",     "133.7"},
        N{"quantity",  "1"},}}}},
   N{"bill-to", L{
        N{QV, "street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}, AR(VALANCH, "id001")},
   N{"ship-to", "*id001", AR(VALREF, "id001")},
   N{QV, "specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
  }
),


C("anchor example 2, resolved", RESOLVE_REFS,
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
L{
  N{"receipt", "Oz-Ware Purchase Invoice"},
  N{"date",    "2012-08-06"},
  N{"customer", L{N{"first_name", "Dorothy"}, N{"family_name", "Gale"}}},
  N{"items", L{
    N{L{N{"part_no",   "A4786"},
        N{"descrip",   "Water Bucket (Filled)"},
        N{"price",     "1.47"},
        N{"quantity",  "4"},}},
    N{L{N{"part_no", "E1628"},
        N{"descrip",   "High Heeled \"Ruby\" Slippers"},
        N{"size",      "8"},
        N{"price",     "133.7"},
        N{"quantity",  "1"},}}}},
   N{"bill-to", L{
        N{QV, "street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}},
   N{"ship-to", L{
        N{QV, "street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}},
   N{QV, "specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
  }
),

C("anchor example 3, unresolved",
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
L{N(L{
N("step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }, AR(VALANCH, "id001")),
    }), N(L{
N("step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }, AR(VALANCH, "id002")),
    }), N(L{
N{"step", "*id001", AR(VALREF, "id001")},
    }), N(L{
N{"step", "*id002", AR(VALREF, "id002")},
    }), N(L{
N{"step", L{
    N{"<<", AR(KEYREF, "<<"), "*id001", AR(VALREF, "id002")},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{"step", "*id002", AR(VALREF, "id002")},
    }),
    }
),

C("anchor example 3, resolved", RESOLVE_REFS,
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
  L{N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
    }),
    }
),

C("tagged doc with anchors 9KAX",
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
    N(DOCVAL, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOCVAL, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOCVAL, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOCVAL, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOCVAL, TS("!!str", "scalar2"), AR(VALANCH, "a2")),
    N(DOCVAL, TS("!!str", "scalar2"), AR(VALANCH, "a2")),
    N(DOCVAL, TS("!!str", "scalar3"), AR(VALANCH, "a3")),
    N(DOCVAL, TS("!!str", "scalar3"), AR(VALANCH, "a3")),
    N(DOCMAP, TL("!!map", L{N(TS("!!str", "key5"), AR(KEYANCH, "a5"), "value4")}), AR(VALANCH, "a4")),
    N(DOCMAP, TL("!!map", L{N(TS("!!str", "key5"), AR(KEYANCH, "a5"), "value4")}), AR(VALANCH, "a4")),
    N(DOCMAP, L{N("a6", "1"), N("b6", AR(KEYANCH, "anchor6"), "2")}),
    N(DOCMAP, TL("!!map", L{N(TS("!!str", "key8"), AR(KEYANCH, "a8"), "value7")})),
    N(DOCMAP, TL("!!map", L{N(TS("!!str", "key8"), AR(KEYANCH, "a8"), "value7")})),
    N(DOCMAP, TL("!!map", L{N(TS("!!str", "key10"), AR(KEYANCH, "a10"), "value9")})),
    N(DOCMAP, TL("!!map", L{N(TS("!!str", "key10"), AR(KEYANCH, "a10"), "value9")})),
    N(DOCVAL, TS("!!str", "value11"), AR(VALANCH, "a11")),
    N(DOCVAL, TS("!!str", "value11"), AR(VALANCH, "a11")),
})
),

C("github131 1, unresolved",
R"(
a: &vref b
*vref: c
&kref aa: bb
aaa: &kvref bbb
foo:
  *kref: cc
  *kvref: cc
)",
L{
    N("a", "b", AR(VALANCH, "vref")),
    N("*vref", AR(KEYREF, "vref"), "c"),
    N("aa", AR(KEYANCH, "kref"), "bb"),
    N("aaa", "bbb", AR(VALANCH, "kvref")),
    N("foo", L{
         N("*kref", AR(KEYREF, "kref"), "cc"),
         N("*kvref", AR(KEYREF, "kvref"), "cc"),
    })
}),

C("github131 1, resolved", RESOLVE_REFS,
R"(
a: &vref b
*vref: c
&kref aa: bb
aaa: &kvref bbb
foo:
  *kref: cc
  *kvref: cc
)",
L{
    N("a", "b"),
    N("b", "c"),
    N("aa", "bb"),
    N("aaa", "bbb"),
    N("foo", L{N("aa", "cc"), N("bbb", "cc")})
}),


C("anchors+refs on key+val, unresolved",
R"({&a0 a0: &b0 b0, *b0: *a0})",
L{
    N("a0", AR(KEYANCH, "a0"), "b0", AR(VALANCH, "b0")),
    N(AR(KEYREF, "*b0"), AR(VALREF, "*a0")),
}),

C("anchors+refs on key+val, resolved", RESOLVE_REFS,
R"({&a0 a0: &b0 b0, *b0: *a0})",
L{
    N("a0", "b0"),
    N("b0", "a0"),
}),


C("ambiguous anchor, unresolved",
R"(&rootanchor
&a0 a0: &b0 b0
*b0: *a0
map1:
  &a1 a1: &b1 b1  # &a1 must be a KEY anchor on a1, not a VAL anchor on map1
  *b1: *a1
map2:
  *b0: *a0  # ensure the anchor is enough to establish the indentation
  &a2 a2: &b2 b2
  *b2: *a2
map3: &a3   # &a3 must be a VAL anchor on map3, not a KEY anchor on a3
  a3: &b3 b3
  *b3: *b0
map4: *a0
map5:
  &map5
  &a5 a5: &b5 b5
  *b5: *a5
map6:
  &map6
  a6: &b6 b6
  *b6: *b6
)",
N(L{
    N("a0", AR(KEYANCH, "a0"), "b0", AR(VALANCH, "b0")),
    N(AR(KEYREF, "*b0"), AR(VALREF, "*a0")),
    N("map1", L{N("a1", AR(KEYANCH, "a1"), "b1", AR(VALANCH, "b1")), N(AR(KEYREF, "*b1"), AR(VALREF, "*a1")),}),
    N("map2", L{N(AR(KEYREF, "*b0"), AR(VALREF, "*a0")), N("a2", AR(KEYANCH, "a2"), "b2", AR(VALANCH, "b2")), N(AR(KEYREF, "*b2"), AR(VALREF, "*a2")),}),
    N("map3", L{N("a3", "b3", AR(VALANCH, "b3")), N(AR(KEYREF, "*b3"), AR(VALREF, "*b0")),}, AR(VALANCH, "a3")),
    N("map4", "*a0", AR(VALREF, "a0")),
    N("map5", L{N("a5", AR(KEYANCH, "a5"), "b5", AR(VALANCH, "b5")), N(AR(KEYREF, "*b5"), AR(VALREF, "*a5")),}, AR(VALANCH, "map5")),
    N("map6", L{N("a6", "b6", AR(VALANCH, "b6")), N(AR(KEYREF, "*b6"), AR(VALREF, "*b6")),}, AR(VALANCH, "map6")),
}, AR(VALANCH, "rootanchor"))),

C("ambiguous anchor, resolved", RESOLVE_REFS,
R"(
&a0 a0: &b0 b0
*b0: *a0
map1:
  &a1 a1: &b1 b1  # &a1 must be a KEY anchor on a1, not a VAL anchor on map1
  *b1: *a1
map2:
  *b0: *a0  # ensure the anchor is enough to establish the indentation
  &a2 a2: &b2 b2
  *b2: *a2
map3: &a3   # &a3 must be a VAL anchor on map3, not a KEY anchor on a3
  a3: &b3 b3
  *b3: *b0
map4: *a0
map5:
  &map5
  &a5 a5: &b5 b5
  *b5: *a5
map6:
  &map6
  a6: &b6 b6
  *b6: *b6
)",
L{
    N("a0", "b0"), N("b0", "a0"),
    N("map1", L{N("a1", "b1"), N("b1", "a1"),}),
    N("map2", L{N("b0", "a0"), N("a2", "b2"), N("b2", "a2"),}),
    N("map3", L{N("a3", "b3"), N("b3", "b0"),}),
    N("map4", "a0"),
    N("map5", L{N("a5", "b5"), N("b5", "a5"),}),
    N("map6", L{N("a6", "b6"), N("b6", "b6"),}),
}),


C("ambiguous anchor in seq, unresolved",
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
- *a1: w1
  *a2: w2
  *a3: w3
  *a4: w4
  *a5: w5
  *a6: w6
  *a8: w8
- *a0
- *a7
- *a9
)",
N(L{
  N(L{N("k1", AR(KEYANCH, "a1"), "v1"), N("k2", AR(KEYANCH, "a2"), "v2"), N("k3", AR(KEYANCH, "a3"), "v3")}, AR(VALANCH, "a0")),
  N(L{N("k4", AR(KEYANCH, "a4"), "v4"), N("k5", AR(KEYANCH, "a5"), "v5"), N("k6", AR(KEYANCH, "a6"), "v6")}),
  N(L{N("k8", AR(KEYANCH, "a8"), "v8")}, AR(VALANCH, "a7")),
  N(L{N("k10", "v10")}, AR(VALANCH, "a9")),
  N(L{
      N("*a1", AR(KEYREF, "*a1"), "w1"),
      N("*a2", AR(KEYREF, "*a2"), "w2"),
      N("*a3", AR(KEYREF, "*a3"), "w3"),
      N("*a4", AR(KEYREF, "*a4"), "w4"),
      N("*a5", AR(KEYREF, "*a5"), "w5"),
      N("*a6", AR(KEYREF, "*a6"), "w6"),
      N("*a8", AR(KEYREF, "*a8"), "w8"),
  }),
  N("*a0", AR(VALREF, "*a0")),
  N("*a7", AR(VALREF, "*a7")),
  N("*a9", AR(VALREF, "*a9")),
}, AR(VALANCH, "seq"))),

C("ambiguous anchor in seq, resolved", RESOLVE_REFS,
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
- *a1: w1
  *a2: w2
  *a3: w3
  *a4: w4
  *a5: w5
  *a6: w6
  *a8: w8
- *a0
- *a7
- *a9
)",
L{
  N(L{N("k1", "v1"), N("k2", "v2"), N("k3", "v3")}),
  N(L{N("k4", "v4"), N("k5", "v5"), N("k6", "v6")}),
  N(L{N("k8", "v8")}),
  N(L{N("k10", "v10")}),
  N(L{
      N("k1", "w1"),
      N("k2", "w2"),
      N("k3", "w3"),
      N("k4", "w4"),
      N("k5", "w5"),
      N("k6", "w6"),
      N("k8", "w8"),
  }),
  N(L{N("k1", AR(KEYANCH, "a1"), "v1"), N("k2", AR(KEYANCH, "a2"), "v2"), N("k3", AR(KEYANCH, "a3"), "v3")}),
  N(L{N("k8", AR(KEYANCH, "a8"), "v8")}),
  N(L{N("k10", "v10")}),
}),

C("anchor after complex key without value ZWK4",
R"(
a: 1
? b
&anchor c: 3
)",
  L{
    N("a", "1"), N(KEYVAL, "b", {}), N("c", AR(KEYANCH, "anchor"), "3")
  }
),

C("anchor mixed with tag HMQ5, unresolved",
R"(
!!str &a1 "foo":
  !!str bar
&a2 baz : *a1
)",
  L{
      N(KEYVAL|KEYQUO, TS("!!str", "foo"), AR(KEYANCH, "a1"), TS("!!str", "bar")),
      N("baz", AR(KEYANCH, "a2"), "*a1", AR(VALREF, "*a1")),
  }
),

C("anchor mixed with tag HMQ5, resolved", RESOLVE_REFS,
R"(
!!str &a1 "foo":
  !!str bar
&a2 baz : *a1
)",
  L{
      N(KEYVAL|KEYQUO, TS("!!str", "foo"), TS("!!str", "bar")),
      N("baz", "foo"),
  }
)

    )
}

INSTANTIATE_GROUP(SIMPLE_ANCHOR)

C4_SUPPRESS_WARNING_GCC_POP

} // namespace yml
} // namespace c4
