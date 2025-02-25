#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"
#include "./test_lib/test_case.hpp"

namespace c4 {
namespace yml {


//-----------------------------------------------------------------------------

TEST(tag_functions, to_tag)
{
    EXPECT_EQ(to_tag("!"), TAG_NONE);
    EXPECT_EQ(to_tag("!."), TAG_NONE);
    EXPECT_EQ(to_tag("!good_type"), TAG_NONE);
    EXPECT_EQ(to_tag("!!"          ), TAG_NONE);
    EXPECT_EQ(to_tag("!!."         ), TAG_NONE);

    EXPECT_EQ(to_tag("!!map"       ), TAG_MAP);
    EXPECT_EQ(to_tag("!!omap"      ), TAG_OMAP);
    EXPECT_EQ(to_tag("!!pairs"     ), TAG_PAIRS);
    EXPECT_EQ(to_tag("!!set"       ), TAG_SET);
    EXPECT_EQ(to_tag("!!seq"       ), TAG_SEQ);
    EXPECT_EQ(to_tag("!!binary"    ), TAG_BINARY);
    EXPECT_EQ(to_tag("!!bool"      ), TAG_BOOL);
    EXPECT_EQ(to_tag("!!float"     ), TAG_FLOAT);
    EXPECT_EQ(to_tag("!!int"       ), TAG_INT);
    EXPECT_EQ(to_tag("!!merge"     ), TAG_MERGE);
    EXPECT_EQ(to_tag("!!null"      ), TAG_NULL);
    EXPECT_EQ(to_tag("!!str"       ), TAG_STR);
    EXPECT_EQ(to_tag("!!timestamp" ), TAG_TIMESTAMP);
    EXPECT_EQ(to_tag("!!value"     ), TAG_VALUE);

    EXPECT_EQ(to_tag("!!map."      ), TAG_NONE);
    EXPECT_EQ(to_tag("!!omap."     ), TAG_NONE);
    EXPECT_EQ(to_tag("!!pairs."    ), TAG_NONE);
    EXPECT_EQ(to_tag("!!set."      ), TAG_NONE);
    EXPECT_EQ(to_tag("!!seq."      ), TAG_NONE);
    EXPECT_EQ(to_tag("!!binary."   ), TAG_NONE);
    EXPECT_EQ(to_tag("!!bool."     ), TAG_NONE);
    EXPECT_EQ(to_tag("!!float."    ), TAG_NONE);
    EXPECT_EQ(to_tag("!!int."      ), TAG_NONE);
    EXPECT_EQ(to_tag("!!merge."    ), TAG_NONE);
    EXPECT_EQ(to_tag("!!null."     ), TAG_NONE);
    EXPECT_EQ(to_tag("!!str."      ), TAG_NONE);
    EXPECT_EQ(to_tag("!!timestamp."), TAG_NONE);
    EXPECT_EQ(to_tag("!!value."    ), TAG_NONE);
}

TEST(tag_functions, to_tag__with_namespace)
{
    EXPECT_EQ(to_tag("tag:yaml.org,2002:"          ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:."         ), TAG_NONE);

    EXPECT_EQ(to_tag("tag:yaml.org,2002:map"       ), TAG_MAP);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:omap"      ), TAG_OMAP);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:pairs"     ), TAG_PAIRS);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:set"       ), TAG_SET);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:seq"       ), TAG_SEQ);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:binary"    ), TAG_BINARY);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:bool"      ), TAG_BOOL);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:float"     ), TAG_FLOAT);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:int"       ), TAG_INT);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:merge"     ), TAG_MERGE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:null"      ), TAG_NULL);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:str"       ), TAG_STR);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:timestamp" ), TAG_TIMESTAMP);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:value"     ), TAG_VALUE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:yaml"      ), TAG_YAML);

    EXPECT_EQ(to_tag("tag:yaml.org,2002:map."      ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:omap."     ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:pairs."    ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:set."      ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:seq."      ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:binary."   ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:bool."     ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:float."    ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:int."      ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:merge."    ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:null."     ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:str."      ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:timestamp."), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:value."    ), TAG_NONE);
    EXPECT_EQ(to_tag("tag:yaml.org,2002:yaml."     ), TAG_NONE);
}

TEST(tag_functions, to_tag__with_namespace_bracket)
{
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:"           ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:."          ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:foo"        ), TAG_NONE);

    EXPECT_EQ(to_tag("<tag:yaml.org,2002:map>"       ), TAG_MAP);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:omap>"      ), TAG_OMAP);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:pairs>"     ), TAG_PAIRS);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:set>"       ), TAG_SET);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:seq>"       ), TAG_SEQ);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:binary>"    ), TAG_BINARY);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:bool>"      ), TAG_BOOL);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:float>"     ), TAG_FLOAT);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:int>"       ), TAG_INT);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:merge>"     ), TAG_MERGE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:null>"      ), TAG_NULL);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:str>"       ), TAG_STR);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:timestamp>" ), TAG_TIMESTAMP);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:value>"     ), TAG_VALUE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:yaml>"      ), TAG_YAML);

    EXPECT_EQ(to_tag("<tag:yaml.org,2002:map.>"      ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:omap.>"     ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:pairs.>"    ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:set.>"      ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:seq.>"      ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:binary.>"   ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:bool.>"     ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:float.>"    ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:int.>"      ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:merge.>"    ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:null.>"     ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:str.>"      ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:timestamp.>"), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:value.>"    ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:yaml.>"     ), TAG_NONE);
}

TEST(tag_functions, from_tag)
{
    EXPECT_EQ("", from_tag(TAG_NONE));

    EXPECT_EQ("!!map"       , from_tag(TAG_MAP));
    EXPECT_EQ("!!omap"      , from_tag(TAG_OMAP));
    EXPECT_EQ("!!pairs"     , from_tag(TAG_PAIRS));
    EXPECT_EQ("!!set"       , from_tag(TAG_SET));
    EXPECT_EQ("!!seq"       , from_tag(TAG_SEQ));
    EXPECT_EQ("!!binary"    , from_tag(TAG_BINARY));
    EXPECT_EQ("!!bool"      , from_tag(TAG_BOOL));
    EXPECT_EQ("!!float"     , from_tag(TAG_FLOAT));
    EXPECT_EQ("!!int"       , from_tag(TAG_INT));
    EXPECT_EQ("!!merge"     , from_tag(TAG_MERGE));
    EXPECT_EQ("!!null"      , from_tag(TAG_NULL));
    EXPECT_EQ("!!str"       , from_tag(TAG_STR));
    EXPECT_EQ("!!timestamp" , from_tag(TAG_TIMESTAMP));
    EXPECT_EQ("!!value"     , from_tag(TAG_VALUE));
    EXPECT_EQ("!!yaml"      , from_tag(TAG_YAML));
}

