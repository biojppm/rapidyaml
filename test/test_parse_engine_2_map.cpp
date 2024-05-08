#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleMapFlow,
            ("{foo: bar,foo2: bar2}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedMapFlow,
            (HAS_CONTAINER_KEYS, Location(1,1,2), "{{}: {}}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "-MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map());
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedMap3FlowEmpty,
            (HAS_CONTAINER_KEYS, Location(1,1,2), "{{{}: {}}: {{}: {}}}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "-MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "-MAP\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "-MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map());
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map());
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapFlowMultiline,
            ("{\nfoo:\n bar\n,\nfoo2:\nbar2\n}", "{foo: bar,foo2: bar2}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(SimpleMapFlowErr0, Location(1,1,2), "{")
ENGINE_TEST_ERRLOC(SimpleMapFlowErr1, Location(5,1,6), "{a: b")


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleMapBlock,
            ("foo: bar\nfoo2: bar2\nfoo3: bar3\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :foo3\n"
            "=VAL :bar3\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo3"));
    ___(ps.set_val_scalar_plain("bar3"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapBlockEmptyVals,
            ("a:\nb:\nc:\nd:\n", "a: \nb: \nc: \nd: \n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :\n"
            "=VAL :b\n"
            "=VAL :\n"
            "=VAL :c\n"
            "=VAL :\n"
            "=VAL :d\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_map_val_block();
    ps.set_key_scalar_plain("a");
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain("b");
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain("c");
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain("d");
    ps.set_val_scalar_plain({});
    ps.end_map();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleMapBlockEmptyKeys,
            (": a\n: b\n: c\n: d\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :a\n"
            "=VAL :\n"
            "=VAL :b\n"
            "=VAL :\n"
            "=VAL :c\n"
            "=VAL :\n"
            "=VAL :d\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_map_val_block();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain("a");
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain("b");
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain("c");
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain("d");
    ps.end_map();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleMapBlockEmpty,
            (":\n:\n:\n:\n", ": \n: \n: \n: \n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_map_val_block();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain({});
    ps.end_map();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleMapIndentlessSeq,
            ("foo:\n"
             "- bar\n"
             "-\n"
             "baz: qux\n"
             "foo2:\n"
             "- bar2\n"
             "- \n"
             "baz2: qux2\n",
             "foo:\n"
             "  - bar\n"
             "  - \n"
             "baz: qux\n"
             "foo2:\n"
             "  - bar2\n"
             "  - \n"
             "baz2: qux2\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "+SEQ\n"
            "=VAL :bar\n"
            "=VAL :\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "=VAL :qux\n"
            "=VAL :foo2\n"
            "+SEQ\n"
            "=VAL :bar2\n"
            "=VAL :\n"
            "-SEQ\n"
            "=VAL :baz2\n"
            "=VAL :qux2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_map_val_block();
    ps.set_key_scalar_plain("foo");
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain("bar");
    ps.add_sibling();
    ps.set_val_scalar_plain({});
    ps.end_seq();
    ps.add_sibling();
    ps.set_key_scalar_plain("baz");
    ps.set_val_scalar_plain("qux");
    ps.add_sibling();
    ps.set_key_scalar_plain("foo2");
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain("bar2");
    ps.add_sibling();
    ps.set_val_scalar_plain({});
    ps.end_seq();
    ps.add_sibling();
    ps.set_key_scalar_plain("baz2");
    ps.set_val_scalar_plain("qux2");
    ps.end_map();
    ps.end_doc();
    ps.end_stream();
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleMapContainerKeyFlow,
            (HAS_CONTAINER_KEYS,
             "{{this: is, a: keymap}: [and,now,a,seq,val]}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block0_0,
            (HAS_CONTAINER_KEYS,
             "{this: is, a: keymap}: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block0_1,
            (HAS_CONTAINER_KEYS,
             "{this: is, a: keymap}: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block1_0,
            (HAS_CONTAINER_KEYS,
             "[this,is,a,seq,key]: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :key\n"
            "-SEQ\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("this"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("key"));
    ___(ps.end_seq());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block1_1,
            (HAS_CONTAINER_KEYS,
             "[this,is,a,seq,key]: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :key\n"
            "-SEQ\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("this"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("key"));
    ___(ps.end_seq());
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block2_0,
            (HAS_CONTAINER_KEYS,
             "{this: is, a: keymap}: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block2_1,
            (HAS_CONTAINER_KEYS,
             "{this: is, a: keymap}: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block3_0,
            (HAS_CONTAINER_KEYS,
             "---\n"
             "{a: map}: [a,seq]\n"
             "---\n"
             "[A,SEQ]: {A: MAP}\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :map\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :A\n"
            "=VAL :SEQ\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :A\n"
            "=VAL :MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("map"));
    ___(ps.end_map());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("A"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("SEQ"));
    ___(ps.end_seq());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("A"));
    ___(ps.set_val_scalar_plain("MAP"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block3_1,
            (HAS_CONTAINER_KEYS,
             "---\n"
             "{a: map}: [a,seq]\n"
             "---\n"
             "[A,SEQ]: {A: MAP}\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :map\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :A\n"
            "=VAL :SEQ\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :A\n"
            "=VAL :MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("map"));
    ___(ps.end_map());
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("A"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("SEQ"));
    ___(ps.end_seq());
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("A"));
    ___(ps.set_val_scalar_plain("MAP"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


// the examples above have the starting '[' / '{' at the beginning,
// where it is parsed in UNK state. This one has those tokens already
// in RMAP|BLCK|RKEY state, ie, they don't come first.
ENGINE_TEST(SimpleMapContainerKey2Block_1,
            (HAS_CONTAINER_KEYS,
             "\n"
             "foo: bar\n"
             "!maptag &mapanchor {this: is, a: keymap}: [and,now,a,seq,val]\n"
             "!seqtag &seqanchor [now, reversed]: {of: course}\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "+MAP {} &mapanchor <!maptag>\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "+SEQ [] &seqanchor <!seqtag>\n"
            "=VAL :now\n"
            "=VAL :reversed\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :of\n"
            "=VAL :course\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!maptag"));
    ___(ps.set_key_anchor("mapanchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!seqtag"));
    ___(ps.set_key_anchor("seqanchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("reversed"));
    ___(ps.end_seq());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("of"));
    ___(ps.set_val_scalar_plain("course"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//-----------------------------------------------------------------------------

ENGINE_TEST(MapMapFlow,
            ("{map1: {foo1: bar1,FOO1: BAR1},map2: {foo2: bar2,FOO2: BAR2}}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :map1\n"
            "+MAP {}\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :FOO1\n"
            "=VAL :BAR1\n"
            "-MAP\n"
            "=VAL :map2\n"
            "+MAP {}\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :FOO2\n"
            "=VAL :BAR2\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("map1"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo1"));
    ___(ps.set_val_scalar_plain("bar1"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("FOO1"));
    ___(ps.set_val_scalar_plain("BAR1"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("map2"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("FOO2"));
    ___(ps.set_val_scalar_plain("BAR2"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(MapMapBlock,
            ("map1:\n"
             "  foo1: bar1\n"
             "  FOO1: BAR1\n"
             "map2:\n"
             "  foo2: bar2\n"
             "  FOO2: BAR2\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map1\n"
            "+MAP\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :FOO1\n"
            "=VAL :BAR1\n"
            "-MAP\n"
            "=VAL :map2\n"
            "+MAP\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :FOO2\n"
            "=VAL :BAR2\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
      ___(ps.set_key_scalar_plain("map1"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo1"));
        ___(ps.set_val_scalar_plain("bar1"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("FOO1"));
        ___(ps.set_val_scalar_plain("BAR1"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo2"));
        ___(ps.set_val_scalar_plain("bar2"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("FOO2"));
        ___(ps.set_val_scalar_plain("BAR2"));
      ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapMapMapBlock,
            ("map0:\n"
             "  map01:\n"
             "    foo01: bar01\n"
             "    FOO01: BAR01\n"
             "  map02:\n"
             "    foo02: bar02\n"
             "    FOO02: BAR02\n"
             "    child02:\n"
             "      foo020: bar020\n"
             "      foo021: bar021\n"
             "map1:\n"
             "  map11:\n"
             "    foo11: bar11\n"
             "    FOO11: BAR11\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map0\n"
            "+MAP\n"
            "=VAL :map01\n"
            "+MAP\n"
            "=VAL :foo01\n"
            "=VAL :bar01\n"
            "=VAL :FOO01\n"
            "=VAL :BAR01\n"
            "-MAP\n"
            "=VAL :map02\n"
            "+MAP\n"
            "=VAL :foo02\n"
            "=VAL :bar02\n"
            "=VAL :FOO02\n"
            "=VAL :BAR02\n"
            "=VAL :child02\n"
            "+MAP\n"
            "=VAL :foo020\n"
            "=VAL :bar020\n"
            "=VAL :foo021\n"
            "=VAL :bar021\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "=VAL :map1\n"
            "+MAP\n"
            "=VAL :map11\n"
            "+MAP\n"
            "=VAL :foo11\n"
            "=VAL :bar11\n"
            "=VAL :FOO11\n"
            "=VAL :BAR11\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
      ___(ps.set_key_scalar_plain("map0"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("map01"));
        ___(ps.begin_map_val_block());
          ___(ps.set_key_scalar_plain("foo01"));
          ___(ps.set_val_scalar_plain("bar01"));
          ___(ps.add_sibling());
          ___(ps.set_key_scalar_plain("FOO01"));
          ___(ps.set_val_scalar_plain("BAR01"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("map02"));
        ___(ps.begin_map_val_block());
          ___(ps.set_key_scalar_plain("foo02"));
          ___(ps.set_val_scalar_plain("bar02"));
          ___(ps.add_sibling());
          ___(ps.set_key_scalar_plain("FOO02"));
          ___(ps.set_val_scalar_plain("BAR02"));
          ___(ps.add_sibling());
          ___(ps.set_key_scalar_plain("child02"));
          ___(ps.begin_map_val_block());
            ___(ps.set_key_scalar_plain("foo020"));
            ___(ps.set_val_scalar_plain("bar020"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("foo021"));
            ___(ps.set_val_scalar_plain("bar021"));
          ___(ps.end_map());
        ___(ps.end_map());
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("map1"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("map11"));
        ___(ps.begin_map_val_block());
          ___(ps.set_key_scalar_plain("foo11"));
          ___(ps.set_val_scalar_plain("bar11"));
          ___(ps.add_sibling());
          ___(ps.set_key_scalar_plain("FOO11"));
          ___(ps.set_val_scalar_plain("BAR11"));
        ___(ps.end_map());
      ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(MapKeyFlow,
            (HAS_CONTAINER_KEYS, Location(1, 1, 2), "{{foo: bar}: baz}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock,
            (HAS_CONTAINER_KEYS, Location(6,1,7), "? foo: bar\n: baz"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlockFlow,
            (HAS_CONTAINER_KEYS, Location(2,1,3), "? {foo: bar}\n: baz"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyFlow,
            (HAS_CONTAINER_KEYS, Location(1,1,2), "{[foo, bar]: baz}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlock,
            (HAS_CONTAINER_KEYS, Location(3,2,2),
             "?\n"
             " - foo\n"
             " - bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlockFlow,
            (HAS_CONTAINER_KEYS, Location(2,1,3),
             "? [foo, bar]\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlock2,
            (HAS_CONTAINER_KEYS, Location(2,2,1),
             "?\n"
             "- foo\n"
             "- bar\n"
             ":\n"
             "- baz\n"
             "- bat\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :baz\n"
            "=VAL :bat\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlock3,
            (HAS_CONTAINER_KEYS, Location(3,2,2),
             "?\n"
             " - foo\n"
             " - bar\n"
             ":\n"
             " - baz\n"
             " - bat\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :baz\n"
            "=VAL :bat\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Squo0,
            (HAS_CONTAINER_KEYS,
             "\n"
             "? 'foo' : bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL 'foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_squoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Squo1,
            (HAS_CONTAINER_KEYS,
             "&blockanchor\n"
             "? &mapkey\n"
             "  &scalarkey 'foo' : bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &blockanchor\n"
            "+MAP &mapkey\n"
            "=VAL &scalarkey 'foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("blockanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapkey"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("scalarkey"));
    ___(ps.set_key_scalar_squoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Dquo0,
            (HAS_CONTAINER_KEYS,
             "\n"
             "? \"foo\" : bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL \"foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_dquoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Dquo1,
            (HAS_CONTAINER_KEYS,
             "&blockanchor\n"
             "? &mapkey\n"
             "  &scalarkey \"foo\" : bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &blockanchor\n"
            "+MAP &mapkey\n"
            "=VAL &scalarkey \"foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("blockanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapkey"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("scalarkey"));
    ___(ps.set_key_scalar_dquoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Ref0,
            ("\n"
             "? *ref\n"
             ": baz\n",
             "*ref : baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=ALI *ref\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Ref1,
            (HAS_CONTAINER_KEYS,
             "\n"
             "? &mapanchor\n"
             "  *ref : bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP &mapanchor\n"
            "=ALI *ref\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapanchor"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// this is needed to use the test case library

#ifndef RYML_SINGLE_HEADER
#include "c4/substr.hpp"
#endif

namespace c4 {
namespace yml {
struct Case;
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}
} // namespace yml
} // namespace c4
