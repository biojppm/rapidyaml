#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


static constexpr const bool singleline = false;

static const ParserOptions resolve_custom_tags = ParserOptions{}.resolve_tags(true);
static const ParserOptions resolve_all_tags = ParserOptions{}.resolve_tags(true).resolve_tags_all(true);


ENGINE_TEST(TagBlockSeq,
            "- !light fluorescent\n- notag\n"
            ,
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
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagFlowSeq,
            "[!light fluorescent,notag]"
            ,
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
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DirectiveAndTag0,
            "%TAG !m! !my-\n"
            "--- !m!light green\n"
            "--- !light fluorescent\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!m!light> :green\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!light> :fluorescent\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!m!", "!my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!m!light"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_(DirectiveAndTag0_resolved_custom, resolve_custom_tags,
            "%TAG !m! !my-\n"
            "--- !m!light green\n"
            "--- !light fluorescent\n"
            ,
            "%TAG !m! !my-\n"
            "--- !<!my-light> green\n"
            "--- !light fluorescent\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!my-light> :green\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!light> :fluorescent\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!m!", "!my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!my-light>"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_(DirectiveAndTag0_resolved_all, resolve_all_tags,
            "%TAG !m! !my-\n"
            "--- !m!light green\n"
            "--- !light fluorescent\n"
            ,
            "%TAG !m! !my-\n"
            "--- !<!my-light> green\n"
            "--- !<!light> fluorescent\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!my-light> :green\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!light> :fluorescent\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!m!", "!my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!my-light>"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!light>"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DirectiveAndTag0expl,
            "%TAG !m! !my-\n"
            "--- !m!light green\n"
            "...\n"
            "--- !light fluorescent\n"
            ,
            "%TAG !m! !my-\n"
            "--- !m!light green\n"
            "--- !light fluorescent\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!m!light> :green\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <!light> :fluorescent\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!m!", "!my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!m!light"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc_expl());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_(DirectiveAndTag0expl_custom, resolve_custom_tags,
            "%TAG !m! !my-\n"
            "--- !m!light green\n"
            "...\n"
            "--- !light fluorescent\n"
            ,
            "%TAG !m! !my-\n"
            "--- !<!my-light> green\n"
            "--- !light fluorescent\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!my-light> :green\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <!light> :fluorescent\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!m!", "!my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!my-light>"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc_expl());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_(DirectiveAndTag0expl_all, resolve_all_tags,
            "%TAG !m! !my-\n"
            "--- !m!light green\n"
            "...\n"
            "--- !light fluorescent\n"
            ,
            "%TAG !m! !my-\n"
            "--- !<!my-light> green\n"
            "--- !<!light> fluorescent\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!my-light> :green\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <!light> :fluorescent\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!m!", "!my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!my-light>"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc_expl());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!light>"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DirectiveAndTag1,
            "%YAML 1.2\n"
            "---\n"
            "!light fluorescent\n"
            "...\n"
            "%TAG !m! !my-\n"
            "---\n"
            "!m!light green\n"
            ,
            "--- !light fluorescent\n"
            "...\n"
            "%TAG !m! !my-\n"
            "--- !m!light green\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!light> :fluorescent\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <!m!light> :green\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_yaml("1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!m!", "!my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!m!light"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_(DirectiveAndTag1_custom, resolve_custom_tags,
            "%YAML 1.2\n"
            "---\n"
            "!light fluorescent\n"
            "...\n"
            "%TAG !m! !my-\n"
            "---\n"
            "!m!light green\n"
            ,
            "--- !light fluorescent\n"
            "...\n"
            "%TAG !m! !my-\n"
            "--- !<!my-light> green\n"
            ,
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
    ___(ps.add_directive_yaml("1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!m!", "!my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!my-light>"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_(DirectiveAndTag1_all, resolve_all_tags,
            "%YAML 1.2\n"
            "---\n"
            "!light fluorescent\n"
            "...\n"
            "%TAG !m! !my-\n"
            "---\n"
            "!m!light green\n"
            ,
            "--- !<!light> fluorescent\n"
            "...\n"
            "%TAG !m! !my-\n"
            "--- !<!my-light> green\n"
            ,
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
    ___(ps.add_directive_yaml("1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!light>"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!m!", "!my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!my-light>"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DirectiveAndTag2,
            "%TAG !! foo-\n"
            "--- !!a b\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!!a> :b\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!!", "foo-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!!a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(DirectiveAndTag2_custom, resolve_custom_tags,
            "%TAG !! foo-\n"
            "--- !!a b\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!!a> :b\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!!", "foo-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!!a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(DirectiveAndTag2_all, resolve_all_tags,
            "%TAG !! foo-\n"
            "--- !!a b\n"
            ,
            "%TAG !! foo-\n"
            "--- !<foo-a> b\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <foo-a> :b\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!!", "foo-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<foo-a>"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagCustomNotFound,
            "--- !foo \"bar\"\n"
            "...\n"
            "%TAG !m! tag:example.com,2000:app/\n"
            "--- !n!foo \"bar\"\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!foo> \"bar\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <!n!foo> \"bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!m!", "tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!n!foo"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERROPT(TagCustomNotFound_all, resolve_all_tags,
            "--- !foo \"bar\"\n"
            "...\n"
            "%TAG !m! tag:example.com,2000:app/\n"
            "--- !n!foo \"bar\"\n")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DirectiveMultipleYAML0, Location(2, 1),
                   "%YAML 1.2\n"
                   "%YAML 1.2\n"
                   "--- foo\n"
                   "")
ENGINE_TEST_ERRLOC(DirectiveMultipleYAML1, Location(2, 1),
                   "%YAML 1.2\n"
                   "%YAML 1.2\n"
                   "")
ENGINE_TEST_ERRLOC(DirectiveMultipleYAML2, Location(3, 1),
                   "%YAML 1.2\n"
                   "---\n"
                   "%YAML 1.2\n"
                   "---\n"
                   "")
ENGINE_TEST(DirectiveMultipleYAML3,
            "%YAML 1.2\n"
            "--- a\n"
            "...\n"
            "%YAML 1.2\n"
            "--- b\n"
            ,
            "--- a\n"
            "--- b\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL :a\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL :b\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_yaml("1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_doc_expl());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(DirectiveMissingDoc0, Location(2, 1),
                   "%YAML 1.2\n"
                   "")
ENGINE_TEST_ERRLOC(DirectiveMissingDoc1, Location(2, 1),
                   "%TAG !a! b-\n"
                   "")
ENGINE_TEST_ERRLOC(DirectiveMissingDoc2, Location(3, 1),
                   "%TAG !a! b-\n"
                   "%TAG !c! d-\n"
                   "")
ENGINE_TEST_ERRLOC(DirectiveMissingDoc3, Location(3, 1),
                   "%YAML 1.2\n"
                   "%TAG !a! b-\n"
                   "")
ENGINE_TEST_ERRLOC(DirectiveMissingDoc4, Location(3, 1),
                   "%TAG !a! b-\n"
                   "%YAML 1.2\n"
                   "")
ENGINE_TEST_ERRLOC(DirectiveMissingDoc5, Location(5, 1),
                   "%TAG ! haha:\n"
                   "---\n"
                   "...\n"
                   "%YAML 1.2\n"
                   "")
ENGINE_TEST_ERRLOC(DirectiveMissingDoc6, Location(5, 1),
                   "%YAML 1.2\n"
                   "---\n"
                   "...\n"
                   "%TAG ! haha:\n"
                   "")


//-----------------------------------------------------------------------------

ENGINE_TEST(TestSuite9WXW_0_scalar,
            "!foo bar\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "---\n"
            "!foo bar\n"
            ,
            "--- !foo bar\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !foo bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL <!foo> :bar\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <!foo> :bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!", "tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_(TestSuite9WXW_0_scalar_resolve_custom, resolve_custom_tags,
            "!foo bar\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "---\n"
            "!foo bar\n"
            ,
            "--- !foo bar\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !foo bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL <!foo> :bar\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <!foo> :bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!", "tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_(TestSuite9WXW_0_scalar_resolved, resolve_all_tags,
            "!foo bar\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "---\n"
            "!foo bar\n"
            ,
            "--- !<!foo> bar\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !<tag:example.com,2000:app/foo> bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL <!foo> :bar\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <tag:example.com,2000:app/foo> :bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("<!foo>"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!", "tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<tag:example.com,2000:app/foo>"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TestSuite9WXW_0_seqflow,
            "!foo [a, b, c]\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "---\n"
            "!foo [a, b, c]\n"
            ,
            "--- !foo [a,b,c]\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !foo [a,b,c]\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ [] <!foo>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "=VAL :c\n"
            "-SEQ\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "+SEQ [] <!foo>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "=VAL :c\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!foo"));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!", "tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TestSuite9WXW_0_seqflow_all, resolve_all_tags,
            "!foo [a, b, c]\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "---\n"
            "!foo [a, b, c]\n"
            ,
            "--- !<!foo> [a,b,c]\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !<tag:example.com,2000:app/foo> [a,b,c]\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ [] <!foo>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "=VAL :c\n"
            "-SEQ\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "+SEQ [] <tag:example.com,2000:app/foo>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "=VAL :c\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("<!foo>"));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!", "tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<tag:example.com,2000:app/foo>"));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TestSuite9WXW_0_mapflow,
            "!foo {a, b, c}\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "---\n"
            "!foo {a, b, c}\n"
            ,
            "--- !foo {a: ,b: ,c: }\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !foo {a: ,b: ,c: }\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {} <!foo>\n"
            "=VAL :a\n"
            "=VAL :\n"
            "=VAL :b\n"
            "=VAL :\n"
            "=VAL :c\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "+MAP {} <!foo>\n"
            "=VAL :a\n"
            "=VAL :\n"
            "=VAL :b\n"
            "=VAL :\n"
            "=VAL :c\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!foo"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!", "tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TestSuite9WXW_0_mapflow_all, resolve_all_tags,
            "!foo {a, b, c}\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "---\n"
            "!foo {a, b, c}\n"
            ,
            "--- !<!foo> {a: ,b: ,c: }\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !<tag:example.com,2000:app/foo> {a: ,b: ,c: }\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {} <!foo>\n"
            "=VAL :a\n"
            "=VAL :\n"
            "=VAL :b\n"
            "=VAL :\n"
            "=VAL :c\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "+MAP {} <tag:example.com,2000:app/foo>\n"
            "=VAL :a\n"
            "=VAL :\n"
            "=VAL :b\n"
            "=VAL :\n"
            "=VAL :c\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("<!foo>"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!", "tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<tag:example.com,2000:app/foo>"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TestSuite9WXW_1_followup0,
            "%TAG ! single-\n"
            "%TAG !! double-\n"
            "%TAG !m! prefixed-\n"
            "---\n"
            "- !foo bar\n"
            "- !!foo bat\n"
            "- !m!foo bat\n"
            "---\n"
            "- !foo bar\n"
            "- !!foo bat\n"
            "- !!str bat\n"
            ,
            "%TAG ! single-\n"
            "%TAG !! double-\n"
            "%TAG !m! prefixed-\n"
            "---\n"
            "- !foo bar\n"
            "- !!foo bat\n"
            "- !m!foo bat\n"
            "---\n"
            "- !foo bar\n"
            "- !!foo bat\n"
            "- !!str bat\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "+SEQ\n"
            "=VAL <!foo> :bar\n"
            "=VAL <!!foo> :bat\n"
            "=VAL <!m!foo> :bat\n"
            "-SEQ\n"
            "-DOC\n"
            "+DOC ---\n"
            "+SEQ\n"
            "=VAL <!foo> :bar\n"
            "=VAL <!!foo> :bat\n"
            "=VAL <!!str> :bat\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!", "single-"));
    ___(ps.add_directive_tag("!!", "double-"));
    ___(ps.add_directive_tag("!m!", "prefixed-"));
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!foo"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!m!foo"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!foo"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TestSuite9WXW_1_followup0_all, resolve_all_tags,
            "%TAG ! single-\n"
            "%TAG !! double-\n"
            "%TAG !m! prefixed-\n"
            "---\n"
            "- !foo bar\n"
            "- !!foo bat\n"
            "- !m!foo bat\n"
            "---\n"
            "- !foo bar\n"
            "- !!foo bat\n"
            "- !!str bat\n"
            ,
            "%TAG ! single-\n"
            "%TAG !! double-\n"
            "%TAG !m! prefixed-\n"
            "---\n"
            "- !<single-foo> bar\n"
            "- !<double-foo> bat\n"
            "- !<prefixed-foo> bat\n"
            "---\n"
            "- !<!foo> bar\n"
            "- !<tag:yaml.org,2002:foo> bat\n"
            "- !<tag:yaml.org,2002:str> bat\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "+SEQ\n"
            "=VAL <single-foo> :bar\n"
            "=VAL <double-foo> :bat\n"
            "=VAL <prefixed-foo> :bat\n"
            "-SEQ\n"
            "-DOC\n"
            "+DOC ---\n"
            "+SEQ\n"
            "=VAL <!foo> :bar\n"
            "=VAL <tag:yaml.org,2002:foo> :bat\n"
            "=VAL <tag:yaml.org,2002:str> :bat\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!", "single-"));
    ___(ps.add_directive_tag("!!", "double-"));
    ___(ps.add_directive_tag("!m!", "prefixed-"));
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("<single-foo>"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<double-foo>"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<prefixed-foo>"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("<!foo>"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:foo>"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TestSuite9WXW_1_followup1,
            "---\n"
            "- !<single-foo> bar\n"
            "- !<double-foo> bat\n"
            "- !<prefixed-foo> bat\n"
            "---\n"
            "- !<!foo> bar\n"
            "- !<tag:yaml.org,2002:foo> bat\n"
            "- !<tag:yaml.org,2002:str> bat\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "+SEQ\n"
            "=VAL <single-foo> :bar\n"
            "=VAL <double-foo> :bat\n"
            "=VAL <prefixed-foo> :bat\n"
            "-SEQ\n"
            "-DOC\n"
            "+DOC ---\n"
            "+SEQ\n"
            "=VAL <!foo> :bar\n"
            "=VAL <tag:yaml.org,2002:foo> :bat\n"
            "=VAL <tag:yaml.org,2002:str> :bat\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("<single-foo>"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<double-foo>"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<prefixed-foo>"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("<!foo>"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:foo>"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
