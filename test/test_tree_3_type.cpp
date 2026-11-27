#include "./test_tree.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


static constexpr const ExpectedErrorType visit = ExpectedErrorType::err_visit;


TEST(NodeType, is_stream)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_stream());
    EXPECT_TRUE(NodeType(STREAM).is_stream());
}

TEST(Tree, is_stream)
{
    Tree t = parse_in_arena(R"(---
foo: bar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type keyval_id = t.first_child(doc_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    EXPECT_TRUE(t.is_stream(stream_id));
    EXPECT_FALSE(t.is_stream(doc_id));
    EXPECT_FALSE(t.is_stream(keyval_id));
    EXPECT_TRUE(stream.is_stream());
    EXPECT_FALSE(doc.is_stream());
    EXPECT_FALSE(keyval.is_stream());
    EXPECT_EQ(t.is_stream(stream_id), t._p(stream_id)->m_type.is_stream());
    EXPECT_EQ(t.is_stream(doc_id), t._p(doc_id)->m_type.is_stream());
    EXPECT_EQ(t.is_stream(keyval_id), t._p(keyval_id)->m_type.is_stream());
    EXPECT_EQ(stream.is_stream(), stream.get()->m_type.is_stream());
    EXPECT_EQ(doc.is_stream(), doc.get()->m_type.is_stream());
    EXPECT_EQ(keyval.is_stream(), keyval.get()->m_type.is_stream());
    //
    ASSERT_TRUE(!t.docref(0)["none"].invalid());
    ASSERT_TRUE(t.docref(0)["none"].is_seed());
    ASSERT_FALSE(t.docref(0)["none"].readable());
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_stream(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_stream(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_stream(NONE); }, visit);
}

TEST(NodeType, is_doc)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_doc());
    EXPECT_TRUE(NodeType(DOC).is_doc());
}

TEST(Tree, is_doc)
{
    Tree t = parse_in_arena(R"(---
foo: bar
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type keyval_id = t.first_child(doc_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.is_doc(stream_id));
    EXPECT_TRUE(t.is_doc(doc_id));
    EXPECT_FALSE(t.is_doc(keyval_id));
    EXPECT_TRUE(t.is_doc(docval_id));
    EXPECT_FALSE(stream.is_doc());
    EXPECT_TRUE(doc.is_doc());
    EXPECT_FALSE(keyval.is_doc());
    EXPECT_TRUE(docval.is_doc());
    EXPECT_FALSE(mstream.is_doc());
    EXPECT_TRUE(mdoc.is_doc());
    EXPECT_FALSE(mkeyval.is_doc());
    EXPECT_TRUE(mdocval.is_doc());
    EXPECT_EQ(t.is_doc(stream_id), t._p(stream_id)->m_type.is_doc());
    EXPECT_EQ(t.is_doc(doc_id), t._p(doc_id)->m_type.is_doc());
    EXPECT_EQ(t.is_doc(keyval_id), t._p(keyval_id)->m_type.is_doc());
    EXPECT_EQ(t.is_doc(docval_id), t._p(docval_id)->m_type.is_doc());
    EXPECT_EQ(stream.is_doc(), stream.get()->m_type.is_doc());
    EXPECT_EQ(doc.is_doc(), doc.get()->m_type.is_doc());
    EXPECT_EQ(keyval.is_doc(), keyval.get()->m_type.is_doc());
    EXPECT_EQ(docval.is_doc(), docval.get()->m_type.is_doc());
    EXPECT_EQ(mstream.is_doc(), mstream.get()->m_type.is_doc());
    EXPECT_EQ(mdoc.is_doc(), mdoc.get()->m_type.is_doc());
    EXPECT_EQ(mkeyval.is_doc(), mkeyval.get()->m_type.is_doc());
    EXPECT_EQ(mdocval.is_doc(), mdocval.get()->m_type.is_doc());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_doc(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_doc(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_doc(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_doc(NONE); }, visit);
}

TEST(NodeType, is_container)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_container());
    EXPECT_FALSE(NodeType(VAL).is_container());
    EXPECT_FALSE(NodeType(KEY).is_container());
    EXPECT_FALSE(NodeType(KEYVAL).is_container());
    EXPECT_TRUE(NodeType(MAP).is_container());
    EXPECT_TRUE(NodeType(SEQ).is_container());
    EXPECT_TRUE(NodeType(KEYMAP).is_container());
    EXPECT_TRUE(NodeType(KEYSEQ).is_container());
    EXPECT_TRUE(NodeType(DOCMAP).is_container());
    EXPECT_TRUE(NodeType(DOCSEQ).is_container());
}

