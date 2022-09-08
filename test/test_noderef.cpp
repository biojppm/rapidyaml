#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include "./test_case.hpp"
#include "./callbacks_tester.hpp"

#include <gtest/gtest.h>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4389) // signed/unsigned mismatch
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#endif

namespace c4 {
namespace yml {

TEST(NodeRef, general)
{
    Tree t;

    NodeRef root(&t);

    //using S = csubstr;
    //using V = NodeScalar;
    using N = NodeInit;

    root = N{MAP};
    root.append_child({"a", "0"});
    root.append_child({MAP, "b"});
    root["b"].append_child({SEQ, "seq"});
    root["b"]["seq"].append_child({"0"});
    root["b"]["seq"].append_child({"1"});
    root["b"]["seq"].append_child({"2"});
    root["b"]["seq"].append_child({NodeScalar{"!!str", "3"}});
    auto ch4 = root["b"]["seq"][3].append_sibling({"4"});
    EXPECT_EQ(ch4.id(), root["b"]["seq"][4].id());
    EXPECT_EQ(ch4.get(), root["b"]["seq"][4].get());
    EXPECT_EQ((type_bits)root["b"]["seq"][4].type(), (type_bits)VAL);
    EXPECT_EQ(root["b"]["seq"][4].val(), "4");
    root["b"]["seq"].append_sibling({NodeScalar{"!!str", "aaa"}, NodeScalar{"!!int", "0"}});
    EXPECT_EQ((type_bits)root["b"]["seq"][4].type(), (type_bits)VAL);
    EXPECT_EQ(root["b"]["seq"][4].val(), "4");

    root["b"]["key"] = "val";
    auto seq = root["b"]["seq"];
    auto seq2 = root["b"]["seq2"];
    EXPECT_TRUE(seq2.is_seed());
    root["b"]["seq2"] = N(SEQ);
    seq2 = root["b"]["seq2"];
    EXPECT_FALSE(seq2.is_seed());
    EXPECT_TRUE(seq2.is_seq());
    EXPECT_EQ(seq2.num_children(), 0);
    EXPECT_EQ(root["b"]["seq2"].get(), seq2.get());
    auto seq20 = seq2[0];
    EXPECT_TRUE(seq20.is_seed());
    EXPECT_TRUE(seq2[0].is_seed());
    EXPECT_EQ(seq2.num_children(), 0);
    EXPECT_TRUE(seq2[0].is_seed());
    EXPECT_TRUE(seq20.is_seed());
    EXPECT_NE(seq.get(), seq2.get());
    seq20 = root["b"]["seq2"][0];
    EXPECT_TRUE(seq20.is_seed());
    root["b"]["seq2"][0] = "00";
    seq20 = root["b"]["seq2"][0];
    EXPECT_FALSE(seq20.is_seed());
    NodeRef before = root["b"]["key"];
    EXPECT_EQ(before.key(), "key");
    EXPECT_EQ(before.val(), "val");
    root["b"]["seq2"][1] = "01";
    NodeRef after = root["b"]["key"];
    EXPECT_EQ(before.key(), "key");
    EXPECT_EQ(before.val(), "val");
    EXPECT_EQ(after.key(), "key");
    EXPECT_EQ(after.val(), "val");
    root["b"]["seq2"][2] = "02";
    root["b"]["seq2"][3] = "03";
    int iv = 0;
    root["b"]["seq2"][4] << 55; root["b"]["seq2"][4] >> iv;
    EXPECT_EQ(iv, 55);
    size_t zv = 0;
    root["b"]["seq2"][5] << size_t(55); root["b"]["seq2"][5] >> zv;
    EXPECT_EQ(zv, size_t(55));
    float fv = 0;
    root["b"]["seq2"][6] << 2.0f; root["b"]["seq2"][6] >> fv;
    EXPECT_EQ(fv, 2.f);
    float dv = 0;
    root["b"]["seq2"][7] << 2.0; root["b"]["seq2"][7] >> dv;
    EXPECT_EQ(dv, 2.0);

    EXPECT_EQ(root["b"]["key"].key(), "key");
    EXPECT_EQ(root["b"]["key"].val(), "val");


    emit_yaml(t);

    EXPECT_TRUE(root.type().is_map());
    EXPECT_TRUE(root["a"].type().is_keyval());
    EXPECT_EQ(root["a"].key(), "a");
    EXPECT_EQ(root["a"].val(), "0");

    EXPECT_TRUE(root["b"].type().has_key());
    EXPECT_TRUE(root["b"].type().is_map());

    EXPECT_TRUE(root["b"]["seq"].type().has_key());
    EXPECT_TRUE(root["b"]["seq"].type().is_seq());
    EXPECT_EQ  (root["b"]["seq"].key(), "seq");
    EXPECT_TRUE(root["b"]["seq"][0].type().is_val());
    EXPECT_EQ(  root["b"]["seq"][0].val(), "0");
    EXPECT_TRUE(root["b"]["seq"][1].type().is_val());
    EXPECT_EQ(  root["b"]["seq"][1].val(), "1");
    EXPECT_TRUE(root["b"]["seq"][2].type().is_val());
    EXPECT_EQ(  root["b"]["seq"][2].val(), "2");
    EXPECT_TRUE(root["b"]["seq"][3].type().is_val());
    EXPECT_EQ(  root["b"]["seq"][3].val(), "3");
    EXPECT_EQ(  root["b"]["seq"][3].val_tag(), "!!str");
    EXPECT_TRUE(root["b"]["seq"][4].type().is_val());
    EXPECT_EQ(  root["b"]["seq"][4].val(), "4");

    int tv;
    EXPECT_EQ(root["b"]["key"].key(), "key");
    EXPECT_EQ(root["b"]["key"].val(), "val");
    EXPECT_EQ(root["b"]["seq2"][0].val(), "00"); root["b"]["seq2"][0] >> tv; EXPECT_EQ(tv, 0);
    EXPECT_EQ(root["b"]["seq2"][1].val(), "01"); root["b"]["seq2"][1] >> tv; EXPECT_EQ(tv, 1);
    EXPECT_EQ(root["b"]["seq2"][2].val(), "02"); root["b"]["seq2"][2] >> tv; EXPECT_EQ(tv, 2);
    EXPECT_EQ(root["b"]["seq2"][3].val(), "03"); root["b"]["seq2"][3] >> tv; EXPECT_EQ(tv, 3);
    EXPECT_EQ(root["b"]["seq2"][4].val(), "55"); EXPECT_EQ(iv, 55);
    EXPECT_EQ(root["b"]["seq2"][5].val(), "55"); EXPECT_EQ(zv, size_t(55));
    EXPECT_EQ(root["b"]["seq2"][6].val(), "2"); EXPECT_EQ(fv, 2.f);
    EXPECT_EQ(root["b"]["seq2"][6].val(), "2"); EXPECT_EQ(dv, 2.);

    root["b"]["seq"][2].set_val_serialized(22);

    emit_yaml(t);

    EXPECT_TRUE(root["b"]["aaa"].type().is_keyval());
    EXPECT_EQ(root["b"]["aaa"].key_tag(), "!!str");
    EXPECT_EQ(root["b"]["aaa"].key(), "aaa");
    EXPECT_EQ(root["b"]["aaa"].val_tag(), "!!int");
    EXPECT_EQ(root["b"]["aaa"].val(), "0");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void noderef_check_tree(ConstNodeRef const& root)
{
    test_invariants(*root.tree());

    EXPECT_EQ(root.tree()->size(), 7u);
    EXPECT_EQ(root.num_children(), 6u);
    EXPECT_EQ(root.is_container(), true);
    EXPECT_EQ(root.is_seq(), true);

    EXPECT_TRUE(root[0].type().is_val());
    EXPECT_EQ(  root[0].val(), "0");
    EXPECT_TRUE(root[1].type().is_val());
    EXPECT_EQ(  root[1].val(), "1");
    EXPECT_TRUE(root[2].type().is_val());
    EXPECT_EQ(  root[2].val(), "2");
    EXPECT_TRUE(root[3].type().is_val());
    EXPECT_EQ(  root[3].val(), "3");
    EXPECT_TRUE(root[4].type().is_val());
    EXPECT_EQ(  root[4].val(), "4");
    EXPECT_TRUE(root[5].type().is_val());
    EXPECT_EQ(  root[5].val(), "5");
}

TEST(NodeRef, append_child)
{
    Tree t;

    NodeRef root(&t);

    root |= SEQ;
    root.append_child({"0"});
    root.append_child({"1"});
    root.append_child({"2"});
    root.append_child({"3"});
    root.append_child({"4"});
    root.append_child({"5"});

    noderef_check_tree(root);
}

TEST(NodeRef, prepend_child)
{
    Tree t;

    NodeRef root(&t);

    root |= SEQ;
    root.prepend_child({"5"});
    root.prepend_child({"4"});
    root.prepend_child({"3"});
    root.prepend_child({"2"});
    root.prepend_child({"1"});
    root.prepend_child({"0"});

    noderef_check_tree(root);
}

TEST(NodeRef, insert_child)
{
    Tree t;

    NodeRef root(&t);
    NodeRef none(&t, NONE);

    root |= SEQ;
    root.insert_child({"3"}, none);
    root.insert_child({"4"}, root[0]);
    root.insert_child({"0"}, none);
    root.insert_child({"5"}, root[2]);
    root.insert_child({"1"}, root[0]);
    root.insert_child({"2"}, root[1]);

    noderef_check_tree(root);
}

TEST(NodeRef, remove_child)
{
    Tree t;

    NodeRef root(&t);
    NodeRef none(&t, NONE);

    root |= SEQ;
    root.insert_child({"3"}, none);
    root.insert_child({"4"}, root[0]);
    root.insert_child({"0"}, none);
    root.insert_child({"5"}, root[2]);
    root.insert_child({"1"}, root[0]);
    root.insert_child({"2"}, root[1]);

    std::vector<int> vec({10, 20, 30, 40, 50, 60, 70, 80, 90});
    root.insert_child(root[0]) << vec; // 1
    root.insert_child(root[2]) << vec; // 3
    root.insert_child(root[4]) << vec; // 5
    root.insert_child(root[6]) << vec; // 7
    root.insert_child(root[8]) << vec; // 9
    root.append_child() << vec;        // 10

    root.remove_child(11);
    root.remove_child(9);
    root.remove_child(7);
    root.remove_child(5);
    root.remove_child(3);
    root.remove_child(1);

    noderef_check_tree(root);

    std::vector<std::vector<int>> vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
    root.prepend_child() << vec2; // 0
    root.insert_child(root[1]) << vec2; // 2
    root.insert_child(root[3]) << vec2; // 4
    root.insert_child(root[5]) << vec2; // 6
    root.insert_child(root[7]) << vec2; // 8
    root.insert_child(root[9]) << vec2; // 10
    root.append_child() << vec2;        // 12

    root.remove_child(12);
    root.remove_child(10);
    root.remove_child(8);
    root.remove_child(6);
    root.remove_child(4);
    root.remove_child(2);
    root.remove_child(0);

    noderef_check_tree(root);
}

TEST(NodeRef, move_in_same_parent)
{
    Tree t;
    NodeRef r = t;

    std::vector<std::vector<int>> vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
    std::map<std::string, int> map2({{"foo", 100}, {"bar", 200}, {"baz", 300}});

    r |= SEQ;
    r.append_child() << vec2;
    r.append_child() << map2;
    r.append_child() << "elm2";
    r.append_child() << "elm3";

    auto s = r[0];
    auto m = r[1];
    EXPECT_TRUE(s.is_seq());
    EXPECT_TRUE(m.is_map());
    EXPECT_EQ(s.num_children(), vec2.size());
    EXPECT_EQ(m.num_children(), map2.size());
    //printf("fonix"); print_tree(t); emit_yaml(r);
    r[0].move(r[1]);
    //printf("fonix"); print_tree(t); emit_yaml(r);
    EXPECT_EQ(r[0].get(), m.get());
    EXPECT_EQ(r[0].num_children(), map2.size());
    EXPECT_EQ(r[1].get(), s.get());
    EXPECT_EQ(r[1].num_children(), vec2.size());
}

TEST(NodeRef, move_to_other_parent)
{
    Tree t;
    NodeRef r = t;

    std::vector<std::vector<int>> vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
    std::map<std::string, int> map2({{"foo", 100}, {"bar", 200}, {"baz", 300}});

    r |= SEQ;
    r.append_child() << vec2;
    r.append_child() << map2;
    r.append_child() << "elm2";
    r.append_child() << "elm3";

    NodeData *elm2 = r[2].get();
    EXPECT_EQ(r[2].val(), "elm2");
    //printf("fonix"); print_tree(t); emit_yaml(r);
    r[2].move(r[0], r[0][0]);
    EXPECT_EQ(r[0][1].get(), elm2);
    EXPECT_EQ(r[0][1].val(), "elm2");
    //printf("fonix"); print_tree(t); emit_yaml(r);
}

TEST(NodeRef, duplicate)
{
    Tree t;
    NodeRef r = t;

    std::vector<std::vector<int>> vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
    std::map<std::string, int> map2({{"bar", 200}, {"baz", 300}, {"foo", 100}});

    r |= SEQ;
    r.append_child() << vec2;
    r.append_child() << map2;
    r.append_child() << "elm2";
    r.append_child() << "elm3";

    EXPECT_EQ(r[0][0].num_children(), 2u);
    NodeRef dup = r[1].duplicate(r[0][0], r[0][0][1]);
    EXPECT_EQ(r[0][0].num_children(), 3u);
    EXPECT_EQ(r[0][0][2].num_children(), map2.size());
    EXPECT_NE(dup.get(), r[1].get());
    EXPECT_EQ(dup[0].key(), "bar");
    EXPECT_EQ(dup[0].val(), "200");
    EXPECT_EQ(dup[1].key(), "baz");
    EXPECT_EQ(dup[1].val(), "300");
    EXPECT_EQ(dup[2].key(), "foo");
    EXPECT_EQ(dup[2].val(), "100");
    EXPECT_EQ(dup[0].key().str, r[1][0].key().str);
    EXPECT_EQ(dup[0].val().str, r[1][0].val().str);
    EXPECT_EQ(dup[0].key().len, r[1][0].key().len);
    EXPECT_EQ(dup[0].val().len, r[1][0].val().len);
    EXPECT_EQ(dup[1].key().str, r[1][1].key().str);
    EXPECT_EQ(dup[1].val().str, r[1][1].val().str);
    EXPECT_EQ(dup[1].key().len, r[1][1].key().len);
    EXPECT_EQ(dup[1].val().len, r[1][1].val().len);
}

TEST(NodeRef, intseq)
{
    Tree t = parse_in_arena("iseq: [8, 10]");
    NodeRef n = t["iseq"];
    int a, b;
    n[0] >> a;
    n[1] >> b;
    EXPECT_EQ(a, 8);
    EXPECT_EQ(b, 10);
}

TEST(NodeRef, vsConstNodeRef)
{
    Tree t = parse_in_arena("iseq: [8, 10]");
    Tree const& ct = t;
    NodeRef mseq = t["iseq"];
    ConstNodeRef seq = t["iseq"];
    EXPECT_EQ(mseq.tree(), seq.tree());
    EXPECT_EQ(mseq.id(), seq.id());
    EXPECT_TRUE(mseq == seq);
    EXPECT_FALSE(mseq != seq);
    EXPECT_TRUE(seq == mseq);
    EXPECT_FALSE(seq != mseq);
    // mseq = ct["iseq"]; // deliberate compile error
    seq = ct["iseq"]; // ok
    // mseq = seq; // deliberate compilation error
    seq = mseq; // ok
    {
        NodeData *nd = mseq.get();
        // nd = seq.get(); // deliberate compile error
        C4_UNUSED(nd);
    }
    {
        NodeData const* nd = seq.get();
        nd = seq.get(); // ok
        C4_UNUSED(nd);
    }
}


// see https://github.com/biojppm/rapidyaml/issues/294
TEST(NodeRef, overload_sets)
{
    // doc()
    {
        Tree t = parse_in_arena("a\n---\nb");
        NodeRef n = t;
        NodeRef const nc = t;
        ConstNodeRef const cn = t;
        EXPECT_EQ(n.doc(0), nc.doc(0));
        EXPECT_EQ(n.doc(0), cn.doc(0));
    }
    Tree t = parse_in_arena("{iseq: [8, 10], imap: {a: b, c: d}}");
    NodeRef n = t;
    NodeRef const nc = t;
    ConstNodeRef const cn = t;
    // get()
    {
        EXPECT_EQ(n["iseq"].get(), nc["iseq"].get());
        EXPECT_EQ(n["iseq"].get(), cn["iseq"].get());
    }
    // parent()
    {
        EXPECT_EQ(n["iseq"].parent(), nc["iseq"].parent());
        EXPECT_EQ(n["iseq"].parent(), cn["iseq"].parent());
    }
    // child_pos()
    {
        EXPECT_EQ(n["iseq"].child_pos(n["iseq"][0]), nc["iseq"].child_pos(n["iseq"][0]));
        EXPECT_EQ(n["iseq"].child_pos(n["iseq"][0]), cn["iseq"].child_pos(n["iseq"][0]));
    }
    // num_children()
    {
        EXPECT_EQ(n["iseq"].num_children(), nc["iseq"].num_children());
        EXPECT_EQ(n["iseq"].num_children(), cn["iseq"].num_children());
    }
    // first_child()
    {
        EXPECT_EQ(n["iseq"].first_child(), nc["iseq"].first_child());
        EXPECT_EQ(n["iseq"].first_child(), cn["iseq"].first_child());
    }
    // last_child()
    {
        EXPECT_EQ(n["iseq"].last_child(), nc["iseq"].last_child());
        EXPECT_EQ(n["iseq"].last_child(), cn["iseq"].last_child());
    }
    // child()
    {
        EXPECT_EQ(n["iseq"].child(0), nc["iseq"].child(0));
        EXPECT_EQ(n["iseq"].child(0), cn["iseq"].child(0));
    }
    // find_child()
    {
        EXPECT_EQ(n.find_child("iseq"), nc.find_child("iseq"));
        EXPECT_EQ(n.find_child("iseq"), cn.find_child("iseq"));
    }
    // prev_sibling()
    {
        EXPECT_EQ(n["iseq"][1].prev_sibling(), nc["iseq"][1].prev_sibling());
        EXPECT_EQ(n["iseq"][1].prev_sibling(), cn["iseq"][1].prev_sibling());
    }
    // next_sibling()
    {
        EXPECT_EQ(n["iseq"][0].next_sibling(), nc["iseq"][0].next_sibling());
        EXPECT_EQ(n["iseq"][0].next_sibling(), cn["iseq"][0].next_sibling());
    }
    // first_sibling()
    {
        EXPECT_EQ(n["iseq"][1].first_sibling(), nc["iseq"][1].first_sibling());
        EXPECT_EQ(n["iseq"][1].first_sibling(), cn["iseq"][1].first_sibling());
    }
    // last_sibling()
    {
        EXPECT_EQ(n["iseq"][0].last_sibling(), nc["iseq"][0].last_sibling());
        EXPECT_EQ(n["iseq"][0].last_sibling(), cn["iseq"][0].last_sibling());
    }
    // sibling()
    {
        EXPECT_EQ(n["iseq"][1].sibling(0), nc["iseq"][1].sibling(0));
        EXPECT_EQ(n["iseq"][1].sibling(0), cn["iseq"][1].sibling(0));
    }
    // find_sibling()
    {
        EXPECT_EQ(n["iseq"].find_sibling("imap"), nc["iseq"].find_sibling("imap"));
        EXPECT_EQ(n["iseq"].find_sibling("imap"), cn["iseq"].find_sibling("imap"));
    }
    // operator[](csubstr)
    {
        EXPECT_EQ(n["iseq"].id(), nc["iseq"].id());
        EXPECT_EQ(n["iseq"].id(), cn["iseq"].id());
    }
    // operator[](size_t)
    {
        EXPECT_EQ(n["iseq"][0].id(), nc["iseq"][0].id());
        EXPECT_EQ(n["iseq"][0].id(), cn["iseq"][0].id());
    }
    // begin()
    {
        EXPECT_EQ(n["iseq"].begin().m_child_id, nc["iseq"].begin().m_child_id);
        EXPECT_EQ(n["iseq"].begin().m_child_id, cn["iseq"].begin().m_child_id);
    }
    // end()
    {
        EXPECT_EQ(n["iseq"].end().m_child_id, nc["iseq"].end().m_child_id);
        EXPECT_EQ(n["iseq"].end().m_child_id, cn["iseq"].end().m_child_id);
    }
    // cbegin()
    {
        EXPECT_EQ(n["iseq"].cbegin().m_child_id, nc["iseq"].cbegin().m_child_id);
        EXPECT_EQ(n["iseq"].cbegin().m_child_id, cn["iseq"].cbegin().m_child_id);
    }
    // cend()
    {
        EXPECT_EQ(n["iseq"].cend().m_child_id, nc["iseq"].cend().m_child_id);
        EXPECT_EQ(n["iseq"].cend().m_child_id, cn["iseq"].cend().m_child_id);
    }
    // children()
    {
        EXPECT_EQ(n["iseq"].children().b.m_child_id, nc["iseq"].children().b.m_child_id);
        EXPECT_EQ(n["iseq"].children().b.m_child_id, cn["iseq"].children().b.m_child_id);
    }
    // cchildren()
    {
        EXPECT_EQ(n["iseq"].cchildren().b.m_child_id, nc["iseq"].cchildren().b.m_child_id);
        EXPECT_EQ(n["iseq"].cchildren().b.m_child_id, cn["iseq"].cchildren().b.m_child_id);
    }
    // siblings()
    {
        EXPECT_EQ(n["iseq"][0].siblings().b.m_child_id, nc["iseq"][0].siblings().b.m_child_id);
        EXPECT_EQ(n["iseq"][0].siblings().b.m_child_id, cn["iseq"][0].siblings().b.m_child_id);
        EXPECT_EQ(n.siblings().b.m_child_id, nc.siblings().b.m_child_id);
        EXPECT_EQ(n.siblings().b.m_child_id, cn.siblings().b.m_child_id);
    }
    // csiblings()
    {
        EXPECT_EQ(n["iseq"][0].csiblings().b.m_child_id, nc["iseq"][0].csiblings().b.m_child_id);
        EXPECT_EQ(n["iseq"][0].csiblings().b.m_child_id, cn["iseq"][0].csiblings().b.m_child_id);
        EXPECT_EQ(n.csiblings().b.m_child_id, nc.csiblings().b.m_child_id);
        EXPECT_EQ(n.csiblings().b.m_child_id, cn.csiblings().b.m_child_id);
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

#if defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif
