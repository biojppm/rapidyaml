#include "./test_lib/test_engine.hpp"

namespace c4 {
namespace yml {

ENGINE_TEST(DocEmpty,
       (""),
        R"(+STR
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocEmptyExpl,
       ("---\n"),
        R"(+STR
+DOC ---
=VAL :
-DOC
-STR
)")
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
            R"(+STR
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm1Indented1,
            (" ...\n", "  ...\n"),
            R"(+STR
+DOC
=VAL :...
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("..."));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm2,
            ("...\n...\n", ""),
            R"(+STR
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm2Indented0,
            (HAS_MULTILINE_SCALAR,
             " ...\n...\n", "  ...\n"),
            R"(+STR
+DOC
=VAL :...
-DOC ...
-STR
)")
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
            R"(+STR
+DOC
=VAL :... ...
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("... ..."));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm3,
            ("...\n...\n...\n", ""),
            R"(+STR
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm3_1,
            ("...\n...\n..\n", "..\n"),
            R"(+STR
+DOC
=VAL :..
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain(".."));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocEmptyTerm4_2,
            (R"(...
...
......
...
...
)",
             "  ......\n"),
            R"(+STR
+DOC
=VAL :......
-DOC ...
-STR
)")
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
        R"(+STR
+DOC ---
=VAL :
-DOC
+DOC ---
=VAL :
-DOC
+DOC ---
=VAL :
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL :foo
-DOC
+DOC ---
=VAL :bar
-DOC
+DOC ---
=VAL :baz
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL :foo
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain1,
       ("--- foo\n"),
        R"(+STR
+DOC ---
=VAL :foo
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain2,
       ("--- ---\n", "---\n  ---\n"),
        R"(+STR
+DOC ---
=VAL :---
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("---"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain2_1,
       ("------\n", "  ------\n"),
        R"(+STR
+DOC
=VAL :------
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("------"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain2_2,
       ("......\n", "  ......\n"),
        R"(+STR
+DOC
=VAL :......
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("......"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain3,
       ("--- ...\n", "---\n  ...\n"),
        R"(+STR
+DOC ---
=VAL :...
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("..."));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarPlain3_1,
       ("---...\n", "  ---...\n"),
        R"(+STR
+DOC
=VAL :---...
-DOC
-STR
)")
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
        R"(+STR
+DOC ---
=VAL :foo bar
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL :--- foo bar
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL :--- foo bar
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL :foo bar\nfew bars
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL :foo bar\nfew bars
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL :foo bar\nfew bars
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo bar\nfew bars"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(DocScalarPlainIndentation,
       (HAS_MULTILINE_SCALAR,
        R"(  foo
 bar
baz
--- foo
bar
baz
---
- foo
 bar
- baz
---
aaa: bbb
 ccc
ddd: eee
)",
        R"(--- foo bar baz
--- foo bar baz
---
- foo bar
- baz
---
aaa: bbb ccc
ddd: eee
)"),
        R"(+STR
+DOC
=VAL :foo bar baz
-DOC
+DOC ---
=VAL :foo bar baz
-DOC
+DOC ---
+SEQ
=VAL :foo bar
=VAL :baz
-SEQ
-DOC
+DOC ---
+MAP
=VAL :aaa
=VAL :bbb ccc
=VAL :ddd
=VAL :eee
-MAP
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL 'foo
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_squoted("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarSQuotedMultiline,
       (HAS_MULTILINE_SCALAR, "'foo\nbar\nbaz\n\nbat'\n", "'foo bar baz\n\n  bat'\n"),
        R"(+STR
+DOC
=VAL 'foo bar baz\nbat
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_squoted("foo bar baz\nbat"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarSQuotedMultilineComment0,
       (HAS_MULTILINE_SCALAR, "# a comment\n'foo\nbar\nbaz\n\nbat'\n", "'foo bar baz\n\n  bat'\n"),
        R"(+STR
+DOC
=VAL 'foo bar baz\nbat
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_squoted("foo bar baz\nbat"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarSQuotedMultilineComment1,
       (HAS_MULTILINE_SCALAR, "'foo\nbar\nbaz\n\nbat'\n# a comment\n", "'foo bar baz\n\n  bat'\n"),
        R"(+STR
+DOC
=VAL 'foo bar baz\nbat
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL "foo
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL "foo bar baz\nbat
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL "foo bar baz\nbat
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL "foo bar baz\nbat
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL |foo
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_literal("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarLiteralComment0,
       (HAS_MULTILINE_SCALAR, "# comment\n|-\n  foo\n", "|-\n  foo\n"),
        R"(+STR
+DOC
=VAL |foo
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_literal("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarLiteralComment1,
       (HAS_MULTILINE_SCALAR, "|-\n  foo\n# comment\n", "|-\n  foo\n"),
        R"(+STR
+DOC
=VAL |foo
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL >foo
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_folded("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarFoldedComment0,
       (HAS_MULTILINE_SCALAR, "# comment\n>-\n  foo\n", ">-\n  foo\n"),
        R"(+STR
+DOC
=VAL >foo
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_folded("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarFoldedComment1,
       (HAS_MULTILINE_SCALAR, ">-\n  foo\n# comment\n", ">-\n  foo\n"),
        R"(+STR
+DOC
=VAL >foo
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_folded("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocStream,
       ("--- doc0\n--- 'doc1'\n--- \"doc2\"\n"),
        R"(+STR
+DOC ---
=VAL :doc0
-DOC
+DOC ---
=VAL 'doc1
-DOC
+DOC ---
=VAL "doc2
-DOC
-STR
)")
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
        R"(+STR
+DOC
=VAL :doc0
-DOC
+DOC ---
=VAL :doc1
-DOC
+DOC ---
=VAL :doc2
-DOC
-STR
)")
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
        R"(+STR
+DOC
+MAP {}
=VAL :foo
=VAL :bar
=VAL :foo2
=VAL :bar2
-MAP
-DOC
-STR
)")
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
        R"(+STR
+DOC
+MAP {}
+MAP {}
-MAP
+MAP {}
-MAP
-MAP
-DOC
-STR
)")
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
        R"(+STR
+DOC
+MAP {}
+MAP {}
+MAP {}
-MAP
+MAP {}
-MAP
-MAP
+MAP {}
+MAP {}
-MAP
+MAP {}
-MAP
-MAP
-MAP
-DOC
-STR
)")
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
        R"(+STR
+DOC
+MAP {}
=VAL :foo
=VAL :bar
=VAL :foo2
=VAL :bar2
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
=VAL :foo
=VAL :bar
=VAL :foo2
=VAL :bar2
=VAL :foo3
=VAL :bar3
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
=VAL :a
=VAL :
=VAL :b
=VAL :
=VAL :c
=VAL :
=VAL :d
=VAL :
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
=VAL :
=VAL :a
=VAL :
=VAL :b
=VAL :
=VAL :c
=VAL :
=VAL :d
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
=VAL :
=VAL :
=VAL :
=VAL :
=VAL :
=VAL :
=VAL :
=VAL :
-MAP
-DOC
-STR
)")
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
       (R"(foo:
- bar
-
baz: qux
foo2:
- bar2
- 
baz2: qux2
)",
        R"(foo:
  - bar
  - 
baz: qux
foo2:
  - bar2
  - 
baz2: qux2
)"),
       R"(+STR
+DOC
+MAP
=VAL :foo
+SEQ
=VAL :bar
=VAL :
-SEQ
=VAL :baz
=VAL :qux
=VAL :foo2
+SEQ
=VAL :bar2
=VAL :
-SEQ
=VAL :baz2
=VAL :qux2
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP {}
+MAP {}
=VAL :this
=VAL :is
=VAL :a
=VAL :keymap
-MAP
+SEQ []
=VAL :and
=VAL :now
=VAL :a
=VAL :seq
=VAL :val
-SEQ
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
+MAP {}
=VAL :this
=VAL :is
=VAL :a
=VAL :keymap
-MAP
+SEQ []
=VAL :and
=VAL :now
=VAL :a
=VAL :seq
=VAL :val
-SEQ
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
+MAP {}
=VAL :this
=VAL :is
=VAL :a
=VAL :keymap
-MAP
+SEQ []
=VAL :and
=VAL :now
=VAL :a
=VAL :seq
=VAL :val
-SEQ
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
+SEQ []
=VAL :this
=VAL :is
=VAL :a
=VAL :seq
=VAL :key
-SEQ
+SEQ []
=VAL :and
=VAL :now
=VAL :a
=VAL :seq
=VAL :val
-SEQ
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
+SEQ []
=VAL :this
=VAL :is
=VAL :a
=VAL :seq
=VAL :key
-SEQ
+SEQ []
=VAL :and
=VAL :now
=VAL :a
=VAL :seq
=VAL :val
-SEQ
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
+MAP {}
=VAL :this
=VAL :is
=VAL :a
=VAL :keymap
-MAP
+SEQ []
=VAL :and
=VAL :now
=VAL :a
=VAL :seq
=VAL :val
-SEQ
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
+MAP {}
=VAL :this
=VAL :is
=VAL :a
=VAL :keymap
-MAP
+SEQ []
=VAL :and
=VAL :now
=VAL :a
=VAL :seq
=VAL :val
-SEQ
-MAP
-DOC
-STR
)")
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
        R"(---
{a: map}: [a,seq]
---
[A,SEQ]: {A: MAP}
)"),
       R"(+STR
+DOC ---
+MAP
+MAP {}
=VAL :a
=VAL :map
-MAP
+SEQ []
=VAL :a
=VAL :seq
-SEQ
-MAP
-DOC
+DOC ---
+MAP
+SEQ []
=VAL :A
=VAL :SEQ
-SEQ
+MAP {}
=VAL :A
=VAL :MAP
-MAP
-MAP
-DOC
-STR
)")
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
        R"(---
{a: map}: [a,seq]
---
[A,SEQ]: {A: MAP}
)"),
       R"(+STR
+DOC ---
+MAP
+MAP {}
=VAL :a
=VAL :map
-MAP
+SEQ []
=VAL :a
=VAL :seq
-SEQ
-MAP
-DOC
+DOC ---
+MAP
+SEQ []
=VAL :A
=VAL :SEQ
-SEQ
+MAP {}
=VAL :A
=VAL :MAP
-MAP
-MAP
-DOC
-STR
)")
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


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleSeqFlow,
       ("[foo,bar,baz]"),
        R"(+STR
+DOC
+SEQ []
=VAL :foo
=VAL :bar
=VAL :baz
-SEQ
-DOC
-STR
)")
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
        R"(+STR
+DOC
+SEQ []
+SEQ []
-SEQ
-SEQ
-DOC
-STR
)")
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
        R"(+STR
+DOC
+SEQ []
+SEQ []
+SEQ []
-SEQ
-SEQ
-SEQ
-DOC
-STR
)")
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
        R"(+STR
+DOC
+SEQ []
+SEQ []
+SEQ []
+SEQ []
-SEQ
-SEQ
-SEQ
-SEQ
-DOC
-STR
)")
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
        R"(+STR
+DOC
+SEQ []
=VAL :foo
=VAL :bar
=VAL :baz
-SEQ
-DOC
-STR
)")
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
        R"(+STR
+DOC
+SEQ
=VAL :foo
=VAL :bar
=VAL :baz
-SEQ
-DOC
-STR
)")
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
        R"(+STR
+DOC
+SEQ
+SEQ []
=VAL :foo
=VAL :bar
=VAL :baz
-SEQ
+MAP {}
=VAL :foo
=VAL :bar
=VAL :baz
=VAL :
-MAP
-SEQ
-DOC
-STR
)")
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
        R"(- a
  b
)", "- a b\n"), R"(+STR
+DOC
+SEQ
=VAL :a b
-SEQ
-DOC
-STR
)")
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
        R"(- a
  - b - c
)", "- a - b - c\n"), R"(+STR
+DOC
+SEQ
=VAL :a - b - c
-SEQ
-DOC
-STR
)")
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
       (R"(-   &wtf
   # this is a comment
  foo

)", "- &wtf foo\n"), R"(+STR
+DOC
+SEQ
=VAL &wtf :foo
-SEQ
-DOC
-STR
)")
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
       (R"(-   &wtf
  foo
   # this is a comment

)", "- &wtf foo\n"), R"(+STR
+DOC
+SEQ
=VAL &wtf :foo
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ
=VAL :
=VAL :
=VAL :
=VAL :
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ
=VAL |
=VAL |
=VAL |
=VAL |
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ
=VAL >
=VAL >
=VAL >
=VAL >
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP {}
=VAL :map1
+MAP {}
=VAL :foo1
=VAL :bar1
=VAL :FOO1
=VAL :BAR1
-MAP
=VAL :map2
+MAP {}
=VAL :foo2
=VAL :bar2
=VAL :FOO2
=VAL :BAR2
-MAP
-MAP
-DOC
-STR
)")
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
       (R"(map1:
  foo1: bar1
  FOO1: BAR1
map2:
  foo2: bar2
  FOO2: BAR2
)"),
       R"(+STR
+DOC
+MAP
=VAL :map1
+MAP
=VAL :foo1
=VAL :bar1
=VAL :FOO1
=VAL :BAR1
-MAP
=VAL :map2
+MAP
=VAL :foo2
=VAL :bar2
=VAL :FOO2
=VAL :BAR2
-MAP
-MAP
-DOC
-STR
)")
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
       (R"(map0:
  map01:
    foo01: bar01
    FOO01: BAR01
  map02:
    foo02: bar02
    FOO02: BAR02
    child02:
      foo020: bar020
      foo021: bar021
map1:
  map11:
    foo11: bar11
    FOO11: BAR11
)"),
       R"(+STR
+DOC
+MAP
=VAL :map0
+MAP
=VAL :map01
+MAP
=VAL :foo01
=VAL :bar01
=VAL :FOO01
=VAL :BAR01
-MAP
=VAL :map02
+MAP
=VAL :foo02
=VAL :bar02
=VAL :FOO02
=VAL :BAR02
=VAL :child02
+MAP
=VAL :foo020
=VAL :bar020
=VAL :foo021
=VAL :bar021
-MAP
-MAP
-MAP
=VAL :map1
+MAP
=VAL :map11
+MAP
=VAL :foo11
=VAL :bar11
=VAL :FOO11
=VAL :BAR11
-MAP
-MAP
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
+SEQ []
=VAL :foo1
=VAL :bar1
=VAL :baz1
-SEQ
+SEQ []
=VAL :foo2
=VAL :bar2
=VAL :baz2
-SEQ
-SEQ
-DOC
-STR
)")
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
       (R"(- - foo1
  - bar1
  - baz1
- - foo2
  - bar2
  - baz2
)"),
       R"(+STR
+DOC
+SEQ
+SEQ
=VAL :foo1
=VAL :bar1
=VAL :baz1
-SEQ
+SEQ
=VAL :foo2
=VAL :bar2
=VAL :baz2
-SEQ
-SEQ
-DOC
-STR
)")
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
       (R"(- - - foo1
    - bar1
    - baz1
  - - foo2
    - bar2
    - baz2
- back
)"),
       R"(+STR
+DOC
+SEQ
+SEQ
+SEQ
=VAL :foo1
=VAL :bar1
=VAL :baz1
-SEQ
+SEQ
=VAL :foo2
=VAL :bar2
=VAL :baz2
-SEQ
-SEQ
=VAL :back
-SEQ
-DOC
-STR
)")
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
       (R"(- foo: 1
  bar: 2
- foo: 10
  bar: 20
- foo: 100
  bar: 200
)"),
       R"(+STR
+DOC
+SEQ
+MAP
=VAL :foo
=VAL :1
=VAL :bar
=VAL :2
-MAP
+MAP
=VAL :foo
=VAL :10
=VAL :bar
=VAL :20
-MAP
+MAP
=VAL :foo
=VAL :100
=VAL :bar
=VAL :200
-MAP
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
=VAL :a:
=VAL :
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
=VAL :key ends with two colons::
=VAL :value
-MAP
-DOC
-STR
)")
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
       (R"(- ::
- x::
- :x::
)",
        R"(- :: 
- x:: 
- :x:: 
)"),
       R"(+STR
+DOC
+SEQ
+MAP
=VAL ::
=VAL :
-MAP
+MAP
=VAL :x:
=VAL :
-MAP
+MAP
=VAL ::x:
=VAL :
-MAP
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
=VAL :a, b
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("a, b"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(BlockPlainScalarCommaMap,
       ("a, b: c, d\n"),
       R"(+STR
+DOC
+MAP
=VAL :a, b
=VAL :c, d
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ
=VAL :a, b
=VAL :c, d
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
=VAL &val :a
-DOC
-STR
)")
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
       R"(+STR
+DOC
=ALI *a
-DOC
-STR
)")
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
       R"(+STR
+DOC
=VAL &val :a
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ &seq
=VAL &val1 :val1
=VAL &val2 :val2
-SEQ
-DOC
-STR
)")
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
       (R"(&seq
- &key1 key1: &val1 val1
  &key2 key2: &val2 val2
- &map2
  &key10 key10: &val10 val10
  &key20 key20: &val20 val20
)"),
       R"(+STR
+DOC
+SEQ &seq
+MAP
=VAL &key1 :key1
=VAL &val1 :val1
=VAL &key2 :key2
=VAL &val2 :val2
-MAP
+MAP &map2
=VAL &key10 :key10
=VAL &val10 :val10
=VAL &key20 :key20
=VAL &val20 :val20
-MAP
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ [] &seq
=VAL &val1 :val1
=VAL &val2 :val2
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ &seq
=VAL &val1 :val1
=ALI *val1
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ [] &seq
=VAL &val1 :val1
=ALI *val1
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ [] &seq
=VAL &val1 :val1
=ALI *val1
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP &map
=VAL &key1 :key1
=VAL &val1 :val1
=VAL &key2 :key2
=VAL &val2 :val2
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP &map
=VAL &rkey1 :key1
=VAL &rval1 :val1
=ALI *rkey1
=ALI *rval1
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP {} &map
=VAL &key1 :key1
=VAL &val1 :val1
=VAL &key2 :key2
=VAL &val2 :val2
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP {} &map
=VAL &key1 :key1
=VAL &val1 :val1
=VAL &key2 :key2
=VAL &val2 :val2
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP {} &map
=VAL &rkey1 :key1
=VAL &rval1 :val1
=ALI *rkey1
=ALI *rval1
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP {} &map
=VAL &rkey1 :key1
=VAL &rval1 :val1
=ALI *rkey1
=ALI *rval1
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP &map
=VAL &mapkey :map
+MAP &mapval
=VAL &key1 :key1
=VAL &val1 :val1
=VAL &key2 :key2
=VAL &val2 :val2
-MAP
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP {} &map
=VAL &mapkey :map
+MAP {} &mapval
=VAL &key1 :key1
=VAL &val1 :val1
=VAL &key2 :key2
=VAL &val2 :val2
=VAL :key3
=VAL :val3
-MAP
=VAL :key4
=VAL :val4
-MAP
-DOC
-STR
)")
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
       (R"(- &a0 !b0 foo0: bar0
- &a1 !b1
  foo1: bar1
- &a2
  !b2 foo2: bar2
- &a3
  !b3
  foo3: bar3
- &a4

  !b4

  foo4: bar4
)",
        R"(- !b0 &a0 foo0: bar0
- !b1 &a1
  foo1: bar1
- &a2
  !b2 foo2: bar2
- !b3 &a3
  foo3: bar3
- !b4 &a4
  foo4: bar4
)"),
       R"(+STR
+DOC
+SEQ
+MAP
=VAL &a0 <!b0> :foo0
=VAL :bar0
-MAP
+MAP &a1 <!b1>
=VAL :foo1
=VAL :bar1
-MAP
+MAP &a2
=VAL <!b2> :foo2
=VAL :bar2
-MAP
+MAP &a3 <!b3>
=VAL :foo3
=VAL :bar3
-MAP
+MAP &a4 <!b4>
=VAL :foo4
=VAL :bar4
-MAP
-SEQ
-DOC
-STR
)")
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
       (R"(!mymap &mymap
&fooanch !footag foo: &seq
  &key1 key1: &val1 val1
  &key2 key2: &val2 val2
!bartag &baranch bar: !a &map2
  !b &key10 key10: !c &val10 val10
  &key20 !b key20: &val20 !c val20
  key10: 20
&bazanch !baztag baz:   &map2 !a
    &key10 !b key10: &val10 !c val10
    !b &key20 key20: !c &val20 val20
    key10: 20
brr: &map2
  !a foo: bar
bra: &map2



  !a foo: bar
bre: &map2
  !a
  
  
  foo: bar
bru:   &wtf
  foo
)",
        R"(!mymap &mymap
!footag &fooanch foo: &seq
  &key1 key1: &val1 val1
  &key2 key2: &val2 val2
!bartag &baranch bar: !a &map2
  !b &key10 key10: !c &val10 val10
  !b &key20 key20: !c &val20 val20
  key10: 20
!baztag &bazanch baz: !a &map2
  !b &key10 key10: !c &val10 val10
  !b &key20 key20: !c &val20 val20
  key10: 20
brr: &map2
  !a foo: bar
bra: &map2
  !a foo: bar
bre: !a &map2
  foo: bar
bru: &wtf foo
)"),
       R"(+STR
+DOC
+MAP &mymap <!mymap>
=VAL &fooanch <!footag> :foo
+MAP &seq
=VAL &key1 :key1
=VAL &val1 :val1
=VAL &key2 :key2
=VAL &val2 :val2
-MAP
=VAL &baranch <!bartag> :bar
+MAP &map2 <!a>
=VAL &key10 <!b> :key10
=VAL &val10 <!c> :val10
=VAL &key20 <!b> :key20
=VAL &val20 <!c> :val20
=VAL :key10
=VAL :20
-MAP
=VAL &bazanch <!baztag> :baz
+MAP &map2 <!a>
=VAL &key10 <!b> :key10
=VAL &val10 <!c> :val10
=VAL &key20 <!b> :key20
=VAL &val20 <!c> :val20
=VAL :key10
=VAL :20
-MAP
=VAL :brr
+MAP &map2
=VAL <!a> :foo
=VAL :bar
-MAP
=VAL :bra
+MAP &map2
=VAL <!a> :foo
=VAL :bar
-MAP
=VAL :bre
+MAP &map2 <!a>
=VAL :foo
=VAL :bar
-MAP
=VAL :bru
=VAL &wtf :foo
-MAP
-DOC
-STR
)")
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
    (R"(&seq
- &key1 key1: &val1 val1
  &key2 key2: &val2 val2
- !a &map2a
  !b &key10 key10: &val10 val10
  &key20 key20: &val20 val20
  key10: 20
-   &map2x !a
    &key10 !b key10: &val10 val10
    &key20 key20: &val20 val20
    key10: 20
- &map2y
  !a foo: bar
- &map2z



  !a foo: bar
- &map2u
  !a
  
  
  foo: bar
-   &wtf
  foo
  )",
     R"(&seq
- &key1 key1: &val1 val1
  &key2 key2: &val2 val2
- !a &map2a
  !b &key10 key10: &val10 val10
  &key20 key20: &val20 val20
  key10: 20
- !a &map2x
  !b &key10 key10: &val10 val10
  &key20 key20: &val20 val20
  key10: 20
- &map2y
  !a foo: bar
- &map2z
  !a foo: bar
- !a &map2u
  foo: bar
- &wtf foo
)"),
       R"(+STR
+DOC
+SEQ &seq
+MAP
=VAL &key1 :key1
=VAL &val1 :val1
=VAL &key2 :key2
=VAL &val2 :val2
-MAP
+MAP &map2a <!a>
=VAL &key10 <!b> :key10
=VAL &val10 :val10
=VAL &key20 :key20
=VAL &val20 :val20
=VAL :key10
=VAL :20
-MAP
+MAP &map2x <!a>
=VAL &key10 <!b> :key10
=VAL &val10 :val10
=VAL &key20 :key20
=VAL &val20 :val20
=VAL :key10
=VAL :20
-MAP
+MAP &map2y
=VAL <!a> :foo
=VAL :bar
-MAP
+MAP &map2z
=VAL <!a> :foo
=VAL :bar
-MAP
+MAP &map2u <!a>
=VAL :foo
=VAL :bar
-MAP
=VAL &wtf :foo
-SEQ
-DOC
-STR
)")
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


//-----------------------------------------------------------------------------


ENGINE_TEST(TagBlock,
       ("- !light fluorescent\n- notag\n"),
       R"(+STR
+DOC
+SEQ
=VAL <!light> :fluorescent
=VAL :notag
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
=VAL <!light> :fluorescent
=VAL :notag
-SEQ
-DOC
-STR
)")
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
       (R"(%YAML 1.2
---
!light fluorescent
...
%TAG !m! !my-
---
!m!light green
)",
        R"(--- !light fluorescent
...
%TAG !m! !my-
--- !m!light green
)"),
       R"(+STR
+DOC ---
=VAL <!light> :fluorescent
-DOC ...
+DOC ---
=VAL <!my-light> :green
-DOC
-STR
)")
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
             R"(+STR
+DOC
=VAL <!> :
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuite6WLZ_0,
            (R"(--- !foo "bar"
...
%TAG ! tag:example.com,2000:app/
--- !foo "bar"
)"),
             R"(+STR
+DOC ---
=VAL <!foo> "bar
-DOC ...
+DOC ---
=VAL <tag:example.com,2000:app/foo> "bar
-DOC
-STR
)")
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
            (R"(--- !foo "bar"
...
%TAG ! tag:example.com,2000:app/
--- !<tag:example.com,2000:app/foo> "bar"
)"),
             R"(+STR
+DOC ---
=VAL <!foo> "bar
-DOC ...
+DOC ---
=VAL <tag:example.com,2000:app/foo> "bar
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP {}
+MAP {}
=VAL :foo
=VAL :bar
-MAP
=VAL :baz
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
+MAP
=VAL :foo
=VAL :bar
-MAP
=VAL :baz
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP
+MAP {}
=VAL :foo
=VAL :bar
-MAP
=VAL :baz
-MAP
-DOC
-STR
)")
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
       R"(+STR
+DOC
+MAP {}
+SEQ []
=VAL :foo
=VAL :bar
-SEQ
=VAL :baz
-MAP
-DOC
-STR
)")
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
       (HAS_CONTAINER_KEYS, Location(3,2,2), R"(?
 - foo
 - bar
: baz
)"),
       R"(+STR
+DOC
+MAP
+SEQ
=VAL :foo
=VAL :bar
-SEQ
=VAL :baz
-MAP
-DOC
-STR
)")
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
       (HAS_CONTAINER_KEYS, Location(2,1,3), R"(? [foo, bar]
: baz
)"),
       R"(+STR
+DOC
+MAP
+SEQ []
=VAL :foo
=VAL :bar
-SEQ
=VAL :baz
-MAP
-DOC
-STR
)")
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
       (HAS_CONTAINER_KEYS, Location(2,2,1), R"(?
- foo
- bar
:
- baz
- bat
)"),
       R"(+STR
+DOC
+MAP
+SEQ
=VAL :foo
=VAL :bar
-SEQ
+SEQ
=VAL :baz
=VAL :bat
-SEQ
-MAP
-DOC
-STR
)")
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
       (HAS_CONTAINER_KEYS, Location(3,2,2), R"(?
 - foo
 - bar
:
 - baz
 - bat
)"),
       R"(+STR
+DOC
+MAP
+SEQ
=VAL :foo
=VAL :bar
-SEQ
+SEQ
=VAL :baz
=VAL :bat
-SEQ
-MAP
-DOC
-STR
)")
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


//-----------------------------------------------------------------------------

ENGINE_TEST(Qmrk0,
       (HAS_CONTAINER_KEYS, Location(41,3,19), R"(
a simple key: a value
? an explicit key: another value
)"),
       R"(+STR
+DOC
+MAP
=VAL :a simple key
=VAL :a value
+MAP
=VAL :an explicit key
=VAL :another value
-MAP
=VAL :
-MAP
-DOC
-STR
)")
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
       (R"(? an explicit key
a simple key: a value
)",
        R"(an explicit key: 
a simple key: a value
)"),
       R"(+STR
+DOC
+MAP
=VAL :an explicit key
=VAL :
=VAL :a simple key
=VAL :a value
-MAP
-DOC
-STR
)")
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
       (HAS_CONTAINER_KEYS, Location(19,2,19), R"(
? an explicit key: another value
a simple key: a value
)"),
       R"(+STR
+DOC
+MAP
+MAP
=VAL :an explicit key
=VAL :another value
-MAP
=VAL :
=VAL :a simple key
=VAL :a value
-MAP
-DOC
-STR
)")
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
        R"(map:
  ? an explicit key: another value
  a simple key: a value
? an explicit key deindented: its value
? more: siblings
)"),
       R"(+STR
+DOC
+MAP
=VAL :map
+MAP
+MAP
=VAL :an explicit key
=VAL :another value
-MAP
=VAL :
=VAL :a simple key
=VAL :a value
-MAP
+MAP
=VAL :an explicit key deindented
=VAL :its value
-MAP
=VAL :
+MAP
=VAL :more
=VAL :siblings
-MAP
=VAL :
-MAP
-DOC
-STR
)")
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
       (R"(? an explicit key
...
? another explicit key
)",
        R"(---
an explicit key: 
---
another explicit key: 
)"),
       R"(+STR
+DOC
+MAP
=VAL :an explicit key
=VAL :
-MAP
-DOC ...
+DOC
+MAP
=VAL :another explicit key
=VAL :
-MAP
-DOC
-STR
)")
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
       (R"(? an explicit key
---
? another explicit key
)",
        R"(---
an explicit key: 
---
another explicit key: 
)"),
       R"(+STR
+DOC
+MAP
=VAL :an explicit key
=VAL :
-MAP
-DOC
+DOC ---
+MAP
=VAL :another explicit key
=VAL :
-MAP
-DOC
-STR
)")
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
       (R"({ ? an explicit key, ? foo,? bar,?baz:,?bat})",
        R"({an explicit key: ,foo: ,bar: ,?baz: ,?bat: })"),
       R"(+STR
+DOC
+MAP {}
=VAL :an explicit key
=VAL :
=VAL :foo
=VAL :
=VAL :bar
=VAL :
=VAL :?baz
=VAL :
=VAL :?bat
=VAL :
-MAP
-DOC
-STR
)")
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
       (R"([ ? an explicit key, ? foo,? bar,?baz:,?bat])",
        R"([{an explicit key: },{foo: },{bar: },{?baz: },?bat])"),
       R"(+STR