TEST(Tree, is_container)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_TRUE(t.is_container(stream_id));
    EXPECT_TRUE(t.is_container(doc_id));
    EXPECT_TRUE(t.is_container(map_id));
    EXPECT_FALSE(t.is_container(keyval_id));
    EXPECT_TRUE(t.is_container(seq_id));
    EXPECT_FALSE(t.is_container(val_id));
    EXPECT_FALSE(t.is_container(docval_id));
    EXPECT_TRUE(stream.is_container());
    EXPECT_TRUE(doc.is_container());
    EXPECT_TRUE(map.is_container());
    EXPECT_FALSE(keyval.is_container());
    EXPECT_TRUE(seq.is_container());
    EXPECT_FALSE(val.is_container());
    EXPECT_FALSE(docval.is_container());
    EXPECT_TRUE(mstream.is_container());
    EXPECT_TRUE(mdoc.is_container());
    EXPECT_TRUE(mmap.is_container());
    EXPECT_FALSE(mkeyval.is_container());
    EXPECT_TRUE(mseq.is_container());
    EXPECT_FALSE(mval.is_container());
    EXPECT_FALSE(mdocval.is_container());
    EXPECT_EQ(t.is_container(stream_id), t._p(stream_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(doc_id), t._p(doc_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(map_id), t._p(map_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(keyval_id), t._p(keyval_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(seq_id), t._p(seq_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(val_id), t._p(val_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(docval_id), t._p(docval_id)->m_type.is_container());
    EXPECT_EQ(stream.is_container(), stream.get()->m_type.is_container());
    EXPECT_EQ(doc.is_container(), doc.get()->m_type.is_container());
    EXPECT_EQ(map.is_container(), map.get()->m_type.is_container());
    EXPECT_EQ(keyval.is_container(), keyval.get()->m_type.is_container());
    EXPECT_EQ(seq.is_container(), seq.get()->m_type.is_container());
    EXPECT_EQ(val.is_container(), val.get()->m_type.is_container());
    EXPECT_EQ(docval.is_container(), docval.get()->m_type.is_container());
    EXPECT_EQ(mstream.is_container(), mstream.get()->m_type.is_container());
    EXPECT_EQ(mdoc.is_container(), mdoc.get()->m_type.is_container());
    EXPECT_EQ(mmap.is_container(), mmap.get()->m_type.is_container());
    EXPECT_EQ(mkeyval.is_container(), mkeyval.get()->m_type.is_container());
    EXPECT_EQ(mseq.is_container(), mseq.get()->m_type.is_container());
    EXPECT_EQ(mval.is_container(), mval.get()->m_type.is_container());
    EXPECT_EQ(mdocval.is_container(), mdocval.get()->m_type.is_container());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_container(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_container(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_container(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_container(NONE); }, visit);
}

TEST(NodeType, is_map)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_map());
    EXPECT_FALSE(NodeType(VAL).is_map());
    EXPECT_FALSE(NodeType(KEY).is_map());
    EXPECT_TRUE(NodeType(MAP).is_map());
    EXPECT_TRUE(NodeType(KEYMAP).is_map());
    EXPECT_FALSE(NodeType(SEQ).is_map());
    EXPECT_FALSE(NodeType(KEYSEQ).is_map());
}

TEST(Tree, is_map)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.is_map(stream_id));
    EXPECT_TRUE(t.is_map(doc_id));
    EXPECT_TRUE(t.is_map(map_id));
    EXPECT_FALSE(t.is_map(keyval_id));
    EXPECT_FALSE(t.is_map(seq_id));
    EXPECT_FALSE(t.is_map(val_id));
    EXPECT_FALSE(t.is_map(docval_id));
    EXPECT_FALSE(stream.is_map());
    EXPECT_TRUE(doc.is_map());
    EXPECT_TRUE(map.is_map());
    EXPECT_FALSE(keyval.is_map());
    EXPECT_FALSE(seq.is_map());
    EXPECT_FALSE(val.is_map());
    EXPECT_FALSE(docval.is_map());
    EXPECT_FALSE(mstream.is_map());
    EXPECT_TRUE(mdoc.is_map());
    EXPECT_TRUE(mmap.is_map());
    EXPECT_FALSE(mkeyval.is_map());
    EXPECT_FALSE(mseq.is_map());
    EXPECT_FALSE(mval.is_map());
    EXPECT_FALSE(mdocval.is_map());
    EXPECT_EQ(t.is_map(stream_id), t._p(stream_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(doc_id), t._p(doc_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(map_id), t._p(map_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(keyval_id), t._p(keyval_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(seq_id), t._p(seq_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(val_id), t._p(val_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(docval_id), t._p(docval_id)->m_type.is_map());
    EXPECT_EQ(stream.is_map(), stream.get()->m_type.is_map());
    EXPECT_EQ(doc.is_map(), doc.get()->m_type.is_map());
    EXPECT_EQ(map.is_map(), map.get()->m_type.is_map());
    EXPECT_EQ(keyval.is_map(), keyval.get()->m_type.is_map());
    EXPECT_EQ(seq.is_map(), seq.get()->m_type.is_map());
    EXPECT_EQ(val.is_map(), val.get()->m_type.is_map());
    EXPECT_EQ(docval.is_map(), docval.get()->m_type.is_map());
    EXPECT_EQ(mstream.is_map(), mstream.get()->m_type.is_map());
    EXPECT_EQ(mdoc.is_map(), mdoc.get()->m_type.is_map());
    EXPECT_EQ(mmap.is_map(), mmap.get()->m_type.is_map());
    EXPECT_EQ(mkeyval.is_map(), mkeyval.get()->m_type.is_map());
    EXPECT_EQ(mseq.is_map(), mseq.get()->m_type.is_map());
    EXPECT_EQ(mval.is_map(), mval.get()->m_type.is_map());
    EXPECT_EQ(mdocval.is_map(), mdocval.get()->m_type.is_map());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_map(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_map(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_map(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_map(NONE); }, visit);
}

TEST(NodeType, is_seq)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_seq());
    EXPECT_FALSE(NodeType(VAL).is_seq());
    EXPECT_FALSE(NodeType(KEY).is_seq());
    EXPECT_FALSE(NodeType(MAP).is_seq());
    EXPECT_FALSE(NodeType(KEYMAP).is_seq());
    EXPECT_TRUE(NodeType(SEQ).is_seq());
    EXPECT_TRUE(NodeType(KEYSEQ).is_seq());
}

TEST(Tree, is_seq)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_TRUE(t.is_seq(stream_id));
    EXPECT_FALSE(t.is_seq(doc_id));
    EXPECT_FALSE(t.is_seq(map_id));
    EXPECT_FALSE(t.is_seq(keyval_id));
    EXPECT_TRUE(t.is_seq(seq_id));
    EXPECT_FALSE(t.is_seq(val_id));
    EXPECT_FALSE(t.is_seq(docval_id));
    EXPECT_TRUE(stream.is_seq());
    EXPECT_FALSE(doc.is_seq());
    EXPECT_FALSE(map.is_seq());
    EXPECT_FALSE(keyval.is_seq());
    EXPECT_TRUE(seq.is_seq());
    EXPECT_FALSE(val.is_seq());
    EXPECT_FALSE(docval.is_seq());
    EXPECT_TRUE(mstream.is_seq());
    EXPECT_FALSE(mdoc.is_seq());
    EXPECT_FALSE(mmap.is_seq());
    EXPECT_FALSE(mkeyval.is_seq());
    EXPECT_TRUE(mseq.is_seq());
    EXPECT_FALSE(mval.is_seq());
    EXPECT_FALSE(mdocval.is_seq());
    EXPECT_EQ(t.is_seq(stream_id), t._p(stream_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(doc_id), t._p(doc_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(map_id), t._p(map_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(keyval_id), t._p(keyval_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(seq_id), t._p(seq_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(val_id), t._p(val_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(docval_id), t._p(docval_id)->m_type.is_seq());
    EXPECT_EQ(stream.is_seq(), stream.get()->m_type.is_seq());
    EXPECT_EQ(doc.is_seq(), doc.get()->m_type.is_seq());
    EXPECT_EQ(map.is_seq(), map.get()->m_type.is_seq());
    EXPECT_EQ(keyval.is_seq(), keyval.get()->m_type.is_seq());
    EXPECT_EQ(seq.is_seq(), seq.get()->m_type.is_seq());
    EXPECT_EQ(val.is_seq(), val.get()->m_type.is_seq());
    EXPECT_EQ(docval.is_seq(), docval.get()->m_type.is_seq());
    EXPECT_EQ(mstream.is_seq(), mstream.get()->m_type.is_seq());
    EXPECT_EQ(mdoc.is_seq(), mdoc.get()->m_type.is_seq());
    EXPECT_EQ(mmap.is_seq(), mmap.get()->m_type.is_seq());
    EXPECT_EQ(mkeyval.is_seq(), mkeyval.get()->m_type.is_seq());
    EXPECT_EQ(mseq.is_seq(), mseq.get()->m_type.is_seq());
    EXPECT_EQ(mval.is_seq(), mval.get()->m_type.is_seq());
    EXPECT_EQ(mdocval.is_seq(), mdocval.get()->m_type.is_seq());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_seq(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_seq(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_seq(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_seq(NONE); }, visit);
}

TEST(NodeType, has_val)
{
    EXPECT_FALSE(NodeType(NOTYPE).has_val());
    EXPECT_FALSE(NodeType(KEY).has_val());
    EXPECT_TRUE(NodeType(VAL).has_val());
    EXPECT_TRUE(NodeType(DOCVAL).has_val());
    EXPECT_TRUE(NodeType(KEYVAL).has_val());
    EXPECT_FALSE(NodeType(KEYMAP).has_val());
    EXPECT_FALSE(NodeType(KEYSEQ).has_val());
}

TEST(Tree, has_val)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.has_val(stream_id));
    EXPECT_FALSE(t.has_val(doc_id));
    EXPECT_FALSE(t.has_val(map_id));
    EXPECT_TRUE(t.has_val(keyval_id));
    EXPECT_FALSE(t.has_val(seq_id));
    EXPECT_TRUE(t.has_val(val_id));
    EXPECT_TRUE(t.has_val(docval_id));
    EXPECT_FALSE(stream.has_val());
    EXPECT_FALSE(doc.has_val());
    EXPECT_FALSE(map.has_val());
    EXPECT_TRUE(keyval.has_val());
    EXPECT_FALSE(seq.has_val());
    EXPECT_TRUE(val.has_val());
    EXPECT_TRUE(docval.has_val());
    EXPECT_FALSE(mstream.has_val());
    EXPECT_FALSE(mdoc.has_val());
    EXPECT_FALSE(mmap.has_val());
    EXPECT_TRUE(mkeyval.has_val());
    EXPECT_FALSE(mseq.has_val());
    EXPECT_TRUE(mval.has_val());
    EXPECT_TRUE(mdocval.has_val());
    EXPECT_EQ(t.has_val(stream_id), t._p(stream_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(doc_id), t._p(doc_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(map_id), t._p(map_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(keyval_id), t._p(keyval_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(seq_id), t._p(seq_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(val_id), t._p(val_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(docval_id), t._p(docval_id)->m_type.has_val());
    EXPECT_EQ(stream.has_val(), stream.get()->m_type.has_val());
    EXPECT_EQ(doc.has_val(), doc.get()->m_type.has_val());
    EXPECT_EQ(map.has_val(), map.get()->m_type.has_val());
    EXPECT_EQ(keyval.has_val(), keyval.get()->m_type.has_val());
    EXPECT_EQ(seq.has_val(), seq.get()->m_type.has_val());
    EXPECT_EQ(val.has_val(), val.get()->m_type.has_val());
    EXPECT_EQ(docval.has_val(), docval.get()->m_type.has_val());
    EXPECT_EQ(mstream.has_val(), mstream.get()->m_type.has_val());
    EXPECT_EQ(mdoc.has_val(), mdoc.get()->m_type.has_val());
    EXPECT_EQ(mmap.has_val(), mmap.get()->m_type.has_val());
    EXPECT_EQ(mkeyval.has_val(), mkeyval.get()->m_type.has_val());
    EXPECT_EQ(mseq.has_val(), mseq.get()->m_type.has_val());
    EXPECT_EQ(mval.has_val(), mval.get()->m_type.has_val());
    EXPECT_EQ(mdocval.has_val(), mdocval.get()->m_type.has_val());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_val(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).has_val(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_val(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_val(NONE); }, visit);
}

TEST(NodeType, is_val)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_val());
    EXPECT_FALSE(NodeType(KEY).is_val());
    EXPECT_TRUE(NodeType(VAL).is_val());
    EXPECT_TRUE(NodeType(DOCVAL).is_val());
    EXPECT_FALSE(NodeType(KEYVAL).is_val());
    EXPECT_FALSE(NodeType(KEYMAP).is_val());
    EXPECT_FALSE(NodeType(KEYSEQ).is_val());
}

TEST(Tree, is_val)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.is_val(stream_id));
    EXPECT_FALSE(t.is_val(doc_id));
    EXPECT_FALSE(t.is_val(map_id));
    EXPECT_FALSE(t.is_val(keyval_id));
    EXPECT_FALSE(t.is_val(seq_id));
    EXPECT_TRUE(t.is_val(val_id));
    EXPECT_TRUE(t.is_val(docval_id));
    EXPECT_FALSE(stream.is_val());
    EXPECT_FALSE(doc.is_val());
    EXPECT_FALSE(map.is_val());
    EXPECT_FALSE(keyval.is_val());
    EXPECT_FALSE(seq.is_val());
    EXPECT_TRUE(val.is_val());
    EXPECT_TRUE(docval.is_val());
    EXPECT_FALSE(mstream.is_val());
    EXPECT_FALSE(mdoc.is_val());
    EXPECT_FALSE(mmap.is_val());
    EXPECT_FALSE(mkeyval.is_val());
    EXPECT_FALSE(mseq.is_val());
    EXPECT_TRUE(mval.is_val());
    EXPECT_TRUE(mdocval.is_val());
    EXPECT_EQ(t.is_val(stream_id), t._p(stream_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(doc_id), t._p(doc_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(map_id), t._p(map_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(keyval_id), t._p(keyval_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(seq_id), t._p(seq_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(val_id), t._p(val_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(docval_id), t._p(docval_id)->m_type.is_val());
    EXPECT_EQ(stream.is_val(), stream.get()->m_type.is_val());
    EXPECT_EQ(doc.is_val(), doc.get()->m_type.is_val());
    EXPECT_EQ(map.is_val(), map.get()->m_type.is_val());
    EXPECT_EQ(keyval.is_val(), keyval.get()->m_type.is_val());
    EXPECT_EQ(seq.is_val(), seq.get()->m_type.is_val());
    EXPECT_EQ(val.is_val(), val.get()->m_type.is_val());
    EXPECT_EQ(docval.is_val(), docval.get()->m_type.is_val());
    EXPECT_EQ(mstream.is_val(), mstream.get()->m_type.is_val());
    EXPECT_EQ(mdoc.is_val(), mdoc.get()->m_type.is_val());
    EXPECT_EQ(mmap.is_val(), mmap.get()->m_type.is_val());
    EXPECT_EQ(mkeyval.is_val(), mkeyval.get()->m_type.is_val());
    EXPECT_EQ(mseq.is_val(), mseq.get()->m_type.is_val());
    EXPECT_EQ(mval.is_val(), mval.get()->m_type.is_val());
    EXPECT_EQ(mdocval.is_val(), mdocval.get()->m_type.is_val());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_val(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(1)[1].is_val(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_val(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_val(NONE); }, visit);
}

TEST(NodeType, has_key)
{
    EXPECT_FALSE(NodeType(NOTYPE).has_key());
    EXPECT_TRUE(NodeType(KEY).has_key());
    EXPECT_FALSE(NodeType(VAL).has_key());
    EXPECT_TRUE(NodeType(KEYVAL).has_key());
    EXPECT_TRUE(NodeType(KEYMAP).has_key());
    EXPECT_TRUE(NodeType(KEYSEQ).has_key());
}

TEST(Tree, key_val_is_null)
{
    Tree t = parse_in_arena(R"(
map: {foo: bar, : knil, vnil: }
seq: [foo, bar]
: knil
vnil:
squoted: ''
dquoted: ""
literal: |
folded: >
)");
    const NodeRef mroot = t.rootref();
    const NodeRef mmap = t["map"];
    const NodeRef mfoo = t["map"]["foo"];
    const NodeRef mfooknil = t["map"][1];
    const NodeRef mfoovnil = t["map"]["vnil"];
    const NodeRef mseq = t["seq"];
    const NodeRef mknil = t[2];
    const NodeRef mvnil = t["vnil"];
    const NodeRef msquo = t["squoted"];
    const NodeRef mdquo = t["dquoted"];
    const NodeRef mliteral = t["literal"];
    const NodeRef mfolded = t["folded"];
    const ConstNodeRef root = mroot;
    const ConstNodeRef map = mmap;
    const ConstNodeRef foo = mfoo;
    const ConstNodeRef fooknil = mfooknil;
    const ConstNodeRef foovnil = mfoovnil;
    const ConstNodeRef seq = mseq;
    const ConstNodeRef knil = mknil;
    const ConstNodeRef vnil = mvnil;
    const ConstNodeRef squo = msquo;
    const ConstNodeRef dquo = mdquo;
    const ConstNodeRef literal = mliteral;
    const ConstNodeRef folded = mfolded;
    const size_t root_id = root.id();
    const size_t map_id = map.id();
    const size_t foo_id = foo.id();
    const size_t fooknil_id = fooknil.id();
    const size_t foovnil_id = foovnil.id();
    const size_t seq_id = seq.id();
    const size_t knil_id = knil.id();
    const size_t vnil_id = vnil.id();
    const size_t squo_id = squo.id();
    const size_t dquo_id = dquo.id();
    const size_t literal_id = literal.id();
    const size_t folded_id = folded.id();
    //
    verify_assertion(t, [&](Tree const&){ return t.key_is_null(root_id); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.val_is_null(root_id); }, visit);
    EXPECT_FALSE(t.key_is_null(map_id));
    verify_assertion(t, [&](Tree const&){ return t.val_is_null(map_id); }, visit);
    EXPECT_FALSE(t.key_is_null(foo_id));
    EXPECT_FALSE(t.val_is_null(foo_id));
    EXPECT_TRUE (t.key_is_null(fooknil_id));
    EXPECT_FALSE(t.val_is_null(fooknil_id));
    EXPECT_FALSE(t.key_is_null(foovnil_id));
    EXPECT_TRUE (t.val_is_null(foovnil_id));
    EXPECT_FALSE(t.key_is_null(seq_id));
    verify_assertion(t, [&](Tree const&){ return t.val_is_null(seq_id); }, visit);
    EXPECT_TRUE (t.key_is_null(knil_id));
    EXPECT_FALSE(t.val_is_null(knil_id));
    EXPECT_FALSE(t.key_is_null(vnil_id));
    EXPECT_TRUE (t.val_is_null(vnil_id));
    EXPECT_FALSE(t.key_is_null(squo_id));
    EXPECT_FALSE(t.val_is_null(squo_id));
    EXPECT_FALSE(t.key_is_null(dquo_id));
    EXPECT_FALSE(t.val_is_null(dquo_id));
    EXPECT_FALSE(t.key_is_null(literal_id));
    EXPECT_FALSE(t.val_is_null(literal_id));
    EXPECT_FALSE(t.key_is_null(folded_id));
    EXPECT_FALSE(t.val_is_null(folded_id));
    //
    verify_assertion(t, [&](Tree const&){ return root.key_is_null(); }, visit);
    verify_assertion(t, [&](Tree const&){ return root.val_is_null(); }, visit);
    EXPECT_FALSE(map.key_is_null());
    verify_assertion(t, [&](Tree const&){ return map.val_is_null(); }, visit);
    EXPECT_FALSE(foo.key_is_null());
    EXPECT_FALSE(foo.val_is_null());
    EXPECT_TRUE (fooknil.key_is_null());
    EXPECT_FALSE(fooknil.val_is_null());
    EXPECT_FALSE(foovnil.key_is_null());
    EXPECT_TRUE (foovnil.val_is_null());
    EXPECT_FALSE(seq.key_is_null());
    verify_assertion(t, [&](Tree const&){ seq.val_is_null(); }, visit);
    EXPECT_TRUE (knil.key_is_null());
    EXPECT_FALSE(knil.val_is_null());
    EXPECT_FALSE(vnil.key_is_null());
    EXPECT_TRUE (vnil.val_is_null());
    EXPECT_FALSE(squo.key_is_null());
    EXPECT_FALSE(squo.val_is_null());
    EXPECT_FALSE(dquo.key_is_null());
    EXPECT_FALSE(dquo.val_is_null());
    EXPECT_FALSE(literal.key_is_null());
    EXPECT_FALSE(literal.val_is_null());
    EXPECT_FALSE(folded.key_is_null());
    EXPECT_FALSE(folded.val_is_null());
    //
    verify_assertion(t, [&](Tree const&){ return mroot.key_is_null(); }, visit);
    verify_assertion(t, [&](Tree const&){ return mroot.val_is_null(); }, visit);
    EXPECT_FALSE(mmap.key_is_null());
    verify_assertion(t, [&](Tree const&){ return mmap.val_is_null(); }, visit);
    EXPECT_FALSE(mfoo.key_is_null());
    EXPECT_FALSE(mfoo.val_is_null());
    EXPECT_TRUE (mfooknil.key_is_null());
    EXPECT_FALSE(mfooknil.val_is_null());
    EXPECT_FALSE(mfoovnil.key_is_null());
    EXPECT_TRUE (mfoovnil.val_is_null());
    EXPECT_FALSE(mseq.key_is_null());
    verify_assertion(t, [&](Tree const&){ mseq.val_is_null(); }, visit);
    EXPECT_TRUE (mknil.key_is_null());
    EXPECT_FALSE(mknil.val_is_null());
    EXPECT_FALSE(mvnil.key_is_null());
    EXPECT_TRUE (mvnil.val_is_null());
    EXPECT_FALSE(msquo.key_is_null());
    EXPECT_FALSE(msquo.val_is_null());
    EXPECT_FALSE(mdquo.key_is_null());
    EXPECT_FALSE(mdquo.val_is_null());
    EXPECT_FALSE(mliteral.key_is_null());
    EXPECT_FALSE(mliteral.val_is_null());
    EXPECT_FALSE(mfolded.key_is_null());
    EXPECT_FALSE(mfolded.val_is_null());
}

TEST(Tree, has_key)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.has_key(stream_id));
    EXPECT_FALSE(t.has_key(doc_id));
    EXPECT_TRUE(t.has_key(map_id));
    EXPECT_TRUE(t.has_key(keyval_id));
    EXPECT_TRUE(t.has_key(seq_id));
    EXPECT_FALSE(t.has_key(val_id));
    EXPECT_FALSE(t.has_key(docval_id));
    EXPECT_FALSE(stream.has_key());
    EXPECT_FALSE(doc.has_key());
    EXPECT_TRUE(map.has_key());
    EXPECT_TRUE(keyval.has_key());
    EXPECT_TRUE(seq.has_key());
    EXPECT_FALSE(val.has_key());
    EXPECT_FALSE(docval.has_key());
    EXPECT_FALSE(mstream.has_key());
    EXPECT_FALSE(mdoc.has_key());
    EXPECT_TRUE(mmap.has_key());
    EXPECT_TRUE(mkeyval.has_key());
    EXPECT_TRUE(mseq.has_key());
    EXPECT_FALSE(mval.has_key());
    EXPECT_FALSE(mdocval.has_key());
    EXPECT_EQ(t.has_key(stream_id), t._p(stream_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(doc_id), t._p(doc_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(map_id), t._p(map_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(keyval_id), t._p(keyval_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(seq_id), t._p(seq_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(val_id), t._p(val_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(docval_id), t._p(docval_id)->m_type.has_key());
    EXPECT_EQ(stream.has_key(), stream.get()->m_type.has_key());
    EXPECT_EQ(doc.has_key(), doc.get()->m_type.has_key());
    EXPECT_EQ(map.has_key(), map.get()->m_type.has_key());
    EXPECT_EQ(keyval.has_key(), keyval.get()->m_type.has_key());
    EXPECT_EQ(seq.has_key(), seq.get()->m_type.has_key());
    EXPECT_EQ(val.has_key(), val.get()->m_type.has_key());
    EXPECT_EQ(docval.has_key(), docval.get()->m_type.has_key());
    EXPECT_EQ(mstream.has_key(), mstream.get()->m_type.has_key());
    EXPECT_EQ(mdoc.has_key(), mdoc.get()->m_type.has_key());
    EXPECT_EQ(mmap.has_key(), mmap.get()->m_type.has_key());
    EXPECT_EQ(mkeyval.has_key(), mkeyval.get()->m_type.has_key());
    EXPECT_EQ(mseq.has_key(), mseq.get()->m_type.has_key());
    EXPECT_EQ(mval.has_key(), mval.get()->m_type.has_key());
    EXPECT_EQ(mdocval.has_key(), mdocval.get()->m_type.has_key());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_key(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).has_key(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_key(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_key(NONE); }, visit);
}

TEST(NodeType, is_keyval)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_keyval());
    EXPECT_FALSE(NodeType(KEY).is_keyval());
    EXPECT_FALSE(NodeType(VAL).is_keyval());
    EXPECT_TRUE(NodeType(KEYVAL).is_keyval());
    EXPECT_FALSE(NodeType(DOCVAL).is_keyval());
    EXPECT_FALSE(NodeType(KEYMAP).is_keyval());
    EXPECT_FALSE(NodeType(KEYSEQ).is_keyval());
}

TEST(Tree, is_keyval)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.is_keyval(stream_id));
    EXPECT_FALSE(t.is_keyval(doc_id));
    EXPECT_FALSE(t.is_keyval(map_id));
    EXPECT_TRUE(t.is_keyval(keyval_id));
    EXPECT_FALSE(t.is_keyval(seq_id));
    EXPECT_FALSE(t.is_keyval(val_id));
    EXPECT_FALSE(t.is_keyval(docval_id));
    EXPECT_FALSE(stream.is_keyval());
    EXPECT_FALSE(doc.is_keyval());
    EXPECT_FALSE(map.is_keyval());
    EXPECT_TRUE(keyval.is_keyval());
    EXPECT_FALSE(seq.is_keyval());
    EXPECT_FALSE(val.is_keyval());
    EXPECT_FALSE(docval.is_keyval());
    EXPECT_FALSE(mstream.is_keyval());
    EXPECT_FALSE(mdoc.is_keyval());
    EXPECT_FALSE(mmap.is_keyval());
    EXPECT_TRUE(mkeyval.is_keyval());
    EXPECT_FALSE(mseq.is_keyval());
    EXPECT_FALSE(mval.is_keyval());
    EXPECT_FALSE(mdocval.is_keyval());
    EXPECT_EQ(t.is_keyval(stream_id), t._p(stream_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(doc_id), t._p(doc_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(map_id), t._p(map_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(keyval_id), t._p(keyval_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(seq_id), t._p(seq_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(val_id), t._p(val_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(docval_id), t._p(docval_id)->m_type.is_keyval());
    EXPECT_EQ(stream.is_keyval(), stream.get()->m_type.is_keyval());
    EXPECT_EQ(doc.is_keyval(), doc.get()->m_type.is_keyval());
    EXPECT_EQ(map.is_keyval(), map.get()->m_type.is_keyval());
    EXPECT_EQ(keyval.is_keyval(), keyval.get()->m_type.is_keyval());
    EXPECT_EQ(seq.is_keyval(), seq.get()->m_type.is_keyval());
    EXPECT_EQ(val.is_keyval(), val.get()->m_type.is_keyval());
    EXPECT_EQ(docval.is_keyval(), docval.get()->m_type.is_keyval());
    EXPECT_EQ(mstream.is_keyval(), mstream.get()->m_type.is_keyval());
    EXPECT_EQ(mdoc.is_keyval(), mdoc.get()->m_type.is_keyval());
    EXPECT_EQ(mmap.is_keyval(), mmap.get()->m_type.is_keyval());
    EXPECT_EQ(mkeyval.is_keyval(), mkeyval.get()->m_type.is_keyval());
    EXPECT_EQ(mseq.is_keyval(), mseq.get()->m_type.is_keyval());
    EXPECT_EQ(mval.is_keyval(), mval.get()->m_type.is_keyval());
    EXPECT_EQ(mdocval.is_keyval(), mdocval.get()->m_type.is_keyval());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_keyval(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_keyval(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_keyval(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_keyval(NONE); }, visit);
}

TEST(NodeType, has_key_tag)
{
    EXPECT_FALSE(NodeType().has_key_tag());
    EXPECT_TRUE(NodeType(KEYTAG).has_key_tag());
    EXPECT_TRUE(NodeType(KEY|KEYTAG).has_key_tag());
}

TEST(Tree, has_key_tag)
{
    Tree t = parse_in_arena(R"(--- !docmaptag
!maptag map: {!footag foo: bar, notag: none}
!seqtag seq: [!footag foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type keyvalnotag_id = t.last_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type valnotag_id = t.last_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef keyvalnotag = t.ref(keyvalnotag_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef valnotag = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mkeyvalnotag = t.ref(keyvalnotag_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mvalnotag = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.has_key_tag(stream_id));
    EXPECT_FALSE(t.has_key_tag(doc_id));
    EXPECT_TRUE(t.has_key_tag(map_id));
    EXPECT_TRUE(t.has_key_tag(keyval_id));
    EXPECT_FALSE(t.has_key_tag(keyvalnotag_id));
    EXPECT_TRUE(t.has_key_tag(seq_id));
    EXPECT_FALSE(t.has_key_tag(val_id));
    EXPECT_FALSE(t.has_key_tag(valnotag_id));
    EXPECT_FALSE(t.has_key_tag(docval_id));
    EXPECT_FALSE(stream.has_key_tag());
    EXPECT_FALSE(doc.has_key_tag());
    EXPECT_TRUE(map.has_key_tag());
    EXPECT_TRUE(keyval.has_key_tag());
    EXPECT_FALSE(keyvalnotag.has_key_tag());
    EXPECT_TRUE(seq.has_key_tag());
    EXPECT_FALSE(val.has_key_tag());
    EXPECT_FALSE(valnotag.has_key_tag());
    EXPECT_FALSE(docval.has_key_tag());
    EXPECT_FALSE(mstream.has_key_tag());
    EXPECT_FALSE(mdoc.has_key_tag());
    EXPECT_TRUE(mmap.has_key_tag());
    EXPECT_TRUE(mkeyval.has_key_tag());
    EXPECT_FALSE(mkeyvalnotag.has_key_tag());
    EXPECT_TRUE(mseq.has_key_tag());
    EXPECT_FALSE(mval.has_key_tag());
    EXPECT_FALSE(mvalnotag.has_key_tag());
    EXPECT_FALSE(mdocval.has_key_tag());
    EXPECT_EQ(t.has_key_tag(stream_id), t._p(stream_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(doc_id), t._p(doc_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(map_id), t._p(map_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(keyval_id), t._p(keyval_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(keyvalnotag_id), t._p(keyvalnotag_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(seq_id), t._p(seq_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(val_id), t._p(val_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(valnotag_id), t._p(valnotag_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(docval_id), t._p(docval_id)->m_type.has_key_tag());
    EXPECT_EQ(stream.has_key_tag(), stream.get()->m_type.has_key_tag());
    EXPECT_EQ(doc.has_key_tag(), doc.get()->m_type.has_key_tag());
    EXPECT_EQ(map.has_key_tag(), map.get()->m_type.has_key_tag());
    EXPECT_EQ(keyval.has_key_tag(), keyval.get()->m_type.has_key_tag());
    EXPECT_EQ(keyvalnotag.has_key_tag(), keyvalnotag.get()->m_type.has_key_tag());
    EXPECT_EQ(seq.has_key_tag(), seq.get()->m_type.has_key_tag());
    EXPECT_EQ(val.has_key_tag(), val.get()->m_type.has_key_tag());
    EXPECT_EQ(valnotag.has_key_tag(), valnotag.get()->m_type.has_key_tag());
    EXPECT_EQ(docval.has_key_tag(), docval.get()->m_type.has_key_tag());
    EXPECT_EQ(mstream.has_key_tag(), mstream.get()->m_type.has_key_tag());
    EXPECT_EQ(mdoc.has_key_tag(), mdoc.get()->m_type.has_key_tag());
    EXPECT_EQ(mmap.has_key_tag(), mmap.get()->m_type.has_key_tag());
    EXPECT_EQ(mkeyval.has_key_tag(), mkeyval.get()->m_type.has_key_tag());
    EXPECT_EQ(mkeyvalnotag.has_key_tag(), mkeyvalnotag.get()->m_type.has_key_tag());
    EXPECT_EQ(mseq.has_key_tag(), mseq.get()->m_type.has_key_tag());
    EXPECT_EQ(mval.has_key_tag(), mval.get()->m_type.has_key_tag());
    EXPECT_EQ(mvalnotag.has_key_tag(), mvalnotag.get()->m_type.has_key_tag());
    EXPECT_EQ(mdocval.has_key_tag(), mdocval.get()->m_type.has_key_tag());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_key_tag(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).has_key_tag(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_key_tag(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_key_tag(NONE); }, visit);
}

TEST(NodeType, has_val_tag)
{
    EXPECT_FALSE(NodeType().has_val_tag());
    EXPECT_TRUE(NodeType(VALTAG).has_val_tag());
    EXPECT_TRUE(NodeType(VAL|VALTAG).has_val_tag());
}

TEST(Tree, has_val_tag)
{
    Tree t = parse_in_arena(R"(--- !docmaptag
map: !maptag {foo: !bartag bar, notag: none}
seq: !seqtag [!footag foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type keyvalnotag_id = t.last_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type valnotag_id = t.last_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef keyvalnotag = t.ref(keyvalnotag_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef valnotag = t.ref(valnotag_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mkeyvalnotag = t.ref(keyvalnotag_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mvalnotag = t.ref(valnotag_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.has_val_tag(stream_id));
    EXPECT_TRUE(t.has_val_tag(doc_id));
    EXPECT_TRUE(t.has_val_tag(map_id));
    EXPECT_TRUE(t.has_val_tag(keyval_id));
    EXPECT_FALSE(t.has_val_tag(keyvalnotag_id));
    EXPECT_TRUE(t.has_val_tag(seq_id));
    EXPECT_TRUE(t.has_val_tag(val_id));
    EXPECT_FALSE(t.has_val_tag(valnotag_id));
    EXPECT_FALSE(t.has_val_tag(docval_id));
    EXPECT_FALSE(stream.has_val_tag());
    EXPECT_TRUE(doc.has_val_tag());
    EXPECT_TRUE(map.has_val_tag());
    EXPECT_TRUE(keyval.has_val_tag());
    EXPECT_FALSE(keyvalnotag.has_val_tag());
    EXPECT_TRUE(seq.has_val_tag());
    EXPECT_TRUE(val.has_val_tag());
    EXPECT_FALSE(valnotag.has_val_tag());
    EXPECT_FALSE(docval.has_val_tag());
    EXPECT_FALSE(mstream.has_val_tag());
    EXPECT_TRUE(mdoc.has_val_tag());
    EXPECT_TRUE(mmap.has_val_tag());
    EXPECT_TRUE(mkeyval.has_val_tag());
    EXPECT_FALSE(mkeyvalnotag.has_val_tag());
    EXPECT_TRUE(mseq.has_val_tag());
    EXPECT_TRUE(mval.has_val_tag());
    EXPECT_FALSE(mvalnotag.has_val_tag());
    EXPECT_FALSE(mdocval.has_val_tag());
    EXPECT_EQ(t.has_val_tag(stream_id), t._p(stream_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(doc_id), t._p(doc_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(map_id), t._p(map_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(keyval_id), t._p(keyval_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(keyvalnotag_id), t._p(keyvalnotag_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(seq_id), t._p(seq_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(val_id), t._p(val_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(valnotag_id), t._p(valnotag_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(docval_id), t._p(docval_id)->m_type.has_val_tag());
    EXPECT_EQ(stream.has_val_tag(), stream.get()->m_type.has_val_tag());
    EXPECT_EQ(doc.has_val_tag(), doc.get()->m_type.has_val_tag());
    EXPECT_EQ(map.has_val_tag(), map.get()->m_type.has_val_tag());
    EXPECT_EQ(keyval.has_val_tag(), keyval.get()->m_type.has_val_tag());
    EXPECT_EQ(keyvalnotag.has_val_tag(), keyvalnotag.get()->m_type.has_val_tag());
    EXPECT_EQ(seq.has_val_tag(), seq.get()->m_type.has_val_tag());
    EXPECT_EQ(val.has_val_tag(), val.get()->m_type.has_val_tag());
    EXPECT_EQ(valnotag.has_val_tag(), valnotag.get()->m_type.has_val_tag());
    EXPECT_EQ(docval.has_val_tag(), docval.get()->m_type.has_val_tag());
    EXPECT_EQ(mstream.has_val_tag(), mstream.get()->m_type.has_val_tag());
    EXPECT_EQ(mdoc.has_val_tag(), mdoc.get()->m_type.has_val_tag());
    EXPECT_EQ(mmap.has_val_tag(), mmap.get()->m_type.has_val_tag());
    EXPECT_EQ(mkeyval.has_val_tag(), mkeyval.get()->m_type.has_val_tag());
    EXPECT_EQ(mkeyvalnotag.has_val_tag(), mkeyvalnotag.get()->m_type.has_val_tag());
    EXPECT_EQ(mseq.has_val_tag(), mseq.get()->m_type.has_val_tag());
    EXPECT_EQ(mval.has_val_tag(), mval.get()->m_type.has_val_tag());
    EXPECT_EQ(mvalnotag.has_val_tag(), mvalnotag.get()->m_type.has_val_tag());
    EXPECT_EQ(mdocval.has_val_tag(), mdocval.get()->m_type.has_val_tag());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_val_tag(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).has_val_tag(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_val_tag(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_val_tag(NONE); }, visit);
}

TEST(NodeType, has_key_anchor)
{
    EXPECT_FALSE(NodeType().has_key_anchor());
    EXPECT_TRUE(NodeType(KEYANCH).has_key_anchor());
    EXPECT_TRUE(NodeType(KEY|KEYANCH).has_key_anchor());
}

TEST(Tree, has_key_anchor)
{
    Tree t = parse_in_arena(R"(--- &docanchor
&mapanchor map: {&keyvalanchor foo: bar, anchor: none}
&seqanchor seq: [&valanchor foo, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type keyvalnoanchor_id = t.last_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type valnoanchor_id = t.last_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef keyvalnoanchor = t.ref(keyvalnoanchor_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef valnoanchor = t.ref(valnoanchor_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mkeyvalnoanchor = t.ref(keyvalnoanchor_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mvalnoanchor = t.ref(valnoanchor_id);
    EXPECT_FALSE(t.has_key_anchor(stream_id));
    EXPECT_FALSE(t.has_key_anchor(doc_id));
    EXPECT_TRUE(t.has_key_anchor(map_id));
    EXPECT_TRUE(t.has_key_anchor(keyval_id));
    EXPECT_FALSE(t.has_key_anchor(keyvalnoanchor_id));
    EXPECT_TRUE(t.has_key_anchor(seq_id));
    EXPECT_FALSE(t.has_key_anchor(val_id));
    EXPECT_FALSE(t.has_key_anchor(valnoanchor_id));
    EXPECT_FALSE(stream.has_key_anchor());
    EXPECT_FALSE(doc.has_key_anchor());
    EXPECT_TRUE(map.has_key_anchor());
    EXPECT_TRUE(keyval.has_key_anchor());
    EXPECT_FALSE(keyvalnoanchor.has_key_anchor());
    EXPECT_TRUE(seq.has_key_anchor());
    EXPECT_FALSE(val.has_key_anchor());
    EXPECT_FALSE(valnoanchor.has_key_anchor());
    EXPECT_FALSE(mstream.has_key_anchor());
    EXPECT_FALSE(mdoc.has_key_anchor());
    EXPECT_TRUE(mmap.has_key_anchor());
    EXPECT_TRUE(mkeyval.has_key_anchor());
    EXPECT_FALSE(mkeyvalnoanchor.has_key_anchor());
    EXPECT_TRUE(mseq.has_key_anchor());
    EXPECT_FALSE(mval.has_key_anchor());
    EXPECT_FALSE(mvalnoanchor.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(stream_id), t._p(stream_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(doc_id), t._p(doc_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(map_id), t._p(map_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(keyval_id), t._p(keyval_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(keyvalnoanchor_id), t._p(keyvalnoanchor_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(seq_id), t._p(seq_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(val_id), t._p(val_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(valnoanchor_id), t._p(valnoanchor_id)->m_type.has_key_anchor());
    EXPECT_EQ(stream.has_key_anchor(), stream.get()->m_type.has_key_anchor());
    EXPECT_EQ(doc.has_key_anchor(), doc.get()->m_type.has_key_anchor());
    EXPECT_EQ(map.has_key_anchor(), map.get()->m_type.has_key_anchor());
    EXPECT_EQ(keyval.has_key_anchor(), keyval.get()->m_type.has_key_anchor());
    EXPECT_EQ(keyvalnoanchor.has_key_anchor(), keyvalnoanchor.get()->m_type.has_key_anchor());
    EXPECT_EQ(seq.has_key_anchor(), seq.get()->m_type.has_key_anchor());
    EXPECT_EQ(val.has_key_anchor(), val.get()->m_type.has_key_anchor());
    EXPECT_EQ(valnoanchor.has_key_anchor(), valnoanchor.get()->m_type.has_key_anchor());
    EXPECT_EQ(mstream.has_key_anchor(), mstream.get()->m_type.has_key_anchor());
    EXPECT_EQ(mdoc.has_key_anchor(), mdoc.get()->m_type.has_key_anchor());
    EXPECT_EQ(mmap.has_key_anchor(), mmap.get()->m_type.has_key_anchor());
    EXPECT_EQ(mkeyval.has_key_anchor(), mkeyval.get()->m_type.has_key_anchor());
    EXPECT_EQ(mkeyvalnoanchor.has_key_anchor(), mkeyvalnoanchor.get()->m_type.has_key_anchor());
    EXPECT_EQ(mseq.has_key_anchor(), mseq.get()->m_type.has_key_anchor());
    EXPECT_EQ(mval.has_key_anchor(), mval.get()->m_type.has_key_anchor());
    EXPECT_EQ(mvalnoanchor.has_key_anchor(), mvalnoanchor.get()->m_type.has_key_anchor());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_key(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(1).has_key_anchor(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_key_anchor(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_key_anchor(NONE); }, visit);
}

TEST(NodeType, has_val_anchor)
{
    EXPECT_FALSE(NodeType().has_val_anchor());
    EXPECT_TRUE(NodeType(VALANCH).has_val_anchor());
    EXPECT_TRUE(NodeType(VAL|VALANCH).has_val_anchor());
}

TEST(Tree, has_val_anchor)
{
    Tree t = parse_in_arena(R"(--- &docanchor
map: &mapanchor {foo: &keyvalanchor bar, anchor: none}
seq: &seqanchor [&valanchor foo, bar]
...)");
    _c4dbg_tree(t);
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type keyvalnoanchor_id = t.last_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type valnoanchor_id = t.last_child(seq_id);
    ConstNodeRef stream = t.cref(stream_id);
    ConstNodeRef doc = t.cref(doc_id);
    ConstNodeRef map = t.cref(map_id);
    ConstNodeRef keyval = t.cref(keyval_id);
    ConstNodeRef keyvalnoanchor = t.cref(keyvalnoanchor_id);
    ConstNodeRef seq = t.cref(seq_id);
    ConstNodeRef val = t.cref(val_id);
    ConstNodeRef valnoanchor = t.cref(valnoanchor_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mkeyvalnoanchor = t.ref(keyvalnoanchor_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mvalnoanchor = t.ref(valnoanchor_id);
    EXPECT_FALSE(t.has_val_anchor(stream_id));
    EXPECT_TRUE(t.has_val_anchor(doc_id));
    EXPECT_TRUE(t.has_val_anchor(map_id));
    EXPECT_TRUE(t.has_val_anchor(keyval_id));
    EXPECT_FALSE(t.has_val_anchor(keyvalnoanchor_id));
    EXPECT_TRUE(t.has_val_anchor(seq_id));
    EXPECT_TRUE(t.has_val_anchor(val_id));
    EXPECT_FALSE(t.has_val_anchor(valnoanchor_id));
    EXPECT_FALSE(stream.has_val_anchor());
    EXPECT_TRUE(doc.has_val_anchor());
    EXPECT_TRUE(map.has_val_anchor());
    EXPECT_TRUE(keyval.has_val_anchor());
    EXPECT_FALSE(keyvalnoanchor.has_val_anchor());
    EXPECT_TRUE(seq.has_val_anchor());
    EXPECT_TRUE(val.has_val_anchor());
    EXPECT_FALSE(valnoanchor.has_val_anchor());
    EXPECT_FALSE(mstream.has_val_anchor());
    EXPECT_TRUE(mdoc.has_val_anchor());
    EXPECT_TRUE(mmap.has_val_anchor());
    EXPECT_TRUE(mkeyval.has_val_anchor());
    EXPECT_FALSE(mkeyvalnoanchor.has_val_anchor());
    EXPECT_TRUE(mseq.has_val_anchor());
    EXPECT_TRUE(mval.has_val_anchor());
    EXPECT_FALSE(mvalnoanchor.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(stream_id), t._p(stream_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(doc_id), t._p(doc_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(map_id), t._p(map_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(keyval_id), t._p(keyval_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(keyvalnoanchor_id), t._p(keyvalnoanchor_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(seq_id), t._p(seq_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(val_id), t._p(val_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(valnoanchor_id), t._p(valnoanchor_id)->m_type.has_val_anchor());
    EXPECT_EQ(stream.has_val_anchor(), stream.get()->m_type.has_val_anchor());
    EXPECT_EQ(doc.has_val_anchor(), doc.get()->m_type.has_val_anchor());
    EXPECT_EQ(map.has_val_anchor(), map.get()->m_type.has_val_anchor());
    EXPECT_EQ(keyval.has_val_anchor(), keyval.get()->m_type.has_val_anchor());
    EXPECT_EQ(keyvalnoanchor.has_val_anchor(), keyvalnoanchor.get()->m_type.has_val_anchor());
    EXPECT_EQ(seq.has_val_anchor(), seq.get()->m_type.has_val_anchor());
    EXPECT_EQ(val.has_val_anchor(), val.get()->m_type.has_val_anchor());
    EXPECT_EQ(valnoanchor.has_val_anchor(), valnoanchor.get()->m_type.has_val_anchor());
    EXPECT_EQ(mstream.has_val_anchor(), mstream.get()->m_type.has_val_anchor());
    EXPECT_EQ(mdoc.has_val_anchor(), mdoc.get()->m_type.has_val_anchor());
    EXPECT_EQ(mmap.has_val_anchor(), mmap.get()->m_type.has_val_anchor());
    EXPECT_EQ(mkeyval.has_val_anchor(), mkeyval.get()->m_type.has_val_anchor());
    EXPECT_EQ(mkeyvalnoanchor.has_val_anchor(), mkeyvalnoanchor.get()->m_type.has_val_anchor());
    EXPECT_EQ(mseq.has_val_anchor(), mseq.get()->m_type.has_val_anchor());
    EXPECT_EQ(mval.has_val_anchor(), mval.get()->m_type.has_val_anchor());
    EXPECT_EQ(mvalnoanchor.has_val_anchor(), mvalnoanchor.get()->m_type.has_val_anchor());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_val_anchor(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(1).has_val_anchor(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_val_anchor(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_val_anchor(NONE); }, visit);
}

TEST(NodeType, has_anchor)
{
    EXPECT_FALSE(NodeType().has_anchor());
    EXPECT_TRUE(NodeType(VALANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEYANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEYANCH|VALANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEY|VALANCH).has_anchor());
    EXPECT_TRUE(NodeType(VAL|KEYANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEY|KEYANCH).has_anchor());
    EXPECT_TRUE(NodeType(VAL|VALANCH).has_anchor());
}

TEST(Tree, has_anchor)
{
    Tree t = parse_in_arena(R"(--- &docanchor
map: &mapanchor {foo: &keyvalanchor bar, anchor: none}
&seqanchor seq: [&valanchor foo, bar]
...)");
    _c4dbg_tree(t);
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type keyvalnoanchor_id = t.last_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type valnoanchor_id = t.last_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef keyvalnoanchor = t.ref(keyvalnoanchor_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef valnoanchor = t.ref(valnoanchor_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mkeyvalnoanchor = t.ref(keyvalnoanchor_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mvalnoanchor = t.ref(valnoanchor_id);
    EXPECT_FALSE(t.has_anchor(stream_id));
    EXPECT_TRUE(t.has_anchor(doc_id));
    EXPECT_TRUE(t.has_anchor(map_id));
    EXPECT_TRUE(t.has_anchor(keyval_id));
    EXPECT_FALSE(t.has_anchor(keyvalnoanchor_id));
    EXPECT_TRUE(t.has_anchor(seq_id));
    EXPECT_TRUE(t.has_anchor(val_id));
    EXPECT_FALSE(t.has_anchor(valnoanchor_id));
    EXPECT_FALSE(stream.has_anchor());
    EXPECT_TRUE(doc.has_anchor());
    EXPECT_TRUE(map.has_anchor());
    EXPECT_TRUE(keyval.has_anchor());
    EXPECT_FALSE(keyvalnoanchor.has_anchor());
    EXPECT_TRUE(seq.has_anchor());
    EXPECT_TRUE(val.has_anchor());
    EXPECT_FALSE(valnoanchor.has_anchor());
    EXPECT_FALSE(mstream.has_anchor());
    EXPECT_TRUE(mdoc.has_anchor());
    EXPECT_TRUE(mmap.has_anchor());
    EXPECT_TRUE(mkeyval.has_anchor());
    EXPECT_FALSE(mkeyvalnoanchor.has_anchor());
    EXPECT_TRUE(mseq.has_anchor());
    EXPECT_TRUE(mval.has_anchor());
    EXPECT_FALSE(mvalnoanchor.has_anchor());
    EXPECT_EQ(t.has_anchor(stream_id), t._p(stream_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(doc_id), t._p(doc_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(map_id), t._p(map_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(keyval_id), t._p(keyval_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(keyvalnoanchor_id), t._p(keyvalnoanchor_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(seq_id), t._p(seq_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(val_id), t._p(val_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(valnoanchor_id), t._p(valnoanchor_id)->m_type.has_anchor());
    EXPECT_EQ(stream.has_anchor(), stream.get()->m_type.has_anchor());
    EXPECT_EQ(doc.has_anchor(), doc.get()->m_type.has_anchor());
    EXPECT_EQ(map.has_anchor(), map.get()->m_type.has_anchor());
    EXPECT_EQ(keyval.has_anchor(), keyval.get()->m_type.has_anchor());
    EXPECT_EQ(keyvalnoanchor.has_anchor(), keyvalnoanchor.get()->m_type.has_anchor());
    EXPECT_EQ(seq.has_anchor(), seq.get()->m_type.has_anchor());
    EXPECT_EQ(val.has_anchor(), val.get()->m_type.has_anchor());
    EXPECT_EQ(valnoanchor.has_anchor(), valnoanchor.get()->m_type.has_anchor());
    EXPECT_EQ(mstream.has_anchor(), mstream.get()->m_type.has_anchor());
    EXPECT_EQ(mdoc.has_anchor(), mdoc.get()->m_type.has_anchor());
    EXPECT_EQ(mmap.has_anchor(), mmap.get()->m_type.has_anchor());
    EXPECT_EQ(mkeyval.has_anchor(), mkeyval.get()->m_type.has_anchor());
    EXPECT_EQ(mkeyvalnoanchor.has_anchor(), mkeyvalnoanchor.get()->m_type.has_anchor());
    EXPECT_EQ(mseq.has_anchor(), mseq.get()->m_type.has_anchor());
    EXPECT_EQ(mval.has_anchor(), mval.get()->m_type.has_anchor());
    EXPECT_EQ(mvalnoanchor.has_anchor(), mvalnoanchor.get()->m_type.has_anchor());
    verify_assertion(t, [&](Tree const&){ return t.has_anchor(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.has_anchor(NONE); }, visit);
}

TEST(NodeType, is_key_ref)
{
    EXPECT_FALSE(NodeType().is_key_ref());
    EXPECT_TRUE(NodeType(KEYREF).is_key_ref());
    EXPECT_TRUE(NodeType(KEY|KEYREF).is_key_ref());
}

TEST(Tree, is_key_ref)
{
    Tree t = parse_in_arena(R"(---
*mapref : {foo: bar, notag: none}
*seqref : [foo, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef stream = t.cref(stream_id);
    ConstNodeRef doc = t.cref(doc_id);
    ConstNodeRef map = t.cref(map_id);
    ConstNodeRef keyval = t.cref(keyval_id);
    ConstNodeRef seq = t.cref(seq_id);
    ConstNodeRef val = t.cref(val_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    EXPECT_FALSE(t.is_key_ref(stream_id));
    EXPECT_FALSE(t.is_key_ref(doc_id));
    EXPECT_TRUE(t.is_key_ref(map_id));
    EXPECT_FALSE(t.is_key_ref(keyval_id));
    EXPECT_TRUE(t.is_key_ref(seq_id));
    EXPECT_FALSE(t.is_key_ref(val_id));
    EXPECT_FALSE(stream.is_key_ref());
    EXPECT_FALSE(doc.is_key_ref());
    EXPECT_TRUE(map.is_key_ref());
    EXPECT_FALSE(keyval.is_key_ref());
    EXPECT_TRUE(seq.is_key_ref());
    EXPECT_FALSE(val.is_key_ref());
    EXPECT_FALSE(mstream.is_key_ref());
    EXPECT_FALSE(mdoc.is_key_ref());
    EXPECT_TRUE(mmap.is_key_ref());
    EXPECT_FALSE(mkeyval.is_key_ref());
    EXPECT_TRUE(mseq.is_key_ref());
    EXPECT_FALSE(mval.is_key_ref());
    EXPECT_EQ(t.is_key_ref(stream_id), t._p(stream_id)->m_type.is_key_ref());
    EXPECT_EQ(t.is_key_ref(doc_id), t._p(doc_id)->m_type.is_key_ref());
    EXPECT_EQ(t.is_key_ref(map_id), t._p(map_id)->m_type.is_key_ref());
    EXPECT_EQ(t.is_key_ref(keyval_id), t._p(keyval_id)->m_type.is_key_ref());
    EXPECT_EQ(t.is_key_ref(seq_id), t._p(seq_id)->m_type.is_key_ref());
    EXPECT_EQ(t.is_key_ref(val_id), t._p(val_id)->m_type.is_key_ref());
    EXPECT_EQ(stream.is_key_ref(), stream.get()->m_type.is_key_ref());
    EXPECT_EQ(doc.is_key_ref(), doc.get()->m_type.is_key_ref());
    EXPECT_EQ(map.is_key_ref(), map.get()->m_type.is_key_ref());
    EXPECT_EQ(keyval.is_key_ref(), keyval.get()->m_type.is_key_ref());
    EXPECT_EQ(seq.is_key_ref(), seq.get()->m_type.is_key_ref());
    EXPECT_EQ(val.is_key_ref(), val.get()->m_type.is_key_ref());
    EXPECT_EQ(mstream.is_key_ref(), mstream.get()->m_type.is_key_ref());
    EXPECT_EQ(mdoc.is_key_ref(), mdoc.get()->m_type.is_key_ref());
    EXPECT_EQ(mmap.is_key_ref(), mmap.get()->m_type.is_key_ref());
    EXPECT_EQ(mkeyval.is_key_ref(), mkeyval.get()->m_type.is_key_ref());
    EXPECT_EQ(mseq.is_key_ref(), mseq.get()->m_type.is_key_ref());
    EXPECT_EQ(mval.is_key_ref(), mval.get()->m_type.is_key_ref());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_key_ref(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_key_ref(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_key_ref(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_key_ref(NONE); }, visit);
}

TEST(NodeType, is_val_ref)
{
    EXPECT_FALSE(NodeType().is_val_ref());
    EXPECT_TRUE(NodeType(VALREF).is_val_ref());
    EXPECT_TRUE(NodeType(VAL|VALREF).is_val_ref());
}

TEST(Tree, is_val_ref)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: [*valref, bar]
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
    EXPECT_FALSE(t.is_val_ref(stream_id));
    EXPECT_FALSE(t.is_val_ref(doc_id));
    EXPECT_FALSE(t.is_val_ref(map_id));
    EXPECT_TRUE(t.is_val_ref(keyval_id));
    EXPECT_FALSE(t.is_val_ref(seq_id));
    EXPECT_TRUE(t.is_val_ref(val_id));
    EXPECT_FALSE(stream.is_val_ref());
    EXPECT_FALSE(doc.is_val_ref());
    EXPECT_FALSE(map.is_val_ref());
    EXPECT_TRUE(keyval.is_val_ref());
    EXPECT_FALSE(seq.is_val_ref());
    EXPECT_TRUE(val.is_val_ref());
    EXPECT_FALSE(mstream.is_val_ref());
    EXPECT_FALSE(mdoc.is_val_ref());
    EXPECT_FALSE(mmap.is_val_ref());
    EXPECT_TRUE(mkeyval.is_val_ref());
    EXPECT_FALSE(mseq.is_val_ref());
    EXPECT_TRUE(mval.is_val_ref());
    EXPECT_EQ(t.is_val_ref(stream_id), t._p(stream_id)->m_type.is_val_ref());
    EXPECT_EQ(t.is_val_ref(doc_id), t._p(doc_id)->m_type.is_val_ref());
    EXPECT_EQ(t.is_val_ref(map_id), t._p(map_id)->m_type.is_val_ref());
    EXPECT_EQ(t.is_val_ref(keyval_id), t._p(keyval_id)->m_type.is_val_ref());
    EXPECT_EQ(t.is_val_ref(seq_id), t._p(seq_id)->m_type.is_val_ref());
    EXPECT_EQ(t.is_val_ref(val_id), t._p(val_id)->m_type.is_val_ref());
    EXPECT_EQ(stream.is_val_ref(), stream.get()->m_type.is_val_ref());
    EXPECT_EQ(doc.is_val_ref(), doc.get()->m_type.is_val_ref());
    EXPECT_EQ(map.is_val_ref(), map.get()->m_type.is_val_ref());
    EXPECT_EQ(keyval.is_val_ref(), keyval.get()->m_type.is_val_ref());
    EXPECT_EQ(seq.is_val_ref(), seq.get()->m_type.is_val_ref());
    EXPECT_EQ(val.is_val_ref(), val.get()->m_type.is_val_ref());
    EXPECT_EQ(mstream.is_val_ref(), mstream.get()->m_type.is_val_ref());
    EXPECT_EQ(mdoc.is_val_ref(), mdoc.get()->m_type.is_val_ref());
    EXPECT_EQ(mmap.is_val_ref(), mmap.get()->m_type.is_val_ref());
    EXPECT_EQ(mkeyval.is_val_ref(), mkeyval.get()->m_type.is_val_ref());
    EXPECT_EQ(mseq.is_val_ref(), mseq.get()->m_type.is_val_ref());
    EXPECT_EQ(mval.is_val_ref(), mval.get()->m_type.is_val_ref());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_val_ref(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_val_ref(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_val_ref(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_val_ref(NONE); }, visit);
}

TEST(NodeType, is_ref)
{
    EXPECT_FALSE(NodeType().is_ref());
    EXPECT_FALSE(NodeType(KEYVAL).is_ref());
    EXPECT_TRUE(NodeType(KEYREF).is_ref());
    EXPECT_TRUE(NodeType(VALREF).is_ref());
    EXPECT_TRUE(NodeType(KEY|VALREF).is_ref());
    EXPECT_TRUE(NodeType(VAL|KEYREF).is_ref());
    EXPECT_TRUE(NodeType(KEYREF|VALREF).is_ref());
}

TEST(Tree, is_ref)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: [*valref, bar]
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
    EXPECT_FALSE(t.is_ref(stream_id));
    EXPECT_FALSE(t.is_ref(doc_id));
    EXPECT_FALSE(t.is_ref(map_id));
    EXPECT_TRUE(t.is_ref(keyval_id));
    EXPECT_FALSE(t.is_ref(seq_id));
    EXPECT_TRUE(t.is_ref(val_id));
    EXPECT_FALSE(stream.is_ref());
    EXPECT_FALSE(doc.is_ref());
    EXPECT_FALSE(map.is_ref());
    EXPECT_TRUE(keyval.is_ref());
    EXPECT_FALSE(seq.is_ref());
    EXPECT_TRUE(val.is_ref());
    EXPECT_FALSE(mstream.is_ref());
    EXPECT_FALSE(mdoc.is_ref());
    EXPECT_FALSE(mmap.is_ref());
    EXPECT_TRUE(mkeyval.is_ref());
    EXPECT_FALSE(mseq.is_ref());
    EXPECT_TRUE(mval.is_ref());
    EXPECT_EQ(t.is_ref(stream_id), t._p(stream_id)->m_type.is_ref());
    EXPECT_EQ(t.is_ref(doc_id), t._p(doc_id)->m_type.is_ref());
    EXPECT_EQ(t.is_ref(map_id), t._p(map_id)->m_type.is_ref());
    EXPECT_EQ(t.is_ref(keyval_id), t._p(keyval_id)->m_type.is_ref());
    EXPECT_EQ(t.is_ref(seq_id), t._p(seq_id)->m_type.is_ref());
    EXPECT_EQ(t.is_ref(val_id), t._p(val_id)->m_type.is_ref());
    EXPECT_EQ(stream.is_ref(), stream.get()->m_type.is_ref());
    EXPECT_EQ(doc.is_ref(), doc.get()->m_type.is_ref());
    EXPECT_EQ(map.is_ref(), map.get()->m_type.is_ref());
    EXPECT_EQ(keyval.is_ref(), keyval.get()->m_type.is_ref());
    EXPECT_EQ(seq.is_ref(), seq.get()->m_type.is_ref());
    EXPECT_EQ(val.is_ref(), val.get()->m_type.is_ref());
    EXPECT_EQ(mstream.is_ref(), mstream.get()->m_type.is_ref());
    EXPECT_EQ(mdoc.is_ref(), mdoc.get()->m_type.is_ref());
    EXPECT_EQ(mmap.is_ref(), mmap.get()->m_type.is_ref());
    EXPECT_EQ(mkeyval.is_ref(), mkeyval.get()->m_type.is_ref());
    EXPECT_EQ(mseq.is_ref(), mseq.get()->m_type.is_ref());
    EXPECT_EQ(mval.is_ref(), mval.get()->m_type.is_ref());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_ref(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_ref(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_ref(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_ref(NONE); }, visit);
}

TEST(NodeType, is_key_quoted)
{
    EXPECT_FALSE(NodeType().is_key_quoted());
    EXPECT_TRUE(NodeType(KEYQUO).is_key_quoted());
    EXPECT_TRUE(NodeType(KEY|KEYQUO).is_key_quoted());
}

TEST(Tree, is_key_quoted)
{
    Tree t = parse_in_arena(R"(---
"quoted": foo
notquoted: bar
...)");
    const id_type map_id = t.first_child(t.root_id());
    const id_type quoted_id = t.first_child(map_id);
    const id_type notquoted_id = t.last_child(map_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef quoted = t.ref(quoted_id);
    ConstNodeRef notquoted = t.ref(notquoted_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mquoted = t.ref(quoted_id);
    NodeRef mnotquoted = t.ref(notquoted_id);
    EXPECT_FALSE(t.is_key_quoted(map_id));
    EXPECT_TRUE(t.is_key_quoted(quoted_id));
    EXPECT_FALSE(t.is_key_quoted(notquoted_id));
    EXPECT_FALSE(map.is_key_quoted());
    EXPECT_TRUE(quoted.is_key_quoted());
    EXPECT_FALSE(notquoted.is_key_quoted());
    EXPECT_FALSE(mmap.is_key_quoted());
    EXPECT_TRUE(mquoted.is_key_quoted());
    EXPECT_FALSE(mnotquoted.is_key_quoted());
    EXPECT_EQ(t.is_key_quoted(map_id), t._p(map_id)->m_type.is_key_quoted());
    EXPECT_EQ(t.is_key_quoted(quoted_id), t._p(quoted_id)->m_type.is_key_quoted());
    EXPECT_EQ(t.is_key_quoted(notquoted_id), t._p(notquoted_id)->m_type.is_key_quoted());
    EXPECT_EQ(map.is_key_quoted(), map.get()->m_type.is_key_quoted());
    EXPECT_EQ(quoted.is_key_quoted(), quoted.get()->m_type.is_key_quoted());
    EXPECT_EQ(notquoted.is_key_quoted(), notquoted.get()->m_type.is_key_quoted());
    EXPECT_EQ(mmap.is_key_quoted(), mmap.get()->m_type.is_key_quoted());
    EXPECT_EQ(mquoted.is_key_quoted(), mquoted.get()->m_type.is_key_quoted());
    EXPECT_EQ(mnotquoted.is_key_quoted(), mnotquoted.get()->m_type.is_key_quoted());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_key_quoted(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_key_quoted(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_key_quoted(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_key_quoted(NONE); }, visit);
}

TEST(NodeType, is_val_quoted)
{
    EXPECT_FALSE(NodeType().is_val_quoted());
    EXPECT_TRUE(NodeType(VALQUO).is_val_quoted());
    EXPECT_TRUE(NodeType(VAL|VALQUO).is_val_quoted());
}

TEST(Tree, is_val_quoted)
{
    Tree t = parse_in_arena(R"(---
"quoted": "foo"
notquoted: bar
...)");
    const id_type map_id = t.first_child(t.root_id());
    const id_type quoted_id = t.first_child(map_id);
    const id_type notquoted_id = t.last_child(map_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef quoted = t.ref(quoted_id);
    ConstNodeRef notquoted = t.ref(notquoted_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mquoted = t.ref(quoted_id);
    NodeRef mnotquoted = t.ref(notquoted_id);
    EXPECT_FALSE(t.is_val_quoted(map_id));
    EXPECT_TRUE(t.is_val_quoted(quoted_id));
    EXPECT_FALSE(t.is_val_quoted(notquoted_id));
    EXPECT_FALSE(map.is_val_quoted());
    EXPECT_TRUE(quoted.is_val_quoted());
    EXPECT_FALSE(notquoted.is_val_quoted());
    EXPECT_FALSE(mmap.is_val_quoted());
    EXPECT_TRUE(mquoted.is_val_quoted());
    EXPECT_FALSE(mnotquoted.is_val_quoted());
    EXPECT_EQ(t.is_val_quoted(map_id), t._p(map_id)->m_type.is_val_quoted());
    EXPECT_EQ(t.is_val_quoted(quoted_id), t._p(quoted_id)->m_type.is_val_quoted());
    EXPECT_EQ(t.is_val_quoted(notquoted_id), t._p(notquoted_id)->m_type.is_val_quoted());
    EXPECT_EQ(map.is_val_quoted(), map.get()->m_type.is_val_quoted());
    EXPECT_EQ(quoted.is_val_quoted(), quoted.get()->m_type.is_val_quoted());
    EXPECT_EQ(notquoted.is_val_quoted(), notquoted.get()->m_type.is_val_quoted());
    EXPECT_EQ(mmap.is_val_quoted(), mmap.get()->m_type.is_val_quoted());
    EXPECT_EQ(mquoted.is_val_quoted(), mquoted.get()->m_type.is_val_quoted());
    EXPECT_EQ(mnotquoted.is_val_quoted(), mnotquoted.get()->m_type.is_val_quoted());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_val_quoted(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_val_quoted(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_val_quoted(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_val_quoted(NONE); }, visit);
}

TEST(NodeType, is_quoted)
{
    EXPECT_FALSE(NodeType().is_quoted());
    EXPECT_TRUE(NodeType(KEYQUO).is_quoted());
    EXPECT_TRUE(NodeType(VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(KEYQUO|VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(KEY|KEYQUO).is_quoted());
    EXPECT_TRUE(NodeType(VAL|VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(KEY|VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(VAL|KEYQUO).is_quoted());
}

TEST(Tree, is_quoted)
{
    Tree t = parse_in_arena(R"(---
"quoted1": foo
quoted2: "foo"
"quoted3": "foo"
'quoted4': foo
quoted5: 'foo'
'quoted6': 'foo'
notquoted: bar
...)");
    const id_type map_id = t.first_child(t.root_id());
    const id_type quoted1_id = t.find_child(map_id, "quoted1");
    const id_type quoted2_id = t.find_child(map_id, "quoted2");
    const id_type quoted3_id = t.find_child(map_id, "quoted3");
    const id_type quoted4_id = t.find_child(map_id, "quoted4");
    const id_type quoted5_id = t.find_child(map_id, "quoted5");
    const id_type quoted6_id = t.find_child(map_id, "quoted6");
    const id_type notquoted_id = t.last_child(map_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef quoted1 = t.ref(quoted1_id);
    ConstNodeRef quoted2 = t.ref(quoted2_id);
    ConstNodeRef quoted3 = t.ref(quoted3_id);
    ConstNodeRef quoted4 = t.ref(quoted4_id);
    ConstNodeRef quoted5 = t.ref(quoted5_id);
    ConstNodeRef quoted6 = t.ref(quoted6_id);
    ConstNodeRef notquoted = t.ref(notquoted_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mquoted1 = t.ref(quoted1_id);
    NodeRef mquoted2 = t.ref(quoted2_id);
    NodeRef mquoted3 = t.ref(quoted3_id);
    NodeRef mquoted4 = t.ref(quoted4_id);
    NodeRef mquoted5 = t.ref(quoted5_id);
    NodeRef mquoted6 = t.ref(quoted6_id);
    NodeRef mnotquoted = t.ref(notquoted_id);
    EXPECT_FALSE(t.is_quoted(map_id));
    EXPECT_TRUE(t.is_quoted(quoted1_id));
    EXPECT_TRUE(t.is_quoted(quoted2_id));
    EXPECT_TRUE(t.is_quoted(quoted3_id));
    EXPECT_TRUE(t.is_quoted(quoted4_id));
    EXPECT_TRUE(t.is_quoted(quoted5_id));
    EXPECT_TRUE(t.is_quoted(quoted6_id));
    EXPECT_FALSE(t.is_quoted(notquoted_id));
    EXPECT_FALSE(map.is_quoted());
    EXPECT_TRUE(quoted1.is_quoted());
    EXPECT_TRUE(quoted2.is_quoted());
    EXPECT_TRUE(quoted3.is_quoted());
    EXPECT_TRUE(quoted4.is_quoted());
    EXPECT_TRUE(quoted5.is_quoted());
    EXPECT_TRUE(quoted6.is_quoted());
    EXPECT_FALSE(notquoted.is_quoted());
    EXPECT_FALSE(mmap.is_quoted());
    EXPECT_TRUE(mquoted1.is_quoted());
    EXPECT_TRUE(mquoted2.is_quoted());
    EXPECT_TRUE(mquoted3.is_quoted());
    EXPECT_TRUE(mquoted4.is_quoted());
    EXPECT_TRUE(mquoted5.is_quoted());
    EXPECT_TRUE(mquoted6.is_quoted());
    EXPECT_FALSE(mnotquoted.is_quoted());
    EXPECT_EQ(t.is_quoted(map_id), t._p(map_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted1_id), t._p(quoted1_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted2_id), t._p(quoted2_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted3_id), t._p(quoted3_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted4_id), t._p(quoted4_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted5_id), t._p(quoted5_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted6_id), t._p(quoted6_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(notquoted_id), t._p(notquoted_id)->m_type.is_quoted());
    EXPECT_EQ(map.is_quoted(), map.get()->m_type.is_quoted());
    EXPECT_EQ(quoted1.is_quoted(), quoted1.get()->m_type.is_quoted());
    EXPECT_EQ(quoted2.is_quoted(), quoted2.get()->m_type.is_quoted());
    EXPECT_EQ(quoted3.is_quoted(), quoted3.get()->m_type.is_quoted());
    EXPECT_EQ(quoted4.is_quoted(), quoted4.get()->m_type.is_quoted());
    EXPECT_EQ(quoted5.is_quoted(), quoted5.get()->m_type.is_quoted());
    EXPECT_EQ(quoted6.is_quoted(), quoted6.get()->m_type.is_quoted());
    EXPECT_EQ(notquoted.is_quoted(), notquoted.get()->m_type.is_quoted());
    EXPECT_EQ(mmap.is_quoted(), mmap.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted1.is_quoted(), mquoted1.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted2.is_quoted(), mquoted2.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted3.is_quoted(), mquoted3.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted4.is_quoted(), mquoted4.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted5.is_quoted(), mquoted5.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted6.is_quoted(), mquoted6.get()->m_type.is_quoted());
    EXPECT_EQ(mnotquoted.is_quoted(), mnotquoted.get()->m_type.is_quoted());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_quoted(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_quoted(); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_quoted(t.capacity()); }, visit);
    verify_assertion(t, [&](Tree const&){ return t.is_quoted(NONE); }, visit);
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TEST(change_type, from_val)
{
    Tree t = parse_in_arena("[val0, val1, val2]");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); }, visit);
    Tree expected = parse_in_arena("[val0, {}, []]");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}
TEST(change_type, from_keyval)
{
    Tree t = parse_in_arena("{keyval0: val0, keyval1: val1, keyval2: val2}");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); }, visit);
    Tree expected = parse_in_arena("{keyval0: val0, keyval1: {}, keyval2: []}");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}

TEST(change_type, from_map)
{
    Tree t = parse_in_arena("[{map0: val0}, {map1: {map1key0: a, map1key1: b}}, {map2: [map2val0, map2val1]}]");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); }, visit);
    EXPECT_FALSE(t[0].val_is_null());
    EXPECT_NE(t[0].val(), nullptr);
    Tree expected = parse_in_arena("['', {map1: {map1key0: a, map1key1: b}}, []]");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}
TEST(change_type, from_keymap)
{
    Tree t = parse_in_arena("{map0: {map0: val0}, map1: {map1: {map1key0: a, map1key1: b}}, map2: {map2: [map2val0, map2val1]}}");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); }, visit);
    EXPECT_FALSE(t[0].val_is_null());
    EXPECT_NE(t[0].val(), nullptr);
    Tree expected = parse_in_arena("{map0: '', map1: {map1: {map1key0: a, map1key1: b}}, map2: []}");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}

TEST(change_type, from_seq)
{
    Tree t = parse_in_arena("[[seq00, seq01], [seq10, seq11], [seq20, seq21]]");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); }, visit);
    EXPECT_FALSE(t[0].val_is_null());
    EXPECT_NE(t[0].val(), nullptr);
    Tree expected = parse_in_arena("['', {}, [seq20, seq21]]");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}
TEST(change_type, from_keyseq)
{
    Tree t = parse_in_arena("{map0: [seq00, seq01], map1: [seq10, seq11], map2: [seq20, seq21]}");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); }, visit);
    EXPECT_FALSE(t[0].val_is_null());
    EXPECT_NE(t[0].val(), nullptr);
    Tree expected = parse_in_arena("{map0: '', map1: {}, map2: [seq20, seq21]}");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}



//-----------------------------------------------------------------------------

void test_key_nostyle(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_key_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_key_folded(n.id()));
    EXPECT_FALSE(n.tree()->is_key_quoted(n.id()));
    EXPECT_EQ(n.tree()->is_quoted(n.id()), n.tree()->is_val_quoted(n.id()));
    EXPECT_FALSE(n.tree()->type_has_any(n.id(), KEY_STYLE));
    EXPECT_FALSE(n.tree()->type_has_all(n.id(), KEY_STYLE));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE));
    EXPECT_FALSE(n.is_key_styled());
    EXPECT_FALSE(n.is_key_plain());
    EXPECT_FALSE(n.is_key_squo());
    EXPECT_FALSE(n.is_key_dquo());
    EXPECT_FALSE(n.is_key_literal());
    EXPECT_FALSE(n.is_key_folded());
    EXPECT_FALSE(n.is_key_quoted());
    EXPECT_EQ(n.is_quoted(), n.is_val_quoted());
    EXPECT_FALSE(n.type_has_any(KEY_STYLE));
    EXPECT_FALSE(n.type_has_all(KEY_STYLE));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE));
}
void test_val_nostyle(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_container_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_val_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_val_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_val_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_val_folded(n.id()));
    EXPECT_FALSE(n.tree()->is_val_quoted(n.id()));
    EXPECT_EQ(n.tree()->is_quoted(n.id()), n.tree()->is_key_quoted(n.id()));
    EXPECT_FALSE(n.tree()->type_has_any(n.id(), VAL_STYLE));
    EXPECT_FALSE(n.tree()->type_has_all(n.id(), VAL_STYLE));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE));
    EXPECT_FALSE(n.is_val_styled());
    EXPECT_FALSE(n.is_val_plain());
    EXPECT_FALSE(n.is_val_squo());
    EXPECT_FALSE(n.is_val_dquo());
    EXPECT_FALSE(n.is_val_literal());
    EXPECT_FALSE(n.is_val_folded());
    EXPECT_FALSE(n.is_val_quoted());
    EXPECT_EQ(n.is_quoted(), n.is_key_quoted());
    EXPECT_FALSE(n.type_has_any(VAL_STYLE));
    EXPECT_FALSE(n.type_has_all(VAL_STYLE));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE));
}

void test_key_plain(ConstNodeRef n)
{
    EXPECT_TRUE(n.tree()->is_key_styled(n.id()));
    EXPECT_TRUE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_key_folded(n.id()));
    EXPECT_FALSE(n.tree()->is_key_quoted(n.id()));
    EXPECT_EQ(n.tree()->is_quoted(n.id()), n.tree()->is_val_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), KEY_PLAIN));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), KEY_PLAIN));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE & ~KEY_PLAIN));
    EXPECT_TRUE(n.is_key_styled());
    EXPECT_TRUE(n.is_key_plain());
    EXPECT_FALSE(n.is_key_squo());
    EXPECT_FALSE(n.is_key_dquo());
    EXPECT_FALSE(n.is_key_literal());
    EXPECT_FALSE(n.is_key_folded());
    EXPECT_FALSE(n.is_key_quoted());
    EXPECT_EQ(n.is_quoted(), n.is_val_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(KEY_PLAIN));
    EXPECT_TRUE(n.type_has_all(KEY_PLAIN));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE & ~KEY_PLAIN));
}
void test_key_squo(ConstNodeRef n)
{
    EXPECT_TRUE(n.tree()->is_key_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_TRUE(n.tree()->is_key_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_key_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_key_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), KEY_SQUO));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), KEY_SQUO));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE & ~KEY_SQUO));
    EXPECT_TRUE(n.is_key_styled());
    EXPECT_TRUE(n.is_key_squo());
    EXPECT_FALSE(n.is_key_dquo());
    EXPECT_FALSE(n.is_key_literal());
    EXPECT_FALSE(n.is_key_folded());
    EXPECT_TRUE(n.is_key_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(KEY_SQUO));
    EXPECT_TRUE(n.type_has_all(KEY_SQUO));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE & ~KEY_SQUO));
}
void test_key_dquo(ConstNodeRef n)
{
    EXPECT_TRUE(n.tree()->is_key_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_squo(n.id()));
    EXPECT_TRUE(n.tree()->is_key_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_key_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_key_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), KEY_DQUO));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), KEY_DQUO));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE & ~KEY_DQUO));
    EXPECT_TRUE(n.is_key_styled());
    EXPECT_FALSE(n.is_key_plain());
    EXPECT_FALSE(n.is_key_squo());
    EXPECT_TRUE(n.is_key_dquo());
    EXPECT_FALSE(n.is_key_literal());
    EXPECT_FALSE(n.is_key_folded());
    EXPECT_TRUE(n.is_key_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(KEY_DQUO));
    EXPECT_TRUE(n.type_has_all(KEY_DQUO));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE & ~KEY_DQUO));
}
void test_key_literal(ConstNodeRef n)
{
    EXPECT_TRUE(n.tree()->is_key_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_dquo(n.id()));
    EXPECT_TRUE(n.tree()->is_key_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_key_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_key_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), KEY_LITERAL));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), KEY_LITERAL));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE & ~KEY_LITERAL));
    EXPECT_TRUE(n.is_key_styled());
    EXPECT_FALSE(n.is_key_plain());
    EXPECT_FALSE(n.is_key_squo());
    EXPECT_TRUE(n.is_key_literal());
    EXPECT_FALSE(n.is_key_folded());
    EXPECT_TRUE(n.is_key_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(KEY_LITERAL));
    EXPECT_TRUE(n.type_has_all(KEY_LITERAL));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE & ~KEY_LITERAL));
}
void test_key_folded(ConstNodeRef n)
{
    EXPECT_TRUE(n.tree()->is_key_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_literal(n.id()));
    EXPECT_TRUE(n.tree()->is_key_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_key_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), KEY_FOLDED));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), KEY_FOLDED));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE & ~KEY_FOLDED));
    EXPECT_TRUE(n.is_key_styled());
    EXPECT_FALSE(n.is_key_plain());
    EXPECT_FALSE(n.is_key_squo());
    EXPECT_FALSE(n.is_key_dquo());
    EXPECT_TRUE(n.is_key_folded());
    EXPECT_TRUE(n.is_key_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(KEY_FOLDED));
    EXPECT_TRUE(n.type_has_all(KEY_FOLDED));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE & ~KEY_FOLDED));
}

void test_val_plain(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_container_styled(n.id()));
    EXPECT_TRUE(n.tree()->is_val_styled(n.id()));
    EXPECT_TRUE(n.tree()->is_val_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_val_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_val_folded(n.id()));
    EXPECT_FALSE(n.tree()->is_val_quoted(n.id()));
    EXPECT_EQ(n.tree()->is_quoted(n.id()), n.tree()->is_key_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), VAL_PLAIN));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), VAL_PLAIN));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE & ~VAL_PLAIN));
    EXPECT_FALSE(n.is_container_styled());
    EXPECT_TRUE(n.is_val_styled());
    EXPECT_TRUE(n.is_val_plain());
    EXPECT_FALSE(n.is_val_squo());
    EXPECT_FALSE(n.is_val_dquo());
    EXPECT_FALSE(n.is_val_literal());
    EXPECT_FALSE(n.is_val_folded());
    EXPECT_FALSE(n.is_val_quoted());
    EXPECT_EQ(n.is_quoted(), n.is_key_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(VAL_PLAIN));
    EXPECT_TRUE(n.type_has_all(VAL_PLAIN));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE & ~VAL_PLAIN));
}
void test_val_squo(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_container_styled(n.id()));
    EXPECT_TRUE(n.tree()->is_val_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_val_plain(n.id()));
    EXPECT_TRUE(n.tree()->is_val_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_val_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_val_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), VAL_SQUO));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), VAL_SQUO));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE & ~VAL_SQUO));
    EXPECT_FALSE(n.is_container_styled());
    EXPECT_TRUE(n.is_val_squo());
    EXPECT_FALSE(n.is_val_dquo());
    EXPECT_FALSE(n.is_val_literal());
    EXPECT_FALSE(n.is_val_folded());
    EXPECT_TRUE(n.is_val_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(VAL_SQUO));
    EXPECT_TRUE(n.type_has_all(VAL_SQUO));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE & ~VAL_SQUO));
}
void test_val_dquo(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_container_styled(n.id()));
    EXPECT_TRUE(n.tree()->is_val_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_val_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_val_squo(n.id()));
    EXPECT_TRUE(n.tree()->is_val_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_val_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_val_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), VAL_DQUO));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), VAL_DQUO));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE & ~VAL_DQUO));
    EXPECT_FALSE(n.is_container_styled());
    EXPECT_FALSE(n.is_val_plain());
    EXPECT_FALSE(n.is_val_squo());
    EXPECT_TRUE(n.is_val_dquo());
    EXPECT_FALSE(n.is_val_literal());
    EXPECT_FALSE(n.is_val_folded());
    EXPECT_TRUE(n.is_val_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(VAL_DQUO));
    EXPECT_TRUE(n.type_has_all(VAL_DQUO));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE & ~VAL_DQUO));
}
void test_val_literal(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_container_styled(n.id()));
    EXPECT_TRUE(n.tree()->is_val_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_val_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_val_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_dquo(n.id()));
    EXPECT_TRUE(n.tree()->is_val_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_val_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_val_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), VAL_LITERAL));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), VAL_LITERAL));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE & ~VAL_LITERAL));
    EXPECT_FALSE(n.is_container_styled());
    EXPECT_FALSE(n.is_val_plain());
    EXPECT_FALSE(n.is_val_squo());
    EXPECT_TRUE(n.is_val_literal());
    EXPECT_FALSE(n.is_val_folded());
    EXPECT_TRUE(n.is_val_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(VAL_LITERAL));
    EXPECT_TRUE(n.type_has_all(VAL_LITERAL));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE & ~VAL_LITERAL));
}
void test_val_folded(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_container_styled(n.id()));
    EXPECT_TRUE(n.tree()->is_val_styled(n.id()));
    EXPECT_FALSE(n.tree()->is_val_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_val_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_literal(n.id()));
    EXPECT_TRUE(n.tree()->is_val_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_val_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), VAL_FOLDED));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), VAL_FOLDED));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE & ~VAL_FOLDED));
    EXPECT_FALSE(n.is_container_styled());
    EXPECT_FALSE(n.is_val_plain());
    EXPECT_FALSE(n.is_val_squo());
    EXPECT_FALSE(n.is_val_dquo());
    EXPECT_TRUE(n.is_val_folded());
    EXPECT_TRUE(n.is_val_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(VAL_FOLDED));
    EXPECT_TRUE(n.type_has_all(VAL_FOLDED));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE & ~VAL_FOLDED));
}

