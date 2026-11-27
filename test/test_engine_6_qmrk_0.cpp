#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


//static constexpr const bool multiline = true;
static constexpr const bool singleline = false;

ENGINE_TEST(Qmrk0,
            HAS_CONTAINER_KEYS, Location(41+1,3,19+1),
            "\n"
            "a simple key: a value\n"
            "? an explicit key: another value\n"
            ,
            ""
            "a simple key: a value\n"
            "?\n"
            "  an explicit key: another value\n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a simple key\n"
            "=VAL :a value\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :another value\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a simple key"));
    ___(ps.set_val_scalar_plain("a value"));
    ___(ps.add_sibling());
    ___(ps.begin_map_key_block());
        ___(ps.set_key_scalar_plain("an explicit key"));
        ___(ps.set_val_scalar_plain("another value"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk1_0,
            "? an explicit key\n"
            "a simple key: a value\n"
            ,
            "an explicit key: \n"
            "a simple key: a value\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :\n"
            "=VAL :a simple key\n"
            "=VAL :a value\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("an explicit key"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a simple key"));
    ___(ps.set_val_scalar_plain("a value"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk1_1,
            HAS_CONTAINER_KEYS, Location(19+1,2,19+1),
            "\n"
            "? an explicit key: another value\n"
            "a simple key: a value\n"
            ,
            ""
            "?\n"
            "  an explicit key: another value\n"
            ": \n"
            "a simple key: a value\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :another value\n"
            "-MAP\n"
            "=VAL :\n"
            "=VAL :a simple key\n"
            "=VAL :a value\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
        ___(ps.set_key_scalar_plain("an explicit key"));
        ___(ps.set_val_scalar_plain("another value"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a simple key"));
    ___(ps.set_val_scalar_plain("a value"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk1_2,
            HAS_CONTAINER_KEYS, Location(25+1,2,21+1),
            "map:\n"
            "  ? an explicit key: another value\n"
            "  a simple key: a value\n"
            "? an explicit key deindented: its value\n"
            "? more: siblings\n"
            ,
            "map:\n"
            "  ?\n"
            "    an explicit key: another value\n"
            "  : \n"
            "  a simple key: a value\n"
            "?\n"
            "  an explicit key deindented: its value\n"
            ": \n"
            "?\n"
            "  more: siblings\n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :another value\n"
            "-MAP\n"
            "=VAL :\n"
            "=VAL :a simple key\n"
            "=VAL :a value\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :an explicit key deindented\n"
            "=VAL :its value\n"
            "-MAP\n"
            "=VAL :\n"
            "+MAP\n"
            "=VAL :more\n"
            "=VAL :siblings\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("map"));
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain("an explicit key"));
                ___(ps.set_val_scalar_plain("another value"));
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("a simple key"));
            ___(ps.set_val_scalar_plain("a value"));
        ___(ps.end_map_block());
        ___(ps.add_sibling());
        ___(ps.begin_map_key_block());
            ___(ps.set_key_scalar_plain("an explicit key deindented"));
            ___(ps.set_val_scalar_plain("its value"));
        ___(ps.end_map_block());
        ___(ps.set_val_scalar_plain_empty());
        ___(ps.add_sibling());
        ___(ps.begin_map_key_block());
            ___(ps.set_key_scalar_plain("more"));
            ___(ps.set_val_scalar_plain("siblings"));
        ___(ps.end_map_block());
        ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk2_0, NO_COMPARE_EMITTED_INTS,
            "? an explicit key\n"
            "...\n"
            "? another explicit key\n"
            ,
            "---\n"
            "an explicit key: \n"
            "---\n"
            "another explicit key: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :another explicit key\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("an explicit key"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("another explicit key"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk2_1,
            "? an explicit key\n"
            "---\n"
            "? another explicit key\n"
            ,
            "---\n"
            "an explicit key: \n"
            "---\n"
            "another explicit key: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "=VAL :another explicit key\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("an explicit key"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("another explicit key"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk3,
            "{ ? an explicit key, ? foo,? bar,?baz:,?bat}"
            ,
             "{an explicit key: ,foo: ,bar: ,?baz: ,?bat: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :an explicit key\n"
            "=VAL :\n"
            "=VAL :foo\n"
            "=VAL :\n"
            "=VAL :bar\n"
            "=VAL :\n"
            "=VAL :?baz\n"
            "=VAL :\n"
            "=VAL :?bat\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("an explicit key"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("?baz"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("?bat"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk4_0,
            "[?baz:,]"
            ,
            "[{?baz: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :?baz\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("?baz"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk4,
            "[ ? an explicit key, ? foo,? bar,?baz:,?bat]"
            ,
            "[{an explicit key: },{foo: },{bar: },{?baz: },?bat]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :an explicit key\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :?baz\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :?bat\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("an explicit key"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("?baz"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("?bat"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk5,
            HAS_CONTAINER_KEYS, Location(4+1,1,5+1),
            "? a: b\n"
            "?\n"
            "?\n"
            ,
            "?\n"
            "  a: b\n"
            ": \n"
            ": \n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(Qmrk6,
            HAS_CONTAINER_KEYS, Location(21+1,2,21+1),
            "\n"
            "- ? an explicit key: another value\n"
            "  a simple key: a value\n"
            "- ? another explicit key: its value\n"
            ,
            "- ?\n"
            "    an explicit key: another value\n"
            "  : \n"
            "  a simple key: a value\n"
            "- ?\n"
            "    another explicit key: its value\n"
            "  : \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :another value\n"
            "-MAP\n"
            "=VAL :\n"
            "=VAL :a simple key\n"
            "=VAL :a value\n"
            "-MAP\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :another explicit key\n"
            "=VAL :its value\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain("an explicit key"));
                ___(ps.set_val_scalar_plain("another value"));
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("a simple key"));
            ___(ps.set_val_scalar_plain("a value"));
        ___(ps.end_map_block());
        ___(ps.add_sibling());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain("another explicit key"));
                ___(ps.set_val_scalar_plain("its value"));
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
        ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk7,
            HAS_CONTAINER_KEYS, Location(5,2,5),
            "\n"
            "- ? : x\n"
            "- ? : \n"
            "- ? :\n"
            ,
            "- ?\n"
            "    : x\n"
            "  : \n"
            "- ?\n"
            "    : \n"
            "  : \n"
            "- ?\n"
            "    : \n"
            "  : \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain_empty());
                ___(ps.set_val_scalar_plain("x"));
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
        ___(ps.end_map_block());
        ___(ps.add_sibling());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain_empty());
                ___(ps.set_val_scalar_plain_empty());
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
        ___(ps.end_map_block());
        ___(ps.add_sibling());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain_empty());
                ___(ps.set_val_scalar_plain_empty());
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
        ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk7SuddenEnd0_0,
            HAS_CONTAINER_KEYS, Location(5,2,5),
            "- ? []:\n"
            ,
            "- ?\n"
            "    ? []\n"
            "    : \n"
            "  : \n"
            ,
            "+STR"    "\n"
            "+DOC"    "\n"
            "+SEQ"    "\n"
            "+MAP"    "\n"
            "+MAP"    "\n"
            "+SEQ []" "\n"
            "-SEQ"    "\n"
            "=VAL :"  "\n"
            "-MAP"    "\n"
            "=VAL :"  "\n"
            "-MAP"    "\n"
            "-SEQ"    "\n"
            "-DOC"    "\n"
            "-STR"    "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.begin_seq_key_flow());
                ___(ps.end_seq_flow(singleline));
                ___(ps.set_val_scalar_plain_empty());
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
        ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk7SuddenEnd0_0_1,
            HAS_CONTAINER_KEYS, Location(5,2,5),
            "- ? []: a\n"
            ,
            "- ?\n"
            "    ? []\n"
            "    : a\n"
            "  : \n"
            ,
            "+STR"    "\n"
            "+DOC"    "\n"
            "+SEQ"    "\n"
            "+MAP"    "\n"
            "+MAP"    "\n"
            "+SEQ []" "\n"
            "-SEQ"    "\n"
            "=VAL :a" "\n"
            "-MAP"    "\n"
            "=VAL :"  "\n"
            "-MAP"    "\n"
            "-SEQ"    "\n"
            "-DOC"    "\n"
            "-STR"    "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.begin_seq_key_flow());
                ___(ps.end_seq_flow(singleline));
                ___(ps.set_val_scalar_plain("a"));
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
        ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk7SuddenEnd0_1,
            HAS_CONTAINER_KEYS, Location(5,2,5),
            "- ? []:\n"
            "  :\n"
            ,
            "- ?\n"
            "    ? []\n"
            "    : \n"
            "  : \n"
            ,
            "+STR"    "\n"
            "+DOC"    "\n"
            "+SEQ"    "\n"
            "+MAP"    "\n"
            "+MAP"    "\n"
            "+SEQ []" "\n"
            "-SEQ"    "\n"
            "=VAL :"  "\n"
            "-MAP"    "\n"
            "=VAL :"  "\n"
            "-MAP"    "\n"
            "-SEQ"    "\n"
            "-DOC"    "\n"
            "-STR"    "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.begin_seq_key_flow());
                ___(ps.end_seq_flow(singleline));
                ___(ps.set_val_scalar_plain_empty());
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
        ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk7SuddenEnd0_2,
            HAS_CONTAINER_KEYS, Location(5,2,5),
            "- ? []:\n"
            "  : a\n"
            ,
            "- ?\n"
            "    ? []\n"
            "    : \n"
            "  : a\n"
            ,
            "+STR"    "\n"
            "+DOC"    "\n"
            "+SEQ"    "\n"
            "+MAP"    "\n"
            "+MAP"    "\n"
            "+SEQ []" "\n"
            "-SEQ"    "\n"
            "=VAL :"  "\n"
            "-MAP"    "\n"
            "=VAL :a" "\n"
            "-MAP"    "\n"
            "-SEQ"    "\n"
            "-DOC"    "\n"
            "-STR"    "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.begin_seq_key_flow());
                ___(ps.end_seq_flow(singleline));
                ___(ps.set_val_scalar_plain_empty());
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain("a"));
        ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk7SuddenEnd0_3,
            HAS_CONTAINER_KEYS, Location(5,2,5),
            "- ? []:\n"
            "- a\n"
            ,
            "- ?\n"
            "    ? []\n"
            "    : \n"
            "  : \n"
            "- a\n"
            ,
            "+STR"    "\n"
            "+DOC"    "\n"
            "+SEQ"    "\n"
            "+MAP"    "\n"
            "+MAP"    "\n"
            "+SEQ []" "\n"
            "-SEQ"    "\n"
            "=VAL :"  "\n"
            "-MAP"    "\n"
            "=VAL :"  "\n"
            "-MAP"    "\n"
            "=VAL :a" "\n"
            "-SEQ"    "\n"
            "-DOC"    "\n"
            "-STR"    "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.begin_seq_key_flow());
                ___(ps.end_seq_flow(singleline));
                ___(ps.set_val_scalar_plain_empty());
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
        ___(ps.end_map_block());
        ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk7SuddenEnd1,
            HAS_CONTAINER_KEYS, Location(5,2,5),
            "- ? [a,b]:\n"
            ,
            "- ?\n"
            "    ? [a,b]\n"
            "    : \n"
            "  : \n"
            ,
            "+STR"    "\n"
            "+DOC"    "\n"
            "+SEQ"    "\n"
            "+MAP"    "\n"
            "+MAP"    "\n"
            "+SEQ []" "\n"
            "=VAL :a" "\n"
            "=VAL :b" "\n"
            "-SEQ"    "\n"
            "=VAL :"  "\n"
            "-MAP"    "\n"
            "=VAL :"  "\n"
            "-MAP"    "\n"
            "-SEQ"    "\n"
            "-DOC"    "\n"
            "-STR"    "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.begin_seq_key_flow());
                    ___(ps.set_val_scalar_plain("a"));
                    ___(ps.add_sibling());
                    ___(ps.set_val_scalar_plain("b"));
                ___(ps.end_seq_flow(singleline));
                ___(ps.set_val_scalar_plain_empty());
            ___(ps.end_map_block());
            ___(ps.set_val_scalar_plain_empty());
        ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(Qmrk8,
            "?"        "\n"
            "? "       "\n"
            "?"        "\n"
            "? a"      "\n"
            "? b"      "\n"
            "?"        "\n"
            "---"      "\n"
            "?"        "\n"
            "? a"      "\n"
            "? b"      "\n"
            "?"        "\n"
            ,
            "---"       "\n"
            ": "        "\n"
            ": "        "\n"
            ": "        "\n"
            "a: "       "\n"
            "b: "       "\n"
            ": "        "\n"
            "---"       "\n"
            ": "        "\n"
            "a: "       "\n"
            "b: "       "\n"
            ": "        "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "=VAL :a"     "\n"
            "=VAL :"      "\n"
            "=VAL :b"     "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "+DOC ---"    "\n"
            "+MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "=VAL :a"     "\n"
            "=VAL :"      "\n"
            "=VAL :b"     "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(Qmrk9,
            "?"        "\n"
            ,
            ": "        "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk10,
            "?"        "\n"
            ":"        "\n"
            ,
            ": "        "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk11,
            HAS_CONTAINER_KEYS, Location(4,2,3),
            "?"        "\n"
            "  :"      "\n"
            ,
            "?"        "\n"
            "  : "     "\n"
            ": "       "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk12,
            HAS_CONTAINER_KEYS, Location(3,2,2),
            "?"        "\n"
            " :"       "\n"
            ,
            "?"        "\n"
            "  : "     "\n"
            ": "       "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk13,
            "? ."        "\n"
            ":"        "\n"
            ,
            ".: "        "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "=VAL :."      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("."));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk14,
            "? ..."        "\n"
            ":"           "\n"
            ,
            "  ...: "     "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "=VAL :..."   "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("..."));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
