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

ENGINE_TEST(SimpleMapFlow,
            ("{foo: bar,foo2: bar2}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedMapFlow,
            (HAS_CONTAINER_KEYS, Location(1,1,2), "{{}: {}}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "-MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map());
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedMap3FlowEmpty,
            (HAS_CONTAINER_KEYS, Location(1,1,2), "{{{}: {}}: {{}: {}}}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "-MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "-MAP\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "-MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map());
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map());
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapFlowMultiline,
            ("{\nfoo:\n bar\n,\nfoo2:\nbar2\n}", "{foo: bar,foo2: bar2}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(SimpleMapFlowErr0, Location(1,1,2), "{")
ENGINE_TEST_ERRLOC(SimpleMapFlowErr1, Location(5,1,6), "{a: b")


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleMapBlock,
            ("foo: bar\nfoo2: bar2\nfoo3: bar3\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :foo3\n"
            "=VAL :bar3\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo3"));
    ___(ps.set_val_scalar_plain("bar3"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapBlockEmptyVals,
            ("a:\nb:\nc:\nd:\n", "a: \nb: \nc: \nd: \n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :\n"
            "=VAL :b\n"
            "=VAL :\n"
            "=VAL :c\n"
            "=VAL :\n"
            "=VAL :d\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_map_val_block();
    ps.set_key_scalar_plain("a");
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain("b");
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain("c");
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain("d");
    ps.set_val_scalar_plain({});
    ps.end_map();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleMapBlockEmptyKeys,
            (": a\n: b\n: c\n: d\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :a\n"
            "=VAL :\n"
            "=VAL :b\n"
            "=VAL :\n"
            "=VAL :c\n"
            "=VAL :\n"
            "=VAL :d\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_map_val_block();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain("a");
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain("b");
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain("c");
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain("d");
    ps.end_map();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleMapBlockEmpty,
            (":\n:\n:\n:\n", ": \n: \n: \n: \n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_map_val_block();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_key_scalar_plain({});
    ps.set_val_scalar_plain({});
    ps.end_map();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleMapIndentlessSeq,
            ("foo:\n"
             "- bar\n"
             "-\n"
             "baz: qux\n"
             "foo2:\n"
             "- bar2\n"
             "- \n"
             "baz2: qux2\n",
             "foo:\n"
             "  - bar\n"
             "  - \n"
             "baz: qux\n"
             "foo2:\n"
             "  - bar2\n"
             "  - \n"
             "baz2: qux2\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "+SEQ\n"
            "=VAL :bar\n"
            "=VAL :\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "=VAL :qux\n"
            "=VAL :foo2\n"
            "+SEQ\n"
            "=VAL :bar2\n"
            "=VAL :\n"
            "-SEQ\n"
            "=VAL :baz2\n"
            "=VAL :qux2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_map_val_block();
    ps.set_key_scalar_plain("foo");
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain("bar");
    ps.add_sibling();
    ps.set_val_scalar_plain({});
    ps.end_seq();
    ps.add_sibling();
    ps.set_key_scalar_plain("baz");
    ps.set_val_scalar_plain("qux");
    ps.add_sibling();
    ps.set_key_scalar_plain("foo2");
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain("bar2");
    ps.add_sibling();
    ps.set_val_scalar_plain({});
    ps.end_seq();
    ps.add_sibling();
    ps.set_key_scalar_plain("baz2");
    ps.set_val_scalar_plain("qux2");
    ps.end_map();
    ps.end_doc();
    ps.end_stream();
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleMapContainerKeyFlow,
            (HAS_CONTAINER_KEYS,
             "{{this: is, a: keymap}: [and,now,a,seq,val]}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block0_0,
            (HAS_CONTAINER_KEYS,
             "{this: is, a: keymap}: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block0_1,
            (HAS_CONTAINER_KEYS,
             "{this: is, a: keymap}: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block1_0,
            (HAS_CONTAINER_KEYS,
             "[this,is,a,seq,key]: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :key\n"
            "-SEQ\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("this"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("key"));
    ___(ps.end_seq());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block1_1,
            (HAS_CONTAINER_KEYS,
             "[this,is,a,seq,key]: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :key\n"
            "-SEQ\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("this"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("key"));
    ___(ps.end_seq());
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block2_0,
            (HAS_CONTAINER_KEYS,
             "{this: is, a: keymap}: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block2_1,
            (HAS_CONTAINER_KEYS,
             "{this: is, a: keymap}: [and,now,a,seq,val]"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block3_0,
            (HAS_CONTAINER_KEYS,
             "---\n"
             "{a: map}: [a,seq]\n"
             "---\n"
             "[A,SEQ]: {A: MAP}\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :map\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :A\n"
            "=VAL :SEQ\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :A\n"
            "=VAL :MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("map"));
    ___(ps.end_map());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("A"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("SEQ"));
    ___(ps.end_seq());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("A"));
    ___(ps.set_val_scalar_plain("MAP"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapContainerKey1Block3_1,
            (HAS_CONTAINER_KEYS,
             "---\n"
             "{a: map}: [a,seq]\n"
             "---\n"
             "[A,SEQ]: {A: MAP}\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :map\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :A\n"
            "=VAL :SEQ\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :A\n"
            "=VAL :MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("map"));
    ___(ps.end_map());
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("A"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("SEQ"));
    ___(ps.end_seq());
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("A"));
    ___(ps.set_val_scalar_plain("MAP"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


// the examples above have the starting '[' / '{' at the beginning,
// where it is parsed in UNK state. This one has those tokens already
// in RMAP|BLCK|RKEY state, ie, they don't come first.
ENGINE_TEST(SimpleMapContainerKey2Block_1,
            (HAS_CONTAINER_KEYS,
             "\n"
             "foo: bar\n"
             "!maptag &mapanchor {this: is, a: keymap}: [and,now,a,seq,val]\n"
             "!seqtag &seqanchor [now, reversed]: {of: course}\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "+MAP {} &mapanchor <!maptag>\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "+SEQ [] &seqanchor <!seqtag>\n"
            "=VAL :now\n"
            "=VAL :reversed\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :of\n"
            "=VAL :course\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!maptag"));
    ___(ps.set_key_anchor("mapanchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!seqtag"));
    ___(ps.set_key_anchor("seqanchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("reversed"));
    ___(ps.end_seq());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("of"));
    ___(ps.set_val_scalar_plain("course"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleSeqFlow,
            ("[foo,bar,baz]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST_ERRLOC(SimpleSeqFlowErr0, Location(1,1,2), "[")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr1, Location(2,1,3), "[[")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr2, Location(3,1,4), "[[]")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr3, Location(5,1,6), "[[[]]")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr4, Location(4,1,5), "[foo")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr5, Location(5,1,6), "[foo,")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr6, Location(12,1,13), "[foo,bar,baz")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr7, Location(8,1,9), "[[a,b,c]")


ENGINE_TEST(NestedSeqFlowEmpty,
            ("[[]]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedSeq3FlowEmpty,
            ("[[[]]]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedSeq4FlowEmpty,
            ("[[[[]]]]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleSeqFlowMultiline,
            ("[\nfoo\n,\nbar\n,\nbaz\n]", "[foo,bar,baz]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleSeqBlock,
            ("- foo\n- bar\n- baz\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleSeqBlock2,
            ("- [foo, bar, baz]\n- {foo: bar,baz}\n",
             "- [foo,bar,baz]\n- {foo: bar,baz: }\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
      ___(ps.begin_seq_val_flow());
        ___(ps.set_val_scalar_plain("foo"));
        ___(ps.add_sibling());
        ___(ps.set_val_scalar_plain("bar"));
        ___(ps.add_sibling());
        ___(ps.set_val_scalar_plain("baz"));
      ___(ps.end_seq());
      ___(ps.add_sibling());
      ___(ps.begin_map_val_flow());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("bar"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("baz"));
        ___(ps.set_val_scalar_plain({}));
      ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(SimpleSeqBlockPlainScalar0,
            (HAS_MULTILINE_SCALAR,
             "- a\n"
             "  b\n"
             ,
             "- a b\n")
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a b\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain("a b");
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockPlainScalar1,
            (HAS_MULTILINE_SCALAR,
             "- a\n"
             "  - b - c\n",
             "- a - b - c\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a - b - c\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain("a - b - c");
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockComment0,
            ("-   &wtf\n"
             "   # this is a comment\n"
             "  foo\n"
             "\n",
             "- &wtf foo\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL &wtf :foo\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_anchor("wtf");
    ps.set_val_scalar_plain("foo");
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockComment1,
            ("-   &wtf\n"
             "  foo\n"
             "   # this is a comment\n"
             "\n", "- &wtf foo\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL &wtf :foo\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_anchor("wtf");
    ps.set_val_scalar_plain("foo");
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockEmptyScalars,
            ("-\n-\n-\n-\n", "- \n- \n- \n- \n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_val_scalar_plain({});
    ps.add_sibling();
    ps.set_val_scalar_plain({});
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockEmptyLiterals,
            (HAS_MULTILINE_SCALAR,
             "- |\n- |\n- |\n- |\n",
             "- |-\n- |-\n- |-\n- |-\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL |\n"
            "=VAL |\n"
            "=VAL |\n"
            "=VAL |\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_scalar_literal({});
    ps.add_sibling();
    ps.set_val_scalar_literal({});
    ps.add_sibling();
    ps.set_val_scalar_literal({});
    ps.add_sibling();
    ps.set_val_scalar_literal({});
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}

ENGINE_TEST(SimpleSeqBlockEmptyFolded,
            (HAS_MULTILINE_SCALAR,
             "- >\n- >\n- >\n- >\n",
             "- >-\n- >-\n- >-\n- >-\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL >\n"
            "=VAL >\n"
            "=VAL >\n"
            "=VAL >\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ps.begin_stream();
    ps.begin_doc();
    ps.begin_seq_val_block();
    ps.set_val_scalar_folded({});
    ps.add_sibling();
    ps.set_val_scalar_folded({});
    ps.add_sibling();
    ps.set_val_scalar_folded({});
    ps.add_sibling();
    ps.set_val_scalar_folded({});
    ps.end_seq();
    ps.end_doc();
    ps.end_stream();
}


//-----------------------------------------------------------------------------

ENGINE_TEST(MapMapFlow,
            ("{map1: {foo1: bar1,FOO1: BAR1},map2: {foo2: bar2,FOO2: BAR2}}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :map1\n"
            "+MAP {}\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :FOO1\n"
            "=VAL :BAR1\n"
            "-MAP\n"
            "=VAL :map2\n"
            "+MAP {}\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :FOO2\n"
            "=VAL :BAR2\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("map1"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo1"));
    ___(ps.set_val_scalar_plain("bar1"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("FOO1"));
    ___(ps.set_val_scalar_plain("BAR1"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("map2"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("FOO2"));
    ___(ps.set_val_scalar_plain("BAR2"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(MapMapBlock,
            ("map1:\n"
             "  foo1: bar1\n"
             "  FOO1: BAR1\n"
             "map2:\n"
             "  foo2: bar2\n"
             "  FOO2: BAR2\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map1\n"
            "+MAP\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :FOO1\n"
            "=VAL :BAR1\n"
            "-MAP\n"
            "=VAL :map2\n"
            "+MAP\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :FOO2\n"
            "=VAL :BAR2\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
      ___(ps.set_key_scalar_plain("map1"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo1"));
        ___(ps.set_val_scalar_plain("bar1"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("FOO1"));
        ___(ps.set_val_scalar_plain("BAR1"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo2"));
        ___(ps.set_val_scalar_plain("bar2"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("FOO2"));
        ___(ps.set_val_scalar_plain("BAR2"));
      ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapMapMapBlock,
            ("map0:\n"
             "  map01:\n"
             "    foo01: bar01\n"
             "    FOO01: BAR01\n"
             "  map02:\n"
             "    foo02: bar02\n"
             "    FOO02: BAR02\n"
             "    child02:\n"
             "      foo020: bar020\n"
             "      foo021: bar021\n"
             "map1:\n"
             "  map11:\n"
             "    foo11: bar11\n"
             "    FOO11: BAR11\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map0\n"
            "+MAP\n"
            "=VAL :map01\n"
            "+MAP\n"
            "=VAL :foo01\n"
            "=VAL :bar01\n"
            "=VAL :FOO01\n"
            "=VAL :BAR01\n"
            "-MAP\n"
            "=VAL :map02\n"
            "+MAP\n"
            "=VAL :foo02\n"
            "=VAL :bar02\n"
            "=VAL :FOO02\n"
            "=VAL :BAR02\n"
            "=VAL :child02\n"
            "+MAP\n"
            "=VAL :foo020\n"
            "=VAL :bar020\n"
            "=VAL :foo021\n"
            "=VAL :bar021\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "=VAL :map1\n"
            "+MAP\n"
            "=VAL :map11\n"
            "+MAP\n"
            "=VAL :foo11\n"
            "=VAL :bar11\n"
            "=VAL :FOO11\n"
            "=VAL :BAR11\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
      ___(ps.set_key_scalar_plain("map0"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("map01"));
        ___(ps.begin_map_val_block());
          ___(ps.set_key_scalar_plain("foo01"));
          ___(ps.set_val_scalar_plain("bar01"));
          ___(ps.add_sibling());
          ___(ps.set_key_scalar_plain("FOO01"));
          ___(ps.set_val_scalar_plain("BAR01"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("map02"));
        ___(ps.begin_map_val_block());
          ___(ps.set_key_scalar_plain("foo02"));
          ___(ps.set_val_scalar_plain("bar02"));
          ___(ps.add_sibling());
          ___(ps.set_key_scalar_plain("FOO02"));
          ___(ps.set_val_scalar_plain("BAR02"));
          ___(ps.add_sibling());
          ___(ps.set_key_scalar_plain("child02"));
          ___(ps.begin_map_val_block());
            ___(ps.set_key_scalar_plain("foo020"));
            ___(ps.set_val_scalar_plain("bar020"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("foo021"));
            ___(ps.set_val_scalar_plain("bar021"));
          ___(ps.end_map());
        ___(ps.end_map());
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("map1"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("map11"));
        ___(ps.begin_map_val_block());
          ___(ps.set_key_scalar_plain("foo11"));
          ___(ps.set_val_scalar_plain("bar11"));
          ___(ps.add_sibling());
          ___(ps.set_key_scalar_plain("FOO11"));
          ___(ps.set_val_scalar_plain("BAR11"));
        ___(ps.end_map());
      ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SeqSeqFlow,
            ("[[foo1,bar1,baz1],\n[foo2,bar2,baz2]]", "[[foo1,bar1,baz1],[foo2,bar2,baz2]]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :baz1\n"
            "-SEQ\n"
            "+SEQ []\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :baz2\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz1"));
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz2"));
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SeqSeqBlock,
            ("- - foo1\n"
             "  - bar1\n"
             "  - baz1\n"
             "- - foo2\n"
             "  - bar2\n"
             "  - baz2\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+SEQ\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :baz1\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :baz2\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("foo1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz1"));
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("foo2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz2"));
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqSeqSeqBlock,
            ("- - - foo1\n"
             "    - bar1\n"
             "    - baz1\n"
             "  - - foo2\n"
             "    - bar2\n"
             "    - baz2\n"
             "- back\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+SEQ\n"
            "+SEQ\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :baz1\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :baz2\n"
            "-SEQ\n"
            "-SEQ\n"
            "=VAL :back\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
      ___(ps.begin_seq_val_block());
        ___(ps.begin_seq_val_block());
          ___(ps.set_val_scalar_plain("foo1"));
          ___(ps.add_sibling());
          ___(ps.set_val_scalar_plain("bar1"));
          ___(ps.add_sibling());
          ___(ps.set_val_scalar_plain("baz1"));
        ___(ps.end_seq());
        ___(ps.add_sibling());
        ___(ps.begin_seq_val_block());
          ___(ps.set_val_scalar_plain("foo2"));
          ___(ps.add_sibling());
          ___(ps.set_val_scalar_plain("bar2"));
          ___(ps.add_sibling());
          ___(ps.set_val_scalar_plain("baz2"));
          ___(ps.end_seq());
        ___(ps.end_seq());
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("back"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqMapBlock,
            ("- foo: 1\n"
             "  bar: 2\n"
             "- foo: 10\n"
             "  bar: 20\n"
             "- foo: 100\n"
             "  bar: 200\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :1\n"
            "=VAL :bar\n"
            "=VAL :2\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :10\n"
            "=VAL :bar\n"
            "=VAL :20\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :100\n"
            "=VAL :bar\n"
            "=VAL :200\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("1"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("bar"));
        ___(ps.set_val_scalar_plain("2"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("10"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("bar"));
        ___(ps.set_val_scalar_plain("20"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("100"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("bar"));
        ___(ps.set_val_scalar_plain("200"));
      ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(PlainScalarWithColon0,
            ("a::\n", "a:: \n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a:\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a:"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(PlainScalarWithColon1,
            ("key ends with two colons::: value\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :key ends with two colons::\n"
            "=VAL :value\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("key ends with two colons::"));
    ___(ps.set_val_scalar_plain("value"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(PlainScalarWithColonSeq,
            ("- ::\n"
             "- x::\n"
             "- :x::\n",
             "- :: \n"
             "- x:: \n"
             "- :x:: \n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL ::\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :x:\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL ::x:\n"
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
    ___(ps.set_key_scalar_plain(":"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("x:"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain(":x:"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(BlockPlainScalarCommaDoc,
            ("a, b\n"),
            "+STR\n"
            "+DOC\n"
            "=VAL :a, b\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("a, b"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(BlockPlainScalarCommaMap,
            ("a, b: c, d\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a, b\n"
            "=VAL :c, d\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a, b"));
    ___(ps.set_val_scalar_plain("c, d"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(BlockPlainScalarCommaSeq,
            ("- a, b\n- c, d\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a, b\n"
            "=VAL :c, d\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a, b"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("c, d"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorDocVal,
            ("&val a\n"),
            "+STR\n"
            "+DOC\n"
            "=VAL &val :a\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("val"));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorDocRef,
            ("*a\n"),
            "+STR\n"
            "+DOC\n"
            "=ALI *a\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_ref("*a"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorExplDocVal,
            ("&val a\n"),
            "+STR\n"
            "+DOC\n"
            "=VAL &val :a\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("val"));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorSeqBlock,
            ("&seq\n- &val1 val1\n- &val2 val2\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ &seq\n"
            "=VAL &val1 :val1\n"
            "=VAL &val2 :val2\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqMapBlock,
            ("&seq\n"
             "- &key1 key1: &val1 val1\n"
             "  &key2 key2: &val2 val2\n"
             "- &map2\n"
             "  &key10 key10: &val10 val10\n"
             "  &key20 key20: &val20 val20\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ &seq\n"
            "+MAP\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "+MAP &map2\n"
            "=VAL &key10 :key10\n"
            "=VAL &val10 :val10\n"
            "=VAL &key20 :key20\n"
            "=VAL &val20 :val20\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("map2"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("key10"));
    ___(ps.set_key_scalar_plain("key10"));
    ___(ps.set_val_anchor("val10"));
    ___(ps.set_val_scalar_plain("val10"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key20"));
    ___(ps.set_key_scalar_plain("key20"));
    ___(ps.set_val_anchor("val20"));
    ___(ps.set_val_scalar_plain("val20"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqFlow,
            ("&seq [&val1 val1,\n&val2 val2]", "&seq [&val1 val1,&val2 val2]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ [] &seq\n"
            "=VAL &val1 :val1\n"
            "=VAL &val2 :val2\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqBlockWithRef,
            ("&seq\n- &val1 val1\n- *val1\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ &seq\n"
            "=VAL &val1 :val1\n"
            "=ALI *val1\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_ref("*val1"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqFlowWithRef,
            ("&seq [&val1 val1,*val1]", "&seq [&val1 val1,*val1]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ [] &seq\n"
            "=VAL &val1 :val1\n"
            "=ALI *val1\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_ref("*val1"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqFlowWithRefMultiLine,
            ("&seq\n[\n&val1 val1\n,*val1\n]", "&seq [&val1 val1,*val1]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ [] &seq\n"
            "=VAL &val1 :val1\n"
            "=ALI *val1\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_ref("*val1"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorMapBlock,
            ("&map\n&key1 key1: &val1 val1\n&key2 key2: &val2 val2\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &map\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorMapBlockWithRef,
            ("&map\n&rkey1 key1: &rval1 val1\n*rkey1 : *rval1\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &map\n"
            "=VAL &rkey1 :key1\n"
            "=VAL &rval1 :val1\n"
            "=ALI *rkey1\n"
            "=ALI *rval1\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("rkey1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("rval1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_ref("*rkey1"));
    ___(ps.set_val_ref("*rval1"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorMapFlow,
            ("&map\n{&key1 key1: &val1 val1,&key2 key2: &val2 val2}", "&map {&key1 key1: &val1 val1,&key2 key2: &val2 val2}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {} &map\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorMapFlowMultiLine,
            ("&map\n{&key1 key1: &val1 val1,\n&key2 key2: &val2 val2\n}", "&map {&key1 key1: &val1 val1,&key2 key2: &val2 val2}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {} &map\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

// WATCHOUT: see https://play.yaml.io/main/parser?input=Jm1hcAomcmtleTEgZm9vOiAmcnZhbDEgYmFyCipya2V5MSA6ICpydmFsMQ==
ENGINE_TEST(AnchorMapFlowWithRef,
            ("&map\n{&rkey1 key1: &rval1 val1,*rkey1 : *rval1}", "&map {&rkey1 key1: &rval1 val1,*rkey1 : *rval1}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {} &map\n"
            "=VAL &rkey1 :key1\n"
            "=VAL &rval1 :val1\n"
            "=ALI *rkey1\n"
            "=ALI *rval1\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("rkey1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("rval1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_ref("*rkey1"));
    ___(ps.set_val_ref("*rval1"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

// WATCHOUT: see https://play.yaml.io/main/parser?input=Jm1hcAomcmtleTEgZm9vOiAmcnZhbDEgYmFyCipya2V5MSA6ICpydmFsMQ==
ENGINE_TEST(AnchorMapFlowWithRefMultiLine,
            ("&map\n{\n&rkey1 key1: &rval1 val1,\n*rkey1 : *rval1\n}", "&map {&rkey1 key1: &rval1 val1,*rkey1 : *rval1}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {} &map\n"
            "=VAL &rkey1 :key1\n"
            "=VAL &rval1 :val1\n"
            "=ALI *rkey1\n"
            "=ALI *rval1\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("rkey1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("rval1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_ref("*rkey1"));
    ___(ps.set_val_ref("*rval1"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorMapMapBlock,
            ("&map\n&mapkey map: &mapval\n  &key1 key1: &val1 val1\n  &key2 key2: &val2 val2\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &map\n"
            "=VAL &mapkey :map\n"
            "+MAP &mapval\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapkey"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.set_val_anchor("mapval"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorMapMapFlow,
            ("&map\n{&mapkey map: &mapval {&key1 key1: &val1 val1,&key2 key2: &val2 val2,key3: val3},key4: val4}",
             "&map {&mapkey map: &mapval {&key1 key1: &val1 val1,&key2 key2: &val2 val2,key3: val3},key4: val4}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {} &map\n"
            "=VAL &mapkey :map\n"
            "+MAP {} &mapval\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "=VAL :key3\n"
            "=VAL :val3\n"
            "-MAP\n"
            "=VAL :key4\n"
            "=VAL :val4\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("mapkey"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.set_val_anchor("mapval"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("key3"));
    ___(ps.set_val_scalar_plain("val3"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("key4"));
    ___(ps.set_val_scalar_plain("val4"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}



//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorTagPlacement,
            ("- &a0 !b0 foo0: bar0\n"
             "- &a1 !b1\n"
             "  foo1: bar1\n"
             "- &a2\n"
             "  !b2 foo2: bar2\n"
             "- &a3\n"
             "  !b3\n"
             "  foo3: bar3\n"
             "- &a4\n"
             "\n"
             "  !b4\n"
             "\n"
             "  foo4: bar4\n",
             "- !b0 &a0 foo0: bar0\n"
             "- !b1 &a1\n"
             "  foo1: bar1\n"
             "- &a2\n"
             "  !b2 foo2: bar2\n"
             "- !b3 &a3\n"
             "  foo3: bar3\n"
             "- !b4 &a4\n"
             "  foo4: bar4\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL &a0 <!b0> :foo0\n"
            "=VAL :bar0\n"
            "-MAP\n"
            "+MAP &a1 <!b1>\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "-MAP\n"
            "+MAP &a2\n"
            "=VAL <!b2> :foo2\n"
            "=VAL :bar2\n"
            "-MAP\n"
            "+MAP &a3 <!b3>\n"
            "=VAL :foo3\n"
            "=VAL :bar3\n"
            "-MAP\n"
            "+MAP &a4 <!b4>\n"
            "=VAL :foo4\n"
            "=VAL :bar4\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a0"));
    ___(ps.set_key_tag("!b0"));
    ___(ps.set_key_scalar_plain("foo0"));
    ___(ps.set_val_scalar_plain("bar0"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("a1"));
    ___(ps.set_val_tag("!b1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo1"));
    ___(ps.set_val_scalar_plain("bar1"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("a2"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!b2"));
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!b3"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo3"));
    ___(ps.set_val_scalar_plain("bar3"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("a4"));
    ___(ps.set_val_tag("!b4"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo4"));
    ___(ps.set_val_scalar_plain("bar4"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(AnchorMapMapSuckerPunch,
            ("!mymap &mymap\n"
             "&fooanch !footag foo: &seq\n"
             "  &key1 key1: &val1 val1\n"
             "  &key2 key2: &val2 val2\n"
             "!bartag &baranch bar: !a &map2\n"
             "  !b &key10 key10: !c &val10 val10\n"
             "  &key20 !b key20: &val20 !c val20\n"
             "  key10: 20\n"
             "&bazanch !baztag baz:   &map2 !a\n"
             "    &key10 !b key10: &val10 !c val10\n"
             "    !b &key20 key20: !c &val20 val20\n"
             "    key10: 20\n"
             "brr: &map2\n"
             "  !a foo: bar\n"
             "bra: &map2\n"
             "\n"
             "\n"
             "\n"
             "  !a foo: bar\n"
             "bre: &map2\n"
             "  !a\n"
             "  \n"
             "  \n"
             "  foo: bar\n"
             "bru:   &wtf\n"
             "  foo\n"
             ,
             "!mymap &mymap\n"
             "!footag &fooanch foo: &seq\n"
             "  &key1 key1: &val1 val1\n"
             "  &key2 key2: &val2 val2\n"
             "!bartag &baranch bar: !a &map2\n"
             "  !b &key10 key10: !c &val10 val10\n"
             "  !b &key20 key20: !c &val20 val20\n"
             "  key10: 20\n"
             "!baztag &bazanch baz: !a &map2\n"
             "  !b &key10 key10: !c &val10 val10\n"
             "  !b &key20 key20: !c &val20 val20\n"
             "  key10: 20\n"
             "brr: &map2\n"
             "  !a foo: bar\n"
             "bra: &map2\n"
             "  !a foo: bar\n"
             "bre: !a &map2\n"
             "  foo: bar\n"
             "bru: &wtf foo\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &mymap <!mymap>\n"
            "=VAL &fooanch <!footag> :foo\n"
            "+MAP &seq\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "=VAL &baranch <!bartag> :bar\n"
            "+MAP &map2 <!a>\n"
            "=VAL &key10 <!b> :key10\n"
            "=VAL &val10 <!c> :val10\n"
            "=VAL &key20 <!b> :key20\n"
            "=VAL &val20 <!c> :val20\n"
            "=VAL :key10\n"
            "=VAL :20\n"
            "-MAP\n"
            "=VAL &bazanch <!baztag> :baz\n"
            "+MAP &map2 <!a>\n"
            "=VAL &key10 <!b> :key10\n"
            "=VAL &val10 <!c> :val10\n"
            "=VAL &key20 <!b> :key20\n"
            "=VAL &val20 <!c> :val20\n"
            "=VAL :key10\n"
            "=VAL :20\n"
            "-MAP\n"
            "=VAL :brr\n"
            "+MAP &map2\n"
            "=VAL <!a> :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :bra\n"
            "+MAP &map2\n"
            "=VAL <!a> :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :bre\n"
            "+MAP &map2 <!a>\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :bru\n"
            "=VAL &wtf :foo\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!mymap"));
    ___(ps.set_val_anchor("mymap"));
    ___(ps.begin_map_val_block());
      ___(ps.set_key_tag("!footag"));
      ___(ps.set_key_anchor("fooanch"));
      ___(ps.set_key_scalar_plain("foo"));
      ___(ps.set_val_anchor("seq"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_anchor("key1"));
        ___(ps.set_key_scalar_plain("key1"));
        ___(ps.set_val_anchor("val1"));
        ___(ps.set_val_scalar_plain("val1"));
        ___(ps.add_sibling());
        ___(ps.set_key_anchor("key2"));
        ___(ps.set_key_scalar_plain("key2"));
        ___(ps.set_val_anchor("val2"));
        ___(ps.set_val_scalar_plain("val2"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_tag("!bartag"));
      ___(ps.set_key_anchor("baranch"));
      ___(ps.set_key_scalar_plain("bar"));
      ___(ps.set_val_tag("!a"));
      ___(ps.set_val_anchor("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_tag("!b"));
        ___(ps.set_key_anchor("key10"));
        ___(ps.set_key_scalar_plain("key10"));
        ___(ps.set_val_tag("!c"));
        ___(ps.set_val_anchor("val10"));
        ___(ps.set_val_scalar_plain("val10"));
        ___(ps.add_sibling());
        ___(ps.set_key_tag("!b"));
        ___(ps.set_key_anchor("key20"));
        ___(ps.set_key_scalar_plain("key20"));
        ___(ps.set_val_tag("!c"));
        ___(ps.set_val_anchor("val20"));
        ___(ps.set_val_scalar_plain("val20"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("key10"));
        ___(ps.set_val_scalar_plain("20"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_tag("!baztag"));
      ___(ps.set_key_anchor("bazanch"));
      ___(ps.set_key_scalar_plain("baz"));
      ___(ps.set_val_tag("!a"));
      ___(ps.set_val_anchor("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_tag("!b"));
        ___(ps.set_key_anchor("key10"));
        ___(ps.set_key_scalar_plain("key10"));
        ___(ps.set_val_tag("!c"));
        ___(ps.set_val_anchor("val10"));
        ___(ps.set_val_scalar_plain("val10"));
        ___(ps.add_sibling());
        ___(ps.set_key_tag("!b"));
        ___(ps.set_key_anchor("key20"));
        ___(ps.set_key_scalar_plain("key20"));
        ___(ps.set_val_tag("!c"));
        ___(ps.set_val_anchor("val20"));
        ___(ps.set_val_scalar_plain("val20"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("key10"));
        ___(ps.set_val_scalar_plain("20"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("brr"));
      ___(ps.set_val_anchor("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_tag("!a"));
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("bar"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("bra"));
      ___(ps.set_val_anchor("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_tag("!a"));
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("bar"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("bre"));
      ___(ps.set_val_anchor("map2"));
      ___(ps.set_val_tag("!a"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("bar"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("bru"));
      ___(ps.set_val_anchor("wtf"));
      ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqMapSuckerPunch,
            ("&seq\n"
             "- &key1 key1: &val1 val1\n"
             "  &key2 key2: &val2 val2\n"
             "- !a &map2a\n"
             "  !b &key10 key10: &val10 val10\n"
             "  &key20 key20: &val20 val20\n"
             "  key10: 20\n"
             "-   &map2x !a\n"
             "    &key10 !b key10: &val10 val10\n"
             "    &key20 key20: &val20 val20\n"
             "    key10: 20\n"
             "- &map2y\n"
             "  !a foo: bar\n"
             "- &map2z\n"
             "\n"
             "\n"
             "\n"
             "  !a foo: bar\n"
             "- &map2u\n"
             "  !a\n"
             "  \n"
             "  \n"
             "  foo: bar\n"
             "-   &wtf\n"
             "  foo\n"
             "  ",
             "&seq\n"
             "- &key1 key1: &val1 val1\n"
             "  &key2 key2: &val2 val2\n"
             "- !a &map2a\n"
             "  !b &key10 key10: &val10 val10\n"
             "  &key20 key20: &val20 val20\n"
             "  key10: 20\n"
             "- !a &map2x\n"
             "  !b &key10 key10: &val10 val10\n"
             "  &key20 key20: &val20 val20\n"
             "  key10: 20\n"
             "- &map2y\n"
             "  !a foo: bar\n"
             "- &map2z\n"
             "  !a foo: bar\n"
             "- !a &map2u\n"
             "  foo: bar\n"
             "- &wtf foo\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ &seq\n"
            "+MAP\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "+MAP &map2a <!a>\n"
            "=VAL &key10 <!b> :key10\n"
            "=VAL &val10 :val10\n"
            "=VAL &key20 :key20\n"
            "=VAL &val20 :val20\n"
            "=VAL :key10\n"
            "=VAL :20\n"
            "-MAP\n"
            "+MAP &map2x <!a>\n"
            "=VAL &key10 <!b> :key10\n"
            "=VAL &val10 :val10\n"
            "=VAL &key20 :key20\n"
            "=VAL &val20 :val20\n"
            "=VAL :key10\n"
            "=VAL :20\n"
            "-MAP\n"
            "+MAP &map2y\n"
            "=VAL <!a> :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "+MAP &map2z\n"
            "=VAL <!a> :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "+MAP &map2u <!a>\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL &wtf :foo\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.set_key_anchor("key1"));
            ___(ps.set_key_scalar_plain("key1"));
            ___(ps.set_val_anchor("val1"));
            ___(ps.set_val_scalar_plain("val1"));
            ___(ps.add_sibling());
            ___(ps.set_key_anchor("key2"));
            ___(ps.set_key_scalar_plain("key2"));
            ___(ps.set_val_anchor("val2"));
            ___(ps.set_val_scalar_plain("val2"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("map2a"));
        ___(ps.set_val_tag("!a"));
        ___(ps.begin_map_val_block());
            ___(ps.set_key_anchor("key10"));
            ___(ps.set_key_tag("!b"));
            ___(ps.set_key_scalar_plain("key10"));
            ___(ps.set_val_anchor("val10"));
            ___(ps.set_val_scalar_plain("val10"));
            ___(ps.add_sibling());
            ___(ps.set_key_anchor("key20"));
            ___(ps.set_key_scalar_plain("key20"));
            ___(ps.set_val_anchor("val20"));
            ___(ps.set_val_scalar_plain("val20"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("key10"));
            ___(ps.set_val_scalar_plain("20"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("map2x"));
        ___(ps.set_val_tag("!a"));
        ___(ps.begin_map_val_block());
            ___(ps.set_key_anchor("key10"));
            ___(ps.set_key_tag("!b"));
            ___(ps.set_key_scalar_plain("key10"));
            ___(ps.set_val_anchor("val10"));
            ___(ps.set_val_scalar_plain("val10"));
            ___(ps.add_sibling());
            ___(ps.set_key_anchor("key20"));
            ___(ps.set_key_scalar_plain("key20"));
            ___(ps.set_val_anchor("val20"));
            ___(ps.set_val_scalar_plain("val20"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("key10"));
            ___(ps.set_val_scalar_plain("20"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("map2y"));
        ___(ps.begin_map_val_block());
            ___(ps.set_key_tag("!a"));
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("map2z"));
        ___(ps.begin_map_val_block());
            ___(ps.set_key_tag("!a"));
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("map2u"));
        ___(ps.set_val_tag("!a"));
        ___(ps.begin_map_val_block());
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("wtf"));
        ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(DocDoubleAnchorSeqMap,
            (HAS_CONTAINER_KEYS,
             "--- &docanchor\n"
             "&anchor [seq, yeah]: foo\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP &docanchor\n"
            "+SEQ [] &anchor\n"
            "=VAL :seq\n"
            "=VAL :yeah\n"
            "-SEQ\n"
            "=VAL :foo\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_anchor("docanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.set_val_scalar_plain("yeah"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DoubleAnchorSeqMap,
            (HAS_CONTAINER_KEYS,
             "&docanchor\n"
             "&anchor [seq, yeah]: foo\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &docanchor\n"
            "+SEQ [] &anchor\n"
            "=VAL :seq\n"
            "=VAL :yeah\n"
            "-SEQ\n"
            "=VAL :foo\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("docanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.set_val_scalar_plain("yeah"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocDoubleAnchorKeyMap,
            (HAS_CONTAINER_KEYS,
             "--- &docanchor\n"
             "&anchor {key: val}: foo\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP &docanchor\n"
            "+MAP {} &anchor\n"
            "=VAL :key\n"
            "=VAL :val\n"
            "-MAP\n"
            "=VAL :foo\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_anchor("docanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_val_scalar_plain("key"));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DoubleAnchorKeyMap,
            (HAS_CONTAINER_KEYS,
             "&docanchor\n"
             "&anchor {key: val}: foo\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &docanchor\n"
            "+MAP {} &anchor\n"
            "=VAL :key\n"
            "=VAL :val\n"
            "-MAP\n"
            "=VAL :foo\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("docanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_val_scalar_plain("key"));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------


ENGINE_TEST(TagBlock,
            ("- !light fluorescent\n- notag\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL <!light> :fluorescent\n"
            "=VAL :notag\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("notag"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagFlow,
            ("[!light fluorescent,notag]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <!light> :fluorescent\n"
            "=VAL :notag\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("notag"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DirectiveAndTag,
            ("%YAML 1.2\n"
             "---\n"
             "!light fluorescent\n"
             "...\n"
             "%TAG !m! !my-\n"
             "---\n"
             "!m!light green\n",
             "--- !light fluorescent\n"
             "...\n"
             "%TAG !m! !my-\n"
             "--- !m!light green\n"),
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!light> :fluorescent\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <!my-light> :green\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive("%YAML 1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive("%TAG !m! !my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!m!light"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteUKK6_02_0,
            ("!", "! \n"),
            "+STR\n"
            "+DOC\n"
            "=VAL <!> :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuite6WLZ_0,
            ("--- !foo \"bar\"\n"
             "...\n"
             "%TAG ! tag:example.com,2000:app/\n"
             "--- !foo \"bar\"\n"),
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!foo> \"bar\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <tag:example.com,2000:app/foo> \"bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive("%TAG ! tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuite6WLZ_1,
            ("--- !foo \"bar\"\n"
             "...\n"
             "%TAG ! tag:example.com,2000:app/\n"
             "--- !<tag:example.com,2000:app/foo> \"bar\"\n"),
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!foo> \"bar\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <tag:example.com,2000:app/foo> \"bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive("%TAG ! tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<tag:example.com,2000:app/foo>"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(MapKeyFlow,
            (HAS_CONTAINER_KEYS, Location(1, 1, 2), "{{foo: bar}: baz}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock,
            (HAS_CONTAINER_KEYS, Location(6,1,7), "? foo: bar\n: baz"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlockFlow,
            (HAS_CONTAINER_KEYS, Location(2,1,3), "? {foo: bar}\n: baz"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyFlow,
            (HAS_CONTAINER_KEYS, Location(1,1,2), "{[foo, bar]: baz}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlock,
            (HAS_CONTAINER_KEYS, Location(3,2,2),
             "?\n"
             " - foo\n"
             " - bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlockFlow,
            (HAS_CONTAINER_KEYS, Location(2,1,3),
             "? [foo, bar]\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlock2,
            (HAS_CONTAINER_KEYS, Location(2,2,1),
             "?\n"
             "- foo\n"
             "- bar\n"
             ":\n"
             "- baz\n"
             "- bat\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :baz\n"
            "=VAL :bat\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlock3,
            (HAS_CONTAINER_KEYS, Location(3,2,2),
             "?\n"
             " - foo\n"
             " - bar\n"
             ":\n"
             " - baz\n"
             " - bat\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :baz\n"
            "=VAL :bat\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Squo0,
            (HAS_CONTAINER_KEYS,
             "\n"
             "? 'foo' : bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL 'foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_squoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Squo1,
            (HAS_CONTAINER_KEYS,
             "&blockanchor\n"
             "? &mapkey\n"
             "  &scalarkey 'foo' : bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &blockanchor\n"
            "+MAP &mapkey\n"
            "=VAL &scalarkey 'foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("blockanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapkey"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("scalarkey"));
    ___(ps.set_key_scalar_squoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Dquo0,
            (HAS_CONTAINER_KEYS,
             "\n"
             "? \"foo\" : bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL \"foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_dquoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Dquo1,
            (HAS_CONTAINER_KEYS,
             "&blockanchor\n"
             "? &mapkey\n"
             "  &scalarkey \"foo\" : bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &blockanchor\n"
            "+MAP &mapkey\n"
            "=VAL &scalarkey \"foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("blockanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapkey"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("scalarkey"));
    ___(ps.set_key_scalar_dquoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Ref0,
            ("\n"
             "? *ref\n"
             ": baz\n",
             "*ref : baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=ALI *ref\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Ref1,
            (HAS_CONTAINER_KEYS,
             "\n"
             "? &mapanchor\n"
             "  *ref : bar\n"
             ": baz\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP &mapanchor\n"
            "=ALI *ref\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapanchor"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(Qmrk0,
            (HAS_CONTAINER_KEYS, Location(41,3,19),
             "\n"
             "a simple key: a value\n"
             "? an explicit key: another value\n"),
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
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk1_0,
            ("? an explicit key\n"
             "a simple key: a value\n",
             "an explicit key: \n"
             "a simple key: a value\n"),
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
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a simple key"));
    ___(ps.set_val_scalar_plain("a value"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk1_1,
            (HAS_CONTAINER_KEYS, Location(19,2,19),
             "\n"
             "? an explicit key: another value\n"
             "a simple key: a value\n"),
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
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a simple key"));
    ___(ps.set_val_scalar_plain("a value"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk1_2,
            (HAS_CONTAINER_KEYS, Location(25,2,21),
             "map:\n"
             "  ? an explicit key: another value\n"
             "  a simple key: a value\n"
             "? an explicit key deindented: its value\n"
             "? more: siblings\n"),
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
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("a simple key"));
            ___(ps.set_val_scalar_plain("a value"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.begin_map_key_block());
            ___(ps.set_key_scalar_plain("an explicit key deindented"));
            ___(ps.set_key_scalar_plain("its value"));
        ___(ps.end_map());
        ___(ps.set_val_scalar_plain({}));
        ___(ps.add_sibling());
        ___(ps.begin_map_key_block());
            ___(ps.set_key_scalar_plain("more"));
            ___(ps.set_key_scalar_plain("siblings"));
        ___(ps.end_map());
        ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk2_0,
            ("? an explicit key\n"
             "...\n"
             "? another explicit key\n",
             "---\n"
             "an explicit key: \n"
             "---\n"
             "another explicit key: \n"),
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
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc_expl());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("another explicit key"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk2_1,
            ("? an explicit key\n"
             "---\n"
             "? another explicit key\n",
             "---\n"
             "an explicit key: \n"
             "---\n"
             "another explicit key: \n"),
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
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("another explicit key"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk3,
            ("{ ? an explicit key, ? foo,? bar,?baz:,?bat}",
             "{an explicit key: ,foo: ,bar: ,?baz: ,?bat: }"),
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
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("?baz"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("?bat"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk4,
            ("[ ? an explicit key, ? foo,? bar,?baz:,?bat]",
             "[{an explicit key: },{foo: },{bar: },{?baz: },?bat]"),
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
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("?baz"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("?bat"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk5,
            (HAS_CONTAINER_KEYS, Location(4,1,5),
             "? a: b\n"
             "?\n"
             "?\n"),
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
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(Qmrk6,
            (HAS_CONTAINER_KEYS, Location(21,2,21),
             "\n"
             "- ? an explicit key: another value\n"
             "  a simple key: a value\n"
             "- ? another explicit key: its value\n"),
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
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("a simple key"));
            ___(ps.set_val_scalar_plain("a value"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain("another explicit key"));
                ___(ps.set_val_scalar_plain("its value"));
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
        ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk7,
            (HAS_CONTAINER_KEYS, Location(5,2,5),
             "\n"
             "- ? : x\n"
             "- ? : \n"
             "- ? :\n"),
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
                ___(ps.set_key_scalar_plain({}));
                ___(ps.set_val_scalar_plain("x"));
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain({}));
                ___(ps.set_val_scalar_plain({}));
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain({}));
                ___(ps.set_val_scalar_plain({}));
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
        ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkWithTags,
            (HAS_CONTAINER_KEYS, Location(18,3,6),
             "a1: b1\n"
             "? !at\n"
             " !bt : b2\n"
             ": c3\n"),
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
        ___(ps.set_key_scalar_plain({}));
        ___(ps.set_val_scalar_plain("b2"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("c3"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkFlow0,
            ("{ ? }", "{: }"),
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
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1,
            ("{ ? , }", "{: }"),
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
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Anch,
            ("{ ? &anch , }", "{&anch : }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch"));
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Tag,
            ("{ ? !tag , }", "{!tag : }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Squo,
            ("{ ? 'squo', }", "{'squo': }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1AnchSquo,
            ("{ ? &anch 'squo', }", "{&anch 'squo': }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch"));
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1TagSquo,
            ("{ ? !tag 'squo', }", "{!tag 'squo': }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag> 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Dquo,
            ("{ ? \"dquo\", }", "{\"dquo\": }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1AnchDquo,
            ("{ ? &anch \"dquo\", }", "{&anch \"dquo\": }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch"));
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1TagDquo,
            ("{ ? !tag \"dquo\", }", "{!tag \"dquo\": }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag> \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Ref,
            ("{ ? *ref , }", "{*ref : }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=ALI *ref\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Seq,
            (HAS_CONTAINER_KEYS,
             "{ ? [a, b]: c , }", "{? [a, b] : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1SeqAnchor,
            (HAS_CONTAINER_KEYS,
             "{ ? &anchor [a, b]: c , }", "{? &anchor [a, b] : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ [] &anchor\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1SeqTag,
            (HAS_CONTAINER_KEYS,
             "{ ? !tag [a, b]: c , }", "{? !tag [a, b] : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ [] <!tag>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Map,
            (HAS_CONTAINER_KEYS,
             "{ ? {a: b}: c , }", "{? {a: b} : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1MapAnchor,
            (HAS_CONTAINER_KEYS,
             "{ ? &anchor {a: b}: c , }", "{? &anchor {a: b} : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {} &anchor\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1MapTag,
            (HAS_CONTAINER_KEYS,
             "{ ? !tag {a: b}: c , }", "{? !tag {a: b} : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {} <!tag>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SeqIMap0Ref,
            ("[val0,val1,{key: val2},wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+MAP {}\n"
            "=VAL :key\n"
            "=VAL :val2\n"
            "-MAP\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("key"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap1,
            ("[val0,val1,key: val2,wtf]", "[val0,val1,{key: val2},wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+MAP {}\n"
            "=VAL :key\n"
            "=VAL :val2\n"
            "-MAP\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
        // first this is read as a val because the parent is read as a
        // seq...
    ___(ps.set_val_scalar_plain("key"));
        // ... then this will create a flow map, and move the existing
        // val to the key of the first child
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap2,
            ("[val0,val1,key: [foo: bar],wtf]", "[val0,val1,{key: [{foo: bar}]},wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+MAP {}\n"
            "=VAL :key\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
      ___(ps.set_val_scalar_plain("val0"));
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("val1"));
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("key"));
      ___(ps.actually_val_is_first_key_of_new_map_flow());
        ___(ps.begin_seq_val_flow());
        ___(ps.set_val_scalar_plain("foo"));
        ___(ps.actually_val_is_first_key_of_new_map_flow());
        ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_map());
        ___(ps.end_seq());
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(SeqIMap2Nested,
            ("[val: [0: [1: [2: [3: [4: [5: [6: 7]]]]]]], :]",
             "[{val: [{0: [{1: [{2: [{3: [{4: [{5: [{6: 7}]}]}]}]}]}]}]},{: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :val\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :0\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :1\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :2\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :3\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :4\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :5\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :6\n"
            "=VAL :7\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
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
        ___(ps.set_val_scalar_plain("val"));
        ___(ps.actually_val_is_first_key_of_new_map_flow());
          ___(ps.begin_seq_val_flow());
            ___(ps.set_val_scalar_plain("0"));
            ___(ps.actually_val_is_first_key_of_new_map_flow());
              ___(ps.begin_seq_val_flow());
                ___(ps.set_val_scalar_plain("1"));
                ___(ps.actually_val_is_first_key_of_new_map_flow());
                  ___(ps.begin_seq_val_flow());
                    ___(ps.set_val_scalar_plain("2"));
                    ___(ps.actually_val_is_first_key_of_new_map_flow());
                      ___(ps.begin_seq_val_flow());
                        ___(ps.set_val_scalar_plain("3"));
                        ___(ps.actually_val_is_first_key_of_new_map_flow());
                          ___(ps.begin_seq_val_flow());
                            ___(ps.set_val_scalar_plain("4"));
                            ___(ps.actually_val_is_first_key_of_new_map_flow());
                              ___(ps.begin_seq_val_flow());
                                ___(ps.set_val_scalar_plain("5"));
                                ___(ps.actually_val_is_first_key_of_new_map_flow());
                                  ___(ps.begin_seq_val_flow());
                                    ___(ps.set_val_scalar_plain("6"));
                                    ___(ps.actually_val_is_first_key_of_new_map_flow());
                                      ___(ps.set_val_scalar_plain("7"));
                                    ___(ps.end_map());
                                  ___(ps.end_seq());
                                ___(ps.end_map());
                              ___(ps.end_seq());
                            ___(ps.end_map());
                          ___(ps.end_seq());
                        ___(ps.end_map());
                      ___(ps.end_seq());
                    ___(ps.end_map());
                  ___(ps.end_seq());
                ___(ps.end_map());
              ___(ps.end_seq());
            ___(ps.end_map());
          ___(ps.end_seq());
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_scalar_plain({}));
        ___(ps.actually_val_is_first_key_of_new_map_flow());
          ___(ps.set_val_scalar_plain({}));
        ___(ps.end_map());
      ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(SeqIMap3EmptyKey,
            ("[val0, : wtf]", "[val0,{: wtf}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL :wtf\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap3EmptyVal,
            ("[val0, wtf: ]", "[val0,{wtf: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "+MAP {}\n"
            "=VAL :wtf\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap3EmptyKeyVal,
            ("[val0, : ]", "[val0,{: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
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
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap3EmptyKeyValNested,
            ("[val0: val1, : ]", "[{val0: val1},{: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "-MAP\n"
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
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap3EmptyKeyValNested2,
            ("[val0: [val1: val2], : ]", "[{val0: [{val1: val2}]},{: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :val0\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :val1\n"
            "=VAL :val2\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
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
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap4Ref,
            ("[val0,val1,[rat,rot],wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+SEQ []\n"
            "=VAL :rat\n"
            "=VAL :rot\n"
            "-SEQ\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("rat"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("rot"));
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap4,
            (HAS_CONTAINER_KEYS, Location(20,1,21), "[val0,val1,[rat,rot]: [foo: bar],wtf]", "[val0,val1,{[rat,rot]: [{foo: bar}]},wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :rat\n"
            "=VAL :rot\n"
            "-SEQ\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
      ___(ps.begin_seq_key_flow());
      ___(ps.set_val_scalar_plain("rat"));
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("rot"));
      ___(ps.end_seq());
      ___(ps.begin_seq_val_flow());
        ___(ps.begin_map_val_flow());
          ___(ps.set_key_scalar_plain("foo"));
          ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_map());
      ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap4_actually,
            (HAS_CONTAINER_KEYS, Location(20,1,21), "[val0,val1,[rat,rot]: [foo: bar],wtf]", "[val0,val1,{[rat,rot]: [{foo: bar}]},wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :rat\n"
            "=VAL :rot\n"
            "-SEQ\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
       ___(ps.set_val_scalar_plain("val0"));
       ___(ps.add_sibling());
       ___(ps.set_val_scalar_plain("val1"));
       ___(ps.add_sibling());
       ___(ps.begin_seq_val_flow());
           ___(ps.set_val_scalar_plain("rat"));
           ___(ps.add_sibling());
           ___(ps.set_val_scalar_plain("rot"));
       ___(ps.end_seq());
       ___(ps.actually_val_is_first_key_of_new_map_flow());
           ___(ps.begin_seq_val_flow());
               ___(ps.set_val_scalar_plain("foo"));
               ___(ps.actually_val_is_first_key_of_new_map_flow());
                   ___(ps.set_val_scalar_plain("bar"));
               ___(ps.end_map());
           ___(ps.end_seq());
       ___(ps.end_map());
       ___(ps.add_sibling());
       ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SeqIMap5Squo,
            ("[a: &anchor 'b']", "[{a: &anchor 'b'}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL &anchor 'b\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_anchor("anchor"));
    ___(ps.set_val_scalar_squoted("b"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5Dquo,
            ("[a: &anchor \"b\"]", "[{a: &anchor \"b\"}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL &anchor \"b\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_anchor("anchor"));
    ___(ps.set_val_scalar_dquoted("b"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5Ref,
            ("[a: *ref]", "[{a: *ref}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=ALI *ref\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_ref("*ref"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkNone0,
            ("[? ,]", "[{: }]"),
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
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkNone1,
            ("[? ]", "[{: }]"),
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
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkSquo1,
            ("[? &anchor 'squo' ]", "[{&anchor 'squo': }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &anchor 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkDquo1,
            ("[? &anchor \"dquo\" ]", "[{&anchor \"dquo\": }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &anchor \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkRef1,
            ("[? a: *ref]", "[{a: *ref}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=ALI *ref\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_ref("*ref"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkRef2,
            ("[? *ref: b]", "[{*ref : b}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=ALI *ref\n"
            "=VAL :b\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkSeq,
            (HAS_CONTAINER_KEYS,
             "[? &anchor [a, seq]: ]", "[{&anchor [a, seq]: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "+SEQ [] &anchor\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "-SEQ\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkMap,
            (HAS_CONTAINER_KEYS,
             "[? &anchor {a: map}: ]", "[{&anchor {a: map}: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "+MAP {} &anchor\n"
            "=VAL :a\n"
            "=VAL :map\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("map"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
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


//-----------------------------------------------------------------------------


ENGINE_TEST_ERRLOC(tokens_after_mapflow_0_0, Location(   1,32), "{ first: Sammy, last: Sosa }foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_0_1, Location(   2, 4), "{ first: Sammy, last: Sosa }\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_2_0, Location(   1,29), "{ first: Sammy, last: Sosa }- foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_2_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n- foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_3_0, Location(   1,29), "{ first: Sammy, last: Sosa }[foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_3_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n[foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_4_0, Location(   1,29), "{ first: Sammy, last: Sosa }{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_4_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_5_0, Location(   1,29), "{ first: Sammy, last: Sosa }'foo'")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_5_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n'foo'")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_6_0, Location(   1,29), "{ first: Sammy, last: Sosa }|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_6_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_7_0, Location(   1,29), "{ first: Sammy, last: Sosa }>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_7_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_8_0, Location(   1,33), "{ first: Sammy, last: Sosa }?foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_8_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n?\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_9_0, Location(   1,33), "{ first: Sammy, last: Sosa }:foo")
//ENGINE_TEST_ERRLOC(tokens_after_mapflow_9_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n:\nfoo")//fixme
//ENGINE_TEST_ERRLOC(tokens_after_mapflow_10_0, Location(  1,29), "{ first: Sammy, last: Sosa }: foo")// fixme
//ENGINE_TEST_ERRLOC(tokens_after_mapflow_10_1, Location(  2, 1), "{ first: Sammy, last: Sosa }\n: \nfoo")//fixme
ENGINE_TEST_ERRLOC(tokens_after_mapflow_11_0, Location(  1,34), "{ first: Sammy, last: Sosa } &foo")// fixme
ENGINE_TEST_ERRLOC(tokens_after_mapflow_11_1, Location(  2, 5), "{ first: Sammy, last: Sosa }\n&foo")


ENGINE_TEST_ERRLOC(tokens_after_seqflow_0_0, Location(1,32), "[a,b,c,                   d]foo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_0_1, Location(2, 4), "[a,b,c,                   d]\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_2_0, Location(1,29), "[a,b,c,                   d]- foo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_2_1, Location(2, 1), "[a,b,c,                   d]\n- foo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_3_0, Location(1,29), "[a,b,c,                   d][foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_3_1, Location(2, 1), "[a,b,c,                   d]\n[foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_4_0, Location(1,29), "[a,b,c,                   d]{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_4_1, Location(2, 1), "[a,b,c,                   d]\n{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_5_0, Location(1,29), "[a,b,c,                   d]'foo'")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_5_1, Location(2, 1), "[a,b,c,                   d]\n'foo'")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_6_0, Location(1,29), "[a,b,c,                   d]|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_6_1, Location(2, 1), "[a,b,c,                   d]\n|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_7_0, Location(1,29), "[a,b,c,                   d]>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_7_1, Location(2, 1), "[a,b,c,                   d]\n>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_8_0, Location(1,33), "[a,b,c,                   d]?foo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_8_1, Location(2, 1), "[a,b,c,                   d]\n?\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_9_0, Location(1,33), "[a,b,c,                   d]:foo")// fixme
//ENGINE_TEST_ERRLOC(tokens_after_seqflow_9_1, Location(2, 1), "[a,b,c,                   d]\n:\nfoo")//fixme
//ENGINE_TEST_ERRLOC(tokens_after_seqflow_10_0, Location(1,29), "[a,b,c,                   d]: foo")// fixme
//ENGINE_TEST_ERRLOC(tokens_after_seqflow_10_1, Location(2, 1), "[a,b,c,                   d]\n: \nfoo")//fixme
ENGINE_TEST_ERRLOC(tokens_after_seqflow_11_0, Location(1,34), "[a,b,c,                   d] &foo")// fixme
ENGINE_TEST_ERRLOC(tokens_after_seqflow_11_1, Location(2, 5), "[a,b,c,                   d]\n&foo")

ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq1, Location(1, 9), "[a,b,c]]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq2, Location(1, 9), "[a,b,c]}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq3, Location(1, 8), "[a,b,c]{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq4, Location(1, 8), "[a,b,c][")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq5, Location(1, 8), "[a,b,c]-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq1_0, Location(1,10), "[a,b,c] ]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq2_0, Location(1,10), "[a,b,c] }")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq3_0, Location(1,9), "[a,b,c] {")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq4_0, Location(1,9), "[a,b,c] [")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq5_0, Location(1,9), "[a,b,c] -")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq1_1, Location(2,2), "[a,b,c]\n]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq2_1, Location(2,2), "[a,b,c]\n}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq3_1, Location(2,1), "[a,b,c]\n{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq4_1, Location(2,1), "[a,b,c]\n[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq5_1, Location(2,1), "[a,b,c]\n-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map1, Location(1, 8), "{a: b}]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map2, Location(1, 8), "{a: b}}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map3, Location(1, 7), "{a: b}{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map4, Location(1, 7), "{a: b}[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map5, Location(1, 7), "{a: b}-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map1_0, Location(1,9), "{a: b} ]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map2_0, Location(1,9), "{a: b} }")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map3_0, Location(1,8), "{a: b} {")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map4_0, Location(1,8), "{a: b} [")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map5_0, Location(1,8), "{a: b} -")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map1_1, Location(2,2), "{a: b}\n]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map2_1, Location(2,2), "{a: b}\n}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map3_1, Location(2,1), "{a: b}\n{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map4_1, Location(2,1), "{a: b}\n[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map5_1, Location(2,1), "{a: b}\n-")

ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq1, Location(2, 9), "---\n[a,b,c]]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq2, Location(2, 9), "---\n[a,b,c]}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq3, Location(2, 8), "---\n[a,b,c]{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq4, Location(2, 8), "---\n[a,b,c][")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq5, Location(2, 8), "---\n[a,b,c]-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq1_0, Location(2,10), "---\n[a,b,c] ]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq2_0, Location(2,10), "---\n[a,b,c] }")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq3_0, Location(2,9), "---\n[a,b,c] {")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq4_0, Location(2,9), "---\n[a,b,c] [")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq5_0, Location(2,9), "---\n[a,b,c] -")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq1_1, Location(3,2), "---\n[a,b,c]\n]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq2_1, Location(3,2), "---\n[a,b,c]\n}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq3_1, Location(3,1), "---\n[a,b,c]\n{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq4_1, Location(3,1), "---\n[a,b,c]\n[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq5_1, Location(3,1), "---\n[a,b,c]\n-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map1, Location(2, 8), "---\n{a: b}]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map2, Location(2, 8), "---\n{a: b}}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map3, Location(2, 7), "---\n{a: b}{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map4, Location(2, 7), "---\n{a: b}[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map5, Location(2, 7), "---\n{a: b}-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map1_0, Location(2,9), "---\n{a: b} ]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map2_0, Location(2,9), "---\n{a: b} }")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map3_0, Location(2,8), "---\n{a: b} {")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map4_0, Location(2,8), "---\n{a: b} [")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map5_0, Location(2,8), "---\n{a: b} -")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map1_1, Location(3,2), "---\n{a: b}\n]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map2_1, Location(3,2), "---\n{a: b}\n}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map3_1, Location(3,1), "---\n{a: b}\n{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map4_1, Location(3,1), "---\n{a: b}\n[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map5_1, Location(3,1), "---\n{a: b}\n-")

ENGINE_TEST_ERRLOC(ExtraTokensScalar0, Location(2,6), "word1  # comment\nword2")

ENGINE_TEST(ExtraTokensNoFalseError0,
            ("---\n"
             "map : {foo: bar, notag: none}\n"
             "seq : [foo, bar]\n"
             "...\n",
             "---\n"
             "map: {foo: bar,notag: none}\n"
             "seq: [foo,bar]\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :notag\n"
            "=VAL :none\n"
            "-MAP\n"
            "=VAL :seq\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("map"));
        ___(ps.begin_map_val_flow());
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("notag"));
            ___(ps.set_val_scalar_plain("none"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("seq"));
        ___(ps.begin_seq_val_flow());
            ___(ps.set_val_scalar_plain("foo"));
            ___(ps.add_sibling());
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}

ENGINE_TEST(ExtraTokensNoFalseError1,
            ("---\n"
             "*mapref : {foo: bar, notag: none}\n"
             "*seqref : [foo, bar]\n"
             "...\n",
             "---\n"
             "*mapref : {foo: bar,notag: none}\n"
             "*seqref : [foo,bar]\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "=ALI *mapref\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :notag\n"
            "=VAL :none\n"
            "-MAP\n"
            "=ALI *seqref\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
        ___(ps.set_key_ref("*mapref"));
        ___(ps.begin_map_val_flow());
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("notag"));
            ___(ps.set_val_scalar_plain("none"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_key_ref("*seqref"));
        ___(ps.begin_seq_val_flow());
            ___(ps.set_val_scalar_plain("foo"));
            ___(ps.add_sibling());
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(PlainScalarDoc0,
            ("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~",
             "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n"),
            "+STR\n"
            "+DOC\n"
            "=VAL :a!\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(PlainScalarBlockSeq0,
            ("- a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~",
             "- a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a!\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(PlainScalarBlockMap0,
            ("key: a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n"
             "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~: val\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :key\n"
            "=VAL :a!\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~\n"
            "=VAL :a!\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~\n"
            "=VAL :val\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("key"));
    ___(ps.set_val_scalar_plain("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq0_0, Location(1,29), "[a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~]")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq0_1, Location(1,29), "[a!\"#$%&'()*+,-./09:;<=>?@AZ{|}^_`az{|}~]")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq1_0, Location(2,28), "[\n"
                   "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n]")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq1_1, Location(2,28), "[\n"
                   "a!\"#$%&'()*+,-./09:;<=>?@AZ{|}^_`az{|}~\n]")
ENGINE_TEST_ERRLOC(PlainScalarFlow1Seq0, Location(1,38), "[a!\"#$%&'()*+,-./09:;<=>?@]^_`az{|}~]")
ENGINE_TEST(PlainScalarFlow0Seq1,
            ("[\n"
             "a!\"#$%&'()*+,-./09:;<=>?@AZ\\\n]",
             "[a!\"#$%&'()*+,-./09:;<=>?@AZ\\]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :a!\"#$%&'()*+\n"
            "=VAL :-./09:;<=>?@AZ\\\\\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a!\"#$%&'()*+"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("-./09:;<=>?@AZ\\"));
    ___(ps.end_seq());
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
