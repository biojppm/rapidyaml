#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

static constexpr const bool singleline = false; // NOLINT

ENGINE_TEST(DocEmpty,
            "",
            "+STR\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocEmptyExpl,
            "---\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocEmptyTerm1,
            "...\n"
            ,
            ""
            ,
            "+STR\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm1Indented1,
            " ...\n"
            ,
            "  ...\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :...\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("..."));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm2,
            "...\n...\n"
            ,
            ""
            ,
            "+STR\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm2Indented0,
            HAS_MULTILINE_SCALAR
            ,
            " ...\n...\n"
            ,
            "  ...\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :...\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("..."));
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm2Indented1,
            HAS_MULTILINE_SCALAR
            ,
            " ...\n ...\n"
            ,
            "  ... ...\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :... ...\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("... ..."));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm3,
            "...\n...\n...\n"
            ,
            ""
            ,
            "+STR\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm3_1,
            "...\n...\n..\n"
            ,
            "..\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :..\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain(".."));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm4_2,
            "...\n"
            "...\n"
            "......\n"
            "...\n"
            "...\n"
            ,
            "  ......\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :......\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("......"));
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}


ENGINE_TEST(DocEmptyTermNotErr,
            "...\n...\n"
            ,
            ""
            ,
            "+STR\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------


ENGINE_TEST(DocTerm_MapBlck_ok,
            "foo: bar\n"
            "... #bad\n"
            ,
            "foo: bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}


ENGINE_TEST_ERRLOC(DocTerm_MapBlck_err0_0,
                   Location(2, 5),
                   "foo: bar\n"
                   "... bad\n"
                   )
ENGINE_TEST(DocTerm_MapBlck_err0_1,
            "foo: bar\n"
            "--- bad\n"
            ,
            "---\n"
            "foo: bar\n"
            "--- bad\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :bad\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("bad"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(DocTerm_MapBlck_err1_0,
                   Location(2, 5),
                   "foo: bar\n"
                   "... [bad]\n"
                   )
ENGINE_TEST(DocTerm_MapBlck_err1_1,
            "foo: bar\n"
            "--- [bad]\n"
            ,
            "---\n"
            "foo: bar\n"
            "--- [bad]\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+SEQ []\n"
            "=VAL :bad\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("bad"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(DocTerm_MapBlck_err2_0,
                   Location(2, 5),
                   "foo: bar\n"
                   "... {bad: yes}\n"
                   )
ENGINE_TEST(DocTerm_MapBlck_err2_1,
            "foo: bar\n"
            "--- {bad: yes}\n"
            ,
            "---\n"
            "foo: bar\n"
            "--- {bad: yes}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP {}\n"
            "=VAL :bad\n"
            "=VAL :yes\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("bad"));
    ___(ps.set_val_scalar_plain("yes"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(DocTerm_MapBlck_err3_0,
                   Location(2, 5),
                   "foo:\n"
                   "... bad\n"
    )
ENGINE_TEST(DocTerm_MapBlck_err3_1,
            "foo:\n"
            "--- bad\n"
            ,
            "---\n"
            "foo: \n"
            "--- bad\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :bad\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("bad"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(DocTerm_MapBlck_err4_0,
                   Location(2, 5),
                   "foo:\n"
                   "... bad\n"
                   )
ENGINE_TEST(DocTerm_MapBlck_err4_1,
            "foo:\n"
            "--- bad\n"
            ,
            "---\n"
            "foo: \n"
            "--- bad\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :bad\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("bad"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(DocTerm_MapBlck_err5_0,
                   Location(2, 5),
                   "?\n"
                   "... bad\n"
                   )
ENGINE_TEST(DocTerm_MapBlck_err5_1,
            "?\n"
            "--- bad\n"
            ,
            "---\n"
            ": \n"
            "--- bad\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :bad\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("bad"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(DocTerm_MapBlck_err6_0,
                   Location(2, 5),
                   "? foo\n"
                   "... bad\n"
                   )
ENGINE_TEST(DocTerm_MapBlck_err6_1,
            "? foo\n"
            "--- bad\n"
            ,
            "---\n"
            "foo: \n"
            "--- bad\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :bad\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("bad"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(DocTerm_MapBlck_err7_0,
                   Location(3, 5),
                   "? foo\n"
                   ":\n"
                   "... bad\n"
                   )
ENGINE_TEST(DocTerm_MapBlck_err7_1,
            "? foo\n"
            ":\n"
            "--- bad\n"
            ,
            "---\n"
            "foo: \n"
            "--- bad\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :bad\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("bad"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST_ERRLOC(DocTerm_SeqBlck_err0_0,
                   Location(2, 5),
                   "- foo\n"
                   "... bad\n"
                   )
ENGINE_TEST_ERRLOC(DocTerm_SeqBlck_err0_1,
                   Location(2, 5),
                   "-\n"
                   "... bad\n"
                   )
ENGINE_TEST_ERRLOC(DocTerm_SeqBlck_err1_0,
                   Location(3, 5),
                   "-\n"
                   "  - foo\n"
                   "... bad\n"
                   )
ENGINE_TEST_ERRLOC(DocTerm_SeqBlck_err1_1,
                   Location(3, 5),
                   "-\n"
                   "  -\n"
                   "... bad\n"
                   )


ENGINE_TEST_ERRLOC(DocTerm_MapFlow_err0,
                   Location(2, 5),
                   "{key: val}\n"
                   "... bad\n"
                   )
ENGINE_TEST_ERRLOC(DocTerm_SeqFlow_err0,
                   Location(2, 5),
                   "{key: val}\n"
                   "... bad\n"
                   )
ENGINE_TEST_ERRLOC(DocTerm_ScalarPlain_err0,
                   Location(2, 5),
                   "scalar\n"
                   "... bad\n"
                   )
ENGINE_TEST_ERRLOC(DocTerm_ScalarSquo_err0,
                   Location(2, 5),
                   "'scalar'\n"
                   "... bad\n"
                   )
ENGINE_TEST_ERRLOC(DocTerm_ScalarDquo_err0,
                   Location(2, 5),
                   "\"scalar\"\n"
                   "... bad\n"
                   )
ENGINE_TEST_ERRLOC(DocTerm_ScalarLit_err0,
                   Location(3, 5),
                   "|\n  scalar\n"
                   "... bad\n"
                   )


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SquotedDoc_0,
                   Location(2, 1),
                   "'\n"
                   "---\n"
                   "'\n"
                   )
ENGINE_TEST_ERRLOC(SquotedDoc_1,
                   Location(2, 1),
                   "'\n"
                   "...\n"
                   "'\n"
                   )
ENGINE_TEST(SquotedDoc_2,
            HAS_MULTILINE_SCALAR,
            "'\n"
            " ---\n"
            "'\n"
            ,
            "' --- '\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL ' --- \n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_squoted(" --- "));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(SquotedDoc_3,
            HAS_MULTILINE_SCALAR,
            "'\n"
            " ...\n"
            "'\n"
            ,
            "' ... '\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL ' ... \n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_squoted(" ... "));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DquotedDoc_0,
                   Location(2, 1),
                   "'\n"
                   "---\n"
                   "'\n"
                   )
ENGINE_TEST_ERRLOC(DquotedDoc_1,
                   Location(2, 1),
                   "\"\n"
                   "...\n"
                   "\"\n"
                   )
ENGINE_TEST(DquotedDoc_2,
            HAS_MULTILINE_SCALAR,
            "\"\n"
            " ---\n"
            "\"\n"
            ,
            "\" --- \"\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL \" --- \n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_dquoted(" --- "));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(DquotedDoc_3,
            HAS_MULTILINE_SCALAR,
            "\"\n"
            " ...\n"
            "\"\n"
            ,
            "\" ... \"\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL \" ... \n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_dquoted(" ... "));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocEmptyExplMult,
            "---\n---\n---\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocEmptyExplMultImplFirst,
            "foo\n--- bar\n--- baz\n"
            ,
            "--- foo\n--- bar\n--- baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :foo\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :bar\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :baz\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("baz"));
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