+DOC
+SEQ []
+MAP {}
=VAL :an explicit key
=VAL :
-MAP
+MAP {}
=VAL :foo
=VAL :
-MAP
+MAP {}
=VAL :bar
=VAL :
-MAP
+MAP {}
=VAL :?baz
=VAL :
-MAP
=VAL :?bat
-SEQ
-DOC
-STR
)")
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
        R"(? a: b
?
?
)"),
       R"(+STR
+DOC
+MAP
+MAP
=VAL :a
=VAL :b
-MAP
=VAL :
=VAL :
=VAL :
=VAL :
=VAL :
-MAP
-DOC
-STR
)")
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
        R"(
- ? an explicit key: another value
  a simple key: a value
- ? another explicit key: its value
)"),
       R"(+STR
+DOC
+SEQ
+MAP
+MAP
=VAL :an explicit key
=VAL :another value
-MAP
=VAL :
=VAL :a simple key
=VAL :a value
-MAP
+MAP
+MAP
=VAL :another explicit key
=VAL :its value
-MAP
=VAL :
-MAP
-SEQ
-DOC
-STR
)")
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
        R"(
- ? : x
- ? : 
- ? :
)"),
        R"(+STR
+DOC
+SEQ
+MAP
+MAP
=VAL :
=VAL :x
-MAP
=VAL :
-MAP
+MAP
+MAP
=VAL :
=VAL :
-MAP
=VAL :
-MAP
+MAP
+MAP
=VAL :
=VAL :
-MAP
=VAL :
-MAP
-SEQ
-DOC
-STR
)")
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
        R"(a1: b1
? !at
 !bt : b2
: c3
)"),
       R"(+STR
+DOC
+MAP
=VAL :a1
=VAL :b1
+MAP <!at>
=VAL <!bt> :
=VAL :b2
-MAP
=VAL :c3
-MAP
-DOC
-STR
)")
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

ENGINE_TEST(SeqIMap0Ref,
       ("[val0,val1,{key: val2},wtf]"),
       R"(+STR
+DOC
+SEQ []
=VAL :val0
=VAL :val1
+MAP {}
=VAL :key
=VAL :val2
-MAP
=VAL :wtf
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
=VAL :val0
=VAL :val1
+MAP {}
=VAL :key
=VAL :val2
-MAP
=VAL :wtf
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
=VAL :val0
=VAL :val1
+MAP {}
=VAL :key
+SEQ []
+MAP {}
=VAL :foo
=VAL :bar
-MAP
-SEQ
-MAP
=VAL :wtf
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
+MAP {}
=VAL :val
+SEQ []
+MAP {}
=VAL :0
+SEQ []
+MAP {}
=VAL :1
+SEQ []
+MAP {}
=VAL :2
+SEQ []
+MAP {}
=VAL :3
+SEQ []
+MAP {}
=VAL :4
+SEQ []
+MAP {}
=VAL :5
+SEQ []
+MAP {}
=VAL :6
=VAL :7
-MAP
-SEQ
-MAP
-SEQ
-MAP
-SEQ
-MAP
-SEQ
-MAP
-SEQ
-MAP
-SEQ
-MAP
-SEQ
-MAP
+MAP {}
=VAL :
=VAL :
-MAP
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
=VAL :val0
+MAP {}
=VAL :
=VAL :wtf
-MAP
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
=VAL :val0
+MAP {}
=VAL :wtf
=VAL :
-MAP
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
=VAL :val0
+MAP {}
=VAL :
=VAL :
-MAP
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
+MAP {}
=VAL :val0
=VAL :val1
-MAP
+MAP {}
=VAL :
=VAL :
-MAP
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
+MAP {}
=VAL :val0
+SEQ []
+MAP {}
=VAL :val1
=VAL :val2
-MAP
-SEQ
-MAP
+MAP {}
=VAL :
=VAL :
-MAP
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
=VAL :val0
=VAL :val1
+SEQ []
=VAL :rat
=VAL :rot
-SEQ
=VAL :wtf
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
=VAL :val0
=VAL :val1
+MAP {}
+SEQ []
=VAL :rat
=VAL :rot
-SEQ
+SEQ []
+MAP {}
=VAL :foo
=VAL :bar
-MAP
-SEQ
-MAP
=VAL :wtf
-SEQ
-DOC
-STR
)")
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
       R"(+STR
+DOC
+SEQ []
=VAL :val0
=VAL :val1
+MAP {}
+SEQ []
=VAL :rat
=VAL :rot
-SEQ
+SEQ []
+MAP {}
=VAL :foo
=VAL :bar
-MAP
-SEQ
-MAP
=VAL :wtf
-SEQ
-DOC
-STR
)")
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

ENGINE_TEST(SuddenDoc0, (R"(- map:
   foo: bar
...
seq:
  - a
  - b
...
)",
                    R"(---
- map:
    foo: bar
---
seq:
  - a
  - b
)"),
       R"(+STR
+DOC
+SEQ
+MAP
=VAL :map
+MAP
=VAL :foo
=VAL :bar
-MAP
-MAP
-SEQ
-DOC ...
+DOC
+MAP
=VAL :seq
+SEQ
=VAL :a
=VAL :b
-SEQ
-MAP
-DOC ...
-STR
)")
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