void test_key_styles(NodeRef n)
{
    NodeType orig = n.type();
    n.set_key_style({});
    test_key_nostyle(n);
    n.set_key_style(KEY_PLAIN);
    test_key_plain(n);
    n.set_key_style(KEY_SQUO);
    test_key_squo(n);
    n.set_key_style(KEY_DQUO);
    n.set_key_style(KEY_DQUO);
    test_key_dquo(n);
    n.set_key_style(KEY_LITERAL);
    test_key_literal(n);
    n.set_key_style(KEY_FOLDED);
    test_key_folded(n);
    n.set_key_style(orig);
}
void test_val_styles(NodeRef n)
{
    NodeType orig = n.type();
    n.set_val_style({});
    test_val_nostyle(n);
    n.set_val_style(VAL_PLAIN);
    test_val_plain(n);
    n.set_val_style(VAL_SQUO);
    test_val_squo(n);
    n.set_val_style(VAL_DQUO);
    test_val_dquo(n);
    n.set_val_style(VAL_LITERAL);
    test_val_literal(n);
    n.set_val_style(VAL_FOLDED);
    test_val_folded(n);
    n.set_val_style(orig);
}

void test_nostyle(ConstNodeRef n)
{
    EXPECT_FALSE(n.is_block());
    EXPECT_FALSE(n.is_flow_sl());
    EXPECT_FALSE(n.is_flow_ml1());
    EXPECT_FALSE(n.is_flow_mln());
    EXPECT_FALSE(n.is_flow_mlx());
    EXPECT_FALSE(n.is_flow());
}
void test_block(ConstNodeRef n)
{
    EXPECT_TRUE(n.is_block());
    EXPECT_FALSE(n.is_flow_sl());
    EXPECT_FALSE(n.is_flow_mlx());
    EXPECT_FALSE(n.is_flow_ml1());
    EXPECT_FALSE(n.is_flow_mln());
    EXPECT_FALSE(n.is_flow());
}
void test_flow_sl(ConstNodeRef n)
{
    EXPECT_FALSE(n.is_block());
    EXPECT_TRUE(n.is_flow_sl());
    EXPECT_FALSE(n.is_flow_mlx());
    EXPECT_FALSE(n.is_flow_ml1());
    EXPECT_FALSE(n.is_flow_mln());
    EXPECT_TRUE(n.is_flow());
}
void test_flow_ml1(ConstNodeRef n)
{
    EXPECT_FALSE(n.is_block());
    EXPECT_FALSE(n.is_flow_sl());
    EXPECT_TRUE(n.is_flow_mlx());
    EXPECT_TRUE(n.is_flow_ml1());
    EXPECT_FALSE(n.is_flow_mln());
    EXPECT_TRUE(n.is_flow());
}
void test_flow_mln(ConstNodeRef n)
{
    EXPECT_FALSE(n.is_block());
    EXPECT_FALSE(n.is_flow_sl());
    EXPECT_TRUE(n.is_flow_mlx());
    EXPECT_FALSE(n.is_flow_ml1());
    EXPECT_TRUE(n.is_flow_mln());
    EXPECT_TRUE(n.is_flow());
}