TEST(tag_functions, from_tag_long)
{
    EXPECT_EQ("", from_tag_long(TAG_NONE));

    EXPECT_EQ("<tag:yaml.org,2002:map>"       , from_tag_long(TAG_MAP));
    EXPECT_EQ("<tag:yaml.org,2002:omap>"      , from_tag_long(TAG_OMAP));
    EXPECT_EQ("<tag:yaml.org,2002:pairs>"     , from_tag_long(TAG_PAIRS));
    EXPECT_EQ("<tag:yaml.org,2002:set>"       , from_tag_long(TAG_SET));
    EXPECT_EQ("<tag:yaml.org,2002:seq>"       , from_tag_long(TAG_SEQ));
    EXPECT_EQ("<tag:yaml.org,2002:binary>"    , from_tag_long(TAG_BINARY));
    EXPECT_EQ("<tag:yaml.org,2002:bool>"      , from_tag_long(TAG_BOOL));
    EXPECT_EQ("<tag:yaml.org,2002:float>"     , from_tag_long(TAG_FLOAT));
    EXPECT_EQ("<tag:yaml.org,2002:int>"       , from_tag_long(TAG_INT));
    EXPECT_EQ("<tag:yaml.org,2002:merge>"     , from_tag_long(TAG_MERGE));
    EXPECT_EQ("<tag:yaml.org,2002:null>"      , from_tag_long(TAG_NULL));
    EXPECT_EQ("<tag:yaml.org,2002:str>"       , from_tag_long(TAG_STR));
    EXPECT_EQ("<tag:yaml.org,2002:timestamp>" , from_tag_long(TAG_TIMESTAMP));
    EXPECT_EQ("<tag:yaml.org,2002:value>"     , from_tag_long(TAG_VALUE));
    EXPECT_EQ("<tag:yaml.org,2002:yaml>"      , from_tag_long(TAG_YAML));
}

TEST(tag_functions, normalize_tag)
{
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:"          ), "<tag:yaml.org,2002:");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:."         ), "<tag:yaml.org,2002:.");

    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:map>"       ), "!!map");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:omap>"      ), "!!omap");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:pairs>"     ), "!!pairs");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:set>"       ), "!!set");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:seq>"       ), "!!seq");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:binary>"    ), "!!binary");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:bool>"      ), "!!bool");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:float>"     ), "!!float");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:int>"       ), "!!int");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:merge>"     ), "!!merge");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:null>"      ), "!!null");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:str>"       ), "!!str");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:timestamp>" ), "!!timestamp");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:value>"     ), "!!value");
    EXPECT_EQ(normalize_tag("<tag:yaml.org,2002:yaml>"      ), "!!yaml");

    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:map>"      ), "!!map");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:omap>"     ), "!!omap");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:pairs>"    ), "!!pairs");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:set>"      ), "!!set");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:seq>"      ), "!!seq");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:binary>"   ), "!!binary");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:bool>"     ), "!!bool");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:float>"    ), "!!float");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:int>"      ), "!!int");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:merge>"    ), "!!merge");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:null>"     ), "!!null");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:str>"      ), "!!str");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:timestamp>"), "!!timestamp");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:value>"    ), "!!value");
    EXPECT_EQ(normalize_tag("!<tag:yaml.org,2002:yaml>"     ), "!!yaml");

    EXPECT_EQ(normalize_tag("!!map"      ), "!!map");
    EXPECT_EQ(normalize_tag("!!omap"     ), "!!omap");
    EXPECT_EQ(normalize_tag("!!pairs"    ), "!!pairs");
    EXPECT_EQ(normalize_tag("!!set"      ), "!!set");
    EXPECT_EQ(normalize_tag("!!seq"      ), "!!seq");
    EXPECT_EQ(normalize_tag("!!binary"   ), "!!binary");
    EXPECT_EQ(normalize_tag("!!bool"     ), "!!bool");
    EXPECT_EQ(normalize_tag("!!float"    ), "!!float");
    EXPECT_EQ(normalize_tag("!!int"      ), "!!int");
    EXPECT_EQ(normalize_tag("!!merge"    ), "!!merge");
    EXPECT_EQ(normalize_tag("!!null"     ), "!!null");
    EXPECT_EQ(normalize_tag("!!str"      ), "!!str");
    EXPECT_EQ(normalize_tag("!!timestamp"), "!!timestamp");
    EXPECT_EQ(normalize_tag("!!value"    ), "!!value");
    EXPECT_EQ(normalize_tag("!!yaml"     ), "!!yaml");

    EXPECT_EQ(normalize_tag("!!foo"      ), "!!foo");

    EXPECT_EQ(normalize_tag("!my-light"), "!my-light");
    EXPECT_EQ(normalize_tag("!foo"), "!foo");
    EXPECT_EQ(normalize_tag("<!foo>"), "<!foo>");
    EXPECT_EQ(normalize_tag("<foo>"), "<foo>");
    EXPECT_EQ(normalize_tag("<!>"), "<!>");

    EXPECT_EQ(normalize_tag("!<!foo>"), "<!foo>");
    EXPECT_EQ(normalize_tag("!<foo>"), "<foo>");
    EXPECT_EQ(normalize_tag("!<!>"), "<!>");
}

TEST(tag_functions, normalize_tag_long)
{
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:"          ), "<tag:yaml.org,2002:");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:."         ), "<tag:yaml.org,2002:.");

    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:map>"       ), "<tag:yaml.org,2002:map>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:omap>"      ), "<tag:yaml.org,2002:omap>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:pairs>"     ), "<tag:yaml.org,2002:pairs>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:set>"       ), "<tag:yaml.org,2002:set>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:seq>"       ), "<tag:yaml.org,2002:seq>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:binary>"    ), "<tag:yaml.org,2002:binary>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:bool>"      ), "<tag:yaml.org,2002:bool>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:float>"     ), "<tag:yaml.org,2002:float>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:int>"       ), "<tag:yaml.org,2002:int>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:merge>"     ), "<tag:yaml.org,2002:merge>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:null>"      ), "<tag:yaml.org,2002:null>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:str>"       ), "<tag:yaml.org,2002:str>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:timestamp>" ), "<tag:yaml.org,2002:timestamp>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:value>"     ), "<tag:yaml.org,2002:value>");
    EXPECT_EQ(normalize_tag_long("<tag:yaml.org,2002:yaml>"      ), "<tag:yaml.org,2002:yaml>");

    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:map>"      ), "<tag:yaml.org,2002:map>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:omap>"     ), "<tag:yaml.org,2002:omap>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:pairs>"    ), "<tag:yaml.org,2002:pairs>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:set>"      ), "<tag:yaml.org,2002:set>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:seq>"      ), "<tag:yaml.org,2002:seq>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:binary>"   ), "<tag:yaml.org,2002:binary>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:bool>"     ), "<tag:yaml.org,2002:bool>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:float>"    ), "<tag:yaml.org,2002:float>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:int>"      ), "<tag:yaml.org,2002:int>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:merge>"    ), "<tag:yaml.org,2002:merge>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:null>"     ), "<tag:yaml.org,2002:null>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:str>"      ), "<tag:yaml.org,2002:str>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:timestamp>"), "<tag:yaml.org,2002:timestamp>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:value>"    ), "<tag:yaml.org,2002:value>");
    EXPECT_EQ(normalize_tag_long("!<tag:yaml.org,2002:yaml>"     ), "<tag:yaml.org,2002:yaml>");

    EXPECT_EQ(normalize_tag_long("!!map"      ), "<tag:yaml.org,2002:map>");
    EXPECT_EQ(normalize_tag_long("!!omap"     ), "<tag:yaml.org,2002:omap>");
    EXPECT_EQ(normalize_tag_long("!!pairs"    ), "<tag:yaml.org,2002:pairs>");
    EXPECT_EQ(normalize_tag_long("!!set"      ), "<tag:yaml.org,2002:set>");
    EXPECT_EQ(normalize_tag_long("!!seq"      ), "<tag:yaml.org,2002:seq>");
    EXPECT_EQ(normalize_tag_long("!!binary"   ), "<tag:yaml.org,2002:binary>");
    EXPECT_EQ(normalize_tag_long("!!bool"     ), "<tag:yaml.org,2002:bool>");
    EXPECT_EQ(normalize_tag_long("!!float"    ), "<tag:yaml.org,2002:float>");
    EXPECT_EQ(normalize_tag_long("!!int"      ), "<tag:yaml.org,2002:int>");
    EXPECT_EQ(normalize_tag_long("!!merge"    ), "<tag:yaml.org,2002:merge>");
    EXPECT_EQ(normalize_tag_long("!!null"     ), "<tag:yaml.org,2002:null>");
    EXPECT_EQ(normalize_tag_long("!!str"      ), "<tag:yaml.org,2002:str>");
    EXPECT_EQ(normalize_tag_long("!!timestamp"), "<tag:yaml.org,2002:timestamp>");
    EXPECT_EQ(normalize_tag_long("!!value"    ), "<tag:yaml.org,2002:value>");
    EXPECT_EQ(normalize_tag_long("!!yaml"     ), "<tag:yaml.org,2002:yaml>");

    EXPECT_EQ(normalize_tag_long("!!foo"      ), "!!foo");
    EXPECT_EQ(normalize_tag_long("!!light"    ), "!!light");

    EXPECT_EQ(normalize_tag_long("!m!foo"      ), "!m!foo");
    EXPECT_EQ(normalize_tag_long("!m!light"    ), "!m!light");

    char output[100];
    EXPECT_EQ(normalize_tag_long("!!foo"      , output), "<tag:yaml.org,2002:foo>");
    EXPECT_EQ(normalize_tag_long("!!light"    , output), "<tag:yaml.org,2002:light>");

    EXPECT_EQ(normalize_tag_long("!m!foo"      , output), "!m!foo");
    EXPECT_EQ(normalize_tag_long("!m!light"    , output), "!m!light");

    EXPECT_EQ(normalize_tag_long("!my-light"), "!my-light");
    EXPECT_EQ(normalize_tag_long("!foo"), "!foo");
    EXPECT_EQ(normalize_tag_long("<!foo>"), "<!foo>");
    EXPECT_EQ(normalize_tag_long("<foo>"), "<foo>");
    EXPECT_EQ(normalize_tag_long("<!>"), "<!>");

    EXPECT_EQ(normalize_tag_long("!<!foo>"), "<!foo>");
    EXPECT_EQ(normalize_tag_long("!<foo>"), "<foo>");
    EXPECT_EQ(normalize_tag_long("!<!foo>"), "<!foo>");
    EXPECT_EQ(normalize_tag_long("!<foo>"), "<foo>");
    EXPECT_EQ(normalize_tag_long("!<!>"), "<!>");
}

