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


} // namespace yml
} // namespace c4
