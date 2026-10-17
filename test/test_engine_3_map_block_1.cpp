#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

static constexpr const bool singleline = false;


//-----------------------------------------------------------------------------

ENGINE_TEST(Block,
            "foo: bar\n"
            "foo2: bar2\n"
            "foo3: bar3\n"
            ,
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
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(BlockEmptyFlowMap,
            "foo: {}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "+MAP {}\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(BlockEmptyFlowSeq,
            "foo: []\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(BlockEmptyVals,
            "a:\n"
            "b:\n"
            "c:\n"
            "d:\n"
            ,
            "a: \n"
            "b: \n"
            "c: \n"
            "d: \n"
            ,
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
    ps.set_val_scalar_plain_empty();
    ps.add_sibling();
    ps.set_key_scalar_plain("b");
    ps.set_val_scalar_plain_empty();
    ps.add_sibling();
    ps.set_key_scalar_plain("c");
    ps.set_val_scalar_plain_empty();
    ps.add_sibling();
    ps.set_key_scalar_plain("d");
    ps.set_val_scalar_plain_empty();
    ps.end_map_block();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(BlockEmptyKeys,
            ": a\n: b\n: c\n: d\n"
            ,
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
    ps.set_key_scalar_plain_empty();
    ps.set_val_scalar_plain("a");
    ps.add_sibling();
    ps.set_key_scalar_plain_empty();
    ps.set_val_scalar_plain("b");
    ps.add_sibling();
    ps.set_key_scalar_plain_empty();
    ps.set_val_scalar_plain("c");
    ps.add_sibling();
    ps.set_key_scalar_plain_empty();
    ps.set_val_scalar_plain("d");
    ps.end_map_block();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(BlockEmpty,
            ":\n:\n:\n:\n"
            ,
            ": \n: \n: \n: \n"
            ,
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
    ps.set_key_scalar_plain_empty();
    ps.set_val_scalar_plain_empty();
    ps.add_sibling();
    ps.set_key_scalar_plain_empty();
    ps.set_val_scalar_plain_empty();
    ps.add_sibling();
    ps.set_key_scalar_plain_empty();
    ps.set_val_scalar_plain_empty();
    ps.add_sibling();
    ps.set_key_scalar_plain_empty();
    ps.set_val_scalar_plain_empty();
    ps.end_map_block();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(IndentlessSeq,
            "foo:\n"
            "- bar\n"
            "-\n"
            "baz: qux\n"
            "foo2:\n"
            "- bar2\n"
            "- \n"
            "baz2: qux2\n"
            ,
            "foo:\n"
            "  - bar\n"
            "  - \n"
            "baz: qux\n"
            "foo2:\n"
            "  - bar2\n"
            "  - \n"
            "baz2: qux2\n"
            ,
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
    ps.set_val_scalar_plain_empty();
    ps.end_seq_block();
    ps.add_sibling();
    ps.set_key_scalar_plain("baz");
    ps.set_val_scalar_plain("qux");
    ps.add_sibling();
    ps.set_key_scalar_plain("foo2");
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain("bar2");
    ps.add_sibling();
    ps.set_val_scalar_plain_empty();
    ps.end_seq_block();
    ps.add_sibling();
    ps.set_key_scalar_plain("baz2");
    ps.set_val_scalar_plain("qux2");
    ps.end_map_block();
    ps.end_doc();
    ps.end_stream();
}


//-----------------------------------------------------------------------------

ENGINE_TEST(MapMapBlock,
            "map1:\n"
            "  foo1: bar1\n"
            "  FOO1: BAR1\n"
            "map2:\n"
            "  foo2: bar2\n"
            "  FOO2: BAR2\n"
            ,
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
      ___(ps.end_map_block());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo2"));
        ___(ps.set_val_scalar_plain("bar2"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("FOO2"));
        ___(ps.set_val_scalar_plain("BAR2"));
      ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapMapMapBlock,
            "map0:\n"
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
            "    FOO11: BAR11\n"
            ,
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
        ___(ps.end_map_block());
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
          ___(ps.end_map_block());
        ___(ps.end_map_block());
      ___(ps.end_map_block());
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
        ___(ps.end_map_block());
      ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(ExtraTokensNoFalseError0,
            "---\n"
            "map : {foo: bar, notag: none}\n"
            "seq : [foo, bar]\n"
            "...\n"
            ,
            "---\n"
            "map: {foo: bar,notag: none}\n"
            "seq: [foo,bar]\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :notag\n"
            "=VAL :none\n"
            "-MAP\n"
            "=VAL :seq\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("map"));
        ___(ps.begin_map_val_flow());
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("notag"));
            ___(ps.set_val_scalar_plain("none"));
        ___(ps.end_map_flow(singleline));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("seq"));
        ___(ps.begin_seq_val_flow());
            ___(ps.set_val_scalar_plain("foo"));
            ___(ps.add_sibling());
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}

ENGINE_TEST(ExtraTokensNoFalseError1,
            "---\n"
            "*mapref : {foo: bar, notag: none}\n"
            "*seqref : [foo, bar]\n"
            "...\n"
            ,
            "---\n"
            "*mapref : {foo: bar,notag: none}\n"
            "*seqref : [foo,bar]\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "=ALI *mapref\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :notag\n"
            "=VAL :none\n"
            "-MAP\n"
            "=ALI *seqref\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
        ___(ps.set_key_ref("*mapref"));
        ___(ps.begin_map_val_flow());
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("notag"));
            ___(ps.set_val_scalar_plain("none"));
        ___(ps.end_map_flow(singleline));
        ___(ps.add_sibling());
        ___(ps.set_key_ref("*seqref"));
        ___(ps.begin_seq_val_flow());
            ___(ps.set_val_scalar_plain("foo"));
            ___(ps.add_sibling());
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}

//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(colon_on_newl_after_first_plain_0_0,
                   Location(2, 1),
                   "foo\n"
                   ":\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_plain_0_1,
                   Location(2, 3),
                   "foo\n"
                   "  :\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_plain_0_2,
                   Location(2, 1),
                   "foo\n"
                   ": bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_plain_0_3,
                   Location(2, 2),
                   "foo\n"
                   " : bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_plain_0_4,
                   Location(2, 3),
                   "foo\n"
                   "  : bar")

ENGINE_TEST_ERRLOC(colon_on_newl_after_second_plain_0_0,
                   Location(2, 4),
                   "foo: bar\n"
                   "bad\n"
                   ": yes\n")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_plain_0_1,
                   Location(3, 2),
                   "foo: bar\n"
                   "bad\n"
                   " : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_plain_0_2,
                   Location(3, 3),
                   "foo: bar\n"
                   "bad\n"
                   "  : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_plain_0_3,
                   Location(3, 4),
                   "foo: bar\n"
                   "bad\n"
                   "   : yes")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(colon_on_newl_after_first_squo_0_0,
                   Location(2, 1),
                   "'foo'\n"
                   ":\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_squo_0_1,
                   Location(2, 3),
                   "'foo'\n"
                   "  :\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_squo_0_2,
                   Location(2, 1),
                   "'foo'\n"
                   ": bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_squo_0_3,
                   Location(2, 2),
                   "'foo'\n"
                   " : bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_squo_0_4,
                   Location(2, 3),
                   "'foo'\n"
                   "  : bar")

ENGINE_TEST_ERRLOC(colon_on_newl_after_second_squo_0_0,
                   Location(2, 6),
                   "'foo': bar\n"
                   "'bad'\n"
                   ": yes\n")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_squo_0_1,
                   Location(2, 6),
                   "'foo': bar\n"
                   "'bad'\n"
                   " : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_squo_0_2,
                   Location(2, 6),
                   "'foo': bar\n"
                   "'bad'\n"
                   "  : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_squo_0_3,
                   Location(2, 6),
                   "'foo': bar\n"
                   "'bad'\n"
                   "   : yes")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(colon_on_newl_after_first_dquo_0_0,
                   Location(2, 1),
                   "\"foo\"\n"
                   ":\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_dquo_0_1,
                   Location(2, 3),
                   "\"foo\"\n"
                   "  :\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_dquo_0_2,
                   Location(2, 1),
                   "\"foo\"\n"
                   ": bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_dquo_0_3,
                   Location(2, 2),
                   "\"foo\"\n"
                   " : bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_dquo_0_4,
                   Location(2, 3),
                   "\"foo\"\n"
                   "  : bar")

ENGINE_TEST_ERRLOC(colon_on_newl_after_second_dquo_0_0,
                   Location(2, 6),
                   "\"foo\": bar\n"
                   "\"bad\"\n"
                   ": yes\n")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_dquo_0_1,
                   Location(2, 6),
                   "\"foo\": bar\n"
                   "\"bad\"\n"
                   " : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_dquo_0_2,
                   Location(2, 6),
                   "\"foo\": bar\n"
                   "\"bad\"\n"
                   "  : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_dquo_0_3,
                   Location(2, 6),
                   "\"foo\": bar\n"
                   "\"bad\"\n"
                   "   : yes")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_seq_0_0, HAS_CONTAINER_KEYS,
                    Location(3, 6),
                    "? [seq]\n"
                    ": bar\n"
                    "[bad]\n"
                    ": yes")
ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_seq_0_1, HAS_CONTAINER_KEYS,
                    Location(3, 6),
                    "? [seq]\n"
                    ": bar\n"
                    "[bad]\n"
                    " : yes")
ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_seq_0_2, HAS_CONTAINER_KEYS,
                    Location(3, 6),
                    "? [seq]\n"
                    ": bar\n"
                    "[bad]\n"
                    "  : yes")

ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_map_0_0, HAS_CONTAINER_KEYS,
                    Location(3, 11),
                    "? {foo: bar}\n"
                    ": bar\n"
                    "{foo: bar}\n"
                    ": yes")
ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_map_0_1, HAS_CONTAINER_KEYS,
                    Location(3, 11),
                    "? {foo: bar}\n"
                    ": bar\n"
                    "{foo: bar}\n"
                    " : yes")
ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_map_0_2, HAS_CONTAINER_KEYS,
                    Location(3, 11),
                    "? {foo: bar}\n"
                    ": bar\n"
                    "{foo: bar}\n"
                    "  : yes")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(BlockSameLine0Err, Location(5+1,1,6+1), "a: b: c")
