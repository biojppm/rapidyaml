#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


static constexpr const bool singleline = false;

static const ParserOptions resolve_all_tags = ParserOptions{}.resolve_tags(true).resolve_tags_all(true);


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(TagTestSuiteU99R_00_0, Location(1,3),
                   "- ,xxx\n")
ENGINE_TEST_ERRLOC(TagTestSuiteU99R_00_1, Location(1,3),
                   "- , xxx\n")
ENGINE_TEST_ERRLOC(TagTestSuiteU99R_0, Location(1,8),
                   "- !!str, xxx\n")
ENGINE_TEST_ERRLOC(TagTestSuiteU99R_1, Location(1,7),
                   "- !str, xxx\n")

ENGINE_TEST(TagTestSuiteU99R_2,
            "[!!str, xxx]"
            ,
            "[!!str ,xxx]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <!!str> :\n"
            "=VAL :xxx\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("xxx"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteU99R_2_all, resolve_all_tags,
            "[!!str, xxx]"
            ,
            "[!<tag:yaml.org,2002:str> ,xxx]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL :xxx\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("xxx"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteU99R_2_1,
            "[!str, xxx]","[!str ,xxx]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <!str> :\n"
            "=VAL :xxx\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("xxx"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteU99R_2_1_all, resolve_all_tags,
            "[!str, xxx]","[!<!str> ,xxx]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <!str> :\n"
            "=VAL :xxx\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("<!str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("xxx"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteU99R_3,
            "{!!str, xxx}"
            ,
            "{!!str : ,xxx: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!!str> :\n"
            "=VAL :\n"
            "=VAL :xxx\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!!str"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("xxx"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteU99R_3_all, resolve_all_tags,
            "{!!str, xxx}"
            ,
            "{!<tag:yaml.org,2002:str> : ,xxx: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL :\n"
            "=VAL :xxx\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("xxx"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteU99R_3_1,
            "{!str, xxx}"
            ,
            "{!str : ,xxx: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!str> :\n"
            "=VAL :\n"
            "=VAL :xxx\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!str"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("xxx"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteU99R_3_1_all, resolve_all_tags,
            "{!str, xxx}"
            ,
            "{!<!str> : ,xxx: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!str> :\n"
            "=VAL :\n"
            "=VAL :xxx\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("<!str>"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("xxx"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteWZ62_0_0_0,
            "{foo: !!str , !!str : bar}"
            ,
            "{foo: !!str ,!!str : bar}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <!!str> :\n"
            "=VAL <!!str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!!str"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteWZ62_0_0_0_all, resolve_all_tags,
            "{foo: !!str , !!str : bar}"
            ,
            "{foo: !<tag:yaml.org,2002:str> ,!<tag:yaml.org,2002:str> : bar}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteWZ62_0_0_1,
            "foo: !!str\n!!str : bar\n"
            ,
            "foo: !!str \n!!str : bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <!!str> :\n"
            "=VAL <!!str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!!str"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteWZ62_0_0_1_all, resolve_all_tags,
            "foo: !!str\n!!str : bar\n"
            ,
            "foo: !<tag:yaml.org,2002:str> \n!<tag:yaml.org,2002:str> : bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteWZ62_0_1_0,
            "{foo: !str , !str : bar}"
            ,
            "{foo: !str ,!str : bar}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!str"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteWZ62_0_1_0_all, resolve_all_tags,
            "{foo: !str , !str : bar}"
            ,
            "{foo: !<!str> ,!<!str> : bar}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("<!str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("<!str>"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteWZ62_0_1_1,
            "foo: !str\n!str : bar\n"
            ,
            "foo: !str \n!str : bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!str"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteWZ62_0_1_1_all, resolve_all_tags,
            "foo: !str\n!str : bar\n"
            ,
            "foo: !<!str> \n!<!str> : bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("<!str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("<!str>"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteWZ62_1_0_0,
            "{foo: !!str, !!str: bar}"
            ,
            "{foo: !!str ,!!str: bar: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <!!str> :\n"
            "=VAL <!!str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!!str:"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteWZ62_1_0_0_all, resolve_all_tags,
            "{foo: !!str, !!str: bar}"
            ,
            "{foo: !<tag:yaml.org,2002:str> ,!<tag:yaml.org,2002:str:> bar: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("<tag:yaml.org,2002:str:>"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteWZ62_1_0_1,
            "foo: !!str\n!!str: bar:\n"
            ,
            "foo: !!str \n!!str: bar: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <!!str> :\n"
            "=VAL <!!str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!!str:"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteWZ62_1_0_1_all, resolve_all_tags,
            "foo: !!str\n!!str: bar:\n"
            ,
            "foo: !<tag:yaml.org,2002:str> \n!<tag:yaml.org,2002:str:> bar: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("<tag:yaml.org,2002:str:>"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteWZ62_1_1_0,
            "{foo: !str, !str: bar}"
            ,
            "{foo: !str ,!str: bar: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!str:"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteWZ62_1_1_0_all, resolve_all_tags,
            "{foo: !str, !str: bar}"
            ,
            "{foo: !<!str> ,!<!str:> bar: }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("<!str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("<!str:>"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagTestSuiteWZ62_1_1_1,
            "foo: !str\n"
            "!str: bar:\n"
            ,
            "foo: !str \n"
            "!str: bar: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!str:"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteWZ62_1_1_1_all, resolve_all_tags,
            "foo: !str\n"
            "!str: bar:\n"
            ,
            "foo: !<!str> \n"
            "!<!str:> bar: \n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("<!str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("<!str:>"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

} // namespace yml
} // namespace c4
