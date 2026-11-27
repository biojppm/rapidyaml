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

ENGINE_TEST_ERRLOC(QmrkIndent00, Location(2, 2), "?\na")

ENGINE_TEST_ERRLOC(QmrkIndent01_0, Location(2, 1), "?\n?a")
ENGINE_TEST_ERRLOC(QmrkIndent01_1, Location(2, 1), "?\n-a")

ENGINE_TEST_ERRLOC(QmrkIndent01_2_0, Location(2, 1), "?\n?a?")
ENGINE_TEST_ERRLOC(QmrkIndent01_2_1, Location(2, 1), "?\n-a?")

ENGINE_TEST(QmrkIndent01_3_0,
            "?\n?"
            ,
            ": \n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :\n"
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
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkIndent01_3_1, HAS_CONTAINER_KEYS,
            "?\n-"
            ,
            "?\n"
            "  - \n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :\n"
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
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkIndent02_0,
            "?\n? a"
            ,
            ": \n"
            "a: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkIndent02_1, HAS_CONTAINER_KEYS,
            "?\n- a"
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

ENGINE_TEST(QmrkIndent03_0, HAS_CONTAINER_KEYS,
            "?\n ? a"
            ,
            "?\n"
            "  a: \n"
            ": \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :a\n"
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
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkIndent03_1, HAS_CONTAINER_KEYS,
            "?\n - a"
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
ENGINE_TEST(QmrkIndent04_0,
            "?\n ?a"
            ,
            "?a: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :?a\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
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
ENGINE_TEST(QmrkIndent04_1,
            "?\n -a"
            ,
            "-a: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :-a\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("-a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkIndent05_0, NO_COMPARE_EMITTED_INTS,
            "?\n--- a"
            ,
            "---\n"
            ": \n"
            "--- a\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :a\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkIndent05_1,
            "?\n --- a"
            ,
            "  --- a: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :--- a\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("--- a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkIndent06_0, NO_COMPARE_EMITTED_INTS,
            "?\n...\n--- a"
            ,
            "---\n"
            ": \n"
            "--- a\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL :a\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(QmrkIndent06_1,
            "?\n ... a"
            ,
            "  ... a: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :... a\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("... a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