TEST(tag_functions, is_custom_tag)
{
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:."));

    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:map>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:omap>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:pairs>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:set>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:seq>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:binary>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:bool>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:float>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:int>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:merge>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:null>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:str>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:timestamp>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:value>"));
    EXPECT_FALSE(is_custom_tag("<tag:yaml.org,2002:yaml>"));

    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:map>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:omap>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:pairs>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:set>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:seq>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:binary>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:bool>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:float>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:int>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:merge>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:null>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:str>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:timestamp>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:value>"));
    EXPECT_FALSE(is_custom_tag("!<tag:yaml.org,2002:yaml>"));

    EXPECT_FALSE(is_custom_tag("!!map"));
    EXPECT_FALSE(is_custom_tag("!!omap"));
    EXPECT_FALSE(is_custom_tag("!!pairs"));
    EXPECT_FALSE(is_custom_tag("!!set"));
    EXPECT_FALSE(is_custom_tag("!!seq"));
    EXPECT_FALSE(is_custom_tag("!!binary"));
    EXPECT_FALSE(is_custom_tag("!!bool"));
    EXPECT_FALSE(is_custom_tag("!!float"));
    EXPECT_FALSE(is_custom_tag("!!int"));
    EXPECT_FALSE(is_custom_tag("!!merge"));
    EXPECT_FALSE(is_custom_tag("!!null"));
    EXPECT_FALSE(is_custom_tag("!!str"));
    EXPECT_FALSE(is_custom_tag("!!timestamp"));
    EXPECT_FALSE(is_custom_tag("!!value"));
    EXPECT_FALSE(is_custom_tag("!!yaml"));

    EXPECT_FALSE(is_custom_tag("!!foo"));
    EXPECT_FALSE(is_custom_tag("!!light"));

    EXPECT_TRUE(is_custom_tag("!m!foo"));
    EXPECT_TRUE(is_custom_tag("!m!light"));

    EXPECT_FALSE(is_custom_tag("!my-light"));
    EXPECT_FALSE(is_custom_tag("!foo"));
    EXPECT_FALSE(is_custom_tag("<!foo>"));
    EXPECT_FALSE(is_custom_tag("<foo>"));
    EXPECT_FALSE(is_custom_tag("<!>"));

    EXPECT_FALSE(is_custom_tag("!<!foo>"));
    EXPECT_FALSE(is_custom_tag("!<foo>"));
    EXPECT_FALSE(is_custom_tag("!<!foo>"));
    EXPECT_FALSE(is_custom_tag("!<foo>"));
    EXPECT_FALSE(is_custom_tag("!<!>"));
}


//-----------------------------------------------------------------------------

