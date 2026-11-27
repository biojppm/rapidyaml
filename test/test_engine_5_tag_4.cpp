#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


static constexpr const bool singleline = false;

static const ParserOptions resolve_all_tags = ParserOptions{}.resolve_tags(true).resolve_tags_all(true);

ENGINE_TEST_ERRLOC(TagTestSuiteLHL4_0, Location(2,1),
                   "---\n"
                   "!invalid{}tag scalar\n")

ENGINE_TEST_ERRLOC(TagTestSuiteLHL4_1, Location(2,1),
                   "---\n"
                   "!!invalid{}tag scalar\n")


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteUGM3,
            "--- !<tag:clarkevans.com,2002:invoice>\n"
            "invoice: 34843\n"
            "date: 2001-01-23\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP <tag:clarkevans.com,2002:invoice>\n"
            "=VAL :invoice\n"
            "=VAL :34843\n"
            "=VAL :date\n"
            "=VAL :2001-01-23\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<tag:clarkevans.com,2002:invoice>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("invoice"));
    ___(ps.set_val_scalar_plain("34843"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("date"));
    ___(ps.set_val_scalar_plain("2001-01-23"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteUKK6_02_0,
            "!"
            ,
            "!\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL <!> :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuite6WLZ_0,
            "--- !foo \"bar\"\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !foo \"bar\"\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!foo> \"bar\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <!foo> \"bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!", "tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuite6WLZ_0_all, resolve_all_tags,
            "--- !foo \"bar\"\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !foo \"bar\"\n"
            ,
            "--- !<!foo> \"bar\"\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !<tag:example.com,2000:app/foo> \"bar\"\n"
            ,
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
    ___(ps.set_val_tag("<!foo>"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!", "tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<tag:example.com,2000:app/foo>"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuite6WLZ_1,
            "--- !foo \"bar\"\n"
            "...\n"
            "%TAG ! tag:example.com,2000:app/\n"
            "--- !<tag:example.com,2000:app/foo> \"bar\"\n"
            ,
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
    ___(ps.add_directive_tag("!", "tag:example.com,2000:app/"));
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
ENGINE_TEST(DirectiveTestSuiteMUS6_01,
            "%YAML 1.1 #...\n"
             "---\n"
            ,
             "---\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_yaml("1.1"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(DirectiveTestSuiteMUS6,
            "%YAM 1.1\n" // ignored
            "---\n"
            ,
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

ENGINE_TEST_ERR(DirectiveTestSuite9HCY,
                "!foo \"bar\"\n"
                "%TAG ! tag:example.com,2000:app/\n"
                "---\n"
                "!foo \"bar\"\n")

ENGINE_TEST(DirectiveMultipleYAML_W4TN,
            ""
            "%YAML 1.2\n"
            "---\n"
            "foo\n"
            "...\n"
            "%YAML 1.2\n"
            "---\n"
            "# Empty\n"
            "...\n"
            ""
            ,
            "--- foo\n"
            "---\n"
            ""
            ,
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
    ___(ps.add_directive_yaml("1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive_yaml("1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain_empty());
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

ENGINE_TEST_ERR(DirectiveWrong,
                  "--- !foo \"ba\"\n"
                  "...\n"
                  "%TAG ,,,,,\n"
                  "--- !foo \"bar\"\n")

ENGINE_TEST_ERR(DirectiveTooManyTagd,
                  "--- !foo \"bar\"\n"
                  "...\n"
                  "%TAG ! tag:example.com,2000:app/\n"
                  "--- !foo \"bar\"\n"
                  "...\n"
                  "%TAG ! tag:example.com,2001:app/\n"
                  "...\n"
                  "--- !foo \"bar\"\n"
                  "...\n"
                  "%TAG ! tag:example.com,2002:app/\n"
                  "--- !foo \"bar\"\n"
                  "...\n"
                  "%TAG ! tag:example.com,2003:app/\n"
                  "--- !foo \"bar\"\n"
                  "...\n"
                  "%TAG ! tag:example.com,2004:app/\n"
                  "--- !foo \"bar\"\n")


//-----------------------------------------------------------------------------

ENGINE_TEST(TagEmptySeq0,
            "!!seq []\n"
            ,
            "!!seq []"
            ,
            "+STR\n+DOC\n+SEQ [] <!!seq>\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!!seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagEmptySeq0_all, resolve_all_tags,
            "!!seq []\n"
            ,
            "!<tag:yaml.org,2002:seq> []"
            ,
            "+STR\n+DOC\n+SEQ [] <tag:yaml.org,2002:seq>\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("<tag:yaml.org,2002:seq>"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagEmptySeq1,
            ""
            "- !!seq []\n"
            ""
            ,
            "+STR\n+DOC\n+SEQ\n+SEQ [] <!!seq>\n-SEQ\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!!seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagEmptySeq1_all, resolve_all_tags,
            ""
            "- !<tag:yaml.org,2002:seq> []\n"
            ""
            ,
            "+STR\n+DOC\n+SEQ\n+SEQ [] <tag:yaml.org,2002:seq>\n-SEQ\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("<tag:yaml.org,2002:seq>"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagEmptySeq2,
            ""
            "!!seq\n"
            "- !!seq []\n"
            ""
            ,
            "+STR\n+DOC\n+SEQ <!!seq>\n+SEQ [] <!!seq>\n-SEQ\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!!seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!!seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagEmptySeq2_all, resolve_all_tags,
            ""
            "!!seq\n"
            "- !!seq []\n"
            ""
            ,
            ""
            "!<tag:yaml.org,2002:seq>\n"
            "- !<tag:yaml.org,2002:seq> []\n"
            ""
            ,
            "+STR\n+DOC\n+SEQ <tag:yaml.org,2002:seq>\n+SEQ [] <tag:yaml.org,2002:seq>\n-SEQ\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("<tag:yaml.org,2002:seq>"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("<tag:yaml.org,2002:seq>"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagEmptyMap0,
            "!!map {}\n"
            ,
            "!!map {}"
            ,
            "+STR\n+DOC\n+MAP {} <!!map>\n-MAP\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!!map"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagEmptyMap0_all, resolve_all_tags,
            "!!map {}\n"
            ,
            "!<tag:yaml.org,2002:map> {}"
            ,
            "+STR\n+DOC\n+MAP {} <tag:yaml.org,2002:map>\n-MAP\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("<tag:yaml.org,2002:map>"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagEmptyMap1,
            ""
            "- !!map {}\n"
            ""
            ,
            "+STR\n+DOC\n+SEQ\n+MAP {} <!!map>\n-MAP\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!!map"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagEmptyMap1_all, resolve_all_tags,
            ""
            "- !!map {}\n"
            ""
            ,
            ""
            "- !<tag:yaml.org,2002:map> {}\n"
            ""
            ,
            "+STR\n+DOC\n+SEQ\n+MAP {} <tag:yaml.org,2002:map>\n-MAP\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("<tag:yaml.org,2002:map>"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagEmptyMap2,
            ""
            "!!seq\n"
            "- !!map {}\n"
            ""
            ,
            "+STR\n+DOC\n+SEQ <!!seq>\n+MAP {} <!!map>\n-MAP\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!!seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!!map"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagEmptyMap2_all, resolve_all_tags,
            ""
            "!!seq\n"
            "- !!map {}\n"
            ""
            ,
            ""
            "!<tag:yaml.org,2002:seq>\n"
            "- !<tag:yaml.org,2002:map> {}\n"
            ""
            ,
            "+STR\n+DOC\n+SEQ <tag:yaml.org,2002:seq>\n+MAP {} <tag:yaml.org,2002:map>\n-MAP\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("<tag:yaml.org,2002:seq>"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("<tag:yaml.org,2002:map>"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

} // namespace yml
} // namespace c4
