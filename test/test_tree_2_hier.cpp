#include "./test_tree.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

static constexpr const ExpectedErrorType visit = ExpectedErrorType::err_visit;

TEST(Tree, get)
{
    Tree t = parse_in_arena("[0, 1, 2, 3]");
    Tree const& ct = t;
    {
        EXPECT_EQ(t.get(NONE), nullptr);
        EXPECT_EQ(t.id(t.get(NONE)), NONE);
        EXPECT_EQ(t.id(t.get(0)), 0);
        EXPECT_EQ(t.id(t.get(1)), 1);
        EXPECT_EQ(t.id(t.get(2)), 2);
        EXPECT_EQ(t.id(t.get(3)), 3);
        EXPECT_EQ(t.id(t.get(4)), 4);
        verify_assertion(t, [](Tree & tree){ return tree.get(tree.capacity()); }, visit);
    }
    {
        EXPECT_EQ(ct.get(NONE), nullptr);
        EXPECT_EQ(ct.id(ct.get(NONE)), NONE);
        EXPECT_EQ(ct.id(ct.get(0)), 0);
        EXPECT_EQ(ct.id(ct.get(1)), 1);
        EXPECT_EQ(ct.id(ct.get(2)), 2);
        EXPECT_EQ(ct.id(ct.get(3)), 3);
        EXPECT_EQ(ct.id(ct.get(4)), 4);
        verify_assertion(t, [&](Tree &){ return ct.get(ct.capacity()); }, visit);
    }
}

TEST(Tree, ref)
{
    Tree t = parse_in_arena("[0, 1, 2, 3]");
    EXPECT_EQ(t.ref(0).id(), 0);
    EXPECT_EQ(t.ref(1).id(), 1);
    EXPECT_EQ(t.ref(2).id(), 2);
    EXPECT_EQ(t.ref(3).id(), 3);
    EXPECT_EQ(t.ref(4).id(), 4);
    EXPECT_TRUE(t.ref(0).is_seq());
    EXPECT_TRUE(t.ref(1).is_val());
    EXPECT_TRUE(t.ref(2).is_val());
    EXPECT_TRUE(t.ref(3).is_val());
    EXPECT_TRUE(t.ref(4).is_val());
    verify_assertion(t, [](Tree & tree){ return tree.ref(tree.capacity()); }, visit);
    verify_assertion(t, [](Tree & tree){ return tree.ref(NONE); }, visit);
}

TEST(Tree, cref)
{
    Tree t = parse_in_arena("[0, 1, 2, 3]");
    EXPECT_EQ(t.cref(0).id(), 0);
    EXPECT_EQ(t.cref(1).id(), 1);
    EXPECT_EQ(t.cref(2).id(), 2);
    EXPECT_EQ(t.cref(3).id(), 3);
    EXPECT_EQ(t.cref(4).id(), 4);
    EXPECT_TRUE(t.cref(0).is_seq());
    EXPECT_TRUE(t.cref(1).is_val());
    EXPECT_TRUE(t.cref(2).is_val());
    EXPECT_TRUE(t.cref(3).is_val());
    EXPECT_TRUE(t.cref(4).is_val());
    verify_assertion(t, [](Tree & tree){ return tree.cref(tree.capacity()); }, visit);
    verify_assertion(t, [](Tree & tree){ return tree.cref(NONE); }, visit);
}

TEST(Tree, ref_const)
{
    const Tree t = parse_in_arena("[0, 1, 2, 3]");
    EXPECT_EQ(t.ref(0).id(), 0);
    EXPECT_EQ(t.ref(1).id(), 1);
    EXPECT_EQ(t.ref(2).id(), 2);
    EXPECT_EQ(t.ref(3).id(), 3);
    EXPECT_EQ(t.ref(4).id(), 4);
    EXPECT_TRUE(t.ref(0).is_seq());
    EXPECT_TRUE(t.ref(1).is_val());
    EXPECT_TRUE(t.ref(2).is_val());
    EXPECT_TRUE(t.ref(3).is_val());
    EXPECT_TRUE(t.ref(4).is_val());
    verify_assertion("[0, 1, 2, 3]", [](Tree const& tree){ return tree.cref(tree.capacity()); }, visit);
    verify_assertion("[0, 1, 2, 3]", [](Tree const& tree){ return tree.cref(NONE); }, visit);
}


TEST(Tree, operator_square_brackets_seq)
{
    Tree t = parse_in_arena("[0, 1, 2, 3, 4]");
    Tree &m = t;
    Tree const& cm = t;
    EXPECT_EQ(m[0].tree(), &t);
    EXPECT_EQ(m[0].val(), "0");
    EXPECT_EQ(m[1].val(), "1");
    EXPECT_EQ(m[2].val(), "2");
    EXPECT_EQ(m[3].val(), "3");
    EXPECT_EQ(m[4].val(), "4");
    EXPECT_EQ(cm[0].tree(), &t);
    EXPECT_EQ(cm[0].val(), "0");
    EXPECT_EQ(cm[1].val(), "1");
    EXPECT_EQ(cm[2].val(), "2");
    EXPECT_EQ(cm[3].val(), "3");
    EXPECT_EQ(cm[4].val(), "4");
    //
    verify_assertion(t, [&](Tree const&){ return cm[m.capacity()]; }, visit);
    verify_assertion(t, [&](Tree const&){ return cm[NONE]; }, visit);
    verify_assertion(t, [&](Tree const&){ return m[5][0]; }, visit);
    verify_assertion(t, [&](Tree const&){ return m[5]["a"]; }, visit);
}

TEST(Tree, operator_square_brackets_map)
{
    Tree t = parse_in_arena("{a: 0, b: 1, c: 2, d: 3, e: 4}");
    Tree &m = t;
    Tree const& cm = t;
    EXPECT_EQ(m["a"].val(), "0");
    EXPECT_EQ(m["b"].val(), "1");
    EXPECT_EQ(m["c"].val(), "2");
    EXPECT_EQ(m["d"].val(), "3");
    EXPECT_EQ(m["e"].val(), "4");
    EXPECT_EQ(cm["a"].val(), "0");
    EXPECT_EQ(cm["b"].val(), "1");
    EXPECT_EQ(cm["c"].val(), "2");
    EXPECT_EQ(cm["d"].val(), "3");
    EXPECT_EQ(cm["e"].val(), "4");
    //
    verify_assertion(t, [&](Tree const&){ return cm["f"]; }, visit);
    verify_assertion(t, [&](Tree const&){ return m["f"]["h"]; }, visit);
}

TEST(Tree, noderef_at_seq)
{
    Tree t = parse_in_arena("[0, 1, 2, 3, 4]");
    NodeRef m = t.rootref();
    ConstNodeRef const cm = t.rootref();
    EXPECT_EQ(m.at(0).val(), "0");
    EXPECT_EQ(m.at(1).val(), "1");
    EXPECT_EQ(m.at(2).val(), "2");
    EXPECT_EQ(m.at(3).val(), "3");
    EXPECT_EQ(m.at(4).val(), "4");
    EXPECT_EQ(cm.at(0).val(), "0");
    EXPECT_EQ(cm.at(1).val(), "1");
    EXPECT_EQ(cm.at(2).val(), "2");
    EXPECT_EQ(cm.at(3).val(), "3");
    EXPECT_EQ(cm.at(4).val(), "4");
    //
    EXPECT_EQ(cm.num_children(), 5);
    EXPECT_EQ(cm.num_children(), m.num_children());
    //
    verify_error(t, [&](Tree const&){ return cm.at(NONE); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at(t.capacity()); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at(5); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at(6); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at(7); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at(10); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at(0).at(0); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at("a"); }, visit);
    //
    verify_error(t, [&](Tree const&){ return m.at(NONE); }, visit);
    verify_error(t, [&](Tree const&){ return m.at(t.capacity()); }, visit);
    verify_success(t, [&](Tree const&){ return m.at(5); });
    verify_success(t, [&](Tree const&){ return m.at(6); });
    verify_error(t, [&](Tree const&){ return m.at(0).at(0); }, visit);
    verify_error(t, [&](Tree const&){ return m.at("a"); }, visit);
    EXPECT_TRUE(m.at(5).is_seed());
    EXPECT_TRUE(m.at(6).is_seed());
    //
    NodeRef to_be_removed_orig = m.at(4);
    NodeRef to_be_removed = to_be_removed_orig;
    EXPECT_EQ(to_be_removed.id(), 5);
    EXPECT_EQ(to_be_removed_orig.id(), 5);
    m.remove_child(to_be_removed);
    EXPECT_EQ(to_be_removed.id(), NONE); // it is stale now
    EXPECT_EQ(to_be_removed_orig.id(), 5); // it is stale now
    EXPECT_EQ(m.num_children(), 4);
    EXPECT_TRUE(m.at(4).is_seed());
    verify_success(t, [&](Tree const&){ return m.at(4); });
    verify_error(t, [&](Tree const&){ return cm.at(4); }, visit);
}

TEST(Tree, noderef_at_map)
{
    Tree t = parse_in_arena("{a: 0, b: 1, c: 2, d: 3, e: 4}");
    NodeRef m = t.rootref();
    ConstNodeRef const cm = t.rootref();
    EXPECT_EQ(m.at("a").val(), "0");
    EXPECT_EQ(m.at("b").val(), "1");
    EXPECT_EQ(m.at("c").val(), "2");
    EXPECT_EQ(m.at("d").val(), "3");
    EXPECT_EQ(m.at("e").val(), "4");
    EXPECT_EQ(cm.at("a").val(), "0");
    EXPECT_EQ(cm.at("b").val(), "1");
    EXPECT_EQ(cm.at("c").val(), "2");
    EXPECT_EQ(cm.at("d").val(), "3");
    EXPECT_EQ(cm.at("e").val(), "4");
    //
    verify_error(t, [&](Tree const&){ return cm.at(t.capacity()); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at(NONE); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at(cm.num_children()); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at(5); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at(6); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at("f"); }, visit);
    verify_error(t, [&](Tree const&){ return cm.at("g").at("h"); }, visit);
    //
    verify_error(t, [&](Tree const&){ return m.at(t.capacity()); }, visit);
    verify_error(t, [&](Tree const&){ return m.at(NONE); }, visit);
    verify_success(t, [&](Tree const&){ return m.at(cm.num_children()); });
    verify_success(t, [&](Tree const&){ return m.at(5); });
    verify_success(t, [&](Tree const&){ return m.at(6); });
    verify_success(t, [&](Tree const&){ return m.at("f"); });
    verify_error(t, [&](Tree const&){ return m.at("g").at("h"); }, visit);
    EXPECT_TRUE(m.at(cm.num_children()).is_seed());
    EXPECT_TRUE(m.at(5).is_seed());
    EXPECT_TRUE(m.at(6).is_seed());
    EXPECT_TRUE(m.at("f").is_seed());
}