TEST(tag_directives, basic)
{
    Tree t = parse_in_arena(R"(
%TAG !m! !my-
--- # Bulb here
!m!light fluorescent
...
%TAG !m! !meta-
--- # Color here
!m!light green
)");
    EXPECT_EQ(t[0].val_tag(), "!m!light");
    EXPECT_EQ(t[1].val_tag(), "!m!light");
    EXPECT_EQ(t.num_tag_directives(), 2u);
    char buf_[100];
    EXPECT_EQ(t.resolve_tag_sub(buf_, "!m!light", 1u), csubstr("<!my-light>"));
    EXPECT_EQ(t.resolve_tag_sub(buf_, "!m!light", 2u), csubstr("<!meta-light>"));
    EXPECT_EQ(emitrs_yaml<std::string>(t), std::string(R"(%TAG !m! !my-
--- !m!light fluorescent
...
%TAG !m! !meta-
--- !m!light green
)"));
}

TEST(tag_directives, accepts_comment)
{
    Tree t = parse_in_arena(R"(
%TAG !m! !my-   # comment
--- # Bulb here
!m!light fluorescent
...
%TAG !m! !meta-   # comment
--- # Color here
!m!light green
)");
    EXPECT_EQ(t[0].val_tag(), "!m!light");
    EXPECT_EQ(t[1].val_tag(), "!m!light");
    EXPECT_EQ(t.num_tag_directives(), 2u);
    char buf_[100];
    EXPECT_EQ(t.resolve_tag_sub(buf_, "!m!light", 1u), csubstr("<!my-light>"));
    EXPECT_EQ(t.resolve_tag_sub(buf_, "!m!light", 2u), csubstr("<!meta-light>"));
    EXPECT_EQ(emitrs_yaml<std::string>(t), std::string(R"(%TAG !m! !my-
--- !m!light fluorescent
...
%TAG !m! !meta-
--- !m!light green
)"));
}

TEST(tag_directives, accepts_multiple_spaces)
{
    Tree t = parse_in_arena(R"(
%TAG    !m!    !my-   # comment
--- # Bulb here
!m!light fluorescent
...
%TAG    !m!    !meta-   # comment
--- # Color here
!m!light green
)");
    EXPECT_EQ(t[0].val_tag(), "!m!light");
    EXPECT_EQ(t[1].val_tag(), "!m!light");
    EXPECT_EQ(t.num_tag_directives(), 2u);
    char buf_[100];
    EXPECT_EQ(t.resolve_tag_sub(buf_, "!m!light", 1u), csubstr("<!my-light>"));
    EXPECT_EQ(t.resolve_tag_sub(buf_, "!m!light", 2u), csubstr("<!meta-light>"));
    EXPECT_EQ(emitrs_yaml<std::string>(t), std::string(R"(%TAG !m! !my-
--- !m!light fluorescent
...
%TAG !m! !meta-
--- !m!light green
)"));
}

void test_fail_tag_parsing(csubstr yaml)
{
    Tree t;
    ExpectError::check_error(&t, [&]{
        t = parse_in_arena(yaml);
    });
}

void test_fail_tag_resolve(csubstr yaml)
{
    Tree t;
    t = parse_in_arena(yaml);
    _c4dbg_tree(t);
    ExpectError::check_error(&t, [&]{
        t.resolve_tags();
    });
}

TEST(tag_directives, errors_parsing_tags_1)
{
    test_fail_tag_parsing(R"(
%TAG
--- # Bulb here
!m!light fluorescent)");
}

TEST(tag_directives, errors_parsing_tags_2)
{
    test_fail_tag_parsing(R"(
%TAG !m!
--- # Bulb here
!m!light fluorescent)");
}

TEST(tag_directives, errors_resolving_tags_1)
{
    test_fail_tag_resolve(R"(
!b!a asd
)");
}

TEST(tag_directives, errors_resolving_tags_2_not_an_error_1)
{
    Tree t = parse_in_arena(R"(!local foo)");
    t.resolve_tags();
    EXPECT_EQ(t.rootref().val_tag(), "!local");
    EXPECT_EQ(t.rootref().val(), "foo");
 }

TEST(tag_directives, errors_resolving_tags_2_not_an_error_2)
{
    Tree t = parse_in_arena(R"(! foo)");
    t.resolve_tags();
    EXPECT_EQ(t.rootref().val_tag(), "!");
    EXPECT_EQ(t.rootref().val(), "foo");
 }

TEST(tag_directives, resolve_tags)
{
    Tree t = parse_in_arena(R"(
%TAG !m! !my-   # comment
--- # Bulb here
!m!light fluorescent: !m!light bulb
...
%TAG !m! !meta-   # comment
--- # Color here
!m!light green: !m!light color
)");
    EXPECT_EQ(t.docref(0)[0].key_tag(), "!m!light");
    EXPECT_EQ(t.docref(0)[0].val_tag(), "!m!light");
    EXPECT_EQ(t.num_tag_directives(), 2u);
    t.resolve_tags();
    EXPECT_EQ(t.docref(0)[0].key_tag(), "<!my-light>");
    EXPECT_EQ(t.docref(0)[0].val_tag(), "<!my-light>");
    EXPECT_EQ(emitrs_yaml<std::string>(t), std::string(R"(%TAG !m! !my-
---
!<!my-light> fluorescent: !<!my-light> bulb
...
%TAG !m! !meta-
---
!<!meta-light> green: !<!meta-light> color
)"));
}

TEST(tag_directives, safe_with_empty_tree)
{
    Tree t;
    t.resolve_tags();
    EXPECT_TRUE(t.empty());
}

TEST(tag_directives, decode_uri_chars)
{
    {
        Tree t = parse_in_arena(R"(
%TAG !e! tag:example.com,2000:app/
---
- !e!%61%62%63%21 baz
)");
        t.resolve_tags();
        EXPECT_EQ(t.docref(0)[0].val_tag(), csubstr("<tag:example.com,2000:app/abc!>"));
    }
    {
        Tree t;
        auto checkerr = [&t](csubstr yaml){
            ExpectError::check_error(&t, [&]{
                t.clear();
                t = parse_in_arena(yaml);
                t.resolve_tags();
            });
        };
        {
            SCOPED_TRACE("without numbers at begin");
            checkerr(R"(
%TAG !e! tag:example.com,2000:app/
---
- !e!%%62%63 baz
)");
        }
        {
            SCOPED_TRACE("without numbers in the middle");
            checkerr(R"(
%TAG !e! tag:example.com,2000:app/
---
- !e!%61%%63 baz
)");
        }
        {
            SCOPED_TRACE("without numbers in the end");
            checkerr(R"(
%TAG !e! tag:example.com,2000:app/
---
- !e!%61%62% baz
)");
        }
        {
            SCOPED_TRACE("with wrong characters numbers at begin");
            checkerr(R"(
%TAG !e! tag:example.com,2000:app/
---
- !e!%h%62%63 baz
)");
        }
        {
            SCOPED_TRACE("with wrong characters in the middle");
            checkerr(R"(
%TAG !e! tag:example.com,2000:app/
---
- !e!%61%hh%63 baz
)");
        }
        {
            SCOPED_TRACE("with wrong characters in the end");
            checkerr(R"(
%TAG !e! tag:example.com,2000:app/
---
- !e!%61%62%hh baz
)");
        }
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(tags, test_suite_6WLZ)
{
    Tree t0 = parse_in_arena(R"(--- !foo "bar"
...
%TAG ! tag:example.com,2000:app/
--- !foo "bar"
--- !<tag:example.com,2000:app/foo> "bar"
)");
    EXPECT_EQ(t0.docref(0).val_tag(), "!foo");
    EXPECT_EQ(t0.docref(1).val_tag(), "!foo");
    EXPECT_EQ(t0.docref(2).val_tag(), "!<tag:example.com,2000:app/foo>");
    t0.resolve_tags();
    EXPECT_EQ(t0.docref(0).val_tag(), "!foo");
    EXPECT_EQ(t0.docref(1).val_tag(), "<tag:example.com,2000:app/foo>");
    EXPECT_EQ(t0.docref(2).val_tag(), "<tag:example.com,2000:app/foo>");
    std::string emitted0 = emitrs_yaml<std::string>(t0);
    EXPECT_EQ(emitted0, R"(--- !foo "bar"
...
%TAG ! tag:example.com,2000:app/
--- !<tag:example.com,2000:app/foo> "bar"
--- !<tag:example.com,2000:app/foo> "bar"
)");
    //
    Tree t1 = parse_in_arena(to_csubstr(emitted0));
    t1.resolve_tags();
    EXPECT_EQ(t1.docref(0).val_tag(), "!foo");
    EXPECT_EQ(t1.docref(1).val_tag(), "<tag:example.com,2000:app/foo>");
    EXPECT_EQ(t1.docref(2).val_tag(), "<tag:example.com,2000:app/foo>");
    std::string emitted1 = emitrs_yaml<std::string>(t1);
    EXPECT_EQ(emitted1, emitted0);
    //
    Tree t2 = parse_in_arena(to_csubstr(emitted1));
    t2.resolve_tags();
    EXPECT_EQ(t2.docref(0).val_tag(), "!foo");
    EXPECT_EQ(t2.docref(1).val_tag(), "<tag:example.com,2000:app/foo>");
    EXPECT_EQ(t2.docref(2).val_tag(), "<tag:example.com,2000:app/foo>");
    std::string emitted2 = emitrs_yaml<std::string>(t2);
    EXPECT_EQ(emitted2, emitted0);
    //
    Tree t3 = parse_in_arena(to_csubstr(emitted2));
    t3.resolve_tags();
    EXPECT_EQ(t3.docref(0).val_tag(), "!foo");
    EXPECT_EQ(t3.docref(1).val_tag(), "<tag:example.com,2000:app/foo>");
    EXPECT_EQ(t3.docref(2).val_tag(), "<tag:example.com,2000:app/foo>");
    std::string emitted3 = emitrs_yaml<std::string>(t3);
    EXPECT_EQ(emitted3, emitted0);
}


TEST(tags, test_suite_735Y)
{
    csubstr yaml_without_seq = R"(
!!map # Block collection
foo : bar
)";
    test_check_emit_check(yaml_without_seq, [](Tree const &t){
        EXPECT_TRUE(t.rootref().is_map());
        EXPECT_TRUE(t.rootref().has_val_tag());
        EXPECT_EQ(t.rootref()["foo"].val(), csubstr("bar"));
    });

    csubstr yaml = R"(
-
  foo : bar
- #!!map
  foo : bar
- #!!map # Block collection
  foo : bar
- !!map
  foo : bar
- !!map # Block collection
  foo : bar
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 5u);
        //
        EXPECT_TRUE(t[0].is_map());
        EXPECT_TRUE(!t[0].has_val_tag());
        EXPECT_EQ(t[0]["foo"].val(), csubstr("bar"));
        //
        EXPECT_TRUE(t[1].is_map());
        EXPECT_TRUE(!t[1].has_val_tag());
        EXPECT_EQ(t[1]["foo"].val(), csubstr("bar"));
        //
        EXPECT_TRUE(t[2].is_map());
        EXPECT_TRUE(!t[2].has_val_tag());
        EXPECT_EQ(t[2]["foo"].val(), csubstr("bar"));
        //
        EXPECT_TRUE(t[3].is_map());
        ASSERT_TRUE(t[3].has_val_tag());
        EXPECT_EQ(t[3].val_tag(), csubstr("!!map"));
        EXPECT_EQ(t[3]["foo"].val(), csubstr("bar"));
        //
        EXPECT_TRUE(t[4].is_map());
        ASSERT_TRUE(t[4].has_val_tag());
        EXPECT_EQ(t[4].val_tag(), csubstr("!!map"));
        EXPECT_EQ(t[4]["foo"].val(), csubstr("bar"));
    });
}


TEST(tags, parsing)
{
    Tree t = parse_in_arena(R"(
!!seq
- !!map
  !key key1: !val val1
  !<!key> key2: !<!val> val2
  !<key> key3: !<val> val3
  <!key> key4: <!val> val4  # these are NOT parsed as tags
- !<tag:yaml.org,2002:map>
  !key key1: !val val1
- !<tag:yaml.org,2002:seq>
  - !val val
  - !str val
  - <!str> val # these are NOT parsed as tags
  - !<!str> val
  - !<!!str> val
  - !<tag:yaml.org,2002:str> val
)");
    _c4dbg_tree("parsed tree", t);
    EXPECT_EQ(t.rootref().val_tag(), csubstr("!!seq"));
    EXPECT_EQ(t[0].val_tag(), csubstr("!!map"));
    EXPECT_EQ(t[0]["key1"].key_tag(), csubstr("!key"));
    EXPECT_EQ(t[0]["key1"].val_tag(), csubstr("!val"));
    EXPECT_EQ(t[0]["key1"].val_tag(), csubstr("!val"));
    EXPECT_EQ(t[0]["key2"].key_tag(), csubstr("!<!key>"));
    EXPECT_EQ(t[0]["key2"].val_tag(), csubstr("!<!val>"));
    EXPECT_EQ(t[0]["key3"].key_tag(), csubstr("!<key>"));
    EXPECT_EQ(t[0]["key3"].val_tag(), csubstr("!<val>"));
    EXPECT_EQ(t[0]["<!key> key4"].has_key_tag(), false);
    EXPECT_EQ(t[0]["<!key> key4"].has_val_tag(), false);
    EXPECT_EQ(t[0]["<!key> key4"].key(), csubstr("<!key> key4"));
    EXPECT_EQ(t[0]["<!key> key4"].val(), csubstr("<!val> val4"));
    EXPECT_EQ(t[1].val_tag(), csubstr("!<tag:yaml.org,2002:map>"));
    EXPECT_EQ(t[1]["key1"].key_tag(), csubstr("!key"));
    EXPECT_EQ(t[1]["key1"].val_tag(), csubstr("!val"));
    EXPECT_EQ(t[1]["key1"].key(), csubstr("key1"));
    EXPECT_EQ(t[1]["key1"].val(), csubstr("val1"));
    EXPECT_EQ(t[2].val_tag(), csubstr("!<tag:yaml.org,2002:seq>"));
    EXPECT_EQ(t[2][0].val_tag(), csubstr("!val"));
    EXPECT_EQ(t[2][1].val_tag(), csubstr("!str"));
    EXPECT_FALSE(t[2][2].has_val_tag()); // not tag
    EXPECT_EQ(t[2][2].val(), csubstr("<!str> val")); // not tag
    EXPECT_EQ(t[2][3].val_tag(), csubstr("!<!str>"));
    EXPECT_EQ(t[2][4].val_tag(), csubstr("!<!!str>"));
    EXPECT_EQ(t[2][5].val_tag(), csubstr("!<tag:yaml.org,2002:str>"));
    EXPECT_EQ(emitrs_yaml<std::string>(t), R"(!!seq
- !!map
  !key key1: !val val1
  !<!key> key2: !<!val> val2
  !<key> key3: !<val> val3
  <!key> key4: <!val> val4
- !<tag:yaml.org,2002:map>
  !key key1: !val val1
- !<tag:yaml.org,2002:seq>
  - !val val
  - !str val
  - <!str> val
  - !<!str> val
  - !<!!str> val
  - !<tag:yaml.org,2002:str> val
)");
    EXPECT_EQ(normalize_tag(t[0]["key1"].key_tag()), csubstr("!key"));
    EXPECT_EQ(normalize_tag(t[0]["key1"].val_tag()), csubstr("!val"));
    EXPECT_EQ(normalize_tag(t[0]["key2"].key_tag()), csubstr("<!key>"));
    EXPECT_EQ(normalize_tag(t[0]["key2"].val_tag()), csubstr("<!val>"));
    EXPECT_EQ(normalize_tag(t[0]["key3"].key_tag()), csubstr("<key>"));
    EXPECT_EQ(normalize_tag(t[0]["key3"].val_tag()), csubstr("<val>"));
    EXPECT_EQ(normalize_tag(t[1].val_tag()), csubstr("!!map"));
    EXPECT_EQ(normalize_tag(t[1]["key1"].key_tag()), csubstr("!key"));
    EXPECT_EQ(normalize_tag(t[1]["key1"].val_tag()), csubstr("!val"));
    EXPECT_EQ(normalize_tag(t[2].val_tag()), csubstr("!!seq"));
    EXPECT_EQ(normalize_tag(t[2][0].val_tag()), csubstr("!val"));
    EXPECT_EQ(normalize_tag(t[2][1].val_tag()), csubstr("!str"));
    EXPECT_EQ(normalize_tag(t[2][3].val_tag()), csubstr("<!str>"));
    EXPECT_EQ(normalize_tag(t[2][5].val_tag()), csubstr("!!str"));
}


TEST(tags, setting)
{
    Tree t;
    id_type rid = t.root_id();
    t.to_map(rid);
    t.set_val_tag(rid, "!valtag");
    EXPECT_EQ(t.val_tag(rid), "!valtag");

    // a keymap
    {
        id_type child = t.append_child(rid);
        t.to_seq(child, "key2");
        t.set_key_tag(child, "!keytag");
        t.set_val_tag(child, "!valtag2");
        EXPECT_TRUE(t.has_key(child));
        EXPECT_FALSE(t.has_val(child));
        EXPECT_EQ(t.key(child), "key2");
        EXPECT_EQ(t.key_tag(child), "!keytag");
        EXPECT_EQ(t.val_tag(child), "!valtag2");
    }

    // a keyseq
    {
        id_type child = t.append_child(rid);
        t.to_seq(child, "key2");
        t.set_key_tag(child, "!keytag");
        t.set_val_tag(child, "!valtag2");
        EXPECT_TRUE(t.has_key(child));
        EXPECT_FALSE(t.has_val(child));
        EXPECT_EQ(t.key(child), "key2");
        EXPECT_EQ(t.key_tag(child), "!keytag");
        EXPECT_EQ(t.val_tag(child), "!valtag2");
    }

    // a keyval
    {
        id_type child = t.append_child(rid);
        t.to_keyval(child, "key", "val");
        t.set_key_tag(child, "!keytag");
        t.set_val_tag(child, "!valtag");
        EXPECT_TRUE(t.has_key(child));
        EXPECT_TRUE(t.has_val(child));
        EXPECT_EQ(t.key(child), "key");
        EXPECT_EQ(t.val(child), "val");
        EXPECT_EQ(t.key_tag(child), "!keytag");
        EXPECT_EQ(t.val_tag(child), "!valtag");
    }

    // a val
    {
        id_type seqid = t[1].id();
        ASSERT_TRUE(t.is_seq(seqid));
        id_type child = t.append_child(seqid);
        t.to_val(child, "val");
        t.set_val_tag(child, "!valtag");
        EXPECT_FALSE(t.has_key(child));
        EXPECT_TRUE(t.has_val(child));
        EXPECT_EQ(t.val(child), "val");
        EXPECT_EQ(t.val_tag(child), "!valtag");
    }
}

TEST(tags, errors)
{
    Tree t = parse_in_arena("{key: val, keymap: {}, keyseq: [val]}");
    id_type keyval = t["keyval"].id();
    id_type keymap = t["keymap"].id();
    id_type keyseq = t["keyseq"].id();
    id_type val = t["keyseq"][0].id();
    id_type empty_keyval = t.append_child(keymap);
    id_type empty_val = t.append_child(keyseq);

    ASSERT_NE(keyval, (id_type)npos);
    ASSERT_NE(keymap, (id_type)npos);
    ASSERT_NE(keyseq, (id_type)npos);
    ASSERT_NE(val, (id_type)npos);

    // cannot get key tag in a node that does not have a key tag
    EXPECT_FALSE(t.has_key_tag(empty_keyval));
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.key_tag(empty_keyval), "");
    });
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.key_tag(keyval), "");
    });
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.key_tag(keymap), "");
    });
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.key_tag(keyseq), "");
    });
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.key_tag(val), "");
    });
    // cannot get val tag in a node that does not have a val tag
    EXPECT_FALSE(t.has_val_tag(empty_val));
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.val_tag(empty_val), "");
    });
    EXPECT_FALSE(t.has_val_tag(empty_keyval));
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.val_tag(empty_keyval), "");
    });
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.val_tag(keyval), "");
    });
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.val_tag(keymap), "");
    });
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.val_tag(keyseq), "");
    });
    ExpectError::check_assertion(&t, [&](){
        EXPECT_EQ(t.val_tag(val), "");
    });
    // cannot set key tag in a node that does not have a key
    EXPECT_FALSE(t.has_key(empty_keyval));
    ExpectError::check_assertion(&t, [&](){
        t.set_key_tag(empty_keyval, "!keytag");
    });
    EXPECT_FALSE(t.has_key_tag(val)); // must stay the same
    ExpectError::check_assertion(&t, [&](){
        t.set_key_tag(val, "!valtag");
    });
    EXPECT_FALSE(t.has_key_tag(val)); // must stay the same
    // cannot set val tag in a node that does not have a val
    EXPECT_FALSE(t.has_val(empty_val));
    ExpectError::check_assertion(&t, [&](){
        t.set_val_tag(empty_val, "!valtag");
    });
    EXPECT_FALSE(t.has_val_tag(empty_val)); // must stay the same
    EXPECT_FALSE(t.has_val(empty_keyval));
    ExpectError::check_assertion(&t, [&](){
        t.set_val_tag(empty_keyval, "!valtag");
    });
    EXPECT_FALSE(t.has_val_tag(empty_keyval)); // must stay the same
}