void test_container_styles(NodeRef n)
{
    NodeType orig = n.type();
    n.set_container_style({});
    test_nostyle(n);
    n.set_container_style(BLOCK);
    test_block(n);
    n.set_container_style(FLOW_SL);
    test_flow_sl(n);
    n.set_container_style(FLOW_ML1);
    test_flow_ml1(n);
    n.set_container_style(FLOW_MLN);
    test_flow_mln(n);
    n.set_container_style(orig);
}

csubstr style_yaml = R"(plain key: plain val
'squoted key': 'squoted val'
"dquoted key": "dquoted val"
? |-
    literal key
: |-
    literal val
? >-
    folded key
: >-
    folded val
block map:
  this is: block
block seq:
  - this is
  - block
flow map: {this is: flow}
flow seq: [this is, flow]
)";
TEST(Tree, style)
{
    Tree tree = parse_in_arena(style_yaml);

    test_key_plain(tree["plain key"]);
    test_key_styles(tree["plain key"]);
    test_key_plain(tree["plain key"]);

    test_val_plain(tree["plain key"]);
    test_val_styles(tree["plain key"]);
    test_val_plain(tree["plain key"]);

    test_key_squo(tree["squoted key"]);
    test_key_styles(tree["squoted key"]);
    test_key_squo(tree["squoted key"]);

    test_val_squo(tree["squoted key"]);
    test_val_styles(tree["squoted key"]);
    test_val_squo(tree["squoted key"]);

    test_key_dquo(tree["dquoted key"]);
    test_key_styles(tree["dquoted key"]);
    test_key_dquo(tree["dquoted key"]);

    test_val_dquo(tree["dquoted key"]);
    test_val_styles(tree["squoted key"]);
    test_val_dquo(tree["dquoted key"]);

    test_key_literal(tree["literal key"]);
    test_key_styles(tree["literal key"]);
    test_key_literal(tree["literal key"]);

    test_val_literal(tree["literal key"]);
    test_val_styles(tree["literal key"]);
    test_val_literal(tree["literal key"]);

    test_key_folded(tree["folded key"]);
    test_key_styles(tree["folded key"]);
    test_key_folded(tree["folded key"]);

    test_val_folded(tree["folded key"]);
    test_val_styles(tree["folded key"]);
    test_val_folded(tree["folded key"]);

    test_block(tree["block map"]);
    test_container_styles(tree["block map"]);
    test_block(tree["block map"]);

    test_block(tree["block seq"]);
    test_container_styles(tree["block seq"]);
    test_block(tree["block seq"]);

    test_flow_sl(tree["flow map"]);
    test_container_styles(tree["flow map"]);
    test_flow_sl(tree["flow map"]);

    test_flow_sl(tree["flow seq"]);
    test_container_styles(tree["flow seq"]);
    test_flow_sl(tree["flow seq"]);
}

