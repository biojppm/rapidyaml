#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

namespace c4 {
namespace yml {


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

ENGINE_TEST_ERR(DirectiveTestSuite9MMA_YAML,
                "%YAML 1.2")

ENGINE_TEST_ERR(DirectiveTestSuite9MMA_TAG,
                "%TAG ! tag:example.com,2000:app/\n")

ENGINE_TEST_ERR(DirectiveTestSuiteMUS6_00,
                "%YAML 1.1#...\n"
                "---\n")

ENGINE_TEST_ERR(DirectiveTestSuite9HCY,
                "!foo \"bar\"\n"
                "%TAG ! tag:example.com,2000:app/\n"
                "---\n"
                "!foo \"bar\"\n")

ENGINE_TEST(DirectiveTestSuiteMUS6,
            ("%YAM 1.1\n"
             "---\n",
             "---\n"),
            "+STR\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive("%YAM 1.1"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DirectiveMultipleYAML_W4TN,
            (""
             "%YAML 1.2\n"
             "---\n"
             "foo\n"
             "...\n"
             "%YAML 1.2\n"
             "---\n"
             "# Empty\n"
             "...\n"
             "",
             "--- foo\n"
             "---\n"
             ""),
            "+STR\n"
            "+DOC ---\n"
            "=VAL :foo\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC ...\n"
            "-STR\n"
            "")
{
    ___(ps.begin_stream());
    ___(ps.add_directive("%YAML 1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive("%YAML 1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}

ENGINE_TEST_ERR(DirectiveMultipleYAML_0,
                "%YAML 1.2\n"
                "...\n"
                "%YAML 1.2\n"
                "---\n"
                "bar")

ENGINE_TEST_ERR(DirectiveMultipleYAML_1,
                "%YAML 1.1\n"
                "...\n"
                "%YAML 1.2\n"
                "---\n"
                "bar")

ENGINE_TEST_ERR(DirectiveMultipleYAML_2,
                "%YAML 1.2\n"
                "%YAML 1.2\n"
                "---\n"
                "bar")

ENGINE_TEST_ERR(DirectiveMultipleYAML_3,
                "%YAML 1.1\n"
                "%YAML 1.2\n"
                "---\n"
                "bar")

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