TEST(Tree, doc)
{
    Tree t = parse_in_arena(R"(---
doc0
---
doc1
---
doc2
---
doc3
---
doc4
)");
    id_type ir = t.root_id();
    ASSERT_EQ(t.num_children(ir), 5u);
    ASSERT_TRUE(t.is_stream(ir));
    EXPECT_EQ(t.child(ir, 0), t.doc(0));
    EXPECT_EQ(t.child(ir, 1), t.doc(1));
    EXPECT_EQ(t.child(ir, 2), t.doc(2));
    EXPECT_EQ(t.child(ir, 3), t.doc(3));
    EXPECT_EQ(t.child(ir, 4), t.doc(4));
    {
        NodeRef r = t.rootref();
        EXPECT_EQ(r.id(), ir);
        EXPECT_EQ(r.child(0), r.doc(0));
        EXPECT_EQ(r.child(1), r.doc(1));
        EXPECT_EQ(r.child(2), r.doc(2));
        EXPECT_EQ(r.child(3), r.doc(3));
        EXPECT_EQ(r.child(4), r.doc(4));
        EXPECT_EQ(r.child(0).id(), t.doc(0));
        EXPECT_EQ(r.child(1).id(), t.doc(1));
        EXPECT_EQ(r.child(2).id(), t.doc(2));
        EXPECT_EQ(r.child(3).id(), t.doc(3));
        EXPECT_EQ(r.child(4).id(), t.doc(4));
        EXPECT_EQ(r.child(0).id(), t.docref(0).id());
        EXPECT_EQ(r.child(1).id(), t.docref(1).id());
        EXPECT_EQ(r.child(2).id(), t.docref(2).id());
        EXPECT_EQ(r.child(3).id(), t.docref(3).id());
        EXPECT_EQ(r.child(4).id(), t.docref(4).id());
    }
    {
        const Tree &ct = t;
        const ConstNodeRef r = ct.rootref();
        EXPECT_EQ(r.id(), ir);
        EXPECT_EQ(r.child(0), r.doc(0));
        EXPECT_EQ(r.child(1), r.doc(1));
        EXPECT_EQ(r.child(2), r.doc(2));
        EXPECT_EQ(r.child(3), r.doc(3));
        EXPECT_EQ(r.child(4), r.doc(4));
        EXPECT_EQ(r.child(0).id(), t.doc(0));
        EXPECT_EQ(r.child(1).id(), t.doc(1));
        EXPECT_EQ(r.child(2).id(), t.doc(2));
        EXPECT_EQ(r.child(3).id(), t.doc(3));
        EXPECT_EQ(r.child(4).id(), t.doc(4));
        EXPECT_EQ(r.child(0).id(), t.docref(0).id());
        EXPECT_EQ(r.child(1).id(), t.docref(1).id());
        EXPECT_EQ(r.child(2).id(), t.docref(2).id());
        EXPECT_EQ(r.child(3).id(), t.docref(3).id());
        EXPECT_EQ(r.child(4).id(), t.docref(4).id());
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TEST(Tree, num_children)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    EXPECT_EQ(t.num_children(stream_id), 1u);
    EXPECT_EQ(t.num_children(doc_id), 2u);
    EXPECT_EQ(t.num_children(map_id), 2u);
    EXPECT_EQ(t.num_children(keyval_id), 0u);
    EXPECT_EQ(t.num_children(seq_id), 2u);
    EXPECT_EQ(t.num_children(val_id), 0u);
    EXPECT_EQ(stream.num_children(), t.num_children(stream_id));
    EXPECT_EQ(doc.num_children(), t.num_children(doc_id));
    EXPECT_EQ(map.num_children(), t.num_children(map_id));
    EXPECT_EQ(keyval.num_children(), t.num_children(keyval_id));
    EXPECT_EQ(seq.num_children(), t.num_children(seq_id));
    EXPECT_EQ(val.num_children(), t.num_children(val_id));
    EXPECT_EQ(mstream.num_children(), t.num_children(stream_id));
    EXPECT_EQ(mdoc.num_children(), t.num_children(doc_id));
    EXPECT_EQ(mmap.num_children(), t.num_children(map_id));
    EXPECT_EQ(mkeyval.num_children(), t.num_children(keyval_id));
    EXPECT_EQ(mseq.num_children(), t.num_children(seq_id));
    EXPECT_EQ(mval.num_children(), t.num_children(val_id));
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].num_children(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).num_children(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.num_children(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.num_children(NONE); }, visit);
}

TEST(Tree, child)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    EXPECT_EQ(t.child(stream_id, 0), doc_id);
    EXPECT_EQ(t.child(doc_id, 0), map_id);
    EXPECT_EQ(t.child(map_id, 0), keyval_id);
    EXPECT_EQ(t.child(keyval_id, 0), (id_type)NONE);
    EXPECT_EQ(t.child(seq_id, 0), val_id);
    EXPECT_EQ(t.child(val_id, 0), (id_type)NONE);
    EXPECT_EQ(stream.child(0).id(), t.child(stream_id, 0));
    EXPECT_EQ(doc.child(0).id(), t.child(doc_id, 0));
    EXPECT_EQ(map.child(0).id(), t.child(map_id, 0));
    EXPECT_EQ(keyval.child(0).id(), t.child(keyval_id, 0));
    EXPECT_EQ(seq.child(0).id(), t.child(seq_id, 0));
    EXPECT_EQ(val.child(0).id(), t.child(val_id, 0));
    EXPECT_EQ(mstream.child(0).id(), t.child(stream_id, 0));
    EXPECT_EQ(mdoc.child(0).id(), t.child(doc_id, 0));
    EXPECT_EQ(mmap.child(0).id(), t.child(map_id, 0));
    EXPECT_EQ(mkeyval.child(0).id(), t.child(keyval_id, 0));
    EXPECT_EQ(mseq.child(0).id(), t.child(seq_id, 0));
    EXPECT_EQ(mval.child(0).id(), t.child(val_id, 0));
    //
    ReadResult res;
    id_type node;
    node = 0;    res = t.child_r(stream_id, 0, &node); EXPECT_TRUE(res);  EXPECT_EQ(node, doc_id);
    node = 0;    res = t.child_r(doc_id, 0, &node);    EXPECT_TRUE(res);  EXPECT_EQ(node, map_id);
    node = 0;    res = t.child_r(map_id, 0, &node);    EXPECT_TRUE(res);  EXPECT_EQ(node, keyval_id);
    node = NONE; res = t.child_r(keyval_id, 0, &node); EXPECT_FALSE(res); EXPECT_EQ(node, (id_type)NONE); EXPECT_EQ(res.node, keyval_id);
    node = 0;    res = t.child_r(seq_id, 0, &node);    EXPECT_TRUE(res);  EXPECT_EQ(node, val_id);
    node = NONE; res = t.child_r(val_id, 0, &node);    EXPECT_FALSE(res); EXPECT_EQ(node, (id_type)NONE); EXPECT_EQ(res.node, val_id);
    NodeRef n;
    ConstNodeRef cn;
    cn = {};   res = stream.child_r(0, &cn); EXPECT_TRUE(res);  EXPECT_EQ(cn.id(), t.child(stream_id, 0));
    cn = {};   res = doc.child_r(0, &cn);    EXPECT_TRUE(res);  EXPECT_EQ(cn.id(), t.child(doc_id, 0));
    cn = {};   res = map.child_r(0, &cn);    EXPECT_TRUE(res);  EXPECT_EQ(cn.id(), t.child(map_id, 0));
    cn = {};   res = keyval.child_r(0, &cn); EXPECT_FALSE(res); EXPECT_EQ(cn.id(), NONE);
    cn = {};   res = seq.child_r(0, &cn);    EXPECT_TRUE(res);  EXPECT_EQ(cn.id(), t.child(seq_id, 0));
    cn = {};   res = val.child_r(0, &cn);    EXPECT_FALSE(res); EXPECT_EQ(cn.id(), NONE);
    n = NodeRef{}; res = mstream.child_r(0, &n); EXPECT_TRUE(res);  EXPECT_EQ(n.id(), t.child(stream_id, 0));
    n = NodeRef{}; res = mdoc.child_r(0, &n);    EXPECT_TRUE(res);  EXPECT_EQ(n.id(), t.child(doc_id, 0));
    n = NodeRef{}; res = mmap.child_r(0, &n);    EXPECT_TRUE(res);  EXPECT_EQ(n.id(), t.child(map_id, 0));
    n = NodeRef{}; res = mkeyval.child_r(0, &n); EXPECT_FALSE(res); EXPECT_EQ(n.id(), NONE);
    n = NodeRef{}; res = mseq.child_r(0, &n);    EXPECT_TRUE(res);  EXPECT_EQ(n.id(), t.child(seq_id, 0));
    n = NodeRef{}; res = mval.child_r(0, &n);    EXPECT_FALSE(res); EXPECT_EQ(n.id(), NONE);
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].child(0); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).child(0); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.child(t.capacity(), 0); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.child(NONE, 0); }, visit);
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].child_r(0, &n); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).child_r(0, &n); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.child_r(t.capacity(), 0, &node); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.child_r(NONE, 0, &node); }, visit);
}

