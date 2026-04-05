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


ENGINE_TEST(DocEmptyTermErr,
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


//-----------------------------------------------------------------------------

ENGINE_TEST(DocScalarPlain0,
            "foo\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :foo\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain1,
            "--- foo\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL :foo\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlainEmpty,
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

ENGINE_TEST(DocScalarPlain2,
            "--- ---\n"
            ,
            "---\n  ---\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL :---\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("---"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain2_1,
            "------\n"
            ,
            "  ------\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :------\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("------"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain2_2,
            "......\n"
            ,
            "  ......\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :......\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("......"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain3,
            "--- ...\n"
            ,
            "---\n  ...\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL :...\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("..."));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain3_1,
            "---...\n"
            ,
            "  ---...\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :---...\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("---..."));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlainMultiline0,
            HAS_MULTILINE_SCALAR,
            "--- foo\n bar\n"
            ,
            "--- foo bar\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL :foo bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlainMultiline0Indented,
            HAS_MULTILINE_SCALAR,
            " --- foo\n bar\n"
            , "  --- foo bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :--- foo bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("--- foo bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlainMultiline0Indented1,
            HAS_MULTILINE_SCALAR
            ,
            " --- foo\n bar\n"
            ,
            "  --- foo bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :--- foo bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("--- foo bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlainMultiline,
            HAS_MULTILINE_SCALAR
            ,
            "foo\nbar\n\nfew\nbars"
            ,
            "foo bar\n\n  few bars\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :foo bar\\nfew bars\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo bar\nfew bars"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlainMultilineComment0,
            HAS_MULTILINE_SCALAR
            ,
            "# comment\nfoo\nbar\n\nfew\nbars"
            ,
            "foo bar\n\n  few bars\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :foo bar\\nfew bars\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo bar\nfew bars"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlainMultilineComment1,
            HAS_MULTILINE_SCALAR
            ,
            "foo\nbar\n\nfew\nbars\n# comment\n"
            ,
            "foo bar\n\n  few bars\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :foo bar\\nfew bars\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo bar\nfew bars"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(DocScalarPlainIndentation,
            HAS_MULTILINE_SCALAR,
            "  foo\n"
            " bar\n"
            "baz\n"
            "--- foo\n"
            "bar\n"
            "baz\n"
            "---\n"
            "- foo\n"
            " bar\n"
            "- baz\n"
            "---\n"
            "aaa: bbb\n"
            " ccc\n"
            "ddd: eee\n"
            ,
            "--- foo bar baz\n"
            "--- foo bar baz\n"
            "---\n"
            "- foo bar\n"
            "- baz\n"
            "---\n"
            "aaa: bbb ccc\n"
            "ddd: eee\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :foo bar baz\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :foo bar baz\n"
            "-DOC\n"
            "+DOC ---\n"
            "+SEQ\n"
            "=VAL :foo bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "=VAL :aaa\n"
            "=VAL :bbb ccc\n"
            "=VAL :ddd\n"
            "=VAL :eee\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo bar baz"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo bar baz"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("foo bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("aaa"));
    ___(ps.set_val_scalar_plain("bbb ccc"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("ddd"));
    ___(ps.set_val_scalar_plain("eee"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocScalarSQuoted,
            "'foo'\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL 'foo\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_squoted("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarSQuotedExpl,
            "--- ''"
            ,
            "--- ''\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL '\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_squoted(""));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarSQuotedMultiline,
            HAS_MULTILINE_SCALAR
            ,
            "'foo\nbar\nbaz\n\nbat'\n"
            ,
            "'foo bar baz\n\n  bat'\n",
            "+STR\n"
            "+DOC\n"
            "=VAL 'foo bar baz\\nbat\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_squoted("foo bar baz\nbat"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarSQuotedMultilineComment0,
            HAS_MULTILINE_SCALAR
            ,
            "# a comment\n'foo\nbar\nbaz\n\nbat'\n"
            ,
            "'foo bar baz\n\n  bat'\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL 'foo bar baz\\nbat\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_squoted("foo bar baz\nbat"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarSQuotedMultilineComment1,
            HAS_MULTILINE_SCALAR
            ,
            "'foo\nbar\nbaz\n\nbat'\n# a comment\n"
            ,
            "'foo bar baz\n\n  bat'\n",
            "+STR\n"
            "+DOC\n"
            "=VAL 'foo bar baz\\nbat\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_squoted("foo bar baz\nbat"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocScalarDQuoted,
            "\"foo\"\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL \"foo\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_dquoted("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarDQuotedExpl,
            "--- \"\""
            ,
            "--- \"\"\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL \"\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_dquoted(""));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarDQuotedMultiline,
            HAS_MULTILINE_SCALAR
            ,
            "\"foo\nbar\nbaz\n\nbat\"\n"
            ,
            "\"foo bar baz\\nbat\"\n",
            "+STR\n"
            "+DOC\n"
            "=VAL \"foo bar baz\\nbat\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_dquoted("foo bar baz\nbat"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarDQuotedMultilineComment0,
            HAS_MULTILINE_SCALAR
            ,
            "# a comment\n\"foo\nbar\nbaz\n\nbat\"\n"
            ,
            "\"foo bar baz\\nbat\"\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL \"foo bar baz\\nbat\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_dquoted("foo bar baz\nbat"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarDQuotedMultilineComment1,
            HAS_MULTILINE_SCALAR
            ,
            "\"foo\nbar\nbaz\n\nbat\"\n# a comment\n"
            ,
            "\"foo bar baz\\nbat\"\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL \"foo bar baz\\nbat\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_dquoted("foo bar baz\nbat"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocScalarLiteral,
            HAS_MULTILINE_SCALAR
            ,
            "|-\n  foo\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL |foo\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_literal("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarLiteralExpl,
            HAS_MULTILINE_SCALAR
            ,
            "--- |1-"
            ,
            "--- |-\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL |\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_literal(""));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarLiteralComment0,
            HAS_MULTILINE_SCALAR
            ,
            "# comment\n|-\n  foo\n"
            ,
            "|-\n  foo\n",
            "+STR\n"
            "+DOC\n"
            "=VAL |foo\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_literal("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarLiteralComment1,
            HAS_MULTILINE_SCALAR
            ,
            "|-\n  foo\n# comment\n"
            ,
            "|-\n  foo\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL |foo\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_literal("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocScalarFolded,
            HAS_MULTILINE_SCALAR
            ,
            ">-\n  foo\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL >foo\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_folded("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarFoldedExpl,
            HAS_MULTILINE_SCALAR
            ,
            "--- >1-"
            ,
            "--- >-\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL >\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_folded(""));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarFoldedComment0,
            HAS_MULTILINE_SCALAR
            ,
            "# comment\n>-\n  foo\n"
            ,
            ">-\n  foo\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL >foo\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_folded("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarFoldedComment1,
            HAS_MULTILINE_SCALAR
            ,
            ">-\n  foo\n# comment\n"
            ,
            ">-\n  foo\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL >foo\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_folded("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(ScalarMixedDoc,
            HAS_MULTILINE_SCALAR,
            ". \"dquo\" plain 'squo'"
            ,
            ". \"dquo\" plain 'squo'\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :. \"dquo\" plain 'squo'\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain(". \"dquo\" plain 'squo'"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(ScalarMixedMap,
            HAS_MULTILINE_SCALAR
            ,
            "foo: . \"dquo\" plain 'squo'"
            ,
            "foo: . \"dquo\" plain 'squo'\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :. \"dquo\" plain 'squo'\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain(". \"dquo\" plain 'squo'"));
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
