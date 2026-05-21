#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


#ifdef RYML_WITH_TAB_TOKENS
ENGINE_TEST_ERRLOC(Y79Y_004_00, Location(1, 3),
                   "-\t- a"
                   )
#endif

ENGINE_TEST(Y79Y_004_01,
            "- \ta\n"
            ,
            "- a\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(Y79Y_005_map, Location(1, 7),
                   "- \ta: b\n"
                   )
ENGINE_TEST(Y79Y_005_map_squo,
            "- '\ta: b'\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL '\\ta: b\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_squoted("\ta: b"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Y79Y_005_map_dquo,
            "- \"\ta: b\"\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL \"\\ta: b\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_dquoted("\ta: b"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(Y79Y_005_seq, Location(1, 4),
                   "- \t- a"
                   )
ENGINE_TEST(Y79Y_005_seq_squo,
            "- '\t- a'\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL '\\t- a\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_squoted("\t- a"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Y79Y_005_seq_dquo,
            "- \"\t- a\"\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL \"\\t- a\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_dquoted("\t- a"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(Y79Y_005_seq_nl, Location(2, 4),
                   "-\n"
                   "  \t- a"
    )
ENGINE_TEST(Y79Y_005_seq_nl_squo,
            "-\n"
            "  '\t- a'\n"
            ,
            "- '\t- a'\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL '\\t- a\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_squoted("\t- a"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Y79Y_005_seq_nl_dquo,
            "-\n"
            "  \"\t- a\"\n"
            ,
            "- \"\t- a\"\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL \"\\t- a\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_dquoted("\t- a"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(Y79Y_005_map_nl, Location(2, 7),
                   "-\n"
                   "  \ta: b"
    )
ENGINE_TEST(Y79Y_005_map_nl_squo,
            "-\n"
            "  '\ta: b'"
            ,
            "- '\ta: b'\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL '\\ta: b\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_squoted("\ta: b"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Y79Y_005_map_nl_dquo,
            "-\n"
            "  \"\ta: b\""
            ,
            "- \"\ta: b\"\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL \"\\ta: b\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_dquoted("\ta: b"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(Y79Y_005_on_map_seq, Location(2, 4),
                   "foo:\n"
                   "  \t- a"
    )
ENGINE_TEST(Y79Y_005_on_map_seq_squo,
            "foo:\n"
            "  '\t- a'"
            ,
            "foo: '\t- a'\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL '\\t- a\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_squoted("\t- a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Y79Y_005_on_map_seq_dquo,
            "foo:\n"
            "  \"\t- a\""
            ,
            "foo: \"\t- a\"\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL \"\\t- a\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_dquoted("\t- a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(Y79Y_005_on_map_map, Location(2, 7),
                   "foo:\n"
                   "  \ta: b"
    )
ENGINE_TEST(Y79Y_005_on_map_map_squo,
            "foo:\n"
            "  '\ta': b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "+MAP\n"
            "=VAL '\\ta\n"
            "=VAL :b\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_squoted("\ta"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Y79Y_005_on_map_map_dquo,
            "foo:\n"
            "  \"\ta\": b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "+MAP\n"
            "=VAL \"\\ta\n"
            "=VAL :b\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_dquoted("\ta"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
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