TEST(tags, setting_user_tags_do_not_require_leading_mark)
{
    Tree t = parse_in_arena(R"(
key: val
keymap: {}
keyseq: [val]
)");
    id_type keyval = t["keyval"].id();
    id_type keymap = t["keymap"].id();
    id_type keyseq = t["keyseq"].id();
    id_type val = t["keyseq"][0].id();
    ASSERT_NE(keyval, (id_type)npos);
    ASSERT_NE(keymap, (id_type)npos);
    ASSERT_NE(keyseq, (id_type)npos);
    ASSERT_NE(val, (id_type)npos);

    // without leading mark
    t.set_key_tag(keyseq, "keytag");
    t.set_val_tag(keyseq, "valtag");
    t.set_val_tag(val,    "valtag2");
    EXPECT_EQ(t.key_tag(keyseq), "keytag");
    EXPECT_EQ(t.val_tag(keyseq), "valtag");
    EXPECT_EQ(t.val_tag(val),    "valtag2");

    EXPECT_EQ(emitrs_yaml<std::string>(t), R"(key: val
keymap: {}
!keytag keyseq: !valtag [!valtag2 val]
)");

    // with leading mark
    t.set_key_tag(keyseq, "!keytag");
    t.set_val_tag(keyseq, "!valtag");
    t.set_val_tag(val,    "!valtag2");
    EXPECT_EQ(t.key_tag(keyseq), "!keytag");
    EXPECT_EQ(t.val_tag(keyseq), "!valtag");
    EXPECT_EQ(t.val_tag(val),    "!valtag2");

    EXPECT_EQ(emitrs_yaml<std::string>(t), R"(key: val
keymap: {}
!keytag keyseq: !valtag [!valtag2 val]
)");
}