ENGINE_TEST_ERRLOC(BlockSameLine1Err, Location(5+1,1,6+1), "a: b: ")
ENGINE_TEST_ERRLOC(BlockSameLine2Err, Location(5,1,6), "a: b:")
ENGINE_TEST_ERRLOC(BlockSameLine3Err, Location(2,1,3), ": : :")
ENGINE_TEST_ERRLOC(BlockSameLine4Err, Location(2,1,3), ": : : :")
ENGINE_TEST_ERRLOC(BlockSameLine5Err, Location(9+1,1,10+1), "'a': 'b': 'c'")
ENGINE_TEST_ERRLOC(BlockSameLine6Err, Location(9+1,1,10+1), "\"a\": \"b\": \"c\"")
ENGINE_TEST(BlockSameLine7, HAS_MULTILINE_SCALAR,
            ""
            "? |-\n"
            " a\n"
            ": b: c\n"
            ""
            ,
            ""
            "? |-\n"
            "  a\n"
            ":\n"
            "  b: c\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL |a\n"
            "+MAP\n"
            "=VAL :b\n"
            "=VAL :c\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_literal("a"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST_ERRLOC(ErrLiteralKey, Location(9,2,1),
                   "foo: bar\n"
                   "| literal: val\n"
    )
ENGINE_TEST_ERRLOC(ErrFoldedKey, Location(9,2,1),
                   "foo: bar\n"
                   "> folded: val\n"
    )