TEST(Tree, find_child_by_name)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    ConstNodeRef doc = t.cref(doc_id);
    ConstNodeRef map = t.cref(map_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    EXPECT_EQ(t.find_child(doc_id, "map"), map_id);
    EXPECT_EQ(t.find_child(doc_id, "seq"), seq_id);
    EXPECT_EQ(t.find_child(doc_id, "..."), (id_type)NONE);
    EXPECT_EQ(t.find_child(map_id, "foo"), keyval_id);
    EXPECT_EQ(t.find_child(map_id, "bar"), (id_type)NONE);
    EXPECT_EQ(doc.find_child("map").id(), t.find_child(doc_id, "map"));
    EXPECT_EQ(doc.find_child("seq").id(), t.find_child(doc_id, "seq"));
    EXPECT_EQ(doc.find_child("...").id(), t.find_child(doc_id, "..."));
    EXPECT_EQ(map.find_child("foo").id(), t.find_child(map_id, "foo"));
    EXPECT_EQ(map.find_child("bar").id(), t.find_child(map_id, "bar"));
    EXPECT_EQ(mdoc.find_child("map").id(), t.find_child(doc_id, "map"));
    EXPECT_EQ(mdoc.find_child("seq").id(), t.find_child(doc_id, "seq"));
    EXPECT_EQ(mdoc.find_child("...").id(), t.find_child(doc_id, "..."));
    EXPECT_EQ(mmap.find_child("foo").id(), t.find_child(map_id, "foo"));
    EXPECT_EQ(mmap.find_child("bar").id(), t.find_child(map_id, "bar"));
    //
    ReadResult res;
    id_type node;
    node = NONE; res = t.find_child_r(doc_id, "map", &node); EXPECT_TRUE(res);  EXPECT_EQ(node, map_id);
    node = NONE; res = t.find_child_r(doc_id, "seq", &node); EXPECT_TRUE(res);  EXPECT_EQ(node, seq_id);
    node = 0;    res = t.find_child_r(doc_id, "...", &node); EXPECT_FALSE(res); EXPECT_EQ(node, (id_type)NONE); EXPECT_EQ(res.node, doc_id);
    node = NONE; res = t.find_child_r(map_id, "foo", &node); EXPECT_TRUE(res);  EXPECT_EQ(node, keyval_id);
    node = 0;    res = t.find_child_r(map_id, "bar", &node); EXPECT_FALSE(res); EXPECT_EQ(node, (id_type)NONE); EXPECT_EQ(res.node, map_id);
    ConstNodeRef cn;
    cn = {}; res = doc.find_child_r("map", &cn); EXPECT_TRUE(res);  EXPECT_EQ(cn.id(), t.find_child(doc_id, "map"));
    cn = {}; res = doc.find_child_r("seq", &cn); EXPECT_TRUE(res);  EXPECT_EQ(cn.id(), t.find_child(doc_id, "seq"));
    cn = {}; res = doc.find_child_r("...", &cn); EXPECT_FALSE(res); EXPECT_EQ(cn.id(), t.find_child(doc_id, "...")); EXPECT_EQ(res.node, doc_id);
    cn = {}; res = map.find_child_r("foo", &cn); EXPECT_TRUE(res);  EXPECT_EQ(cn.id(), t.find_child(map_id, "foo"));
    cn = {}; res = map.find_child_r("bar", &cn); EXPECT_FALSE(res); EXPECT_EQ(cn.id(), t.find_child(map_id, "bar")); EXPECT_EQ(res.node, map_id);
    NodeRef n;
    n = NodeRef{}; res = mdoc.find_child_r("map", &n); EXPECT_TRUE(res);  EXPECT_EQ(n.id(), t.find_child(doc_id, "map"));
    n = NodeRef{}; res = mdoc.find_child_r("seq", &n); EXPECT_TRUE(res);  EXPECT_EQ(n.id(), t.find_child(doc_id, "seq"));
    n = NodeRef{}; res = mdoc.find_child_r("...", &n); EXPECT_FALSE(res); EXPECT_EQ(n.id(), t.find_child(doc_id, "...")); EXPECT_EQ(res.node, doc_id);
    n = NodeRef{}; res = mmap.find_child_r("foo", &n); EXPECT_TRUE(res);  EXPECT_EQ(n.id(), t.find_child(map_id, "foo"));
    n = NodeRef{}; res = mmap.find_child_r("bar", &n); EXPECT_FALSE(res); EXPECT_EQ(n.id(), t.find_child(map_id, "bar")); EXPECT_EQ(res.node, map_id);
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].find_child("foo"); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).find_child("foo"); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.find_child(t.capacity(), "foo"); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.find_child(NONE, "foo"); }, visit);
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].find_child_r("foo", &n); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).find_child_r("foo", &n); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.find_child_r(t.capacity(), "foo", &node); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.find_child_r(NONE, "foo", &node); }, visit);
}

TEST(Tree, find_sibling_by_name)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type seq_id = t.last_child(doc_id);
    ConstNodeRef map = t.cref(map_id);
    ConstNodeRef seq = t.cref(seq_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mseq = t.ref(seq_id);
    //
    EXPECT_EQ(t.find_sibling(map_id, "map"), map_id);
    EXPECT_EQ(t.find_sibling(map_id, "seq"), seq_id);
    EXPECT_EQ(t.find_sibling(map_id, "..."), (id_type)NONE);
    EXPECT_EQ(t.find_sibling(seq_id, "map"), map_id);
    EXPECT_EQ(t.find_sibling(seq_id, "seq"), seq_id);
    EXPECT_EQ(t.find_sibling(seq_id, "..."), (id_type)NONE);
    //
    EXPECT_TRUE(t.has_sibling(map_id, "map"));
    EXPECT_TRUE(t.has_sibling(map_id, "seq"));
    EXPECT_TRUE(t.has_sibling(map_id, map.id()));
    EXPECT_TRUE(t.has_sibling(map_id, seq.id()));
    EXPECT_FALSE(t.has_sibling(map_id, "..."));
    EXPECT_TRUE(t.has_sibling(seq_id, "map"));
    EXPECT_TRUE(t.has_sibling(seq_id, "seq"));
    EXPECT_TRUE(t.has_sibling(seq_id, map.id()));
    EXPECT_TRUE(t.has_sibling(seq_id, seq.id()));
    EXPECT_FALSE(t.has_sibling(seq_id, "..."));
    //
    EXPECT_EQ(t.find_sibling(map_id, "map"), t.find_child(doc_id, "map"));
    EXPECT_EQ(t.find_sibling(map_id, "seq"), t.find_child(doc_id, "seq"));
    EXPECT_EQ(t.find_sibling(map_id, "..."), t.find_child(doc_id, "..."));
    EXPECT_EQ(t.find_sibling(seq_id, "map"), t.find_child(doc_id, "map"));
    EXPECT_EQ(t.find_sibling(seq_id, "seq"), t.find_child(doc_id, "seq"));
    EXPECT_EQ(t.find_sibling(seq_id, "..."), t.find_child(doc_id, "..."));
    //
    EXPECT_TRUE(t.has_sibling(map_id, "map"));
    EXPECT_TRUE(t.has_sibling(map_id, "seq"));
    EXPECT_FALSE(t.has_sibling(map_id, "..."));
    EXPECT_TRUE(t.has_sibling(seq_id, "map"));
    EXPECT_TRUE(t.has_sibling(seq_id, "seq"));
    EXPECT_FALSE(t.has_sibling(seq_id, "..."));
    //
    EXPECT_EQ(map.find_sibling("map").id(), t.find_sibling(map_id, "map"));
    EXPECT_EQ(map.find_sibling("seq").id(), t.find_sibling(map_id, "seq"));
    EXPECT_EQ(map.find_sibling("...").id(), t.find_sibling(map_id, "..."));
    EXPECT_EQ(seq.find_sibling("map").id(), t.find_sibling(seq_id, "map"));
    EXPECT_EQ(seq.find_sibling("seq").id(), t.find_sibling(seq_id, "seq"));
    EXPECT_EQ(seq.find_sibling("...").id(), t.find_sibling(seq_id, "..."));
    //
    EXPECT_TRUE(map.has_sibling("map"));
    EXPECT_TRUE(map.has_sibling("seq"));
    EXPECT_TRUE(map.has_sibling(map.id()));
    EXPECT_TRUE(map.has_sibling(seq.id()));
    EXPECT_TRUE(map.has_sibling(map));
    EXPECT_TRUE(map.has_sibling(seq));
    EXPECT_FALSE(map.has_sibling("..."));
    EXPECT_TRUE(seq.has_sibling("map"));
    EXPECT_TRUE(seq.has_sibling("seq"));
    EXPECT_TRUE(seq.has_sibling(map.id()));
    EXPECT_TRUE(seq.has_sibling(seq.id()));
    EXPECT_TRUE(seq.has_sibling(map));
    EXPECT_TRUE(seq.has_sibling(seq));
    EXPECT_FALSE(seq.has_sibling("..."));
    //
    EXPECT_EQ(mmap.find_sibling("map").id(), t.find_sibling(map_id, "map"));
    EXPECT_EQ(mmap.find_sibling("seq").id(), t.find_sibling(map_id, "seq"));
    EXPECT_EQ(mmap.find_sibling("...").id(), t.find_sibling(map_id, "..."));
    EXPECT_EQ(mseq.find_sibling("map").id(), t.find_sibling(seq_id, "map"));
    EXPECT_EQ(mseq.find_sibling("seq").id(), t.find_sibling(seq_id, "seq"));
    EXPECT_EQ(mseq.find_sibling("...").id(), t.find_sibling(seq_id, "..."));
    //
    EXPECT_TRUE(mmap.has_sibling("map"));
    EXPECT_TRUE(mmap.has_sibling("seq"));
    EXPECT_TRUE(mmap.has_sibling(map.id()));
    EXPECT_TRUE(mmap.has_sibling(seq.id()));
    EXPECT_TRUE(mmap.has_sibling(map));
    EXPECT_TRUE(mmap.has_sibling(seq));
    EXPECT_FALSE(mmap.has_sibling("..."));
    EXPECT_TRUE(mseq.has_sibling("map"));
    EXPECT_TRUE(mseq.has_sibling("seq"));
    EXPECT_TRUE(mseq.has_sibling(map.id()));
    EXPECT_TRUE(mseq.has_sibling(seq.id()));
    EXPECT_TRUE(mseq.has_sibling(map));
    EXPECT_TRUE(mseq.has_sibling(seq));
    EXPECT_FALSE(mseq.has_sibling("..."));
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].find_sibling("foo"); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).find_sibling("foo"); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.find_sibling(t.capacity(), "foo"); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.find_sibling(NONE, "foo"); }, visit);
}


