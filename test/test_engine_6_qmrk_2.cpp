#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


//static constexpr const bool multiline = true;
static constexpr const bool singleline = false;


ENGINE_TEST(QmrkNestedBlock0,
            "? \n"
            "  ?a\n"
            ,
            "?a: \n"
            ,
            "+STR"          "\n"
            "+DOC"          "\n"
            "+MAP"          "\n"
            "=VAL :?a"          "\n"
            "=VAL :"          "\n"
            "-MAP"          "\n"
            "-DOC"          "\n"
            "-STR"          "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("?a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedBlock2,
            HAS_CONTAINER_KEYS,
            "? \n"
            "  ? a\n"
            "  : b\n"
            ,
            "+STR"          "\n"
            "+DOC"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "=VAL :a"          "\n"
            "=VAL :b"          "\n"
            "-MAP"          "\n"
            "=VAL :"          "\n"
            "-MAP"          "\n"
            "-DOC"          "\n"
            "-STR"          "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedBlock3,
            HAS_CONTAINER_KEYS,
            "? \n"
            "  ? \n"
            "    ? a\n"
            "    : b\n"
            ,
            "+STR"          "\n"
            "+DOC"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "=VAL :a"       "\n"
            "=VAL :b"       "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "-DOC"          "\n"
            "-STR"          "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
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

ENGINE_TEST(QmrkNestedBlock4,
            HAS_CONTAINER_KEYS,
            "? \n"
            "  ? \n"
            "    ? \n"
            "      ? a\n"
            "      : b\n"
            ,
            "+STR"          "\n"
            "+DOC"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "=VAL :a"       "\n"
            "=VAL :b"       "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "-DOC"          "\n"
            "-STR"          "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedBlock5_0,
            HAS_CONTAINER_KEYS,
            "?"          "\n"
            "  ?"        "\n"
            ,
            "+STR"     "\n"
            "+DOC"     "\n"
            "+MAP"     "\n"
            "+MAP"     "\n"
            "=VAL :"   "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "-DOC"     "\n"
            "-STR"     "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedBlock5_1,
            HAS_CONTAINER_KEYS,
            "?"          "\n"
            "  ?"        "\n"
            "    ?"      "\n"
            ,
            "+STR"     "\n"
            "+DOC"     "\n"
            "+MAP"     "\n"
            "+MAP"     "\n"
            "+MAP"     "\n"
            "=VAL :"   "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "-DOC"     "\n"
            "-STR"     "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedBlock5_2,
            HAS_CONTAINER_KEYS,
            "?"          "\n"
            "  ?"        "\n"
            "    ?"      "\n"
            "      ?"    "\n"
            "        ?"  "\n"
            ,
            "+STR"     "\n"
            "+DOC"     "\n"
            "+MAP"     "\n"
            "+MAP"     "\n"
            "+MAP"     "\n"
            "+MAP"     "\n"
            "+MAP"     "\n"
            "=VAL :"   "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "-DOC"     "\n"
            "-STR"     "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedBlock5_3,
            HAS_CONTAINER_KEYS,
            "?"          "\n"
            "  ?"        "\n"
            "?"          "\n"
            ,
            "+STR"     "\n"
            "+DOC"     "\n"
            "+MAP"     "\n"
            "+MAP"     "\n"
            "=VAL :"   "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "=VAL :"   "\n"
            "=VAL :"   "\n"
            "=VAL :"   "\n"
            "-MAP"     "\n"
            "-DOC"     "\n"
            "-STR"     "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedBlock5_4,
            HAS_CONTAINER_KEYS,
            "?"           "\n"
            "  ?"         "\n"
            "    ?"       "\n"
            "      ?"     "\n"
            "        ?"   "\n"
            "          ?" "\n"
            "        ?"   "\n"
            "      ?"     "\n"
            "    ?"       "\n"
            "  ?"         "\n"
            "?"           "\n"
            ,
            "+STR"          "\n"
            "+DOC"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "-DOC"          "\n"
            "-STR"          "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedBlock5_5,
            HAS_CONTAINER_KEYS,
            "?"           "\n"
            "  ?"         "\n"
            "    ?"       "\n"
            "      ?"     "\n"
            "        ?"   "\n"
            "          ?" "\n"
            "      ?"     "\n"
            "?"           "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
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
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkTestSuiteM2N8_01_key,
            "? k"
            ,
            "k: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_key_in_seq,
            "- ? k"
            ,
            "- k: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_key2,
            "? k\n"
            ": x\n"
            ,
            "k: x\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :x\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_seq,
            HAS_CONTAINER_KEYS,
            "? []"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "-SEQ\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_seq2,
            HAS_CONTAINER_KEYS,
            "? []\n"
            ": x\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_map,
            HAS_CONTAINER_KEYS,
            "? {}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_map2,
            HAS_CONTAINER_KEYS,
            "? {}\n"
            ": x\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyscalar,
            HAS_CONTAINER_KEYS,
            "? k: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyscalar_colon,
            HAS_CONTAINER_KEYS,
            "? k: x\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyscalar_colon_y,
            HAS_CONTAINER_KEYS,
            "? k: x\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyscalar2,
            HAS_CONTAINER_KEYS,
            "? k:"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyscalar2_colon,
            HAS_CONTAINER_KEYS,
            "? k:\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyscalar2_colon_x,
            HAS_CONTAINER_KEYS,
            "? k:\n"
            ": x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :k\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_0,
            HAS_CONTAINER_KEYS,
            "? []: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_0_colon,
            HAS_CONTAINER_KEYS,
            "? []: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_0_colon_y,
            HAS_CONTAINER_KEYS,
            "? []: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_0,
            HAS_CONTAINER_KEYS,
             "? {}: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_0_colon,
            HAS_CONTAINER_KEYS,
            "? {}: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_0_colon_y,
            HAS_CONTAINER_KEYS,
            "? {}: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_1,
            HAS_CONTAINER_KEYS,
            "? [a]: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_1_colon,
            HAS_CONTAINER_KEYS,
            "? [a]: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_1_colon_y,
            HAS_CONTAINER_KEYS,
            "? [a]: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_1,
            HAS_CONTAINER_KEYS,
            "? {a}: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_1_colon,
            HAS_CONTAINER_KEYS,
            "? {a}: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_1_colon_y,
            HAS_CONTAINER_KEYS,
            "? {a}: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_1_1,
            HAS_CONTAINER_KEYS,
            "? {a: }: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_1_1_colon,
            HAS_CONTAINER_KEYS,
            "? {a: }: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_1_1_colon_y,
            HAS_CONTAINER_KEYS,
            "? {a: }: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_1_2,
            HAS_CONTAINER_KEYS,
            "? {a: b}: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_1_2_colon,
            HAS_CONTAINER_KEYS,
            "? {a: b}: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_1_2_colon_y,
            HAS_CONTAINER_KEYS,
            "? {a: b}: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_2,
            HAS_CONTAINER_KEYS,
            "? [a,]: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_2_colon,
            HAS_CONTAINER_KEYS,
            "? [a,]: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_2_colon_y,
            HAS_CONTAINER_KEYS,
            "? [a,]: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_2,
            HAS_CONTAINER_KEYS,
            "? {a: b, }: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_2_colon,
            HAS_CONTAINER_KEYS,
            "? {a: b, }: x"   "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_2_colon_y,
            HAS_CONTAINER_KEYS,
            "? {a: b, }: x"   "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_3,
            HAS_CONTAINER_KEYS,
            "? [a, b]: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_3_colon,
            HAS_CONTAINER_KEYS,
            "? [a, b]: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseq0_3_colon_y,
            HAS_CONTAINER_KEYS,
            "? [a, b]: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_3,
            HAS_CONTAINER_KEYS,
            "? {a: b, c: d}: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_3_colon,
            HAS_CONTAINER_KEYS,
            "? {a: b, c: d}: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keymap0_3_colon_y,
            HAS_CONTAINER_KEYS,
            "? {a: b, c: d}: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseqmap0_1,
            HAS_CONTAINER_KEYS,
            "? [a: b]: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseqmap0_1_colon,
            HAS_CONTAINER_KEYS,
            "? [a: b]: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseqmap0_1_colon_y,
            HAS_CONTAINER_KEYS,
            "? [a: b]: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseqmap0_2,
            HAS_CONTAINER_KEYS,
            "? [a: ]: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseqmap0_2_colon,
            HAS_CONTAINER_KEYS,
            "? [a: ]: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseqmap0_2_colon_y,
            HAS_CONTAINER_KEYS,
            "? [a: ]: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseqmap0_3,
            HAS_CONTAINER_KEYS,
            "? [a: b, ]: x"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseqmap0_3_colon,
            HAS_CONTAINER_KEYS,
            "? [a: b, ]: x"  "\n"
            ":"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_keyseqmap0_3_colon_y,
            HAS_CONTAINER_KEYS,
            "? [a: b, ]: x"  "\n"
            ": y"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :y\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("y"));
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
