#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


ENGINE_TEST(QmrkIndentSeq0, HAS_CONTAINER_KEYS,
            "?\n"
            "  - a\n"
            ,
            "?\n"
            "  - a\n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :a\n"
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
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkIndentSeq1, HAS_CONTAINER_KEYS,
            "?\n"
            "- a\n"
            ,
            "?\n"
            "  - a\n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :a\n"
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
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkIndentSeq2, HAS_CONTAINER_KEYS,
            "?\n"
            "- a\n"
            ":\n"
            "?\n"
            ,
            "?\n"
            "  - a\n"
            ": \n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkIndentSeq3, HAS_CONTAINER_KEYS,
            "?\n"
            "- a\n"
            "?\n"
            ,
            "?\n"
            "  - a\n"
            ": \n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkIndentSeq4, HAS_CONTAINER_KEYS,
            "?\n"
            "- - - - a\n"
            "?\n"
            ,
            "?\n"
            "  - - - - a\n"
            ": \n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "+SEQ\n"
            "+SEQ\n"
            "+SEQ\n"
            "=VAL :a\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.end_seq_block());
    ___(ps.end_seq_block());
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkIndentSeq5, HAS_CONTAINER_KEYS,
            "?\n"
            "- - - - a\n"
            "    - b\n"
            "?\n"
            ,
            "?\n"
            "  - - - - a\n"
            "      - b\n"
            ": \n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "+SEQ\n"
            "+SEQ\n"
            "+SEQ\n"
            "=VAL :a\n"
            "-SEQ\n"
            "=VAL :b\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_block());
    ___(ps.end_seq_block());
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
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