TEST(Tree, docref)
{
    Tree tree = parse_in_arena(R"(--- doc0
--- doc1
--- doc2
--- doc3
)");
    EXPECT_EQ(tree.doc(0), 1u);
    EXPECT_EQ(tree.doc(1), 2u);
    EXPECT_EQ(tree.doc(2), 3u);
    EXPECT_EQ(tree.doc(3), 4u);
    EXPECT_EQ(tree.docref(0).val(), "doc0");
    EXPECT_EQ(tree.docref(1).val(), "doc1");
    EXPECT_EQ(tree.docref(2).val(), "doc2");
    EXPECT_EQ(tree.docref(3).val(), "doc3");
    EXPECT_EQ(tree.cdocref(0).val(), "doc0");
    EXPECT_EQ(tree.cdocref(1).val(), "doc1");
    EXPECT_EQ(tree.cdocref(2).val(), "doc2");
    EXPECT_EQ(tree.cdocref(3).val(), "doc3");
}


TEST(Tree, ancestor_doc)
{
    Tree tree = parse_in_arena(R"(---
doc0:
 - a
 - b
---
doc1:
  doc1:
   - a
   - b
---
doc2:
  doc2:
    doc2:
     - a
     - b
---
doc3:
  doc3:
    doc3:
      doc3:
        - a
        - b
)");
    {
        id_type doc0 = tree.doc(0);
        id_type doc1 = tree.doc(1);
        id_type doc2 = tree.doc(2);
        id_type doc3 = tree.doc(3);
        EXPECT_EQ(tree.ancestor_doc(tree.root_id()), tree.root_id());
        EXPECT_EQ(tree.ancestor_doc(doc0), doc0);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(doc0)), doc0);
        EXPECT_EQ(tree.ancestor_doc(doc1), doc1);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(doc1)), doc1);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(tree.first_child(doc1))), doc1);
        EXPECT_EQ(tree.ancestor_doc(doc2), doc2);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(doc2)), doc2);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(tree.first_child(doc2))), doc2);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(tree.first_child(tree.first_child(doc2)))), doc2);
        EXPECT_EQ(tree.ancestor_doc(doc3), doc3);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(doc3)), doc3);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(tree.first_child(doc3))), doc3);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(tree.first_child(tree.first_child(doc3)))), doc3);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(tree.first_child(tree.first_child(tree.first_child(doc3))))), doc3);
    }
    {
        Tree const& ctree = tree;
        ConstNodeRef doc0 = ctree.docref(0);
        ConstNodeRef doc1 = ctree.docref(1);
        ConstNodeRef doc2 = ctree.docref(2);
        ConstNodeRef doc3 = ctree.docref(3);
        EXPECT_EQ(ctree.rootref().ancestor_doc().id(), tree.root_id());
        EXPECT_EQ(doc0.ancestor_doc().id(), doc0.id());
        EXPECT_EQ(doc0[0].ancestor_doc().id(), doc0.id());
        EXPECT_EQ(doc1.ancestor_doc().id(), doc1.id());
        EXPECT_EQ(doc1[0].ancestor_doc().id(), doc1.id());
        EXPECT_EQ(doc1[0][0].ancestor_doc().id(), doc1.id());
        EXPECT_EQ(doc2.ancestor_doc().id(), doc2.id());
        EXPECT_EQ(doc2[0].ancestor_doc().id(), doc2.id());
        EXPECT_EQ(doc2[0][0].ancestor_doc().id(), doc2.id());
        EXPECT_EQ(doc2[0][0][0].ancestor_doc().id(), doc2.id());
        EXPECT_EQ(doc3.ancestor_doc().id(), doc3.id());
        EXPECT_EQ(doc3[0].ancestor_doc().id(), doc3.id());
        EXPECT_EQ(doc3[0][0].ancestor_doc().id(), doc3.id());
        EXPECT_EQ(doc3[0][0][0].ancestor_doc().id(), doc3.id());
        EXPECT_EQ(doc3[0][0][0][0].ancestor_doc().id(), doc3.id());
    }
    {
        NodeRef doc0 = tree.docref(0);
        NodeRef doc1 = tree.docref(1);
        NodeRef doc2 = tree.docref(2);
        NodeRef doc3 = tree.docref(3);
        EXPECT_EQ(doc0.ancestor_doc().id(), doc0.id());
        EXPECT_EQ(doc0[0].ancestor_doc().id(), doc0.id());
        EXPECT_EQ(doc1.ancestor_doc().id(), doc1.id());
        EXPECT_EQ(doc1[0].ancestor_doc().id(), doc1.id());
        EXPECT_EQ(doc1[0][0].ancestor_doc().id(), doc1.id());
        EXPECT_EQ(doc2.ancestor_doc().id(), doc2.id());
        EXPECT_EQ(doc2[0].ancestor_doc().id(), doc2.id());
        EXPECT_EQ(doc2[0][0].ancestor_doc().id(), doc2.id());
        EXPECT_EQ(doc2[0][0][0].ancestor_doc().id(), doc2.id());
        EXPECT_EQ(doc3.ancestor_doc().id(), doc3.id());
        EXPECT_EQ(doc3[0].ancestor_doc().id(), doc3.id());
        EXPECT_EQ(doc3[0][0].ancestor_doc().id(), doc3.id());
        EXPECT_EQ(doc3[0][0][0].ancestor_doc().id(), doc3.id());
        EXPECT_EQ(doc3[0][0][0][0].ancestor_doc().id(), doc3.id());
    }
}
TEST(Tree, ancestor_doc_without_doc)
{
    Tree tree = parse_in_arena(R"(
doc3:
  doc3:
    doc3:
      doc3:
        - a
        - b
)");
    {
        id_type root = tree.root_id();
        ASSERT_TRUE(tree.is_map(root));
        id_type doc3 = tree.first_child(root);
        EXPECT_EQ(tree.ancestor_doc(root), root);
        EXPECT_EQ(tree.ancestor_doc(doc3), root);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(doc3)), root);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(tree.first_child(doc3))), root);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(tree.first_child(tree.first_child(doc3)))), root);
        EXPECT_EQ(tree.ancestor_doc(tree.first_child(tree.first_child(tree.first_child(tree.first_child(doc3))))), root);
    }
    {
        Tree const& ctree = tree;
        ConstNodeRef root = ctree.rootref();
        ConstNodeRef doc3 = root[0];
        EXPECT_EQ(root.ancestor_doc().id(), root.id());
        EXPECT_EQ(doc3.ancestor_doc().id(), root.id());
        EXPECT_EQ(doc3[0].ancestor_doc().id(), root.id());
        EXPECT_EQ(doc3[0][0].ancestor_doc().id(), root.id());
        EXPECT_EQ(doc3[0][0][0].ancestor_doc().id(), root.id());
        EXPECT_EQ(doc3[0][0][0][0].ancestor_doc().id(), root.id());
    }
    {
        NodeRef root = tree.rootref();
        NodeRef doc3 = root[0];
        EXPECT_EQ(root.ancestor_doc().id(), root.id());
        EXPECT_EQ(doc3.ancestor_doc().id(), root.id());
        EXPECT_EQ(doc3[0].ancestor_doc().id(), root.id());
        EXPECT_EQ(doc3[0][0].ancestor_doc().id(), root.id());
        EXPECT_EQ(doc3[0][0][0].ancestor_doc().id(), root.id());
        EXPECT_EQ(doc3[0][0][0][0].ancestor_doc().id(), root.id());
    }
}


//-----------------------------------------------------------------------------

TEST(Tree, parent_is_seq)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    //EXPECT_FALSE(t.parent_is_seq(stream_id));
    EXPECT_TRUE(t.parent_is_seq(doc_id));
    EXPECT_FALSE(t.parent_is_seq(map_id));
    EXPECT_FALSE(t.parent_is_seq(keyval_id));
    EXPECT_FALSE(t.parent_is_seq(seq_id));
    EXPECT_TRUE(t.parent_is_seq(val_id));
    //EXPECT_FALSE(stream.parent_is_seq());
    EXPECT_TRUE(doc.parent_is_seq());
    EXPECT_FALSE(map.parent_is_seq());
    EXPECT_FALSE(keyval.parent_is_seq());
    EXPECT_FALSE(seq.parent_is_seq());
    EXPECT_TRUE(val.parent_is_seq());
    //EXPECT_FALSE(mstream.parent_is_seq());
    EXPECT_TRUE(mdoc.parent_is_seq());
    EXPECT_FALSE(mmap.parent_is_seq());
    EXPECT_FALSE(mkeyval.parent_is_seq());
    EXPECT_FALSE(mseq.parent_is_seq());
    EXPECT_TRUE(mval.parent_is_seq());
    //EXPECT_EQ(t.parent_is_seq(stream_id), stream.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(doc_id), doc.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(map_id), map.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(keyval_id), keyval.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(seq_id), seq.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(val_id), val.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(doc_id), mdoc.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(map_id), mmap.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(keyval_id), mkeyval.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(seq_id), mseq.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(val_id), mval.parent_is_seq());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].parent_is_seq(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).parent_is_seq(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.parent_is_seq(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.parent_is_seq(NONE); }, visit);
}

TEST(Tree, parent_is_map)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    //EXPECT_FALSE(t.parent_is_map(stream_id));
    EXPECT_FALSE(t.parent_is_map(doc_id));
    EXPECT_TRUE(t.parent_is_map(map_id));
    EXPECT_TRUE(t.parent_is_map(keyval_id));
    EXPECT_TRUE(t.parent_is_map(seq_id));
    EXPECT_FALSE(t.parent_is_map(val_id));
    //EXPECT_FALSE(stream.parent_is_map());
    EXPECT_FALSE(doc.parent_is_map());
    EXPECT_TRUE(map.parent_is_map());
    EXPECT_TRUE(keyval.parent_is_map());
    EXPECT_TRUE(seq.parent_is_map());
    EXPECT_FALSE(val.parent_is_map());
    //EXPECT_FALSE(mstream.parent_is_map());
    EXPECT_FALSE(mdoc.parent_is_map());
    EXPECT_TRUE(mmap.parent_is_map());
    EXPECT_TRUE(mkeyval.parent_is_map());
    EXPECT_TRUE(mseq.parent_is_map());
    EXPECT_FALSE(mval.parent_is_map());
    //EXPECT_EQ(t.parent_is_map(stream_id), stream.parent_is_map());
    EXPECT_EQ(t.parent_is_map(doc_id), doc.parent_is_map());
    EXPECT_EQ(t.parent_is_map(map_id), map.parent_is_map());
    EXPECT_EQ(t.parent_is_map(keyval_id), keyval.parent_is_map());
    EXPECT_EQ(t.parent_is_map(seq_id), seq.parent_is_map());
    EXPECT_EQ(t.parent_is_map(val_id), val.parent_is_map());
    //EXPECT_EQ(t.parent_is_map(stream_id), mstream.parent_is_map());
    EXPECT_EQ(t.parent_is_map(doc_id), mdoc.parent_is_map());
    EXPECT_EQ(t.parent_is_map(map_id), mmap.parent_is_map());
    EXPECT_EQ(t.parent_is_map(keyval_id), mkeyval.parent_is_map());
    EXPECT_EQ(t.parent_is_map(seq_id), mseq.parent_is_map());
    EXPECT_EQ(t.parent_is_map(val_id), mval.parent_is_map());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].parent_is_map(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).parent_is_map(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.parent_is_map(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.parent_is_map(NONE); }, visit);
}

