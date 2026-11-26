#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {



static const ParserOptions resolve_all_tags = ParserOptions{}.resolve_tags(true).resolve_tags_all(true);


//-----------------------------------------------------------------------------

ENGINE_TEST(TagYs0,
            ""
            "!yamlscript/v0/bare\n"
            "--- !code\n"
            "--- !data\n"
            "--- !code\n"
            "--- !data\n"
            ""
            ,
            ""
            "--- !yamlscript/v0/bare\n"
            "--- !code\n"
            "--- !data\n"
            "--- !code\n"
            "--- !data\n"
            ""
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL <!yamlscript/v0/bare> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!code> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!data> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!code> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!data> :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!yamlscript/v0/bare"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!code"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!data"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!code"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!data"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagYs1,
            ""
            "!yamlscript/v0/bare\n"
            "--- !code\n"
            "42\n"
            ""
            ,
            ""
            "--- !yamlscript/v0/bare\n"
            "--- !code 42\n"
            ""
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL <!yamlscript/v0/bare> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!code> :42\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!yamlscript/v0/bare"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!code"));
    ___(ps.set_val_scalar_plain("42"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DirectiveNotAtLineBeginningYamlSpace, Location(1, 2), " %YAML 1.2\n---\n")
ENGINE_TEST_ERRLOC(DirectiveNotAtLineBeginningYamlTab, Location(1, 2), "\t%YAML 1.2\n---\n")
ENGINE_TEST_ERRLOC(DirectiveNotAtLineBeginningOtherTab, Location(1, 2), "\t%FOO\n---\n")
ENGINE_TEST_ERRLOC(DirectiveNotAtLineBeginningTagSpace, Location(1, 2), " %TAG !yaml! tag:yaml.org,2002:\n---\n")
ENGINE_TEST_ERRLOC(DirectiveNotAtLineBeginningTagTab, Location(1, 2), "\t%TAG !yaml! tag:yaml.org,2002:\n---\n")

ENGINE_TEST_ERRLOC(DirectiveYamlBad00, Location(1, 1), "%YAML\n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad01, Location(1, 1), "%YAML    \n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad03, Location(1, 1), "%YAML aa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad04, Location(1, 1), "%YAML\taa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad05, Location(1, 1), "%YAML a.a\n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad06, Location(1, 1), "%YAML\ta.a\n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad07, Location(1, 9), "%YAML 1.a\n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad08, Location(1, 9), "%YAML\t1.a\n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad09, Location(1, 1), "%YAML a.1\n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad10, Location(1, 1), "%YAML\ta.1\n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad11, Location(1, 11), "%YAML 1.2 aa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveYamlBad12, Location(1, 11), "%YAML 1.2\taa\n---\n")

ENGINE_TEST_ERRLOC(DirectiveTagBad00, Location(1, 1), "%TAG\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad01, Location(1, 1), "%TAG    \n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad04, Location(1, 1), "%TAG aa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad05, Location(1, 1), "%TAG\taa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad06, Location(1, 1), "%TAG !\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad07, Location(1, 1), "%TAG\t!\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad08, Location(1, 1), "%TAG !!\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad09, Location(1, 1), "%TAG\t!!\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad10, Location(1, 1), "%TAG !a\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad11, Location(1, 1), "%TAG\t!a\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad12, Location(1, 1), "%TAG !a!\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad13, Location(1, 1), "%TAG\t!a!\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad14, Location(1, 34), "%TAG ! tag:example.com,2000:app/ aa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad15, Location(1, 34), "%TAG\t! tag:example.com,2000:app/ aa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad16, Location(1, 35), "%TAG !! tag:example.com,2000:app/ aa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad17, Location(1, 35), "%TAG\t!! tag:example.com,2000:app/ aa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad18, Location(1, 1), "%TAG !a tag:example.com,2000:app/ aa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad19, Location(1, 1), "%TAG\t!a tag:example.com,2000:app/ aa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad20, Location(1, 36), "%TAG !a! tag:example.com,2000:app/ aa\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad21, Location(1, 36), "%TAG\t!a! tag:example.com,2000:app0 aa/\n---\n")

ENGINE_TEST_ERRLOC(DirectiveTagBad22, Location(1, 1), "%TAG !A-UPPERCASE-now-bad:$! tag:example.com,2000:app0/\n---\n")
ENGINE_TEST_ERRLOC(DirectiveTagBad23, Location(1, 1), "%TAG !A-UPPERCASE-now-bad:$! tag:example.com,2000:app0/\n---\n")


template<class Ps>
static void test_directive(Ps &ps)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
#define DIRECTIVE_TEST(name, directive) \
    ENGINE_TEST(name, NO_COMPARE_EMITTED,                               \
                directive "\n"                                          \
                "---\n"                                                 \
                ,                                                       \
                "---\n"                                                 \
                ,                                                       \
                "+STR\n"                                                \
                "+DOC ---\n"                                            \
                "=VAL :\n"                                              \
                "-DOC\n"                                                \
                "-STR\n")                                               \
    {                                                                   \
        test_directive(ps);                                             \
    }

DIRECTIVE_TEST(DirectiveYamlOk0, "%YAML 1.2  #comm")
DIRECTIVE_TEST(DirectiveYamlOk1, "%YAML 1.2 #comm")
DIRECTIVE_TEST(DirectiveYamlOk2, "%YAML\t1.2\t#comm")
DIRECTIVE_TEST(DirectiveYamlOk3, "%YAML  1.2  #comm")
DIRECTIVE_TEST(DirectiveYamlOk4, "%YAML\t\t1.2\t\t#comm")
DIRECTIVE_TEST(DirectiveYamlOk5, "%YAML  \t  1.2  \t #comm")
DIRECTIVE_TEST(DirectiveYamlOk6, "%YAM  \t  1.2  \t #comm")
DIRECTIVE_TEST(DirectiveYamlOk7, "%YAMLL  \t  1.2  \t #comm")

DIRECTIVE_TEST(DirectiveTagOk14, "%TAG ! tag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk15, "%TAG\t!\ttag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk16, "%TAG !! tag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk17, "%TAG\t!!\ttag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk20, "%TAG !a! tag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk21, "%TAG\t!a!\ttag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk22, "%TAG  !a!  tag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk23, "%TAG\t\t!a!\t\ttag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk24, "%TAG \t !a! \t tag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk25, "%TAG\t \t!a!\t \ttag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk26, "%TA \t !a! \t tag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk28, "%TAGG\t \t!a!\t \ttag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk29, "%TAG !Ab-09! tag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk30, "%TAG\t!Ab-09!\ttag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk31, "%TAG  !Ab-09!  tag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk32, "%TAG\t\t!Ab-09!\t\ttag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk33, "%TAG \t !Ab-09! \t tag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk34, "%TAG\t \t!Ab-09!\t \ttag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk35, "%TA \t !Ab-09! \t tag:example.com,2000:app/")
DIRECTIVE_TEST(DirectiveTagOk36, "%TAGG\t \t!Ab-09!\t \ttag:example.com,2000:app/")


//-----------------------------------------------------------------------------


ENGINE_TEST(TagTestSuiteQLJ7_0_custom,
            "--- !A\n"
            "a: b\n"
            "--- !B\n"
            "c: d\n"
            "--- !C\n"
            "e: f\n"
            ""
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP <!A>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP <!B>\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP <!C>\n"
            "=VAL :e\n"
            "=VAL :f\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
            "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!A"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!B"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!C"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("e"));
    ___(ps.set_val_scalar_plain("f"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteQLJ7_0_custom_all, resolve_all_tags,
            "--- !A\n"
            "a: b\n"
            "--- !B\n"
            "c: d\n"
            "--- !C\n"
            "e: f\n"
            ""
            ,
            "--- !<!A>\n"
            "a: b\n"
            "--- !<!B>\n"
            "c: d\n"
            "--- !<!C>\n"
            "e: f\n"
            ""
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP <!A>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP <!B>\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP <!C>\n"
            "=VAL :e\n"
            "=VAL :f\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
            "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!A>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!B>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!C>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("e"));
    ___(ps.set_val_scalar_plain("f"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteQLJ7_0_correct, resolve_all_tags,
            "%TAG !prefix! doc0:\n"
            "--- !prefix!A\n"
            "a: b\n"
            "...\n"
            "%TAG !prefix! doc1:\n"
            "--- !prefix!B\n"
            "c: d\n"
            "...\n"
            "%TAG !prefix! doc2:\n"
            "--- !prefix!C\n"
            "e: f\n"
            ,
            "%TAG !prefix! doc0:\n"
            "--- !<doc0:A>\n"
            "a: b\n"
            "...\n"
            "%TAG !prefix! doc1:\n"
            "--- !<doc1:B>\n"
            "c: d\n"
            "...\n"
            "%TAG !prefix! doc2:\n"
            "--- !<doc2:C>\n"
            "e: f\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP <doc0:A>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "+MAP <doc1:B>\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "+MAP <doc2:C>\n"
            "=VAL :e\n"
            "=VAL :f\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!prefix!", "doc0:"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<doc0:A>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!prefix!", "doc1:"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<doc1:B>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!prefix!", "doc2:"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<doc2:C>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("e"));
    ___(ps.set_val_scalar_plain("f"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERROPT_(TagTestSuiteQLJ7_0_no_directive, resolve_all_tags, Location(1, 14),
                   "--- !prefix!A\n"
                   "a: b\n"
                   "--- !prefix!B\n"
                   "c: d\n"
                   "--- !prefix!C\n"
                   "e: f\n"
                   "")

ENGINE_TEST_ERROPT_(TagTestSuiteQLJ7_1, resolve_all_tags, Location(4, 14),
                   "%TAG !prefix! tag:example.com,2011:\n"
                   "--- !prefix!A\n"
                   "a: b\n"
                   "--- !prefix!B\n" // error here. %TAG applies only to next doc
                   "c: d\n"
                   "--- !prefix!C\n"
                   "e: f\n"
                   "")

ENGINE_TEST_ERROPT_(TagTestSuiteQLJ7_2, resolve_all_tags, Location(8, 14),
                   "%TAG !prefix! doc0:\n"
                   "--- !prefix!A\n"
                   "a: b\n"
                   "...\n"
                   "%TAG !prefix! doc1:\n"
                   "--- !prefix!B\n"
                   "c: d\n"
                   "--- !prefix!C\n" // error here. %TAG applies only to next doc
                   "e: f\n"
                   "")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_0, Location(1, 7), ""
                   "[!t !u]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_1, Location(1, 9), ""
                   "[!t !u c]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_2, Location(1, 7), ""
                   "[!t !u,]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_3, Location(1, 9), ""
                   "[!t !u c,]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_4, Location(1, 10), ""
                   "[0, !t !u]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_5, Location(1, 12), ""
                   "[0, !t !u c]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_6, Location(1, 10), ""
                   "[0, !t !u,]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_7, Location(1, 12), ""
                   "[0, !t !u c,]")


ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_0, Location(1, 8), ""
                   "- !t !u")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_1, Location(1, 9), ""
                   "- !t !u c")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_2, Location(1, 8), ""
                   "- !t !u\n-")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_3, Location(1, 9), ""
                   "- !t !u c\n-")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_4, Location(2, 8), ""
                   "- 0\n- !t !u")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_5, Location(2, 9), ""
                   "- 0\n- !t !u c")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_6, Location(2, 8), ""
                   "- 0\n- !t !u\n-")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_7, Location(2, 9), ""
                   "- 0\n- !t !u c\n-")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_8, Location(1, 8), ""
                   "- !t !u\n  - nested")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_9, Location(1, 8), ""
                   "- !t !u\n  nested: map")


