#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

namespace c4 {
namespace yml {

ENGINE_TEST(DocEmpty,
            (""),
            "+STR\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocEmptyExpl,
            ("---\n"),
            "+STR\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocEmptyTerm1,
            ("...\n", ""),
            "+STR\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm1Indented1,
            (" ...\n", "  ...\n"),
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
            ("...\n...\n", ""),
            "+STR\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm2Indented0,
            (HAS_MULTILINE_SCALAR,
             " ...\n...\n", "  ...\n"),
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
            (HAS_MULTILINE_SCALAR,
             " ...\n ...\n", "  ... ...\n"),
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
            ("...\n...\n...\n", ""),
            "+STR\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm3_1,
            ("...\n...\n..\n", "..\n"),
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
            ("...\n"
             "...\n"
             "......\n"
             "...\n"
             "...\n"
             ,
             "  ......\n"),
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


//-----------------------------------------------------------------------------

ENGINE_TEST(DocEmptyExplMult,
            ("---\n---\n---\n"),
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
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//-----------------------------------------------------------------------------

ENGINE_TEST(DocEmptyExplMultImplFirst,
            ("foo\n--- bar\n--- baz\n", "--- foo\n--- bar\n--- baz\n"),
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
            ("foo\n"),
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
            ("--- foo\n"),
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

ENGINE_TEST(DocScalarPlain2,
            ("--- ---\n", "---\n  ---\n"),
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
            ("------\n", "  ------\n"),
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
            ("......\n", "  ......\n"),
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
            ("--- ...\n", "---\n  ...\n"),
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
            ("---...\n", "  ---...\n"),
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
            (HAS_MULTILINE_SCALAR,
             "--- foo\n bar\n", "--- foo bar\n"),
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
            (HAS_MULTILINE_SCALAR,
             " --- foo\n bar\n", "  --- foo bar\n"),
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
            (HAS_MULTILINE_SCALAR,
             " --- foo\n bar\n", "  --- foo bar\n"),
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
            (HAS_MULTILINE_SCALAR,
             "foo\nbar\n\nfew\nbars", "foo bar\n\n  few bars\n"),
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
            (HAS_MULTILINE_SCALAR,
             "# comment\nfoo\nbar\n\nfew\nbars", "foo bar\n\n  few bars\n"),
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
            (HAS_MULTILINE_SCALAR,
             "foo\nbar\n\nfew\nbars\n# comment\n", "foo bar\n\n  few bars\n"),
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
            (HAS_MULTILINE_SCALAR,
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
             "ddd: eee\n")
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
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("aaa"));
    ___(ps.set_val_scalar_plain("bbb ccc"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("ddd"));
    ___(ps.set_val_scalar_plain("eee"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocScalarSQuoted,
            ("'foo'\n"),
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

ENGINE_TEST(DocScalarSQuotedMultiline,
            (HAS_MULTILINE_SCALAR, "'foo\nbar\nbaz\n\nbat'\n", "'foo bar baz\n\n  bat'\n"),
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
            (HAS_MULTILINE_SCALAR, "# a comment\n'foo\nbar\nbaz\n\nbat'\n", "'foo bar baz\n\n  bat'\n"),
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
            (HAS_MULTILINE_SCALAR, "'foo\nbar\nbaz\n\nbat'\n# a comment\n", "'foo bar baz\n\n  bat'\n"),
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
            ("\"foo\"\n"),
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

ENGINE_TEST(DocScalarDQuotedMultiline,
            (HAS_MULTILINE_SCALAR,
             "\"foo\nbar\nbaz\n\nbat\"\n",
             "\"foo bar baz\\nbat\"\n"),
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
            (HAS_MULTILINE_SCALAR,
             "# a comment\n\"foo\nbar\nbaz\n\nbat\"\n",
             "\"foo bar baz\\nbat\"\n"),
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
            (HAS_MULTILINE_SCALAR,
             "\"foo\nbar\nbaz\n\nbat\"\n# a comment\n",
             "\"foo bar baz\\nbat\"\n"),
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
            (HAS_MULTILINE_SCALAR, "|-\n  foo\n"),
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

ENGINE_TEST(DocScalarLiteralComment0,
            (HAS_MULTILINE_SCALAR, "# comment\n|-\n  foo\n", "|-\n  foo\n"),
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
            (HAS_MULTILINE_SCALAR, "|-\n  foo\n# comment\n", "|-\n  foo\n"),
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
            (HAS_MULTILINE_SCALAR, ">-\n  foo\n"),
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

ENGINE_TEST(DocScalarFoldedComment0,
            (HAS_MULTILINE_SCALAR, "# comment\n>-\n  foo\n", ">-\n  foo\n"),
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
            (HAS_MULTILINE_SCALAR, ">-\n  foo\n# comment\n", ">-\n  foo\n"),
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
            (HAS_MULTILINE_SCALAR, R"(. "dquo" plain 'squo')", ". \"dquo\" plain 'squo'\n"),
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
            (HAS_MULTILINE_SCALAR, R"(foo: . "dquo" plain 'squo')", "foo: . \"dquo\" plain 'squo'\n"),
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
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocStream,
            ("--- doc0\n--- 'doc1'\n--- \"doc2\"\n"),
            "+STR\n"
            "+DOC ---\n"
            "=VAL :doc0\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL 'doc1\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL \"doc2\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("doc0"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_squoted("doc1"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_dquoted("doc2"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocStreamImplicitDocFirst,
            ("doc0\n--- doc1\n--- doc2\n", "--- doc0\n--- doc1\n--- doc2\n"),
            "+STR\n"
            "+DOC\n"
            "=VAL :doc0\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :doc1\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :doc2\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("doc0"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("doc1"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("doc2"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SuddenDoc0,
            ("- map:\n"
             "   foo: bar\n"
             "...\n"
             "seq:\n"
             "  - a\n"
             "  - b\n"
             "...\n",
             "---\n"
             "- map:\n"
             "    foo: bar\n"
             "---\n"
             "seq:\n"
             "  - a\n"
             "  - b\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC ...\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :seq\n"
            "+SEQ\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc_expl());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}

ENGINE_TEST(SuddenDoc1,
            ("map:\n"
             "  nested:\n"
             "    nested more:\n"
             "      nested even more:\n"
             "      - now inside a seq:\n"
             "          another map:\n"
             "          - another seq\n"
             "          - etc\n"
             "---\n"
             "map:\n"
             "  nested:\n"
             "    nested more:\n"
             "      nested even more:\n"
             "      - now inside a seq:\n"
             "          another map: the value\n"
             "---\n"
             "map:\n"
             "  nested:\n"
             "    nested more:\n"
             "      nested even more:\n"
             "      - now inside a seq:\n"
             "          another map:\n"
             "          - another seq\n"
             "          - etc\n"
             "...\n"
             "map:\n"
             "  nested:\n"
             "    nested more:\n"
             "      nested even more:\n"
             "      - now inside a seq:\n"
             "          another map: the value\n"
             "...\n",
             "---\n"
             "map:\n"
             "  nested:\n"
             "    nested more:\n"
             "      nested even more:\n"
             "        - now inside a seq:\n"
             "            another map:\n"
             "              - another seq\n"
             "              - etc\n"
             "---\n"
             "map:\n"
             "  nested:\n"
             "    nested more:\n"
             "      nested even more:\n"
             "        - now inside a seq:\n"
             "            another map: the value\n"
             "---\n"
             "map:\n"
             "  nested:\n"
             "    nested more:\n"
             "      nested even more:\n"
             "        - now inside a seq:\n"
             "            another map:\n"
             "              - another seq\n"
             "              - etc\n"
             "---\n"
             "map:\n"
             "  nested:\n"
             "    nested more:\n"
             "      nested even more:\n"
             "        - now inside a seq:\n"
             "            another map: the value\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :nested\n"
            "+MAP\n"
            "=VAL :nested more\n"
            "+MAP\n"
            "=VAL :nested even more\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :now inside a seq\n"
            "+MAP\n"
            "=VAL :another map\n"
            "+SEQ\n"
            "=VAL :another seq\n"
            "=VAL :etc\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :nested\n"
            "+MAP\n"
            "=VAL :nested more\n"
            "+MAP\n"
            "=VAL :nested even more\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :now inside a seq\n"
            "+MAP\n"
            "=VAL :another map\n"
            "=VAL :the value\n"
            "-MAP\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :nested\n"
            "+MAP\n"
            "=VAL :nested more\n"
            "+MAP\n"
            "=VAL :nested even more\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :now inside a seq\n"
            "+MAP\n"
            "=VAL :another map\n"
            "+SEQ\n"
            "=VAL :another seq\n"
            "=VAL :etc\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :nested\n"
            "+MAP\n"
            "=VAL :nested more\n"
            "+MAP\n"
            "=VAL :nested even more\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :now inside a seq\n"
            "+MAP\n"
            "=VAL :another map\n"
            "=VAL :the value\n"
            "-MAP\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    auto build_map = [&ps]{
        ___(ps.begin_map_val_block());
            ___(ps.set_key_scalar_plain("map"));
            ___(ps.begin_map_val_block());
                ___(ps.set_key_scalar_plain("nested"));
                ___(ps.begin_map_val_block());
                    ___(ps.set_key_scalar_plain("nested more"));
                    ___(ps.begin_map_val_block());
                        ___(ps.set_key_scalar_plain("nested even more"));
                        ___(ps.begin_seq_val_block());
                            ___(ps.begin_map_val_block());
                                ___(ps.set_key_scalar_plain("now inside a seq"));
                                ___(ps.begin_map_val_block());
                                    ___(ps.set_key_scalar_plain("another map"));
                                    ___(ps.begin_seq_val_block());
                                        ___(ps.set_val_scalar_plain("another seq"));
                                        ___(ps.add_sibling());
                                        ___(ps.set_val_scalar_plain("etc"));
                                    ___(ps.end_seq());
                                ___(ps.end_map());
                            ___(ps.end_map());
                        ___(ps.end_seq());
                    ___(ps.end_map());
                ___(ps.end_map());
            ___(ps.end_map());
        ___(ps.end_map());
    };
    auto build_seq = [&ps]{
        ___(ps.begin_map_val_block());
            ___(ps.set_key_scalar_plain("map"));
            ___(ps.begin_map_val_block());
                ___(ps.set_key_scalar_plain("nested"));
                ___(ps.begin_map_val_block());
                    ___(ps.set_key_scalar_plain("nested more"));
                    ___(ps.begin_map_val_block());
                        ___(ps.set_key_scalar_plain("nested even more"));
                        ___(ps.begin_seq_val_block());
                            ___(ps.begin_map_val_block());
                                ___(ps.set_key_scalar_plain("now inside a seq"));
                                ___(ps.begin_map_val_block());
                                    ___(ps.set_key_scalar_plain("another map"));
                                    ___(ps.set_val_scalar_plain("the value"));
                                ___(ps.end_map());
                            ___(ps.end_map());
                        ___(ps.end_seq());
                    ___(ps.end_map());
                ___(ps.end_map());
            ___(ps.end_map());
        ___(ps.end_map());
    };
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    build_map();
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    build_seq();
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    build_map();
    ___(ps.end_doc_expl());
    ___(ps.begin_doc());
    build_seq();
    ___(ps.end_doc_expl());
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