TEST(Tree, has_parent)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    EXPECT_FALSE(t.has_parent(stream_id));
    EXPECT_TRUE(t.has_parent(doc_id));
    EXPECT_TRUE(t.has_parent(map_id));
    EXPECT_TRUE(t.has_parent(keyval_id));
    EXPECT_TRUE(t.has_parent(seq_id));
    EXPECT_TRUE(t.has_parent(val_id));
    EXPECT_FALSE(stream.has_parent());
    EXPECT_TRUE(doc.has_parent());
    EXPECT_TRUE(map.has_parent());
    EXPECT_TRUE(keyval.has_parent());
    EXPECT_TRUE(seq.has_parent());
    EXPECT_TRUE(val.has_parent());
    EXPECT_FALSE(mstream.has_parent());
    EXPECT_TRUE(mdoc.has_parent());
    EXPECT_TRUE(mmap.has_parent());
    EXPECT_TRUE(mkeyval.has_parent());
    EXPECT_TRUE(mseq.has_parent());
    EXPECT_TRUE(mval.has_parent());
    EXPECT_EQ(t.has_parent(stream_id), stream.has_parent());
    EXPECT_EQ(t.has_parent(doc_id), doc.has_parent());
    EXPECT_EQ(t.has_parent(map_id), map.has_parent());
    EXPECT_EQ(t.has_parent(keyval_id), keyval.has_parent());
    EXPECT_EQ(t.has_parent(seq_id), seq.has_parent());
    EXPECT_EQ(t.has_parent(val_id), val.has_parent());
    EXPECT_EQ(t.has_parent(stream_id), mstream.has_parent());
    EXPECT_EQ(t.has_parent(doc_id), mdoc.has_parent());
    EXPECT_EQ(t.has_parent(map_id), mmap.has_parent());
    EXPECT_EQ(t.has_parent(keyval_id), mkeyval.has_parent());
    EXPECT_EQ(t.has_parent(seq_id), mseq.has_parent());
    EXPECT_EQ(t.has_parent(val_id), mval.has_parent());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_parent(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).has_parent(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_parent(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_parent(NONE); }, visit);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


TEST(Tree, depth_asc_desc)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const size_t stream_id = t.root_id();
    const size_t doc_id = t.first_child(stream_id);
    const size_t map_id = t.first_child(doc_id);
    const size_t seq_id = t.last_child(doc_id);
    const size_t map_child_id = t.first_child(map_id);
    const size_t seq_child_id = t.first_child(seq_id);
    ConstNodeRef stream = t.cref(stream_id);
    ConstNodeRef doc = t.cref(doc_id);
    ConstNodeRef map = t.cref(map_id);
    ConstNodeRef seq = t.cref(seq_id);
    ConstNodeRef map_child = t.cref(map_child_id);
    ConstNodeRef seq_child = t.cref(seq_child_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mmap_child = t.ref(map_child_id);
    NodeRef mseq_child = t.ref(seq_child_id);
    //
    EXPECT_EQ(t.depth_asc(stream_id), id_type(0));
    EXPECT_EQ(t.depth_asc(doc_id), id_type(1));
    EXPECT_EQ(t.depth_asc(map_id), id_type(2));
    EXPECT_EQ(t.depth_asc(seq_id), id_type(2));
    EXPECT_EQ(t.depth_asc(map_child_id), id_type(3));
    EXPECT_EQ(t.depth_asc(seq_child_id), id_type(3));
    //
    EXPECT_EQ(stream.depth_asc(), id_type(0));
    EXPECT_EQ(doc.depth_asc(), id_type(1));
    EXPECT_EQ(map.depth_asc(), id_type(2));
    EXPECT_EQ(seq.depth_asc(), id_type(2));
    EXPECT_EQ(map_child.depth_asc(), id_type(3));
    EXPECT_EQ(seq_child.depth_asc(), id_type(3));
    //
    EXPECT_EQ(mstream.depth_asc(), id_type(0));
    EXPECT_EQ(mdoc.depth_asc(), id_type(1));
    EXPECT_EQ(mmap.depth_asc(), id_type(2));
    EXPECT_EQ(mseq.depth_asc(), id_type(2));
    EXPECT_EQ(mmap_child.depth_asc(), id_type(3));
    EXPECT_EQ(mseq_child.depth_asc(), id_type(3));
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].depth_asc(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).depth_asc(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.depth_asc(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.depth_asc(NONE); }, visit);
    //
    EXPECT_EQ(t.depth_desc(stream_id), id_type(3));
    EXPECT_EQ(t.depth_desc(doc_id), id_type(2));
    EXPECT_EQ(t.depth_desc(map_id), id_type(1));
    EXPECT_EQ(t.depth_desc(seq_id), id_type(1));
    EXPECT_EQ(t.depth_desc(map_child_id), id_type(0));
    EXPECT_EQ(t.depth_desc(seq_child_id), id_type(0));
    //
    EXPECT_EQ(stream.depth_desc(), id_type(3));
    EXPECT_EQ(doc.depth_desc(), id_type(2));
    EXPECT_EQ(map.depth_desc(), id_type(1));
    EXPECT_EQ(seq.depth_desc(), id_type(1));
    EXPECT_EQ(map_child.depth_desc(), id_type(0));
    EXPECT_EQ(seq_child.depth_desc(), id_type(0));
    //
    EXPECT_EQ(mstream.depth_desc(), id_type(3));
    EXPECT_EQ(mdoc.depth_desc(), id_type(2));
    EXPECT_EQ(mmap.depth_desc(), id_type(1));
    EXPECT_EQ(mseq.depth_desc(), id_type(1));
    EXPECT_EQ(mmap_child.depth_desc(), id_type(0));
    EXPECT_EQ(mseq_child.depth_desc(), id_type(0));
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].depth_desc(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).depth_desc(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.depth_desc(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.depth_desc(NONE); }, visit);
}