ENGINE_TEST_ERRLOC(DoubleTagMapflowKey0_0, Location(1, 12), ""
                   "{!t !u: val}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowKey0_1, Location(1, 9), ""
                   "{!t !u c: val}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowKey0_4, Location(1, 15), ""
                   "{0, !t !u: val}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowKey0_5, Location(1, 12), ""
                   "{0, !t !u c: val}")

ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_0, Location(1, 12), ""
                   "{key: !t !u}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_1, Location(1, 14), ""
                   "{key: !t !u c}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_4, Location(1, 20), ""
                   "{0: 1, key: !t !u c}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_5, Location(1, 20), ""
                   "{0: 1, key: !t !u c}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_6, Location(1, 12), ""
                   "{key: !t !u,}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_7, Location(1, 14), ""
                   "{key: !t !u c,}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_8, Location(1, 20), ""
                   "{0: 1, key: !t !u c,}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_9, Location(1, 20), ""
                   "{0: 1, key: !t !u c,}")


ENGINE_TEST_ERRLOC(DoubleTagMapBlckKey0_0, Location(1, 8), ""
                   "!t !u: val")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckKey0_1, Location(1, 7), ""
                   "!t !u c: val")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckKey0_4, Location(2, 6), ""
                   "0\n!t !u: val")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckKey0_5, Location(2, 8), ""
                   "0\n!t !u c: val")

ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_0, Location(1, 11), ""
                   "key: !t !u")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_1, Location(1, 12), ""
                   "key: !t !u c")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_4, Location(2, 12), ""
                   "0: 1\nkey: !t !u c")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_5, Location(2, 12), ""
                   "0: 1\nkey: !t !u c")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_6, Location(1, 11), ""
                   "key: !t !u\n")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_7, Location(1, 12), ""
                   "key: !t !u c\n")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_8, Location(2, 12), ""
                   "0: 1\nkey: !t !u c\n")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_9, Location(2, 12), ""
                   "0: 1\nkey: !t !u c\n")

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
