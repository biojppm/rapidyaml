#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

ENGINE_TEST(QmrkNestedBlock0,
            "? \n"
            "  ?a\n"
            ,
            "?a: \n"
            ,
            "+STR"          "\n"
            "+DOC"          "\n"
            "+MAP"          "\n"
            "=VAL :?a"      "\n"
            "=VAL :"        "\n"
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
            "?\n"
            "  a: b\n"
            ": \n"
            ,
            "+STR"          "\n"
            "+DOC"          "\n"
            "+MAP"          "\n"
            "+MAP"          "\n"
            "=VAL :a"       "\n"
            "=VAL :b"       "\n"
            "-MAP"          "\n"
            "=VAL :"        "\n"
            "-MAP"          "\n"
            "-DOC"          "\n"
            "-STR"          "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
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
            "?\n"
            "  ?\n"
            "    a: b\n"
            "  : \n"
            ": \n"
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

ENGINE_TEST(QmrkNestedBlock4,
            HAS_CONTAINER_KEYS,
            "? \n"
            "  ? \n"
            "    ? \n"
            "      ? a\n"
            "      : b\n"
            ,
            "?\n"
            "  ?\n"
            "    ?\n"
            "      a: b\n"
            "    : \n"
            "  : \n"
            ": \n"
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
    ___(ps.begin_map_val_block());
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
            "?"          "\n"
            "  : "       "\n"
            ": "         "\n"
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

ENGINE_TEST(QmrkNestedBlock5_1,
            HAS_CONTAINER_KEYS,
            "?"          "\n"
            "  ?"        "\n"
            "    ?"      "\n"
            ,
            "?"          "\n"
            "  ?"        "\n"
            "    : "     "\n"
            "  : "       "\n"
            ": "         "\n"
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
    ___(ps.begin_map_val_block());
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
            "?"          "\n"
            "  ?"        "\n"
            "    ?"      "\n"
            "      ?"    "\n"
            "        : " "\n"
            "      : "   "\n"
            "    : "     "\n"
            "  : "       "\n"
            ": "         "\n"
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
    ___(ps.begin_map_val_block());
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
            "?"          "\n"
            "  : "       "\n"
            ": "        "\n"
            ": "        "\n"
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
    ___(ps.begin_map_val_block());
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
            "?"           "\n"
            "  ?"         "\n"
            "    ?"       "\n"
            "      ?"     "\n"
            "        ?"   "\n"
            "          : ""\n"
            "        : "  "\n"
            "        : "  "\n"
            "      : "    "\n"
            "      : "    "\n"
            "    : "      "\n"
            "    : "      "\n"
            "  : "        "\n"
            "  : "        "\n"
            ": "          "\n"
            ": "          "\n"
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
    ___(ps.begin_map_val_block());
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
            "?"             "\n"
            "  ?"           "\n"
            "    ?"         "\n"
            "      ?"       "\n"
            "        ?"     "\n"
            "          : "  "\n"
            "        : "    "\n"
            "      : "      "\n"
            "      : "      "\n"
            "    : "        "\n"
            "  : "          "\n"
            ": "            "\n"
            ": "            "\n"
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
    ___(ps.begin_map_val_block());
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


} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