TEST(Tree, is_ancestor)
{
    Tree t = parse_in_arena(R"(a0:
  a1:
    a2:
      a3: a
b0:
  b1:
    b2:
      b3: b
)");
    const size_t map_id = t.root_id();
    const size_t a0_id = t.first_child(map_id);
    const size_t a1_id = t.first_child(a0_id);
    const size_t a2_id = t.first_child(a1_id);
    const size_t a3_id = t.first_child(a2_id);
    const size_t b0_id = t.first_child(map_id);
    const size_t b1_id = t.first_child(b0_id);
    const size_t b2_id = t.first_child(b1_id);
    const size_t b3_id = t.first_child(b2_id);
    ConstNodeRef map = t.cref(map_id);
    ConstNodeRef a0 = t.cref(a0_id);
    ConstNodeRef a1 = t.cref(a1_id);
    ConstNodeRef a2 = t.cref(a2_id);
    ConstNodeRef a3 = t.cref(a3_id);
    ConstNodeRef b0 = t.cref(b0_id);
    ConstNodeRef b1 = t.cref(b1_id);
    ConstNodeRef b2 = t.cref(b2_id);
    ConstNodeRef b3 = t.cref(b3_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef ma0 = t.ref(a0_id);
    NodeRef ma1 = t.ref(a1_id);
    NodeRef ma2 = t.ref(a2_id);
    NodeRef ma3 = t.ref(a3_id);
    NodeRef mb0 = t.ref(b0_id);
    NodeRef mb1 = t.ref(b1_id);
    NodeRef mb2 = t.ref(b2_id);
    NodeRef mb3 = t.ref(b3_id);
    //
    EXPECT_TRUE(t.is_ancestor(a0_id, map_id));
    EXPECT_TRUE(t.is_ancestor(a1_id, map_id));
    EXPECT_TRUE(t.is_ancestor(a2_id, map_id));
    EXPECT_TRUE(t.is_ancestor(a3_id, map_id));
    EXPECT_FALSE(t.is_ancestor(a0_id, a0_id));
    EXPECT_TRUE(t.is_ancestor(a1_id, a0_id));
    EXPECT_TRUE(t.is_ancestor(a2_id, a0_id));
    EXPECT_TRUE(t.is_ancestor(a3_id, a0_id));
    EXPECT_FALSE(t.is_ancestor(a1_id, a1_id));
    EXPECT_TRUE(t.is_ancestor(a2_id, a1_id));
    EXPECT_TRUE(t.is_ancestor(a3_id, a1_id));
    EXPECT_FALSE(t.is_ancestor(a2_id, a2_id));
    EXPECT_TRUE(t.is_ancestor(a3_id, a2_id));
    EXPECT_FALSE(t.is_ancestor(a3_id, a3_id));
    EXPECT_TRUE(t.is_ancestor(b0_id, map_id));
    EXPECT_TRUE(t.is_ancestor(b1_id, map_id));
    EXPECT_TRUE(t.is_ancestor(b2_id, map_id));
    EXPECT_TRUE(t.is_ancestor(b3_id, map_id));
    EXPECT_FALSE(t.is_ancestor(b0_id, b0_id));
    EXPECT_TRUE(t.is_ancestor(b1_id, b0_id));
    EXPECT_TRUE(t.is_ancestor(b2_id, b0_id));
    EXPECT_TRUE(t.is_ancestor(b3_id, b0_id));
    EXPECT_FALSE(t.is_ancestor(b1_id, b1_id));
    EXPECT_TRUE(t.is_ancestor(b2_id, b1_id));
    EXPECT_TRUE(t.is_ancestor(b3_id, b1_id));
    EXPECT_FALSE(t.is_ancestor(b2_id, b2_id));
    EXPECT_TRUE(t.is_ancestor(b3_id, b2_id));
    EXPECT_FALSE(t.is_ancestor(b3_id, b3_id));
    //
    EXPECT_FALSE(t.is_ancestor(map_id, a0_id));
    EXPECT_FALSE(t.is_ancestor(map_id, a1_id));
    EXPECT_FALSE(t.is_ancestor(map_id, a2_id));
    EXPECT_FALSE(t.is_ancestor(map_id, a3_id));
    EXPECT_FALSE(t.is_ancestor(a0_id, a0_id));
    EXPECT_FALSE(t.is_ancestor(a0_id, a1_id));
    EXPECT_FALSE(t.is_ancestor(a0_id, a2_id));
    EXPECT_FALSE(t.is_ancestor(a0_id, a3_id));
    EXPECT_FALSE(t.is_ancestor(a1_id, a1_id));
    EXPECT_FALSE(t.is_ancestor(a1_id, a2_id));
    EXPECT_FALSE(t.is_ancestor(a1_id, a3_id));
    EXPECT_FALSE(t.is_ancestor(a2_id, a2_id));
    EXPECT_FALSE(t.is_ancestor(a2_id, a3_id));
    EXPECT_FALSE(t.is_ancestor(a3_id, a3_id));
    EXPECT_FALSE(t.is_ancestor(map_id, b0_id));
    EXPECT_FALSE(t.is_ancestor(map_id, b1_id));
    EXPECT_FALSE(t.is_ancestor(map_id, b2_id));
    EXPECT_FALSE(t.is_ancestor(map_id, b3_id));
    EXPECT_FALSE(t.is_ancestor(b0_id, b0_id));
    EXPECT_FALSE(t.is_ancestor(b0_id, b1_id));
    EXPECT_FALSE(t.is_ancestor(b0_id, b2_id));
    EXPECT_FALSE(t.is_ancestor(b0_id, b3_id));
    EXPECT_FALSE(t.is_ancestor(b1_id, b1_id));
    EXPECT_FALSE(t.is_ancestor(b1_id, b2_id));
    EXPECT_FALSE(t.is_ancestor(b1_id, b3_id));
    EXPECT_FALSE(t.is_ancestor(b2_id, b2_id));
    EXPECT_FALSE(t.is_ancestor(b2_id, b3_id));
    EXPECT_FALSE(t.is_ancestor(b3_id, b3_id));
    //
    verify_assertion(t, [&](Tree const&){ return t.is_ancestor(NONE, map_id); }, visit);
    //
    EXPECT_TRUE(a0.is_ancestor(map));
    EXPECT_TRUE(a1.is_ancestor(map));
    EXPECT_TRUE(a2.is_ancestor(map));
    EXPECT_TRUE(a3.is_ancestor(map));
    EXPECT_FALSE(a0.is_ancestor(a0));
    EXPECT_TRUE(a1.is_ancestor(a0));
    EXPECT_TRUE(a2.is_ancestor(a0));
    EXPECT_TRUE(a3.is_ancestor(a0));
    EXPECT_FALSE(a1.is_ancestor(a1));
    EXPECT_TRUE(a2.is_ancestor(a1));
    EXPECT_TRUE(a3.is_ancestor(a1));
    EXPECT_FALSE(a2.is_ancestor(a2));
    EXPECT_TRUE(a3.is_ancestor(a2));
    EXPECT_FALSE(a3.is_ancestor(a3));
    EXPECT_TRUE(b0.is_ancestor(map));
    EXPECT_TRUE(b1.is_ancestor(map));
    EXPECT_TRUE(b2.is_ancestor(map));
    EXPECT_TRUE(b3.is_ancestor(map));
    EXPECT_FALSE(b0.is_ancestor(b0));
    EXPECT_TRUE(b1.is_ancestor(b0));
    EXPECT_TRUE(b2.is_ancestor(b0));
    EXPECT_TRUE(b3.is_ancestor(b0));
    EXPECT_FALSE(b1.is_ancestor(b1));
    EXPECT_TRUE(b2.is_ancestor(b1));
    EXPECT_TRUE(b3.is_ancestor(b1));
    EXPECT_FALSE(b2.is_ancestor(b2));
    EXPECT_TRUE(b3.is_ancestor(b2));
    EXPECT_FALSE(b3.is_ancestor(b3));
    //
    EXPECT_FALSE(map.is_ancestor(a0));
    EXPECT_FALSE(map.is_ancestor(a1));
    EXPECT_FALSE(map.is_ancestor(a2));
    EXPECT_FALSE(map.is_ancestor(a3));
    EXPECT_FALSE(a0.is_ancestor(a0));
    EXPECT_FALSE(a0.is_ancestor(a1));
    EXPECT_FALSE(a0.is_ancestor(a2));
    EXPECT_FALSE(a0.is_ancestor(a3));
    EXPECT_FALSE(a1.is_ancestor(a1));
    EXPECT_FALSE(a1.is_ancestor(a2));
    EXPECT_FALSE(a1.is_ancestor(a3));
    EXPECT_FALSE(a2.is_ancestor(a2));
    EXPECT_FALSE(a2.is_ancestor(a3));
    EXPECT_FALSE(a3.is_ancestor(a3));
    EXPECT_FALSE(map.is_ancestor(b0));
    EXPECT_FALSE(map.is_ancestor(b1));
    EXPECT_FALSE(map.is_ancestor(b2));
    EXPECT_FALSE(map.is_ancestor(b3));
    EXPECT_FALSE(b0.is_ancestor(b0));
    EXPECT_FALSE(b0.is_ancestor(b1));
    EXPECT_FALSE(b0.is_ancestor(b2));
    EXPECT_FALSE(b0.is_ancestor(b3));
    EXPECT_FALSE(b1.is_ancestor(b1));
    EXPECT_FALSE(b1.is_ancestor(b2));
    EXPECT_FALSE(b1.is_ancestor(b3));
    EXPECT_FALSE(b2.is_ancestor(b2));
    EXPECT_FALSE(b2.is_ancestor(b3));
    EXPECT_FALSE(b3.is_ancestor(b3));
    //
    EXPECT_TRUE(ma0.is_ancestor(mmap));
    EXPECT_TRUE(ma1.is_ancestor(mmap));
    EXPECT_TRUE(ma2.is_ancestor(mmap));
    EXPECT_TRUE(ma3.is_ancestor(mmap));
    EXPECT_FALSE(ma0.is_ancestor(ma0));
    EXPECT_TRUE(ma1.is_ancestor(ma0));
    EXPECT_TRUE(ma2.is_ancestor(ma0));
    EXPECT_TRUE(ma3.is_ancestor(ma0));
    EXPECT_FALSE(ma1.is_ancestor(ma1));
    EXPECT_TRUE(ma2.is_ancestor(ma1));
    EXPECT_TRUE(ma3.is_ancestor(ma1));
    EXPECT_FALSE(ma2.is_ancestor(ma2));
    EXPECT_TRUE(ma3.is_ancestor(ma2));
    EXPECT_FALSE(ma3.is_ancestor(ma3));
    EXPECT_TRUE(mb0.is_ancestor(mmap));
    EXPECT_TRUE(mb1.is_ancestor(mmap));
    EXPECT_TRUE(mb2.is_ancestor(mmap));
    EXPECT_TRUE(mb3.is_ancestor(mmap));
    EXPECT_FALSE(mb0.is_ancestor(mb0));
    EXPECT_TRUE(mb1.is_ancestor(mb0));
    EXPECT_TRUE(mb2.is_ancestor(mb0));
    EXPECT_TRUE(mb3.is_ancestor(mb0));
    EXPECT_FALSE(mb1.is_ancestor(mb1));
    EXPECT_TRUE(mb2.is_ancestor(mb1));
    EXPECT_TRUE(mb3.is_ancestor(mb1));
    EXPECT_FALSE(mb2.is_ancestor(mb2));
    EXPECT_TRUE(mb3.is_ancestor(mb2));
    EXPECT_FALSE(mb3.is_ancestor(mb3));
    //
    EXPECT_FALSE(mmap.is_ancestor(ma0));
    EXPECT_FALSE(mmap.is_ancestor(ma1));
    EXPECT_FALSE(mmap.is_ancestor(ma2));
    EXPECT_FALSE(mmap.is_ancestor(ma3));
    EXPECT_FALSE(ma0.is_ancestor(ma0));
    EXPECT_FALSE(ma0.is_ancestor(ma1));
    EXPECT_FALSE(ma0.is_ancestor(ma2));
    EXPECT_FALSE(ma0.is_ancestor(ma3));
    EXPECT_FALSE(ma1.is_ancestor(ma1));
    EXPECT_FALSE(ma1.is_ancestor(ma2));
    EXPECT_FALSE(ma1.is_ancestor(ma3));
    EXPECT_FALSE(ma2.is_ancestor(ma2));
    EXPECT_FALSE(ma2.is_ancestor(ma3));
    EXPECT_FALSE(ma3.is_ancestor(ma3));
    EXPECT_FALSE(mmap.is_ancestor(mb0));
    EXPECT_FALSE(mmap.is_ancestor(mb1));
    EXPECT_FALSE(mmap.is_ancestor(mb2));
    EXPECT_FALSE(mmap.is_ancestor(mb3));
    EXPECT_FALSE(mb0.is_ancestor(mb0));
    EXPECT_FALSE(mb0.is_ancestor(mb1));
    EXPECT_FALSE(mb0.is_ancestor(mb2));
    EXPECT_FALSE(mb0.is_ancestor(mb3));
    EXPECT_FALSE(mb1.is_ancestor(mb1));
    EXPECT_FALSE(mb1.is_ancestor(mb2));
    EXPECT_FALSE(mb1.is_ancestor(mb3));
    EXPECT_FALSE(mb2.is_ancestor(mb2));
    EXPECT_FALSE(mb2.is_ancestor(mb3));
    EXPECT_FALSE(mb3.is_ancestor(mb3));
    //
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(Tree, lookup_path)
{
    const char yaml[] = R"(
a:
  b: bval
  c:
    d:
      - e
      - d
      - f: fval
        g: gval
        h:
          -
            x: a
            y: b
          -
            z: c
            u:
)";
    Tree t = parse_in_arena(yaml);
    print_tree(t);

    EXPECT_EQ(t.lookup_path("a").target, 1);
    EXPECT_EQ(t.lookup_path("a.b").target, 2);
    EXPECT_EQ(t.lookup_path("a.c").target, 3);
    EXPECT_EQ(t.lookup_path("a.c.d").target, 4);
    EXPECT_EQ(t.lookup_path("a.c.d[0]").target, 5);
    EXPECT_EQ(t.lookup_path("a.c.d[1]").target, 6);
    EXPECT_EQ(t.lookup_path("a.c.d[2]").target, 7);
    EXPECT_EQ(t.lookup_path("a.c.d[2].f").target, 8);
    EXPECT_EQ(t.lookup_path("a.c.d[2].g").target, 9);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h").target, 10);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[0]").target, 11);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[0].x").target, 12);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[0].y").target, 13);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[1]").target, 14);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[1].z").target, 15);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[1].u").target, 16);
    EXPECT_EQ(t.lookup_path("d", 3).target, 4);
    EXPECT_EQ(t.lookup_path("d[0]", 3).target, 5);
    EXPECT_EQ(t.lookup_path("d[1]", 3).target, 6);
    EXPECT_EQ(t.lookup_path("d[2]", 3).target, 7);
    EXPECT_EQ(t.lookup_path("d[2].f", 3).target, 8);
    EXPECT_EQ(t.lookup_path("d[2].g", 3).target, 9);
    EXPECT_EQ(t.lookup_path("d[2].h", 3).target, 10);
    EXPECT_EQ(t.lookup_path("d[2].h[0]", 3).target, 11);
    EXPECT_EQ(t.lookup_path("d[2].h[0].x", 3).target, 12);
    EXPECT_EQ(t.lookup_path("d[2].h[0].y", 3).target, 13);
    EXPECT_EQ(t.lookup_path("d[2].h[1]", 3).target, 14);
    EXPECT_EQ(t.lookup_path("d[2].h[1].z", 3).target, 15);
    EXPECT_EQ(t.lookup_path("d[2].h[1].u", 3).target, 16);

    auto lp = t.lookup_path("x");
    EXPECT_FALSE(lp);
    EXPECT_EQ(lp.target, (id_type)NONE);
    EXPECT_EQ(lp.closest, (id_type)NONE);
    EXPECT_EQ(lp.resolved(), "");
    EXPECT_EQ(lp.unresolved(), "x");
    lp = t.lookup_path("a.x");
    EXPECT_FALSE(lp);
    EXPECT_EQ(lp.target, (id_type)NONE);
    EXPECT_EQ(lp.closest, 1);
    EXPECT_EQ(lp.resolved(), "a");
    EXPECT_EQ(lp.unresolved(), "x");
    lp = t.lookup_path("a.b.x");
    EXPECT_FALSE(lp);
    EXPECT_EQ(lp.target, (id_type)NONE);
    EXPECT_EQ(lp.closest, 2);
    EXPECT_EQ(lp.resolved(), "a.b");
    EXPECT_EQ(lp.unresolved(), "x");
    lp = t.lookup_path("a.c.x");
    EXPECT_FALSE(lp);
    EXPECT_EQ(lp.target, (id_type)NONE);
    EXPECT_EQ(lp.closest, 3);
    EXPECT_EQ(lp.resolved(), "a.c");
    EXPECT_EQ(lp.unresolved(), "x");

    id_type sz = t.size();
    EXPECT_EQ(t.lookup_path("x").target, (id_type)NONE);
    EXPECT_EQ(t.lookup_path_or_modify("x", "x"), sz);
    EXPECT_EQ(t.lookup_path("x").target, sz);
    EXPECT_EQ(t.val(sz), "x");
    EXPECT_EQ(t.lookup_path_or_modify("y", "x"), sz);
    EXPECT_EQ(t.val(sz), "y");
    EXPECT_EQ(t.lookup_path_or_modify("z", "x"), sz);
    EXPECT_EQ(t.val(sz), "z");

    sz = t.size();
    EXPECT_EQ(t.lookup_path("a.x").target, (id_type)NONE);
    EXPECT_EQ(t.lookup_path_or_modify("x", "a.x"), sz);
    EXPECT_EQ(t.lookup_path("a.x").target, sz);
    EXPECT_EQ(t.val(sz), "x");
    EXPECT_EQ(t.lookup_path_or_modify("y", "a.x"), sz);
    EXPECT_EQ(t.val(sz), "y");
    EXPECT_EQ(t.lookup_path_or_modify("z", "a.x"), sz);
    EXPECT_EQ(t.val(sz), "z");

    sz = t.size();
    EXPECT_EQ(t.lookup_path("a.c.x").target, (id_type)NONE);
    EXPECT_EQ(t.lookup_path_or_modify("x", "a.c.x"), sz);
    EXPECT_EQ(t.lookup_path("a.c.x").target, sz);
    EXPECT_EQ(t.val(sz), "x");
    EXPECT_EQ(t.lookup_path_or_modify("y", "a.c.x"), sz);
    EXPECT_EQ(t.val(sz), "y");
    EXPECT_EQ(t.lookup_path_or_modify("z", "a.c.x"), sz);
    EXPECT_EQ(t.val(sz), "z");
}