TEST(tags, valid_chars)
{
    Tree t = parse_in_arena(R"(
- !<foo bar> val
- !<foo> bar> val
- !<foo> <bar> val
)");
    EXPECT_EQ(t[0].val_tag(), "!<foo bar>");
    EXPECT_EQ(t[0].val(), "val");
    EXPECT_EQ(t[1].val_tag(), "!<foo>");
    EXPECT_EQ(t[1].val(), "bar> val");
    EXPECT_EQ(t[2].val_tag(), "!<foo>");
    EXPECT_EQ(t[2].val(), "<bar> val");
}


TEST(tags, EHF6)
{
    {
        Tree t = parse_in_arena(R"(!!map {
  k: !!seq [ a, !!str b],
  j: !!seq
     [ a, !!str b]
})");
        ASSERT_TRUE(t.rootref().has_val_tag());
        EXPECT_EQ(t.rootref().val_tag(), "!!map");
        ASSERT_TRUE(t["k"].has_val_tag());
        ASSERT_TRUE(t["j"].has_val_tag());
        EXPECT_EQ(t["k"].val_tag(), "!!seq");
        EXPECT_EQ(t["j"].val_tag(), "!!seq");
    }
    {
        Tree t = parse_in_arena(R"(!!seq [
  !!map { !!str k: v},
  !!map { !!str ? k: v}
])");
        ASSERT_TRUE(t.rootref().has_val_tag());
        EXPECT_EQ(t.rootref().val_tag(), "!!seq");
        ASSERT_TRUE(t[0].has_val_tag());
        ASSERT_TRUE(t[1].has_val_tag());
        EXPECT_EQ(t[0].val_tag(), "!!map");
        EXPECT_EQ(t[1].val_tag(), "!!map");
        ASSERT_TRUE(t[0]["k"].has_key_tag());
        ASSERT_TRUE(t[1]["k"].has_key_tag());
        EXPECT_EQ(t[0]["k"].key_tag(), "!!str");
        EXPECT_EQ(t[1]["k"].key_tag(), "!!str");
    }
}