TEST(Tree, unfiltered)
{
    const Tree tree = parse_in_arena(style_yaml);
    EXPECT_FALSE(tree.is_key_unfiltered(tree[0].id()));
    EXPECT_FALSE(tree.is_val_unfiltered(tree[0].id()));
    EXPECT_FALSE(tree.is_key_unfiltered(tree[1].id()));
    EXPECT_FALSE(tree.is_val_unfiltered(tree[1].id()));
    EXPECT_FALSE(tree.is_key_unfiltered(tree[2].id()));
    EXPECT_FALSE(tree.is_val_unfiltered(tree[2].id()));
    EXPECT_FALSE(tree.is_key_unfiltered(tree[3].id()));
    EXPECT_FALSE(tree.is_val_unfiltered(tree[3].id()));
    EXPECT_FALSE(tree.is_key_unfiltered(tree[4].id()));
    EXPECT_FALSE(tree.is_val_unfiltered(tree[4].id()));
    EXPECT_FALSE(tree[0].is_key_unfiltered());
    EXPECT_FALSE(tree[0].is_val_unfiltered());
    EXPECT_FALSE(tree[1].is_key_unfiltered());
    EXPECT_FALSE(tree[1].is_val_unfiltered());
    EXPECT_FALSE(tree[2].is_key_unfiltered());
    EXPECT_FALSE(tree[2].is_val_unfiltered());
    EXPECT_FALSE(tree[3].is_key_unfiltered());
    EXPECT_FALSE(tree[3].is_val_unfiltered());
    EXPECT_FALSE(tree[4].is_key_unfiltered());
    EXPECT_FALSE(tree[4].is_val_unfiltered());
    EXPECT_EQ(tree[3].key(), "literal key");
    EXPECT_EQ(tree[3].val(), "literal val");
    EXPECT_EQ(tree[3].keysc().scalar, "literal key");
    EXPECT_EQ(tree[3].valsc().scalar, "literal val");
    EXPECT_EQ(tree[4].key(), "folded key");
    EXPECT_EQ(tree[4].val(), "folded val");
    EXPECT_EQ(tree[4].keysc().scalar, "folded key");
    EXPECT_EQ(tree[4].valsc().scalar, "folded val");
    EventHandlerTree evt_handler = {};
    Parser parser(&evt_handler, ParserOptions().scalar_filtering(false));
    const Tree tree2 = parse_in_arena(&parser, style_yaml);
    EXPECT_FALSE(tree2.is_key_unfiltered(tree2[0].id()));
    EXPECT_FALSE(tree2.is_val_unfiltered(tree2[0].id()));
    EXPECT_FALSE(tree2.is_key_unfiltered(tree2[1].id()));
    EXPECT_FALSE(tree2.is_val_unfiltered(tree2[1].id()));
    EXPECT_FALSE(tree2.is_key_unfiltered(tree2[2].id()));
    EXPECT_FALSE(tree2.is_val_unfiltered(tree2[2].id()));
    EXPECT_TRUE(tree2.is_key_unfiltered(tree2[3].id()));
    EXPECT_TRUE(tree2.is_val_unfiltered(tree2[3].id()));
    EXPECT_TRUE(tree2.is_key_unfiltered(tree2[4].id()));
    EXPECT_TRUE(tree2.is_val_unfiltered(tree2[4].id()));
    EXPECT_FALSE(tree2[0].is_key_unfiltered());
    EXPECT_FALSE(tree2[0].is_val_unfiltered());
    EXPECT_FALSE(tree2[1].is_key_unfiltered());
    EXPECT_FALSE(tree2[1].is_val_unfiltered());
    EXPECT_FALSE(tree2[2].is_key_unfiltered());
    EXPECT_FALSE(tree2[2].is_val_unfiltered());
    EXPECT_TRUE(tree2[3].is_key_unfiltered());
    EXPECT_TRUE(tree2[3].is_val_unfiltered());
    EXPECT_TRUE(tree2[4].is_key_unfiltered());
    EXPECT_TRUE(tree2[4].is_val_unfiltered());
    EXPECT_NE(tree2[3].key(), "literal key");
    EXPECT_NE(tree2[3].val(), "literal val");
    EXPECT_NE(tree2[4].key(), "folded key");
    EXPECT_NE(tree2[4].val(), "folded val");
}

} // namespace yml
} // namespace c4