TEST(Tree, lookup_path_or_modify)
{
    {
        Tree dst = parse_in_arena("{}");
        Tree const src = parse_in_arena("{d: [x, y, z]}");
        dst.lookup_path_or_modify("ok", "a.b.c");
        EXPECT_EQ(dst["a"]["b"]["c"].val(), "ok");
        dst.lookup_path_or_modify(&src, src["d"].id(), "a.b.d");
        EXPECT_EQ(dst["a"]["b"]["d"][0].val(), "x");
        EXPECT_EQ(dst["a"]["b"]["d"][1].val(), "y");
        EXPECT_EQ(dst["a"]["b"]["d"][2].val(), "z");
    }
    {
        Tree t;
        t.rootref().set_map();
        ExpectError::check_assert_visit(&t, [&]{
            (void)t.lookup_path_or_modify("x", "newmap.newseq[notnumber].newmap.newseq[0].first");
        });
    }
    {
        Tree t;
        t.rootref().set_map();
        csubstr bigpath = "newmap.newseq[0].newmap.newseq[0].first";
        id_type id = t.lookup_path_or_modify("x", bigpath);
        EXPECT_EQ(t.lookup_path(bigpath).target, id);
        EXPECT_EQ(t.val(id), "x");
        bigpath = "newmap.newseq[0].newmap[4]";
        id = t.lookup_path_or_modify("x", bigpath);
        t.set_key(id, "haha");
        EXPECT_EQ(t.lookup_path(bigpath).target, id);
        EXPECT_EQ(t.val(id), "x");
        EXPECT_EQ(emitrs_yaml<std::string>(t),
                  "newmap:\n"
                  "  newseq:\n"
                  "    - newmap:\n"
                  "        newseq:\n"
                  "          - first: x\n"
                  "        : \n"
                  "        : \n"
                  "        : \n"
                  "        haha: x\n"
                  "");
    }
    {
        Tree t;
        t.rootref().set_map();
        csubstr bigpath = "newmap.newseq[0].newmap.newseq[0].first";
        auto result = t.lookup_path(bigpath);
        EXPECT_EQ(result.target, (id_type)NONE);
        EXPECT_EQ(result.closest, (id_type)NONE);
        EXPECT_EQ(result.resolved(), "");
        EXPECT_EQ(result.unresolved(), bigpath);
        id_type sz = t.lookup_path_or_modify("x", bigpath);
        EXPECT_EQ(t.lookup_path(bigpath).target, sz);
        EXPECT_EQ(t.val(sz), "x");
        EXPECT_EQ(t["newmap"]["newseq"].num_children(), 1u);
        EXPECT_EQ(t["newmap"]["newseq"][0].is_map(), true);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"].is_map(), true);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"]["newseq"].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"]["newseq"].num_children(), 1u);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"]["newseq"][0].is_map(), true);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"]["newseq"][0]["first"].val(), "x");
        id_type sz2 = t.lookup_path_or_modify("y", bigpath);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"]["newseq"][0]["first"].val(), "y");
        EXPECT_EQ(sz2, sz);
        EXPECT_EQ(t.lookup_path(bigpath).target, sz);
        EXPECT_EQ(t.val(sz2), "y");

        sz2 = t.lookup_path_or_modify("y", "newmap2.newseq2[2].newmap2.newseq2[3].first2");
        EXPECT_EQ(t.lookup_path("newmap2.newseq2[2].newmap2.newseq2[3].first2").target, sz2);
        EXPECT_EQ(t.val(sz2), "y");
        EXPECT_EQ(t["newmap2"]["newseq2"].num_children(), 3u);
        EXPECT_EQ(t["newmap2"]["newseq2"][0].val(), nullptr);
        EXPECT_EQ(t["newmap2"]["newseq2"][1].val(), nullptr);
        EXPECT_EQ(t["newmap2"]["newseq2"][2].is_map(), true);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"].is_map(), true);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"].is_seq(), true);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"].num_children(), 4u);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][0].val(), nullptr);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][1].val(), nullptr);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][2].val(), nullptr);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][3].is_map(), true);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][3]["first2"].val(), "y");
        (void)t.lookup_path_or_modify("z", "newmap2.newseq2[2].newmap2.newseq2[3].second2");
        EXPECT_EQ  (t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][3]["second2"].val(), "z");

        (void)t.lookup_path_or_modify("foo", "newmap.newseq1[1]");
        EXPECT_EQ(t["newmap"].is_map(), true);
        EXPECT_EQ(t["newmap"]["newseq1"].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"].num_children(), 2u);
        EXPECT_EQ(t["newmap"]["newseq1"][0].val(), nullptr);
        EXPECT_EQ(t["newmap"]["newseq1"][1].val(), "foo");
        (void)t.lookup_path_or_modify("bar", "newmap.newseq1[2][1]");
        EXPECT_EQ(t["newmap"]["newseq1"].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"].num_children(), 3u);
        EXPECT_EQ(t["newmap"]["newseq1"][0].val(), nullptr);
        EXPECT_EQ(t["newmap"]["newseq1"][1].val(), "foo");
        EXPECT_EQ(t["newmap"]["newseq1"][2].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"][2].num_children(), 2u);
        EXPECT_EQ(t["newmap"]["newseq1"][2][0].val(), nullptr);
        EXPECT_EQ(t["newmap"]["newseq1"][2][1].val(), "bar");
        (void)t.lookup_path_or_modify("Foo?"   , "newmap.newseq1[0]");
        (void)t.lookup_path_or_modify("Bar?"   , "newmap.newseq1[2][0]");
        (void)t.lookup_path_or_modify("happy"  , "newmap.newseq1[2][2][3]");
        (void)t.lookup_path_or_modify("trigger", "newmap.newseq1[2][2][2]");
        (void)t.lookup_path_or_modify("Arnold" , "newmap.newseq1[2][2][0]");
        (void)t.lookup_path_or_modify("is"     , "newmap.newseq1[2][2][1]");
        EXPECT_EQ(t["newmap"]["newseq1"].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"].num_children(), 3u);
        EXPECT_EQ(t["newmap"]["newseq1"][0].val(), "Foo?");
        EXPECT_EQ(t["newmap"]["newseq1"][1].val(), "foo");
        EXPECT_EQ(t["newmap"]["newseq1"][2].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"][2].num_children(), 3u);
        EXPECT_EQ(t["newmap"]["newseq1"][2][0].val(), "Bar?");
        EXPECT_EQ(t["newmap"]["newseq1"][2][1].val(), "bar");
        EXPECT_EQ(t["newmap"]["newseq1"][2][2].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"][2][2].num_children(), 4u);
        EXPECT_EQ(t["newmap"]["newseq1"][2][2][0].val(), "Arnold");
        EXPECT_EQ(t["newmap"]["newseq1"][2][2][1].val(), "is");
        EXPECT_EQ(t["newmap"]["newseq1"][2][2][2].val(), "trigger");
        EXPECT_EQ(t["newmap"]["newseq1"][2][2][3].val(), "happy");

        EXPECT_EQ(emitrs_yaml<std::string>(t), R"(newmap:
  newseq:
    - newmap:
        newseq:
          - first: y
  newseq1:
    - Foo?
    - foo
    - - Bar?
      - bar
      - - Arnold
        - is
        - trigger
        - happy
newmap2:
  newseq2:
    - 
    - 
    - newmap2:
        newseq2:
          - 
          - 
          - 
          - first2: y
            second2: z
)");
    }
}



//-----------------------------------------------------------------------------

TEST(set_root_as_stream, empty_tree)
{
    Tree t;
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.num_children(), 0u);
    t.set_root_as_stream();
    r = t.rootref();
    EXPECT_EQ(r.is_stream(), true);
    ASSERT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), 1u);
}

TEST(set_root_as_stream, already_with_stream)
{
    Tree t;
    t.set_stream(t.root_id());
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.num_children(), 0u);
    t.set_root_as_stream();
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.num_children(), 0u);
}


