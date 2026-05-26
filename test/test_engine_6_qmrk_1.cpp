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
            ___(ps.set_key_scalar_plain("its value"));
        ___(ps.end_map_block());
        ___(ps.set_val_scalar_plain_empty());
        ___(ps.add_sibling());
        ___(ps.begin_map_key_block());
            ___(ps.set_key_scalar_plain("more"));
            ___(ps.set_key_scalar_plain("siblings"));
        ___(ps.end_map_block());
        ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk2_0,
            "? an explicit key\n"
            "...\n"
            "? another explicit key\n",
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
    ___(ps.end_map_block());
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
    ___(ps.end_map_block());
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
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("?baz"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
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
            " :"      "\n"
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


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkWithTags,
            HAS_CONTAINER_KEYS, Location(18,3,6),
            "a1: b1\n"
            "? !at\n"
            " !bt : b2\n"
            ": c3\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a1\n"
            "=VAL :b1\n"
            "+MAP <!at>\n"
            "=VAL <!bt> :\n"
            "=VAL :b2\n"
            "-MAP\n"
            "=VAL :c3\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a1"));
    ___(ps.set_val_scalar_plain("b1"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!at"));
    ___(ps.begin_map_key_block());
        ___(ps.set_key_tag("!bt"));
        ___(ps.set_key_scalar_plain_empty());
        ___(ps.set_val_scalar_plain("b2"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("c3"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(QmrkGithub524_0,
            "? outer\n"
            ": inner1: 1\n"
            "  inner2: 2\n"
            ,
            "outer:\n"
            "  inner1: 1\n"
            "  inner2: 2\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :outer\n"
            "+MAP\n"
            "=VAL :inner1\n"
            "=VAL :1\n"
            "=VAL :inner2\n"
            "=VAL :2\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("outer"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("inner1"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("inner2"));
    ___(ps.set_val_scalar_plain("2"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkGithub524_1,
            "? outer\n"
            ": inner1: 1\n"
            "inner2: 2\n"
            ,
            "outer:\n"
            "  inner1: 1\n"
            "inner2: 2\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :outer\n"
            "+MAP\n"
            "=VAL :inner1\n"
            "=VAL :1\n"
            "-MAP\n"
            "=VAL :inner2\n"
            "=VAL :2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
            )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("outer"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("inner1"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("inner2"));
    ___(ps.set_val_scalar_plain("2"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkFlow0,
            "{ ? }"
            ,
            "{: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow0Seq,
            "[ ? ]"
            ,
            "[{: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :\n"
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
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1,
            "{ ? , }"
            ,
            "{: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Seq,
            "[ ? , ]"
            ,
            "[{: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :\n"
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
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkSameLineUnkSeq, HAS_CONTAINER_KEYS,
            "? - a\n"
            "  - b\n"
            ": - a\n"
            "  - b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_block());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC_(QmrkSameLineUnkSeqErr0, HAS_CONTAINER_KEYS,
                    Location(5, 3),
                    "? - a\n"
                    "  - b\n"
                    ": - a\n"
                    "  - b\n"
                    ": - a\n" // error here
                    "  - b\n"
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineUnkSeqErr1, HAS_CONTAINER_KEYS,
                    Location(5, 4),
                    "? - a\n"
                    "  - b\n"
                    ": - a\n"
                    "  - b\n"
                    "k: - a\n" // error here
                    "   - b\n"
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineUnkSeqErr3, HAS_CONTAINER_KEYS,
                    Location(1, 3),
                    ": - a\n" // error here
                    "  - b\n"
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineUnkSeqErr4, HAS_CONTAINER_KEYS,
                    Location(1, 4),
                    "k: - a\n" // error here
                    "   - b\n"
    )

ENGINE_TEST(QmrkSameLineUnkMap, HAS_CONTAINER_KEYS,
            "? a: b\n"
            ": c: d\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
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
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC_(QmrkSameLineUnkMapErr0, HAS_CONTAINER_KEYS,
                    Location(3, 6), // FIXME
                    "? a: b\n"
                    ": c: d\n"
                    ": e: f\n" // error here
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineUnkMapErr1, HAS_CONTAINER_KEYS,
                    Location(3, 7), // FIXME
                    "? a: b\n"
                    ": c: d\n"
                    "k: e: f\n" // error here
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineUnkMapErr2, HAS_CONTAINER_KEYS,
                    Location(1, 6), // FIXME
                    ": e: f\n" // error here
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineUnkMapErr3, HAS_CONTAINER_KEYS,
                    Location(1, 7),
                    "k: e: f\n" // error here
    )


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkSameLineMapSeq, HAS_CONTAINER_KEYS,
            "k: v\n"
            "? - a\n"
            "  - b\n"
            ": - a\n"
            "  - b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :v\n"
            "+SEQ\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain("v"));
    ___(ps.add_sibling());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_block());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC_(QmrkSameLineMapSeqErr0, HAS_CONTAINER_KEYS,
                    Location(6, 3),
                    "k: v\n"
                    "? - a\n"
                    "  - b\n"
                    ": - a\n"
                    "  - b\n"
                    ": - a\n" // error here
                    "  - b\n"
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineMapSeqErr1, HAS_CONTAINER_KEYS,
                    Location(6, 4),
                    "k: v\n"
                    "? - a\n"
                    "  - b\n"
                    ": - a\n"
                    "  - b\n"
                    "k: - a\n" // error here
                    "   - b\n"
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineMapSeqErr2, HAS_CONTAINER_KEYS,
                    Location(2, 3),
                    "k: v\n"
                    ": - a\n" // error here
                    "  - b\n"
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineMapSeqErr3, HAS_CONTAINER_KEYS,
                    Location(2, 4),
                    "k: v\n"
                    "k: - a\n" // error here
                    "   - b\n"
    )

ENGINE_TEST(QmrkSameLineMapMap, HAS_CONTAINER_KEYS,
            "k: v\n"
            "? a: b\n"
            ": c: d\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :v\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain("v"));
    ___(ps.add_sibling());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC_(QmrkSameLineMapMapErr0, HAS_CONTAINER_KEYS,
                    Location(4, 6),
                    "k: v\n"
                    "? a: b\n"
                    ": c: d\n"
                    ": e: f\n" // error here
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineMapMapErr1, HAS_CONTAINER_KEYS,
                    Location(4, 7), // FIXME
                    "k: v\n"
                    "? a: b\n"
                    ": c: d\n"
                    "k: e: f\n" // error here
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineMapMapErr2, HAS_CONTAINER_KEYS,
                    Location(2, 6), // FIXME
                    "k: v\n"
                    ": e: f\n" // error here
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineMapMapErr3, HAS_CONTAINER_KEYS,
                    Location(2, 7), // FIXME
                    "k: v\n"
                    "k: e: f\n" // error here
    )


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkSameLineSeqSeq, HAS_CONTAINER_KEYS,
            "- ? - a\n"
            "    - b\n"
            "  : - a\n"
            "    - b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_key_block());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_block());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC_(QmrkSameLineSeqSeqErr0, HAS_CONTAINER_KEYS,
                    Location(5, 5), // FIXME
                    "- ? - a\n"
                    "    - b\n"
                    "  : - a\n"
                    "    - b\n"
                    "  : - a\n" // error here
                    "    - b\n"
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineSeqSeqErr1, HAS_CONTAINER_KEYS,
                    Location(5, 6),
                    "- ? - a\n"
                    "    - b\n"
                    "  : - a\n"
                    "    - b\n"
                    "  k: - a\n" // error here
                    "     - b\n"
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineSeqSeqErr2, HAS_CONTAINER_KEYS,
                    Location(1, 5),
                    "- : - a\n" // error here
                    "    - b\n"
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineSeqSeqErr3, HAS_CONTAINER_KEYS,
                    Location(1, 6),
                    "- k: - a\n" // error here
                    "     - b\n"
    )

ENGINE_TEST(QmrkSameLineSeqMap, HAS_CONTAINER_KEYS,
            "- ? a: b\n"
            "  : c: d\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
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
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC_(QmrkSameLineSeqMapErr0, HAS_CONTAINER_KEYS,
                    Location(3, 8), // FIXME
                    "- ? a: b\n"
                    "  : c: d\n"
                    "  : e: f\n" // error here
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineSeqMapErr1, HAS_CONTAINER_KEYS,
                    Location(3, 9),
                    "- ? a: b\n"
                    "  : c: d\n"
                    "  k: e: f\n" // error here
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineSeqMapErr2, HAS_CONTAINER_KEYS,
                    Location(1, 6), // FIXME
                    ": e: f\n" // error here
    )
ENGINE_TEST_ERRLOC_(QmrkSameLineSeqMapErr3, HAS_CONTAINER_KEYS,
                    Location(1, 7), // FIXME
                    "k: e: f\n" // error here
    )


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkSameLineNested0Seq, HAS_CONTAINER_KEYS,
            "? ? - a\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkSameLineNested0Seq_1, HAS_CONTAINER_KEYS,
            "?\n"
            "  ? - a\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkSameLineNested0Map, HAS_CONTAINER_KEYS,
            "? ? a: b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkSameLineNested0Map_1, HAS_CONTAINER_KEYS,
            "?\n"
            "  ? a: b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkSameLineNested1SeqRkcl0, HAS_CONTAINER_KEYS,
            "? ? - a\n"
            "  ? - b\n"
            "? - c\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :\n"
            "+SEQ\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "+SEQ\n"
            "=VAL :c\n"
            "-SEQ\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
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

C4_SUPPRESS_WARNING_MSVC_POP
