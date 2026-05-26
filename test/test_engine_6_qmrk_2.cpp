#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

ENGINE_TEST(QmrkNestedSameline0_0, HAS_CONTAINER_KEYS,
            "? ?"   "\n"
            ": a"         "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :a"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkNestedSameline0_1, HAS_CONTAINER_KEYS,
            "? ? ?"   "\n"
            ": a"         "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :a"     "\n"
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
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkNestedSameline0_2, HAS_CONTAINER_KEYS,
            "? ? ? ?"   "\n"
            ": a"         "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
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
            "-MAP"        "\n"
            "=VAL :a"     "\n"
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
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkNestedSameline0_3, HAS_CONTAINER_KEYS,
            "? ? ? ? ?"   "\n"
            ": a"         "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
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
            "-MAP"        "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :a"     "\n"
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
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedSameline1_00, HAS_CONTAINER_KEYS,
            "? ?"   "\n"
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
ENGINE_TEST(QmrkNestedSameline1_01, HAS_CONTAINER_KEYS,
            "? ? ?"   "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "=VAL :"      "\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
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
ENGINE_TEST(QmrkNestedSameline1_02, HAS_CONTAINER_KEYS,
            "? ? ? ?"   "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
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
            "-MAP"        "\n"
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
    ___(ps.set_key_scalar_plain_empty());
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
ENGINE_TEST(QmrkNestedSameline1_03, HAS_CONTAINER_KEYS,
            "? ? ? ? ?"   "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
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
            "-MAP"        "\n"
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


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkNestedSamelineTag0_00, HAS_CONTAINER_KEYS,
            "? ? &a !t"   "\n"
            ": a"         "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "=VAL &a <!t> :""\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :a"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedSamelineTag0_01, HAS_CONTAINER_KEYS,
            "? &a !t\n"
            "  ? &a !t"   "\n"
            ": a"         "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP &a <!t>""\n"
            "=VAL &a <!t> :""\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :a"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedSamelineTag0_02, HAS_CONTAINER_KEYS,
            "? &a !t ? &a !t"   "\n"
            ": a"         "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP &a <!t>""\n"
            "=VAL &a <!t> :""\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :a"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

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