//-----------------------------------------------------------------------------

ENGINE_TEST(BlockValDash0,
            "a:\n"
            " -\n"
            ,
            "a:\n"
            "  - \n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "+SEQ\n"
            "=VAL :\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(BlockValDash1,
            "a:\n"
            "-\n"
            ,
            "a:\n"
            "  - \n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "+SEQ\n"
            "=VAL :\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(BlockValDash2,
            "a:\n"
            " --\n"
            ,
            "a: --\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :--\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("--"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC(BlockValDash3, Location(3,1),
                   "a:\n"
                   "--\n"
    )
ENGINE_TEST(BlockValDash4,
            "a:\n"
            " ---\n"
            ,
            "a:   ---\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :---\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("---"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(BlockValDash5,
            "a:\n"
            "---\n"
            ,
            "---\n"
            "a: \n"
            "---\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(IndentationMore0,
            "a:\n"
            "  b:\n"
            ""
            ,
            "a:\n"
            "  b: \n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "+MAP\n"
            "=VAL :b\n"
            "=VAL :\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(IndentationMore0Empty,
            "a:\n"
            "  : c\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :c\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(IndentationJump0DocStartLevel0_rkey,
            "a: b\n"
            "--- c\n"
            ""
            ,
            "---\n"
            "a: b\n"
            "--- c\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :c\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(IndentationJump0DocStartLevel0_rval,
            "a:\n"
            "--- c\n"
            ""
            ,
            "---\n"
            "a: \n"
            "--- c\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :c\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(IndentationJump0DocEndLevel0_rkey,
            "a: b\n"
            "...\n"
            ,
            "a: b\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}
ENGINE_TEST(IndentationJump0DocEndLevel0_rval,
            "a:\n"
            "...\n"
            ""
            ,
            "a: \n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(IndentationJump0DocStartLevel1_rkey,
            "map:\n"
            "  a: b\n"
            "--- c\n"
            ""
            ,
            "---\n"
            "map:\n"
            "  a: b\n"
            "--- c\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :c\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(IndentationJump0DocStartLevel1_rval,
            "map:\n"
            "  a:\n"
            "--- c\n"
            ""
            ,
            "---\n"
            "map:\n"
            "  a: \n"
            "--- c\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :c\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(IndentationJump0DocEndLevel1_rkey,
            "map:\n"
            "  a: b\n"
            "...\n"
            ""
            ,
            "map:\n"
            "  a: b\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}
ENGINE_TEST(IndentationJump0DocEndLevel1_rval,
            "map:\n"
            "  a:\n"
            "...\n"
            ""
            ,
            "map:\n"
            "  a: \n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
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