TEST(tags, fuzzcrash0)
{
    Tree tree;
    ExpectError::check_error(&tree, [&]{
        parse_in_arena("%TAG !! " "\n"
                       "})"       "\n"
                       ""         "\n"
                       "!!<"      "\n"
                       ,
                       &tree);
    });
}

TEST(tags, ys0)
{
    csubstr yaml = "!yamlscript/v0/bare\n--- !code\n42\n";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_TRUE(t.rootref().is_stream());
        EXPECT_TRUE(t.docref(0).is_val());
        EXPECT_TRUE(t.docref(0).has_val_tag());
        EXPECT_EQ(t.docref(0).val_tag(), "!yamlscript/v0/bare");
        EXPECT_EQ(t.docref(0).val(), csubstr{});
        EXPECT_TRUE(t.docref(1).is_val());
        EXPECT_TRUE(t.docref(1).has_val_tag());
        EXPECT_EQ(t.docref(1).val_tag(), "!code");
        EXPECT_EQ(t.docref(1).val(), "42");
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CASE_GROUP(TAG_PROPERTY)
{

ADD_CASE_TO_GROUP("user tag, empty, test suite 52DL",
R"(! a)",
N(VP, TS("!", "a"))
);

ADD_CASE_TO_GROUP("tag property in implicit map, std tags",
R"(ivar: !!int 0
svar: !!str 0
fvar: !!float 0.1
!!int 2: !!float 3
!!float 3: !!int 3.4
!!str key: !!int val
myObject: !myClass { name: Joe, age: 15 }
picture: !!binary >-
  R0lGODdhDQAIAIAAAAAAANn
  Z2SwAAAAADQAIAAACF4SDGQ
  ar3xxbJ9p0qa7R0YxwzaFME
  1IAADs=
)",
N(MB, L{
      N(KP|VP, "ivar", TS("!!int", "0")),
      N(KP|VP, "svar", TS("!!str", "0")),
      N(KP|VP, "fvar", TS("!!float", "0.1")),
      N(KP|VP, TS("!!int", "2"), TS("!!float", "3")),
      N(KP|VP, TS("!!float", "3"), TS("!!int", "3.4")),
      N(KP|VP, TS("!!str", "key"), TS("!!int", "val")),
      N(KP|MFS, "myObject", TL("!myClass", L{N(KP|VP, "name", "Joe"), N(KP|VP, "age", "15")})),
      N(KP|VF, "picture", TS("!!binary", R"(R0lGODdhDQAIAIAAAAAAANn Z2SwAAAAADQAIAAACF4SDGQ ar3xxbJ9p0qa7R0YxwzaFME 1IAADs=)")),
    })
);

ADD_CASE_TO_GROUP("tag property in implicit map, usr tags",
R"(ivar: !int 0
svar: !str 0
fvar: !float 0.1
!int 2: !float 3
!float 3: !int 3.4
!str key: !int val
myObject: !myClass { name: Joe, age: 15 }
picture: !binary >-
  R0lGODdhDQAIAIAAAAAAANn
  Z2SwAAAAADQAIAAACF4SDGQ
  ar3xxbJ9p0qa7R0YxwzaFME
  1IAADs=
)",
N(MB, L{
      N(KP|VP, "ivar", TS("!int", "0")),
      N(KP|VP, "svar", TS("!str", "0")),
      N(KP|VP, "fvar", TS("!float", "0.1")),
      N(KP|VP, TS("!int", "2"), TS("!float", "3")),
      N(KP|VP, TS("!float", "3"), TS("!int", "3.4")),
      N(KP|VP, TS("!str", "key"), TS("!int", "val")),
      N(KP|MFS, "myObject", TL("!myClass", L{N(KP|VP, "name", "Joe"), N(KP|VP, "age", "15")})),
      N(KP|VF, "picture", TS("!binary", R"(R0lGODdhDQAIAIAAAAAAANn Z2SwAAAAADQAIAAACF4SDGQ ar3xxbJ9p0qa7R0YxwzaFME 1IAADs=)")),
    })
);

ADD_CASE_TO_GROUP("tag property in explicit map, std tags",
R"({
ivar: !!int 0,
svar: !!str 0,
!!str key: !!int val
})",
N(MFS, L{
      N(KP|VP, "ivar", TS("!!int", "0")),
      N(KP|VP, "svar", TS("!!str", "0")),
      N(KP|VP, TS("!!str", "key"), TS("!!int", "val"))
    })
);

ADD_CASE_TO_GROUP("tag property in explicit map, usr tags",
R"({
ivar: !int 0,
svar: !str 0,
!str key: !int val
}
)",
N(MFS, L{
      N(KP|VP, "ivar", TS("!int", "0")),
      N(KP|VP, "svar", TS("!str", "0")),
      N(KP|VP, TS("!str", "key"), TS("!int", "val"))
    })
);

ADD_CASE_TO_GROUP("tag property in implicit seq, std tags",
R"(- !!int 0
- !!str 0
)",
N(SB, L{
      N(VP, TS("!!int", "0")),
      N(VP, TS("!!str", "0")),
    })
);

ADD_CASE_TO_GROUP("tag property in implicit seq, usr tags",
R"(- !int 0
- !str 0
)",
    N(SB, L{
      N(VP, TS("!int", "0")),
      N(VP, TS("!str", "0")),
    })
);

ADD_CASE_TO_GROUP("tag property in explicit seq, std tags",
R"([
!!int 0,
!!str 0
]
)",
N(SFS, L{
      N(VP, TS("!!int", "0")),
      N(VP, TS("!!str", "0")),
    })
);

ADD_CASE_TO_GROUP("tag property in explicit seq, usr tags",
R"([
!int 0,
!str 0
]
)",
    N(SFS, L{
      N(VP, TS("!int", "0")),
      N(VP, TS("!str", "0")),
    })
);

ADD_CASE_TO_GROUP("tagged explicit sequence in map, std tags",
R"(some_seq: !!its_type [
!!int 0,
!!str 0
]
)",
N(MB, L{N(KP|SFS, "some_seq", TL("!!its_type", L{
              N(VP, TS("!!int", "0")),
              N(VP, TS("!!str", "0")),
                  }))
          })
);

ADD_CASE_TO_GROUP("tagged explicit sequence in map, usr tags",
R"(some_seq: !its_type [
!int 0,
!str 0
]
)",
N(MB, L{N(KP|SFS, "some_seq", TL("!its_type", L{
              N(VP, TS("!int", "0")),
              N(VP, TS("!str", "0")),
                  }))
          })
);

ADD_CASE_TO_GROUP("tagged doc 1",
R"(
--- !!map
a: 0
b: 1
)",
N(STREAM, L{
    N(DOC|MB, TL("!!map", L{N(KP|VP, "a", "0"), N(KP|VP, "b", "1")})),
})
);

ADD_CASE_TO_GROUP("tagged doc 2",
R"(
--- !map
? a
: b
)",
N(STREAM, L{
    N(DOC|MB, TL("!map", L{N(KP|VP, "a", "b")})),
})
);

ADD_CASE_TO_GROUP("tagged doc 3",
R"(
--- !!seq
- a
- b
)",
N(STREAM, L{
    N(DOC|SB, TL("!!seq", L{N(VP, "a"), N(VP, "b")})),
})
);

