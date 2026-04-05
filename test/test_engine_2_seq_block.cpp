#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


static constexpr const bool singleline = false;


//-----------------------------------------------------------------------------

ENGINE_TEST(Simple,
            "- foo\n- bar\n- baz\n"
            ,
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
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Simple2,
            "- [foo, bar, baz]\n- {foo: bar,baz}\n"
            ,
            "- [foo,bar,baz]\n- {foo: bar,baz: }\n"
            ,
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
      ___(ps.end_seq_flow(singleline));
      ___(ps.add_sibling());
      ___(ps.begin_map_val_flow());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("bar"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("baz"));
        ___(ps.set_val_scalar_plain_empty());
      ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(BlockPlainScalar0,
            HAS_MULTILINE_SCALAR,
            "- a\n"
            "  b\n"
            ,
            "- a b\n"
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
    ps.end_seq_block();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(BlockPlainScalar1,
            HAS_MULTILINE_SCALAR,
             "- a\n"
             "  - b - c\n"
            ,
             "- a - b - c\n"
            ,
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
    ps.end_seq_block();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(BlockComment0,
            "-   &wtf\n"
            "   # this is a comment\n"
            "  foo\n"
            "\n"
            ,
            "- &wtf foo\n"
            ,
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
    ps.end_seq_block();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(BlockComment1,
            "-   &wtf\n"
            "  foo\n"
            "   # this is a comment\n"
            "\n"
            ,
            "- &wtf foo\n"
            ,
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
    ps.end_seq_block();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(BlockEmptyScalars,
            "-\n-\n-\n-\n"
            ,
            "- \n- \n- \n- \n"
            ,
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
    ps.set_val_scalar_plain_empty();
    ps.add_sibling();
    ps.set_val_scalar_plain_empty();
    ps.add_sibling();
    ps.set_val_scalar_plain_empty();
    ps.add_sibling();
    ps.set_val_scalar_plain_empty();
    ps.end_seq_block();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(BlockEmptyLiterals,
            HAS_MULTILINE_SCALAR
            ,
            "- |\n- |\n- |\n- |\n"
            ,
            "- |-\n- |-\n- |-\n- |-\n"
            ,
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
    ps.end_seq_block();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(BlockEmptyFolded,
            HAS_MULTILINE_SCALAR,
            "- >\n- >\n- >\n- >\n"
            ,
            "- >-\n- >-\n- >-\n- >-\n"
            ,
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
    ps.end_seq_block();
    ps.end_doc();
    ps.end_stream();
}

//-----------------------------------------------------------------------------


ENGINE_TEST(SeqBlockSpace, HAS_MULTILINE_SCALAR,
            ""
            "- a\n"
            "  - b\n"
            ""
            ,
            ""
            "- a - b\n"
            ""
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a - b\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a - b"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(SeqBlockTab, Location(2, 1),
            ""
            "- a\n"
            "\t- b\n"
            "")


//-----------------------------------------------------------------------------

ENGINE_TEST(SeqSeqBlock,
            "- - foo1\n"
            "  - bar1\n"
            "  - baz1\n"
            "- - foo2\n"
            "  - bar2\n"
            "  - baz2\n"
            ,
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
    ___(ps.end_seq_block());
    ___(ps.add_sibling());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("foo2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz2"));
    ___(ps.end_seq_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqSeqSeqBlock,
            "- - - foo1\n"
            "    - bar1\n"
            "    - baz1\n"
            "  - - foo2\n"
            "    - bar2\n"
            "    - baz2\n"
            "- back\n"
            ,
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
        ___(ps.end_seq_block());
        ___(ps.add_sibling());
        ___(ps.begin_seq_val_block());
          ___(ps.set_val_scalar_plain("foo2"));
          ___(ps.add_sibling());
          ___(ps.set_val_scalar_plain("bar2"));
          ___(ps.add_sibling());
          ___(ps.set_val_scalar_plain("baz2"));
          ___(ps.end_seq_block());
        ___(ps.end_seq_block());
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("back"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqSeqSeqSeqBlock,
            "- - - - foo1\n"
            "      - bar1\n"
            "      - baz1\n"
            "  - - - foo2\n"
            "      - bar2\n"
            "      - baz2\n"
            "- back\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+SEQ\n"
            "+SEQ\n"
            "+SEQ\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :baz1\n"
            "-SEQ\n"
            "-SEQ\n"
            "+SEQ\n"
            "+SEQ\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :baz2\n"
            "-SEQ\n"
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
          ___(ps.begin_seq_val_block());
            ___(ps.set_val_scalar_plain("foo1"));
            ___(ps.add_sibling());
            ___(ps.set_val_scalar_plain("bar1"));
            ___(ps.add_sibling());
            ___(ps.set_val_scalar_plain("baz1"));
          ___(ps.end_seq_block());
        ___(ps.end_seq_block());
        ___(ps.add_sibling());
        ___(ps.begin_seq_val_block());
          ___(ps.begin_seq_val_block());
            ___(ps.set_val_scalar_plain("foo2"));
            ___(ps.add_sibling());
            ___(ps.set_val_scalar_plain("bar2"));
            ___(ps.add_sibling());
            ___(ps.set_val_scalar_plain("baz2"));
          ___(ps.end_seq_block());
        ___(ps.end_seq_block());
      ___(ps.end_seq_block());
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("back"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqMapBlock,
            "- foo: 1\n"
            "  bar: 2\n"
            "- foo: 10\n"
            "  bar: 20\n"
            "- foo: 100\n"
            "  bar: 200\n"
            ,
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
      ___(ps.end_map_block());
      ___(ps.add_sibling());
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("10"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("bar"));
        ___(ps.set_val_scalar_plain("20"));
      ___(ps.end_map_block());
      ___(ps.add_sibling());
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("100"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("bar"));
        ___(ps.set_val_scalar_plain("200"));
      ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(Deindent_err1_0,
                   Location(7, 3),
                   "a:"   "\n"
                   "- 0"  "\n"
                   "-"    "\n"
                   "b:"   "\n"
                   "- 0"  "\n"
                   "-"    "\n"
                   "0 #"    "\n"
                   )
ENGINE_TEST_ERRLOC(Deindent_err1_1,
                   Location(7, 1),
                   "a:"   "\n"
                   "- 0"  "\n"
                   "-"    "\n"
                   "b:"   "\n"
                   "- 0"  "\n"
                   "-"    "\n"
                   "\t0 #"  "\n"
                   )
ENGINE_TEST(Deindent_err1_2,
            "a:"   "\n"
            "- 0"  "\n"
            "-"    "\n"
            "b:"   "\n"
            "- 0"  "\n"
            "-"    "\n"
            " 0"   "\n"
            ,
            "a:"   "\n"
            "  - 0"  "\n"
            "  - "    "\n"
            "b:"   "\n"
            "  - 0"  "\n"
            "  - 0"  "\n"
            ,
            "+STR"    "\n"
            "+DOC"    "\n"
            "+MAP"    "\n"
            "=VAL :a" "\n"
            "+SEQ"    "\n"
            "=VAL :0" "\n"
            "=VAL :"  "\n"
            "-SEQ"    "\n"
            "=VAL :b" "\n"
            "+SEQ"    "\n"
            "=VAL :0" "\n"
            "=VAL :0" "\n"
            "-SEQ"    "\n"
            "-MAP"    "\n"
            "-DOC"    "\n"
            "-STR"    "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_block());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Deindent_err1_3,
            "a:"      "\n"
            "- 0"     "\n"
            "-"       "\n"
            "b:"      "\n"
            "- 0"     "\n"
            "-"       "\n"
            "0:"      "\n"
            ,
            "a:"      "\n"
            "  - 0"     "\n"
            "  - "       "\n"
            "b:"      "\n"
            "  - 0"     "\n"
            "  - "       "\n"
            "0: "      "\n"
            ,
            "+STR"     "\n"
            "+DOC"     "\n"
            "+MAP"     "\n"
            "=VAL :a"  "\n"
            "+SEQ"     "\n"
            "=VAL :0"  "\n"
            "=VAL :"   "\n"
            "-SEQ"     "\n"
            "=VAL :b"  "\n"
            "+SEQ"     "\n"
            "=VAL :0"  "\n"
            "=VAL :"   "\n"
            "-SEQ"     "\n"
            "=VAL :0"  "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "-DOC"     "\n"
            "-STR"     "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_block());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_block());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("0"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(Deindent_err2_0,
                   Location(8, 3),
                   "map:\n"
                   "  a:"   "\n"
                   "  - 0"  "\n"
                   "  -"    "\n"
                   "  b:"   "\n"
                   "  - 0"  "\n"
                   "  -"    "\n"
                   "0 #"    "\n"
                   )
ENGINE_TEST_ERRLOC(Deindent_err2_1,
                   Location(8, 1),
                   "map:"   "\n"
                   "  a:"   "\n"
                   "  - 0"  "\n"
                   "  -"    "\n"
                   "  b:"   "\n"
                   "  - 0"  "\n"
                   "  -"    "\n"
                   "\t0 #"  "\n"
                   )
ENGINE_TEST_ERRLOC(Deindent_err2_1_0,
                   Location(8, 1),
                   "map:"   "\n"
                   "  a:"   "\n"
                   "  - 0"  "\n"
                   "  -"    "\n"
                   "  b:"   "\n"
                   "  - 0"  "\n"
                   "  -"    "\n"
                   " 0 #"  "\n"
                   )
ENGINE_TEST_ERRLOC(Deindent_err2_1_1,
                   Location(8, 5),
                   "map:"   "\n"
                   "  a:"   "\n"
                   "  - 0"  "\n"
                   "  -"    "\n"
                   "  b:"   "\n"
                   "  - 0"  "\n"
                   "  -"    "\n"
                   "  0 #"  "\n"
                   )
ENGINE_TEST(Deindent_err2_2,
            "map:"   "\n"
            "  a:"   "\n"
            "  - 0"  "\n"
            "  -"    "\n"
            "  b:"   "\n"
            "  - 0"  "\n"
            "  -"    "\n"
            "   0 #" "\n"
            ,
            "map:"   "\n"
            "  a:"   "\n"
            "    - 0"  "\n"
            "    - "    "\n"
            "  b:"   "\n"
            "    - 0"  "\n"
            "    - 0"    "\n"
            ,
            "+STR"    "\n"
            "+DOC"    "\n"
            "+MAP"    "\n"
            "=VAL :map" "\n"
            "+MAP"    "\n"
            "=VAL :a" "\n"
            "+SEQ"    "\n"
            "=VAL :0" "\n"
            "=VAL :"  "\n"
            "-SEQ"    "\n"
            "=VAL :b" "\n"
            "+SEQ"    "\n"
            "=VAL :0" "\n"
            "=VAL :0" "\n"
            "-SEQ"    "\n"
            "-MAP"    "\n"
            "-MAP"    "\n"
            "-DOC"    "\n"
            "-STR"    "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_block());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Deindent_err2_3,
            "map:"      "\n"
            "  a:"      "\n"
            "  - 0"     "\n"
            "  -"       "\n"
            "  b:"      "\n"
            "  - 0"     "\n"
            "  -"       "\n"
            "0:"        "\n"
            ,
            "map:"      "\n"
            "  a:"      "\n"
            "    - 0"     "\n"
            "    - "       "\n"
            "  b:"      "\n"
            "    - 0"     "\n"
            "    - "       "\n"
            "0: "        "\n"
            ,
            "+STR"     "\n"
            "+DOC"     "\n"
            "+MAP"     "\n"
            "=VAL :map""\n"
            "+MAP"     "\n"
            "=VAL :a"  "\n"
            "+SEQ"     "\n"
            "=VAL :0"  "\n"
            "=VAL :"   "\n"
            "-SEQ"     "\n"
            "=VAL :b"  "\n"
            "+SEQ"     "\n"
            "=VAL :0"  "\n"
            "=VAL :"   "\n"
            "-SEQ"     "\n"
            "-MAP"     "\n"
            "=VAL :0"  "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "-DOC"     "\n"
            "-STR"     "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_block());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("0"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(RvalWhitespace0,
            "-     a: b\n"
            "      c: d\n"
            "- \t  a\n"
            ,
            "- a: b\n"
            "  c: d\n"
            "- a\n"
            ,
            "+STR"     "\n"
            "+DOC"     "\n"
            "+SEQ"     "\n"
            "+MAP"     "\n"
            "=VAL :a"  "\n"
            "=VAL :b"  "\n"
            "=VAL :c"  "\n"
            "=VAL :d"  "\n"
            "-MAP"     "\n"
            "=VAL :a"  "\n"
            "-SEQ"     "\n"
            "-DOC"     "\n"
            "-STR"     "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
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