TEST(set_root_as_stream, root_is_map)
{
    Tree t = parse_in_arena(R"({a: b, c: d})");
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), true);
    EXPECT_EQ(r.is_seq(), false);
    EXPECT_EQ(r.num_children(), 2u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), true);
    EXPECT_EQ(r[0].is_seq(), false);
    EXPECT_EQ(r[0].num_children(), 2u);
    EXPECT_EQ(r[0]["a"].is_keyval(), true);
    EXPECT_EQ(r[0]["c"].is_keyval(), true);
    EXPECT_EQ(r[0]["a"].val(), "b");
    EXPECT_EQ(r[0]["c"].val(), "d");
}

TEST(set_root_as_stream, root_is_docmap)
{
    Tree t = parse_in_arena(R"({a: b, c: d})");
    t._p(t.root_id())->m_type.add(DOC);
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), true);
    EXPECT_EQ(r.is_map(), true);
    EXPECT_EQ(r.is_seq(), false);
    EXPECT_EQ(r.num_children(), 2u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), true);
    EXPECT_EQ(r[0].is_seq(), false);
    EXPECT_EQ(r[0].num_children(), 2u);
    EXPECT_EQ(r[0]["a"].is_keyval(), true);
    EXPECT_EQ(r[0]["c"].is_keyval(), true);
    EXPECT_EQ(r[0]["a"].val(), "b");
    EXPECT_EQ(r[0]["c"].val(), "d");
}


TEST(set_root_as_stream, root_is_seq)
{
    Tree t = parse_in_arena(R"([a, b, c, d])");
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 4u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), false);
    EXPECT_EQ(r[0].is_seq(), true);
    EXPECT_EQ(r[0].num_children(), 4u);
    EXPECT_EQ(r[0][0].val(), "a");
    EXPECT_EQ(r[0][1].val(), "b");
    EXPECT_EQ(r[0][2].val(), "c");
    EXPECT_EQ(r[0][3].val(), "d");
}

TEST(set_root_as_stream, root_is_docseq)
{
    Tree t = parse_in_arena(R"([a, b, c, d])");
    t._p(t.root_id())->m_type.add(DOC);
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), true);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 4u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), false);
    EXPECT_EQ(r[0].is_seq(), true);
    EXPECT_EQ(r[0].num_children(), 4u);
    EXPECT_EQ(r[0][0].val(), "a");
    EXPECT_EQ(r[0][1].val(), "b");
    EXPECT_EQ(r[0][2].val(), "c");
    EXPECT_EQ(r[0][3].val(), "d");
}

TEST(set_root_as_stream, root_is_seqmap)
{
    Tree t = parse_in_arena(R"([{a: b, c: d}, {e: e, f: f}, {g: g, h: h}, {i: i, j: j}])");
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 4u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), false);
    EXPECT_EQ(r[0].is_seq(), true);
    EXPECT_EQ(r[0].num_children(), 4u);
    EXPECT_EQ(r[0][0].is_map(), true);
    EXPECT_EQ(r[0][1].is_map(), true);
    EXPECT_EQ(r[0][2].is_map(), true);
    EXPECT_EQ(r[0][3].is_map(), true);
    EXPECT_EQ(r[0][0]["a"].val(), "b");
    EXPECT_EQ(r[0][0]["c"].val(), "d");
    EXPECT_EQ(r[0][1]["e"].val(), "e");
    EXPECT_EQ(r[0][1]["f"].val(), "f");
    EXPECT_EQ(r[0][2]["g"].val(), "g");
    EXPECT_EQ(r[0][2]["h"].val(), "h");
    EXPECT_EQ(r[0][3]["i"].val(), "i");
    EXPECT_EQ(r[0][3]["j"].val(), "j");
}

TEST(set_root_as_stream, root_is_mapseq)
{
    Tree t = parse_in_arena(R"({a: [0, 1, 2], b: [3, 4, 5], c: [6, 7, 8]})");
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), true);
    EXPECT_EQ(r.is_seq(), false);
    EXPECT_EQ(r.num_children(), 3u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), true);
    EXPECT_EQ(r[0].is_seq(), false);
    EXPECT_EQ(r[0].num_children(), 3u);
    EXPECT_EQ(r[0]["a"].is_seq(), true);
    EXPECT_EQ(r[0]["b"].is_seq(), true);
    EXPECT_EQ(r[0]["c"].is_seq(), true);
    EXPECT_EQ(r[0]["a"][0].val(), "0");
    EXPECT_EQ(r[0]["a"][1].val(), "1");
    EXPECT_EQ(r[0]["a"][2].val(), "2");
    EXPECT_EQ(r[0]["b"][0].val(), "3");
    EXPECT_EQ(r[0]["b"][1].val(), "4");
    EXPECT_EQ(r[0]["b"][2].val(), "5");
    EXPECT_EQ(r[0]["c"][0].val(), "6");
    EXPECT_EQ(r[0]["c"][1].val(), "7");
    EXPECT_EQ(r[0]["c"][2].val(), "8");
}

TEST(set_root_as_stream, root_is_docval)
{
    Tree t;
    NodeRef r = t.rootref();
    r.set_val("bar", DOC);
    r.set_val_tag("<!foo>");
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), true);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), false);
    EXPECT_EQ(r.is_val(), true);
    EXPECT_EQ(r.has_val_tag(), true);
    EXPECT_EQ(r.val_tag(), "<!foo>");
    EXPECT_EQ(r.num_children(), 0u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.is_val(), false);
    ASSERT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_stream(), false);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), false);
    EXPECT_EQ(r[0].is_seq(), false);
    EXPECT_EQ(r[0].is_val(), true);
    EXPECT_EQ(r[0].has_val_tag(), true);
    EXPECT_EQ(r[0].val_tag(), "<!foo>");
    EXPECT_EQ(r[0].num_children(), 0u);
}


//-------------------------------------------
//-------------------------------------------
//-------------------------------------------

TEST(Tree, add_tag_directives)
{
    #if RYML_MAX_TAG_DIRECTIVES != 4
    #error this test assumes RYML_MAX_TAG_DIRECTIVES == 4
    #endif
    const TagDirective td[RYML_MAX_TAG_DIRECTIVES + 1] = {
        TagDirective{csubstr("!a!"), csubstr("!ay-"), 0u},
        TagDirective{csubstr("!b!"), csubstr("!by-"), 0u},
        TagDirective{csubstr("!c!"), csubstr("!cy-"), 0u},
        TagDirective{csubstr("!d!"), csubstr("!dy-"), 0u},
        TagDirective{csubstr("!e!"), csubstr("!ey-"), 0u},
    };
    Tree t;
    auto check_up_to = [&](id_type num)
    {
        id_type pos = 0;
        EXPECT_EQ(t.num_tag_directives(), num);
        for(TagDirective const& d : t.tag_directives())
        {
            EXPECT_EQ(d.handle.str, td[pos].handle.str);
            EXPECT_EQ(d.handle.len, td[pos].handle.len);
            EXPECT_EQ(d.prefix.str, td[pos].prefix.str);
            EXPECT_EQ(d.prefix.str, td[pos].prefix.str);
            EXPECT_EQ(d.doc_id, td[pos].doc_id);
            ++pos;
        }
        EXPECT_EQ(pos, num);
    };
    check_up_to(0);
    t.add_tag_directive(td[0].handle, td[0].prefix, 0u);
    check_up_to(1);
    t.add_tag_directive(td[1].handle, td[1].prefix, 0u);
    check_up_to(2);
    t.add_tag_directive(td[2].handle, td[2].prefix, 0u);
    check_up_to(3);
    t.add_tag_directive(td[3].handle, td[3].prefix, 0u);
    check_up_to(4);
    ExpectError::check_error_basic(&t, [&]{ // number exceeded
        t.add_tag_directive(td[4].handle, td[4].prefix, 0u);
    });
    t.clear_tag_directives();
    check_up_to(0);
}

} // namespace yml
} // namespace c4