ADD_CASE_TO_GROUP("tagged doc 4",
R"(
--- !!str
a
 b
...
)",
N(STREAM, L{
    N(DOC|VP, TS("!!str", "a b")),
})
);

ADD_CASE_TO_GROUP("tagged doc 5",
R"(
--- !!str
a: b
)",
N(STREAM, L{
    N(DOC|MB, TL("!!str", L{N(KP|VP, "a", "b")})),
})
);

ADD_CASE_TO_GROUP("tagged doc 6",
R"(
---
!!str a: b
)",
N(STREAM, L{
    N(DOC|MB, L{N(KP|VP, TS("!!str", "a"), "b")}),
})
);

ADD_CASE_TO_GROUP("tagged doc 7",
R"(
---
!!str a
 b
)",
N(STREAM, L{
    N(DOC|VP, TS("!!str", "a b")),
})
);

ADD_CASE_TO_GROUP("tagged doc 8",
R"(
---
!!set
? a
? b
)",
N(STREAM, L{
    N(DOC|MB, TL("!!set", L{N(KP|VN, "a", /*"~"*/{}), N(KP|VN, "b", /*"~"*/{})})),
})
);

ADD_CASE_TO_GROUP("tagged doc 9",
R"(
--- !!set
? a
? b
)",
N(STREAM, L{
    N(DOC|MB, TL("!!set", L{N(KP|VN, "a", /*"~"*/{}), N(KP|VN, "b", /*"~"*/{})})),
})
);

ADD_CASE_TO_GROUP("ambiguous tag in map, std tag",
R"(!!map
!!str a0: !!xxx b0
!!str fooz: !!map
  k1: !!float 1.0
  k3: !!float 2.0
!!str foo: !!map
  !!int 1: !!float 20.0
  !!int 3: !!float 40.0
bar: !!map
  10: !!str 2
  30: !!str 4
!!str baz:
  !!int 10: !!float 20
  !!int 30: !!float 40
)",
N(MB, TL("!!map", L{
  N(KP|VP, TS("!!str", "a0"), TS("!!xxx", "b0")),
  N(KP|MB, TS("!!str", "fooz"), TL("!!map", L{N(KP|VP, "k1", TS("!!float", "1.0")), N(KP|VP, "k3", TS("!!float", "2.0"))})),
  N(KP|MB, TS("!!str", "foo"), TL("!!map", L{N(KP|VP, TS("!!int", "1"), TS("!!float", "20.0")), N(KP|VP, TS("!!int", "3"), TS("!!float", "40.0"))})),
  N(KP|MB, "bar", TL("!!map", L{N(KP|VP, "10", TS("!!str", "2")), N(KP|VP, "30", TS("!!str", "4"))})),
  N(KP|MB, TS("!!str", "baz"), L{N(KP|VP, TS("!!int", "10"), TS("!!float", "20")), N(KP|VP, TS("!!int", "30"), TS("!!float", "40"))}),
}))
);

ADD_CASE_TO_GROUP("ambiguous tag in map, usr tag",
R"(!map
!str a0: !xxx b0
!str fooz: !map
  k1: !float 1.0
  k3: !float 2.0
!str foo: !map
  !int 1: !float 20.0
  !int 3: !float 40.0
bar: !map
  10: !str 2
  30: !str 4
!str baz:
  !int 10: !float 20
  !int 30: !float 40
)",
N(MB, TL("!map", L{
  N(KP|VP, TS("!str", "a0"), TS("!xxx", "b0")),
  N(KP|MB, TS("!str", "fooz"), TL("!map", L{N(KP|VP, "k1", TS("!float", "1.0")), N(KP|VP, "k3", TS("!float", "2.0"))})),
  N(KP|MB, TS("!str", "foo"), TL("!map", L{N(KP|VP, TS("!int", "1"), TS("!float", "20.0")), N(KP|VP, TS("!int", "3"), TS("!float", "40.0"))})),
  N(KP|MB, "bar", TL("!map", L{N(KP|VP, "10", TS("!str", "2")), N(KP|VP, "30", TS("!str", "4"))})),
  N(KP|MB, TS("!str", "baz"), L{N(KP|VP, TS("!int", "10"), TS("!float", "20")), N(KP|VP, TS("!int", "30"), TS("!float", "40"))}),
}))
);


ADD_CASE_TO_GROUP("ambiguous tag in seq, std tag",
R"(!!seq
- !!str k1: v1
  !!str k2: v2
  !!str k3: v3
- !!map
  !!str k4: v4
  !!str k5: v5
  !!str k6: v6
- !!map
  k7: v7
  k8: v8
  k9: v9
- - !!str v10
  - !!str v20
  - !!str v30
- !!seq
  - !!str v40
  - !!str v50
  - !!str v60
- !!seq
  - v70
  - v80
  - v90
)",
N(SB, TL("!!seq", L{
  N(MB, L{N(KP|VP, TS("!!str", "k1"), "v1"), N(KP|VP, TS("!!str", "k2"), "v2"), N(KP|VP, TS("!!str", "k3"), "v3"), }),
  N(MB, TL("!!map", L{N(KP|VP, TS("!!str", "k4"), "v4"), N(KP|VP, TS("!!str", "k5"), "v5"), N(KP|VP, TS("!!str", "k6"), "v6"), })),
  N(MB, TL("!!map", L{N(KP|VP, "k7", "v7"), N(KP|VP, "k8", "v8"), N(KP|VP, "k9", "v9"), })),
  N(SB, L{N(VP, TS("!!str", "v10")), N(VP, TS("!!str", "v20")), N(VP, TS("!!str", "v30"))}),
  N(SB, TL("!!seq", L{N(VP, TS("!!str", "v40")), N(VP, TS("!!str", "v50")), N(VP, TS("!!str", "v60"))})),
  N(SB, TL("!!seq", L{N(VP, "v70"), N(VP, "v80"), N(VP, "v90")})),
})));

ADD_CASE_TO_GROUP("ambiguous tag in seq, usr tag",
R"(!seq
- !str k1: v1
  !str k2: v2
  !str k3: v3
- !map
  !str k4: v4
  !str k5: v5
  !str k6: v6
- !map
  k7: v7
  k8: v8
  k9: v9
- - !str v10
  - !str v20
  - !str v30
- !seq
  - !str v40
  - !str v50
  - !str v60
- !seq
  - v70
  - v80
  - v90
)",
N(SB, TL("!seq", L{
  N(MB, L{N(KP|VP, TS("!str", "k1"), "v1"), N(KP|VP, TS("!str", "k2"), "v2"), N(KP|VP, TS("!str", "k3"), "v3"), }),
  N(MB, TL("!map", L{N(KP|VP, TS("!str", "k4"), "v4"), N(KP|VP, TS("!str", "k5"), "v5"), N(KP|VP, TS("!str", "k6"), "v6"), })),
  N(MB, TL("!map", L{N(KP|VP, "k7", "v7"), N(KP|VP, "k8", "v8"), N(KP|VP, "k9", "v9"), })),
  N(SB, L{N(VP, TS("!str", "v10")), N(VP, TS("!str", "v20")), N(VP, TS("!str", "v30"))}),
  N(SB, TL("!seq", L{N(VP, TS("!str", "v40")), N(VP, TS("!str", "v50")), N(VP, TS("!str", "v60"))})),
  N(SB, TL("!seq", L{N(VP, "v70"), N(VP, "v80"), N(VP, "v90")})),
})));

}

} // namespace yml
} // namespace c4