ENGINE_TEST(SuddenDoc1, (R"(map:
  nested:
    nested more:
      nested even more:
      - now inside a seq:
          another map:
          - another seq
          - etc
---
map:
  nested:
    nested more:
      nested even more:
      - now inside a seq:
          another map: the value
---
map:
  nested:
    nested more:
      nested even more:
      - now inside a seq:
          another map:
          - another seq
          - etc
...
map:
  nested:
    nested more:
      nested even more:
      - now inside a seq:
          another map: the value
...
)",
           R"(---
map:
  nested:
    nested more:
      nested even more:
        - now inside a seq:
            another map:
              - another seq
              - etc
---
map:
  nested:
    nested more:
      nested even more:
        - now inside a seq:
            another map: the value
---
map:
  nested:
    nested more:
      nested even more:
        - now inside a seq:
            another map:
              - another seq
              - etc
---
map:
  nested:
    nested more:
      nested even more:
        - now inside a seq:
            another map: the value
)"),
       R"(+STR
+DOC
+MAP
=VAL :map
+MAP
=VAL :nested
+MAP
=VAL :nested more
+MAP
=VAL :nested even more
+SEQ
+MAP
=VAL :now inside a seq
+MAP
=VAL :another map
+SEQ
=VAL :another seq
=VAL :etc
-SEQ
-MAP
-MAP
-SEQ
-MAP
-MAP
-MAP
-MAP
-DOC
+DOC ---
+MAP
=VAL :map
+MAP
=VAL :nested
+MAP
=VAL :nested more
+MAP
=VAL :nested even more
+SEQ
+MAP
=VAL :now inside a seq
+MAP
=VAL :another map
=VAL :the value
-MAP
-MAP
-SEQ
-MAP
-MAP
-MAP
-MAP
-DOC
+DOC ---
+MAP
=VAL :map
+MAP
=VAL :nested
+MAP
=VAL :nested more
+MAP
=VAL :nested even more
+SEQ
+MAP
=VAL :now inside a seq
+MAP
=VAL :another map
+SEQ
=VAL :another seq
=VAL :etc
-SEQ
-MAP
-MAP
-SEQ
-MAP
-MAP
-MAP
-MAP
-DOC ...
+DOC
+MAP
=VAL :map
+MAP
=VAL :nested
+MAP
=VAL :nested more
+MAP
=VAL :nested even more
+SEQ
+MAP
=VAL :now inside a seq
+MAP
=VAL :another map
=VAL :the value
-MAP
-MAP
-SEQ
-MAP
-MAP
-MAP
-MAP
-DOC ...
-STR
)")
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
       (R"(---
map : {foo: bar, notag: none}
seq : [foo, bar]
...
)",
        R"(---
map: {foo: bar,notag: none}
seq: [foo,bar]
)"),
       R"(+STR
+DOC ---
+MAP
=VAL :map
+MAP {}
=VAL :foo
=VAL :bar
=VAL :notag
=VAL :none
-MAP
=VAL :seq
+SEQ []
=VAL :foo
=VAL :bar
-SEQ
-MAP
-DOC ...
-STR
)")
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
       (R"(---
*mapref : {foo: bar, notag: none}
*seqref : [foo, bar]
...
)",
        R"(---
*mapref : {foo: bar,notag: none}
*seqref : [foo,bar]
)"),
       R"(+STR
+DOC ---
+MAP
=ALI *mapref
+MAP {}
=VAL :foo
=VAL :bar
=VAL :notag
=VAL :none
-MAP
=ALI *seqref
+SEQ []
=VAL :foo
=VAL :bar
-SEQ
-MAP
-DOC ...
-STR
)")
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
       (R"(a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~)",
        R"(a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~
)"),
       R"(+STR
+DOC
=VAL :a!"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~
-DOC
-STR
)")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(PlainScalarBlockSeq0,
       (R"(- a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~)",
        R"(- a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~
)"),
       R"(+STR
+DOC
+SEQ
=VAL :a!"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~
-SEQ
-DOC
-STR
)")
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
       (R"(key: a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~
a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~: val
)"),
       R"(+STR
+DOC
+MAP
=VAL :key
=VAL :a!"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~
=VAL :a!"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~
=VAL :val
-MAP
-DOC
-STR
)")
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
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq0_0, Location(1,29), R"([a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~])")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq0_1, Location(1,29), R"([a!"#$%&'()*+,-./09:;<=>?@AZ{|}^_`az{|}~])")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq1_0, Location(2,28), R"([
a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~
])")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq1_1, Location(2,28), R"([
a!"#$%&'()*+,-./09:;<=>?@AZ{|}^_`az{|}~
])")
ENGINE_TEST_ERRLOC(PlainScalarFlow1Seq0, Location(1,38), R"([a!"#$%&'()*+,-./09:;<=>?@]^_`az{|}~])")
ENGINE_TEST(PlainScalarFlow0Seq1,
       (R"([
a!"#$%&'()*+,-./09:;<=>?@AZ\
])",
        R"([a!"#$%&'()*+,-./09:;<=>?@AZ\])"),
       R"(+STR
+DOC
+SEQ []
=VAL :a!"#$%&'()*+
=VAL :-./09:;<=>?@AZ\\
-SEQ
-DOC
-STR
)")
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
