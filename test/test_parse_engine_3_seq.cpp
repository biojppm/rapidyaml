#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

namespace c4 {
namespace yml {


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleSeqFlow,
            ("[foo,bar,baz]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST_ERRLOC(SimpleSeqFlowErr0, Location(1,1,2), "[")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr1, Location(2,1,3), "[[")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr2, Location(3,1,4), "[[]")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr3, Location(5,1,6), "[[[]]")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr4, Location(4,1,5), "[foo")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr5, Location(5,1,6), "[foo,")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr6, Location(12,1,13), "[foo,bar,baz")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr7, Location(8,1,9), "[[a,b,c]")


ENGINE_TEST(NestedSeqFlowEmpty,
            ("[[]]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedSeq3FlowEmpty,
            ("[[[]]]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedSeq4FlowEmpty,
            ("[[[[]]]]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleSeqFlowMultiline,
            ("[\nfoo\n,\nbar\n,\nbaz\n]", "[foo,bar,baz]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleSeqBlock,
            ("- foo\n- bar\n- baz\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleSeqBlock2,
            ("- [foo, bar, baz]\n- {foo: bar,baz}\n",
             "- [foo,bar,baz]\n- {foo: bar,baz: }\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
      ___(ps.begin_seq_val_flow());
        ___(ps.set_val_scalar_plain("foo"));
        ___(ps.add_sibling());
        ___(ps.set_val_scalar_plain("bar"));
        ___(ps.add_sibling());
        ___(ps.set_val_scalar_plain("baz"));
      ___(ps.end_seq());
      ___(ps.add_sibling());
      ___(ps.begin_map_val_flow());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("bar"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("baz"));
        ___(ps.set_val_scalar_plain({}));
      ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(SimpleSeqBlockPlainScalar0,
            (HAS_MULTILINE_SCALAR,
             "- a\n"
             "  b\n"
             ,
             "- a b\n")
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a b\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain("a b");
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockPlainScalar1,
            (HAS_MULTILINE_SCALAR,
             "- a\n"
             "  - b - c\n",
             "- a - b - c\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a - b - c\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain("a - b - c");
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockComment0,
            ("-   &wtf\n"
             "   # this is a comment\n"
             "  foo\n"
             "\n",
             "- &wtf foo\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL &wtf :foo\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_anchor("wtf");
    ps.set_val_scalar_plain("foo");
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockComment1,
            ("-   &wtf\n"
             "  foo\n"
             "   # this is a comment\n"
             "\n", "- &wtf foo\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL &wtf :foo\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_anchor("wtf");
    ps.set_val_scalar_plain("foo");
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockEmptyScalars,
            ("-\n-\n-\n-\n", "- \n- \n- \n- \n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_val_scalar_plain({});
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockEmptyLiterals,
            (HAS_MULTILINE_SCALAR,
             "- |\n- |\n- |\n- |\n",
             "- |-\n- |-\n- |-\n- |-\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL |\n"
            "=VAL |\n"
            "=VAL |\n"
            "=VAL |\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_scalar_literal({});
    ps.add_sibling();
    ps.set_val_scalar_literal({});
    ps.add_sibling();
    ps.set_val_scalar_literal({});
    ps.add_sibling();
    ps.set_val_scalar_literal({});
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockEmptyFolded,
            (HAS_MULTILINE_SCALAR,
             "- >\n- >\n- >\n- >\n",
             "- >-\n- >-\n- >-\n- >-\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL >\n"
            "=VAL >\n"
            "=VAL >\n"
            "=VAL >\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_scalar_folded({});
    ps.add_sibling();
    ps.set_val_scalar_folded({});
    ps.add_sibling();
    ps.set_val_scalar_folded({});
    ps.add_sibling();
    ps.set_val_scalar_folded({});
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

//-----------------------------------------------------------------------------

ENGINE_TEST(SeqSeqFlow,
            ("[[foo1,bar1,baz1],\n[foo2,bar2,baz2]]", "[[foo1,bar1,baz1],[foo2,bar2,baz2]]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :baz1\n"
            "-SEQ\n"
            "+SEQ []\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :baz2\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz1"));
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz2"));
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SeqSeqBlock,
            ("- - foo1\n"
             "  - bar1\n"
             "  - baz1\n"
             "- - foo2\n"
             "  - bar2\n"
             "  - baz2\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+SEQ\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :baz1\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :baz2\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("foo1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz1"));
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("foo2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz2"));
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqSeqSeqBlock,
            ("- - - foo1\n"
             "    - bar1\n"
             "    - baz1\n"
             "  - - foo2\n"
             "    - bar2\n"
             "    - baz2\n"
             "- back\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+SEQ\n"
            "+SEQ\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :baz1\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :baz2\n"
            "-SEQ\n"
            "-SEQ\n"
            "=VAL :back\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
      ___(ps.begin_seq_val_block());
        ___(ps.begin_seq_val_block());
          ___(ps.set_val_scalar_plain("foo1"));
          ___(ps.add_sibling());
          ___(ps.set_val_scalar_plain("bar1"));
          ___(ps.add_sibling());
          ___(ps.set_val_scalar_plain("baz1"));
        ___(ps.end_seq());
        ___(ps.add_sibling());
        ___(ps.begin_seq_val_block());
          ___(ps.set_val_scalar_plain("foo2"));
          ___(ps.add_sibling());
          ___(ps.set_val_scalar_plain("bar2"));
          ___(ps.add_sibling());
          ___(ps.set_val_scalar_plain("baz2"));
          ___(ps.end_seq());
        ___(ps.end_seq());
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("back"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqMapBlock,
            ("- foo: 1\n"
             "  bar: 2\n"
             "- foo: 10\n"
             "  bar: 20\n"
             "- foo: 100\n"
             "  bar: 200\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :1\n"
            "=VAL :bar\n"
            "=VAL :2\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :10\n"
            "=VAL :bar\n"
            "=VAL :20\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :100\n"
            "=VAL :bar\n"
            "=VAL :200\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("1"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("bar"));
        ___(ps.set_val_scalar_plain("2"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("10"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("bar"));
        ___(ps.set_val_scalar_plain("20"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("100"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("bar"));
        ___(ps.set_val_scalar_plain("200"));
      ___(ps.end_map());
    ___(ps.end_seq());
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
