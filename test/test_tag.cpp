#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"
#include "./test_lib/test_case.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

TEST(TagCache, empty)
{
    TagCache tc;
    EXPECT_FALSE(tc.find({}, 0));
}

TEST(TagCache, find_works)
{
    {
        TagCache tc;
        tc.add("!!str", "asdasd", 0, 0);
        TagCache::LookupResult ret = tc.find("!!str", 0);
        ASSERT_TRUE(ret);
    }
    csubstr empty = {};
    empty.len = 10;
    {
        TagCache tc;
        tc.add("!!str", empty, 0, 0);
        TagCache::LookupResult ret = tc.find("!!str", 0);
        ASSERT_TRUE(ret);
    }
    {
        TagCache tc;
        tc.add("!!str", empty, 1, 0);
        tc.add("!!str", empty, 0, 0);
        TagCache::LookupResult ret0 = tc.find("!!str", 0);
        TagCache::LookupResult ret1 = tc.find("!!str", 1);
        ASSERT_TRUE(ret0);
        ASSERT_TRUE(ret1);
        EXPECT_EQ(ret0.pos, 0);
        EXPECT_EQ(ret1.pos, 1);
    }
    {
        TagCache tc;
        tc.add("!!str", empty, 0, 0);
        tc.add("!!str", empty, 1, 1);
        TagCache::LookupResult ret0 = tc.find("!!str", 0);
        TagCache::LookupResult ret1 = tc.find("!!str", 1);
        ASSERT_TRUE(ret0);
        ASSERT_TRUE(ret1);
        EXPECT_EQ(ret0.pos, 0);
        EXPECT_EQ(ret1.pos, 1);
        TagCache::LookupResult ret2 = tc.find("!!str", 2);
        ASSERT_FALSE(ret2);
        EXPECT_EQ(ret2.pos, 2);
    }
}

TEST(TagCache, basic)
{
    const TagCache::Entry universe_[] = {
        #define _(t)                                                     \
            TagCache::Entry{csubstr(t), csubstr(t "-doc0"), /*docid*/0}, \
            TagCache::Entry{csubstr(t), csubstr(t "-doc1"), /*docid*/1}
        _("00"), _("01"), _("02"), _("03"), _("04"), _("05"), _("06"), _("07"), _("08"), _("09"),
        #undef _
    };
    cspan<TagCache::Entry> universe = universe_;
    const size_t sz = universe.size();
    TagCache tc;
    auto showcache = [&](csubstr msg){
        std::cout << msg;
        for(size_t j = 0; j < tc.m_entries.size(); ++j)
            std::cout << "entry[" << j << "]={'"<< tc.m_entries[j].tag << "','" << tc.m_entries[j].resolved << "',docid=" << tc.m_entries[j].doc_id << "}\n";
    };
    auto check_sorted = [&]{
        for(size_t i = 1; i < tc.m_entries.size(); ++i)
        {
            RYML_TRACE_FMT("check_sorted: i={}", i);
            TagCache::Entry const& prev = tc.m_entries[i-1];
            TagCache::Entry const& curr = tc.m_entries[i];
            bool ok = prev.tag < curr.tag || (prev.tag == curr.tag && prev.doc_id < curr.doc_id);
            if(!ok)
            {
                FAIL() << "sort error at position=" << i;
                showcache("");
                break;
            }
        }
    };
    bool exceeded_sso_size = false;
    auto check_add = [&](id_type i, id_type expected_pos, id_type linear_threshold){
        RYML_TRACE_FMT("lazy_add: i={}", i);
        ASSERT_LT(i, universe.size());
        TagCache::Entry const& u = universe[i];
        TagCache::LookupResult ret = tc.find(u.tag, u.doc_id, linear_threshold);
        RYML_TRACE_FMT("ret: pos={} resolved='{}'", ret.pos, ret.resolved);
        EXPECT_FALSE(ret);
        if(expected_pos != NONE)
        {
            ASSERT_EQ(ret.pos, expected_pos);
        }
        tc.add(u.tag, u.resolved, u.doc_id, ret.pos);
        check_sorted();
        TagCache::LookupResult ret2 = tc.find(u.tag, u.doc_id, linear_threshold);
        EXPECT_TRUE(ret2);
        EXPECT_EQ(ret2.pos, ret.pos);
        EXPECT_EQ(ret2.resolved, u.resolved);
        if(tc.m_entries.size())
            exceeded_sso_size = true;
    };
    for(id_type threshold_ = 0; threshold_ < sz; ++threshold_)
    {
        id_type threshold = sz - 1 - threshold_; // start with linear first
        RYML_TRACE_FMT("threshold={}", threshold);
        {
            SCOPED_TRACE("in order");
            tc.clear();
            for(size_t i = 0 ; i < sz; ++i)
            {
                RYML_TRACE_FMT("i={} search={}", i, i < threshold ? "linear" : "binary");
                check_add(i, i, threshold);
            }
        }
        {
            SCOPED_TRACE("in reverse order");
            tc.clear();
            for(size_t i = 0 ; i < sz; ++i)
            {
                RYML_TRACE_FMT("i={} search={}", i, i < threshold ? "linear" : "binary");
                check_add(sz - 1 - i, 0, threshold);
            }
        }
        {
            SCOPED_TRACE("extremities first");
            tc.clear();
            for(size_t i = 0 ; i < sz; ++i)
            {
                RYML_TRACE_FMT("i={} search={}", i, i < threshold ? "linear" : "binary");
                id_type entry = (i & 1u) ? sz - i : i;
                check_add(entry, NONE, threshold);
            }
        }
    }
    EXPECT_TRUE(exceeded_sso_size);
}


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
    char output[100];
    char small[2];
    substr empty = {};
    #define test_normalize_long(input_, expected_)                      \
    {                                                                   \
        csubstr input = input_;                                         \
        csubstr expected = expected_;                                   \
        EXPECT_EQ(normalize_tag_long(input), expected);                 \
        EXPECT_EQ(normalize_tag_long(input, output), expected);         \
        if(to_tag(input) == TAG_NONE                                    \
           && input != expected                                         \
           && !input.begins_with("!<"))                                 \
        {                                                               \
            EXPECT_EQ(normalize_tag_long(input, small).str, nullptr);   \
            EXPECT_EQ(normalize_tag_long(input, small).len, expected.len); \
            EXPECT_EQ(normalize_tag_long(input, empty).str, nullptr);   \
            EXPECT_EQ(normalize_tag_long(input, empty).len, expected.len); \
        }                                                               \
    }

    test_normalize_long("<tag:yaml.org,2002:"          , "<tag:yaml.org,2002:");
    test_normalize_long("<tag:yaml.org,2002:."         , "<tag:yaml.org,2002:.");

    test_normalize_long("<tag:yaml.org,2002:map>"       , "<tag:yaml.org,2002:map>");
    test_normalize_long("<tag:yaml.org,2002:omap>"      , "<tag:yaml.org,2002:omap>");
    test_normalize_long("<tag:yaml.org,2002:pairs>"     , "<tag:yaml.org,2002:pairs>");
    test_normalize_long("<tag:yaml.org,2002:set>"       , "<tag:yaml.org,2002:set>");
    test_normalize_long("<tag:yaml.org,2002:seq>"       , "<tag:yaml.org,2002:seq>");
    test_normalize_long("<tag:yaml.org,2002:binary>"    , "<tag:yaml.org,2002:binary>");
    test_normalize_long("<tag:yaml.org,2002:bool>"      , "<tag:yaml.org,2002:bool>");
    test_normalize_long("<tag:yaml.org,2002:float>"     , "<tag:yaml.org,2002:float>");
    test_normalize_long("<tag:yaml.org,2002:int>"       , "<tag:yaml.org,2002:int>");
    test_normalize_long("<tag:yaml.org,2002:merge>"     , "<tag:yaml.org,2002:merge>");
    test_normalize_long("<tag:yaml.org,2002:null>"      , "<tag:yaml.org,2002:null>");
    test_normalize_long("<tag:yaml.org,2002:str>"       , "<tag:yaml.org,2002:str>");
    test_normalize_long("<tag:yaml.org,2002:timestamp>" , "<tag:yaml.org,2002:timestamp>");
    test_normalize_long("<tag:yaml.org,2002:value>"     , "<tag:yaml.org,2002:value>");
    test_normalize_long("<tag:yaml.org,2002:yaml>"      , "<tag:yaml.org,2002:yaml>");

    test_normalize_long("!<tag:yaml.org,2002:map>"      , "<tag:yaml.org,2002:map>");
    test_normalize_long("!<tag:yaml.org,2002:omap>"     , "<tag:yaml.org,2002:omap>");
    test_normalize_long("!<tag:yaml.org,2002:pairs>"    , "<tag:yaml.org,2002:pairs>");
    test_normalize_long("!<tag:yaml.org,2002:set>"      , "<tag:yaml.org,2002:set>");
    test_normalize_long("!<tag:yaml.org,2002:seq>"      , "<tag:yaml.org,2002:seq>");
    test_normalize_long("!<tag:yaml.org,2002:binary>"   , "<tag:yaml.org,2002:binary>");
    test_normalize_long("!<tag:yaml.org,2002:bool>"     , "<tag:yaml.org,2002:bool>");
    test_normalize_long("!<tag:yaml.org,2002:float>"    , "<tag:yaml.org,2002:float>");
    test_normalize_long("!<tag:yaml.org,2002:int>"      , "<tag:yaml.org,2002:int>");
    test_normalize_long("!<tag:yaml.org,2002:merge>"    , "<tag:yaml.org,2002:merge>");
    test_normalize_long("!<tag:yaml.org,2002:null>"     , "<tag:yaml.org,2002:null>");
    test_normalize_long("!<tag:yaml.org,2002:str>"      , "<tag:yaml.org,2002:str>");
    test_normalize_long("!<tag:yaml.org,2002:timestamp>", "<tag:yaml.org,2002:timestamp>");
    test_normalize_long("!<tag:yaml.org,2002:value>"    , "<tag:yaml.org,2002:value>");
    test_normalize_long("!<tag:yaml.org,2002:yaml>"     , "<tag:yaml.org,2002:yaml>");

    test_normalize_long("!!map"      , "<tag:yaml.org,2002:map>");
    test_normalize_long("!!omap"     , "<tag:yaml.org,2002:omap>");
    test_normalize_long("!!pairs"    , "<tag:yaml.org,2002:pairs>");
    test_normalize_long("!!set"      , "<tag:yaml.org,2002:set>");
    test_normalize_long("!!seq"      , "<tag:yaml.org,2002:seq>");
    test_normalize_long("!!binary"   , "<tag:yaml.org,2002:binary>");
    test_normalize_long("!!bool"     , "<tag:yaml.org,2002:bool>");
    test_normalize_long("!!float"    , "<tag:yaml.org,2002:float>");
    test_normalize_long("!!int"      , "<tag:yaml.org,2002:int>");
    test_normalize_long("!!merge"    , "<tag:yaml.org,2002:merge>");
    test_normalize_long("!!null"     , "<tag:yaml.org,2002:null>");
    test_normalize_long("!!str"      , "<tag:yaml.org,2002:str>");
    test_normalize_long("!!timestamp", "<tag:yaml.org,2002:timestamp>");
    test_normalize_long("!!value"    , "<tag:yaml.org,2002:value>");
    test_normalize_long("!!yaml"     , "<tag:yaml.org,2002:yaml>");

    EXPECT_EQ(normalize_tag_long("!!foo"), "!!foo");
    EXPECT_EQ(normalize_tag_long("!!light"), "!!light");

    test_normalize_long("!m!foo"      , "!m!foo");
    test_normalize_long("!m!light"    , "!m!light");

    test_normalize_long("!my-light", "!my-light");
    test_normalize_long("!foo", "!foo");
    test_normalize_long("<!foo>", "<!foo>");
    test_normalize_long("<foo>", "<foo>");
    test_normalize_long("<!>", "<!>");

    test_normalize_long("!<!foo>", "<!foo>");
    test_normalize_long("!<foo>", "<foo>");
    test_normalize_long("!<!foo>", "<!foo>");
    test_normalize_long("!<foo>", "<foo>");
    test_normalize_long("!<!>", "<!>");

    test_normalize_long("!m!foo", "!m!foo");
    test_normalize_long("!m!light", "!m!light");

    #undef test_normalize_long

    char buf[100];
    EXPECT_EQ(normalize_tag_long("!!foo", buf), "<tag:yaml.org,2002:foo>");
    EXPECT_EQ(normalize_tag_long("!!foo", substr{}).str, nullptr);
    EXPECT_EQ(normalize_tag_long("!!foo", substr{}).len, strlen("<tag:yaml.org,2002:foo>"));
    EXPECT_EQ(normalize_tag_long("!!foo", substr(buf).first(2)).str, nullptr);
    EXPECT_EQ(normalize_tag_long("!!foo", substr(buf).first(2)).len, strlen("<tag:yaml.org,2002:foo>"));
    EXPECT_EQ(normalize_tag_long("!!f", buf), "<tag:yaml.org,2002:f>");
    EXPECT_EQ(normalize_tag_long("!!f", substr{}).str, nullptr);
    EXPECT_EQ(normalize_tag_long("!!f", substr{}).len, strlen("<tag:yaml.org,2002:f>"));
    EXPECT_EQ(normalize_tag_long("!!f", substr(buf).first(2)).str, nullptr);
    EXPECT_EQ(normalize_tag_long("!!f", substr(buf).first(2)).len, strlen("<tag:yaml.org,2002:f>"));
    EXPECT_EQ(normalize_tag_long("!!", buf), "<tag:yaml.org,2002:>");
    EXPECT_EQ(normalize_tag_long("!!", substr{}).str, nullptr);
    EXPECT_EQ(normalize_tag_long("!!", substr{}).len, strlen("<tag:yaml.org,2002:>"));
    EXPECT_EQ(normalize_tag_long("!!", substr(buf).first(2)).str, nullptr);
    EXPECT_EQ(normalize_tag_long("!!", substr(buf).first(2)).len, strlen("<tag:yaml.org,2002:>"));
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

TEST(tag_functions, transform_tag)
{
    auto transform = [](substr buf, size_t *len,
                        csubstr handle, csubstr prefix, csubstr tag,
                        bool with_brackets, Location ymlloc) {
        id_type doc_id = 0;
        TagDirectives tds = {};
        C4_ASSERT(!!handle.len == !!prefix.len);
        if(handle.len && prefix.len)
            tds.m_directives[0] = TagDirective{handle, prefix, doc_id};
        return tds.resolve(buf, len, tag, doc_id, ymlloc, get_callbacks(), with_brackets);
    };
    auto checkerr___ = [&](substr buf,
                         csubstr handle, csubstr prefix, csubstr tag,
                         bool with_brackets, Location ymlloc){
        ExpectedErrorType errtype = ymlloc ? ExpectedErrorType::err_parse : ExpectedErrorType::err_basic;
        ExpectError::check_error(errtype, [&]{
            size_t len = 0;
            transform(buf, &len, handle, prefix, tag, with_brackets, ymlloc);
            FAIL() << "missing error";  // if we reach this, it's a failure
        });
    };
    const Location noloc = {};
    ASSERT_FALSE(noloc);
    const Location ymlloc("test.yml", 4567, 123, 4);
    ASSERT_EQ(ymlloc.name, "test.yml");
    ASSERT_EQ(ymlloc.offset, 4567);
    ASSERT_EQ(ymlloc.line, 123);
    ASSERT_EQ(ymlloc.col, 4);
    ASSERT_TRUE(ymlloc);
    const bool with_brackets = true;
    const bool without_brackets = false;
    auto checkerr__ = [&](substr buf, csubstr handle, csubstr prefix, csubstr tag){
        {
            SCOPED_TRACE("with brackets, noloc");
            checkerr___(buf, handle, prefix, tag, with_brackets, noloc);
        }
        {
            SCOPED_TRACE("with brackets, ymlloc");
            checkerr___(buf, handle, prefix, tag, with_brackets, ymlloc);
        }
        {
            SCOPED_TRACE("without brackets, noloc");
            checkerr___(buf, handle, prefix, tag, without_brackets, noloc);
        }
        {
            SCOPED_TRACE("without brackets, ymlloc");
            checkerr___(buf, handle, prefix, tag, without_brackets, ymlloc);
        }
    };
    auto checkerr_ = [&](csubstr handle, csubstr prefix, csubstr tag){
        RYML_TRACE_FMT("handle={}", handle);
        RYML_TRACE_FMT("prefix={}", prefix);
        RYML_TRACE_FMT("tag={}", tag);
        {
            SCOPED_TRACE("no buf");
            checkerr__(substr{}, handle, prefix, tag);
        }
        char buf[256];
        {
            SCOPED_TRACE("ok buf");
            checkerr__(buf, handle, prefix, tag);
        }
    };
    auto check_output = [](csubstr buf, size_t len, csubstr output, csubstr expected){
        RYML_TRACE_FMT("len={}", len);
        RYML_TRACE_FMT("buf.len={}", buf.len);
        RYML_TRACE_FMT("output={}", output.str ? output : csubstr("(no space)"));
        RYML_TRACE_FMT("expected={}", expected);
        if(len)
        {
            EXPECT_EQ(len, expected.len);
            if(len > buf.len)
            {
                EXPECT_EQ(output.str, nullptr);
            }
            else
            {
                EXPECT_TRUE(output.is_sub(buf));
            }
        }
        else
        {
            EXPECT_NE(output.str, nullptr);
        }
        EXPECT_EQ(output.len, expected.len);
        if(output.str)
        {
            EXPECT_EQ(output, expected);
        }
    };
    auto checkok__ = [&](substr buf,
                        csubstr handle, csubstr prefix, csubstr tag,
                        csubstr expected_with, csubstr expected_without){
        size_t len = 0;
        {
            SCOPED_TRACE("with brackets, noloc");
            csubstr output = transform(buf, &len, handle, prefix, tag, with_brackets, noloc);
            check_output(buf, len, output, expected_with);
        }
        {
            SCOPED_TRACE("with brackets, ymlloc");
            csubstr output = transform(buf, &len, handle, prefix, tag, with_brackets, ymlloc);
            check_output(buf, len, output, expected_with);
        }
        {
            SCOPED_TRACE("without brackets, noloc");
            csubstr output = transform(buf, &len, handle, prefix, tag, without_brackets, noloc);
            check_output(buf, len, output, expected_without);
        }
        {
            SCOPED_TRACE("without brackets, ymlloc");
            csubstr output = transform(buf, &len, handle, prefix, tag, without_brackets, ymlloc);
            check_output(buf, len, output, expected_without);
        }
    };
    auto checkok_ = [&](csubstr handle, csubstr prefix, csubstr tag,
                       csubstr expected_with, csubstr expected_without){
        RYML_TRACE_FMT("handle={}", handle);
        RYML_TRACE_FMT("prefix={}", prefix);
        RYML_TRACE_FMT("tag={}", tag);
        {
            SCOPED_TRACE("no buf");
            checkok__(substr{}, handle, prefix, tag, expected_with, expected_without);
        }
        char buf_[256];
        substr buf = buf_;
        ASSERT_GE(buf.len, expected_with.len);
        ASSERT_GE(buf.len, expected_without.len);
        size_t minsz = expected_with.len < expected_without.len ? expected_with.len : expected_without.len;
        {
            SCOPED_TRACE("small buf");
            checkok__(buf.first(minsz / 2), handle, prefix, tag, expected_with, expected_without);
        }
        {
            SCOPED_TRACE("ok buf");
            checkok__(buf, handle, prefix, tag, expected_with, expected_without);
        }
    };
    #define check(what, ...)                    \
    {                                           \
        if(testing::Test::HasFailure())         \
            printf("-------------------------\n%s:%d: testcase\n", __FILE__, __LINE__); \
        SCOPED_TRACE("test case");              \
        check##what##_(__VA_ARGS__);            \
    }
    const csubstr nohandle = {}, nopfx = {};
    check(ok, nohandle, nopfx, "!<resolved>", "<resolved>", "<resolved>");
    check(ok, nohandle, nopfx, "<resolved>", "<resolved>", "<resolved>");
    check(ok, "!", "b-", "!<resolved>", "<resolved>", "<resolved>");
    check(ok, "!", "b-", "<resolved>", "<resolved>", "<resolved>");
    check(err, "!m!", "prefix-", "!m!<wtf>");
    check(err, "!m!", "prefix-", "!<wtf");
    check(err, "!m!", "prefix-", "<wtf");
    check(err, nohandle, nopfx, "!<wtf");
    check(err, nohandle, nopfx, "<wtf");
    check(err, "!m!", "prefix-", "!m!abc<wtf>");
    check(err, "!m!", "prefix-", "!m!%");
    check(err, "!m!", "prefix-", "!m!%3");
    check(err, "!m!", "prefix-", "!m!%3g");
    check(err, "!m!", "prefix-", "!m!%ff");
    check(err, "!m!", "prefix-", "!m!%fe");
    check(err, "!m!", "prefix-", "!m!%80");
    check(err, "!m!", "prefix-", "!m!aaa%");
    check(err, "!m!", "prefix-", "!m!aaa%3");
    check(err, "!m!", "prefix-", "!m!aaa%3g");
    check(ok, "!m!", "prefix-", "!m!%7f", "<prefix-\x7f>", "prefix-\x7f");
    check(ok, "!m!", "prefix-", "!m!a", "<prefix-a>", "prefix-a");
    check(err, nohandle, nopfx, "!m!a");
    check(ok, "!m!", "prefix-", "!m!a-#;/?:@&=+$_.~*'%32()abc",
          /*                 */ "<prefix-a-#;/?:@&=+$_.~*'2()abc>",
          /*                 */ "prefix-a-#;/?:@&=+$_.~*'2()abc");
    check(ok, "!m!", "prefix-", "!m!%32%33%20%33%32",
          /*                 */ "<prefix-23 32>",
          /*                 */ "prefix-23 32");
    check(ok, "!m!", "prefix-", "!!str", "<tag:yaml.org,2002:str>", "tag:yaml.org,2002:str");
    check(ok, "!m!", "prefix-", "!foo", "<!foo>", "!foo");
    #undef check
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

constexpr const auto errbasic = ExpectedErrorType::err_basic;
constexpr const auto errparse = ExpectedErrorType::err_parse;
constexpr const auto errvisit = ExpectedErrorType::err_visit;

void test_fail_tag_parsing(csubstr yaml, ExpectedErrorType errtype=errparse)
{
    Tree t;
    ExpectError::check_error(errtype, &t, [&]{
        t = parse_in_arena(yaml);
    });
}

void test_fail_tag_resolve(csubstr yaml, ExpectedErrorType errtype=errvisit)
{
    Tree t;
    t = parse_in_arena(yaml);
    _c4dbg_tree(t);
    ExpectError::check_error(errtype, &t, [&]{
        TagCache tag_cache;
        t.resolve_tags(tag_cache);
    });
}

TEST(tag_directives, errors_parsing_tags_1)
{
    test_fail_tag_parsing(R"(
%TAG
--- # Bulb here
!m!light fluorescent)", errparse);
}

TEST(tag_directives, errors_parsing_tags_2)
{
    test_fail_tag_parsing(R"(
%TAG !m!
--- # Bulb here
!m!light fluorescent)", errparse);
}

TEST(tag_directives, errors_resolving_tags_1)
{
    test_fail_tag_resolve(R"(
!b!a asd
)", errbasic);
}

TEST(tag_directives, errors_resolving_tags_2_not_an_error_1)
{
    Tree t = parse_in_arena(R"(!local foo)");
    TagCache tag_cache;
    t.resolve_tags(tag_cache);
    EXPECT_EQ(t.rootref().val_tag(), "<!local>");
    EXPECT_EQ(t.rootref().val(), "foo");
 }

TEST(tag_directives, errors_resolving_tags_2_not_an_error_2)
{
    Tree t = parse_in_arena(R"(! foo)");
    TagCache tag_cache;
    t.resolve_tags(tag_cache);
    EXPECT_EQ(t.rootref().val_tag(), "<!>");
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
    EXPECT_EQ(t.docref(1)[0].key_tag(), "!m!light");
    EXPECT_EQ(t.docref(1)[0].val_tag(), "!m!light");
    EXPECT_EQ(t.num_tag_directives(), 2u);
    TagCache tag_cache;
    t.resolve_tags(tag_cache);
    EXPECT_EQ(t.docref(0)[0].key_tag(), "<!my-light>");
    EXPECT_EQ(t.docref(0)[0].val_tag(), "<!my-light>");
    EXPECT_EQ(t.docref(1)[0].key_tag(), "<!meta-light>");
    EXPECT_EQ(t.docref(1)[0].val_tag(), "<!meta-light>");
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
    Tree t(0);
    TagCache tag_cache;
    t.resolve_tags(tag_cache);
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
        TagCache tag_cache;
        t.resolve_tags(tag_cache);
        EXPECT_EQ(t.docref(0)[0].val_tag(), csubstr("<tag:example.com,2000:app/abc!>"));
    }
    {
        Tree t;
        TagCache tag_cache;
        auto checkerr = [&](csubstr yaml){
            ExpectError::check_error_basic(&t, [&]{
                t.clear();
                t = parse_in_arena(yaml);
                t.resolve_tags(tag_cache);
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


TEST(tag_directives, lookup_range_empty)
{
    TagDirectives tds = {};
    EXPECT_EQ(tds.lookup_range(0).size(), 0);
    EXPECT_EQ(tds.lookup_range(1).size(), 0);
    EXPECT_EQ(tds.lookup_range(npos).size(), 0);
}

TEST(tag_directives, lookup_range_full)
{
    TagDirectives tds = {{
        TagDirective{csubstr("!a!"), csubstr("pa-"), 1},
        TagDirective{csubstr("!b!"), csubstr("pb-"), 1},
        TagDirective{csubstr("!c!"), csubstr("pc-"), 2},
        TagDirective{csubstr("!d!"), csubstr("pd-"), 6},
    }};
    TagDirective const* invalid = &tds.m_directives[0];
    TagDirective const* last = tds.m_directives + RYML_MAX_TAG_DIRECTIVES;
    EXPECT_EQ(tds.lookup_range(0).size(), 0);
    EXPECT_EQ(tds.lookup_range(0).b, invalid);
    EXPECT_EQ(tds.lookup_range(0).e, invalid);
    EXPECT_EQ(tds.lookup_range(1).size(), 2);
    EXPECT_EQ(tds.lookup_range(1).b, &tds.m_directives[0]);
    EXPECT_EQ(tds.lookup_range(1).e, &tds.m_directives[2]);
    EXPECT_EQ(tds.lookup_range(2).size(), 1);
    EXPECT_EQ(tds.lookup_range(2).b, &tds.m_directives[2]);
    EXPECT_EQ(tds.lookup_range(2).e, &tds.m_directives[3]);
    EXPECT_EQ(tds.lookup_range(3).size(), 0);
    EXPECT_EQ(tds.lookup_range(3).b, invalid);
    EXPECT_EQ(tds.lookup_range(3).e, invalid);
    EXPECT_EQ(tds.lookup_range(4).size(), 0);
    EXPECT_EQ(tds.lookup_range(4).b, invalid);
    EXPECT_EQ(tds.lookup_range(4).e, invalid);
    EXPECT_EQ(tds.lookup_range(5).size(), 0);
    EXPECT_EQ(tds.lookup_range(5).b, invalid);
    EXPECT_EQ(tds.lookup_range(5).e, invalid);
    EXPECT_EQ(tds.lookup_range(6).size(), 1);
    EXPECT_EQ(tds.lookup_range(6).b, &tds.m_directives[3]);
    EXPECT_EQ(tds.lookup_range(6).e, last);
    EXPECT_EQ(tds.lookup_range(7).size(), 0);
    EXPECT_EQ(tds.lookup_range(7).b, invalid);
    EXPECT_EQ(tds.lookup_range(7).e, invalid);
    EXPECT_EQ(tds.lookup_range(8).size(), 0);
    EXPECT_EQ(tds.lookup_range(8).b, invalid);
    EXPECT_EQ(tds.lookup_range(8).e, invalid);
    EXPECT_EQ(tds.lookup_range(npos).size(), 0);
    EXPECT_EQ(tds.lookup_range(npos).b, invalid);
    EXPECT_EQ(tds.lookup_range(npos).e, invalid);
}

TEST(tag_directives, lookup_qmrk_1)
{
    TagDirectives tds = {{
        TagDirective{csubstr("!a!"), csubstr("pc-"), 2},
    }};
    EXPECT_EQ(tds.lookup("!", 2), nullptr);
    EXPECT_EQ(tds.lookup("!a", 2), nullptr);
    EXPECT_EQ(tds.lookup("!foo", 2), nullptr);
    EXPECT_EQ(tds.lookup("!!", 2), nullptr);
    EXPECT_EQ(tds.lookup("!!str", 2), nullptr);
    EXPECT_EQ(tds.lookup("!!a", 2), nullptr);
    EXPECT_EQ(tds.lookup("!!!", 2), nullptr);
    EXPECT_EQ(tds.lookup("!a!b", 2) - tds.m_directives, 0);
    EXPECT_EQ(tds.lookup("!b!c", 2), nullptr);
    EXPECT_EQ(tds.lookup("!<resolved>", 2), nullptr);
}

TEST(tag_directives, lookup_qmrk_2)
{
    TagDirectives tds = {{
        TagDirective{csubstr("!"), csubstr("pa-"), 2},
        TagDirective{csubstr("!!"), csubstr("pb-"), 2},
        TagDirective{csubstr("!a!"), csubstr("pc-"), 2},
    }};
    EXPECT_EQ(tds.lookup("!", 2) - tds.m_directives, 0);
    EXPECT_EQ(tds.lookup("!a", 2) - tds.m_directives, 0);
    EXPECT_EQ(tds.lookup("!foo", 2) - tds.m_directives, 0);
    EXPECT_EQ(tds.lookup("!!", 2) - tds.m_directives, 1);
    EXPECT_EQ(tds.lookup("!!str", 2) - tds.m_directives, 1);
    EXPECT_EQ(tds.lookup("!!a", 2) - tds.m_directives, 1);
    EXPECT_EQ(tds.lookup("!!!", 2) - tds.m_directives, 1);
    EXPECT_EQ(tds.lookup("!a!b", 2) - tds.m_directives, 2);
    EXPECT_EQ(tds.lookup("!b!c", 2), nullptr);
    EXPECT_EQ(tds.lookup("!<resolved>", 2), nullptr);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void test_tree_normalize_(std::string const& input, std::string expected={})
{
    SCOPED_TRACE(input);
    if(expected.empty())
        expected = input;
    Tree tree = parse_in_arena(to_csubstr(input));
    tree.normalize_tags();
    EXPECT_EQ(expected, emitrs_yaml<std::string>(tree));
}
#define test_tree_normalize(...)                \
    {                                           \
        SCOPED_TRACE("here");                   \
        test_tree_normalize_(__VA_ARGS__);      \
    }

TEST(tags, tree_normalize_empty_tree)
{
    Tree t(0);
    EXPECT_TRUE(t.empty());
    t.normalize_tags();
    EXPECT_TRUE(t.empty());
}
TEST(tags, tree_normalize)
{
    test_tree_normalize("");
    test_tree_normalize(""
            "- !!str s\n"
            "- !!int 0\n"
            "");
    test_tree_normalize(""
            "- !<tag:yaml.org,2002:str> s\n"
            "- !<tag:yaml.org,2002:int> 0\n"
            "",
            ""
            "- !!str s\n"
            "- !!int 0\n"
            "");
}

void test_tree_normalize_long_(std::string const& input, std::string expected={})
{
    SCOPED_TRACE(input);
    if(expected.empty())
        expected = input;
    Tree tree = parse_in_arena(to_csubstr(input));
    tree.normalize_tags_long();
    EXPECT_EQ(expected, emitrs_yaml<std::string>(tree));
}
#define test_tree_normalize_long(...)           \
    {                                           \
        SCOPED_TRACE("here");                   \
        test_tree_normalize_long_(__VA_ARGS__); \
    }


TEST(tags, tree_normalize_long_empty_tree)
{
    Tree t(0);
    EXPECT_TRUE(t.empty());
    t.normalize_tags_long();
    EXPECT_TRUE(t.empty());
}

TEST(tags, tree_normalize_long)
{
    test_tree_normalize_long("");
    test_tree_normalize_long(""
            "- !!str s\n"
            "- !!int 0\n"
            "- !!foo 0\n"
            ,
            "- !<tag:yaml.org,2002:str> s\n"
            "- !<tag:yaml.org,2002:int> 0\n"
            "- !!foo 0\n"
            "");
    test_tree_normalize_long(""
            "- !<tag:yaml.org,2002:str> s\n"
            "- !<tag:yaml.org,2002:int> 0\n"
            "- !<tag:yaml.org,2002:foo> 0\n"
            "",
            ""
            "- !<tag:yaml.org,2002:str> s\n"
            "- !<tag:yaml.org,2002:int> 0\n"
            "- !<tag:yaml.org,2002:foo> 0\n"
            "");
}

void test_tree_resolve_(std::string const& input, std::string expected={}, bool all_tags=true)
{
    SCOPED_TRACE(input);
    if(expected.empty())
        expected = input;
    Tree tree = parse_in_arena(to_csubstr(input));
    TagCache cache;
    tree.resolve_tags(cache, all_tags);
    std::string actual = emitrs_yaml<std::string>(tree);
    SCOPED_TRACE(actual);
    EXPECT_EQ(expected, actual);
}
#define test_tree_resolve(...)                  \
    {                                           \
        SCOPED_TRACE("here");                   \
        test_tree_resolve_(__VA_ARGS__);        \
    }

TEST(tags, tree_resolve_tags_0)
{
    test_tree_resolve("");
    Tree tree;
    TagCache tag_cache;
    tree.resolve_tags(tag_cache); // ok?
}

TEST(tags, tree_resolve_tags_1)
{
    test_tree_resolve(""
            "- !<foo> 0\n"
            "- !!str s\n"
            "- !!int 0\n"
            "- !!foo 0\n"
            "- !foo 0\n"
            "- !<foo> 0\n"
            "- !<!foo> 0\n"
            ,
            "- !<foo> 0\n"
            "- !!str s\n"
            "- !!int 0\n"
            "- !<tag:yaml.org,2002:foo> 0\n"
            "- !<!foo> 0\n"
            "- !<foo> 0\n"
            "- !<!foo> 0\n"
            ""
            ,
            false);
}

TEST(tags, tree_resolve_tags_2)
{
    test_tree_resolve(""
            "- !<tag:yaml.org,2002:str> s\n"
            "- !<tag:yaml.org,2002:int> 0\n"
            "",
            ""
            "- !<tag:yaml.org,2002:str> s\n"
            "- !<tag:yaml.org,2002:int> 0\n"
            ""
            ,
            false);
}

TEST(tags, tree_resolve_tags_3)
{
    std::string const yaml =
        ""
        "%TAG ! xmark-\n"
        "%TAG !c! custom-\n"
        "---\n"
        "- !!str s\n"
        "- !!foo s\n"
        "- !!string s\n"
        "- !foo s\n"
        "- !<foo> s\n"
        "- !<!foo> s\n"
        "- !c!foo s\n"
        "- !c!bar s\n"
        "";
    test_tree_resolve(yaml,
                      "%TAG ! xmark-\n"
                      "%TAG !c! custom-\n"
                      "---\n"
                      "- !!str s\n"
                      "- !<tag:yaml.org,2002:foo> s\n"
                      "- !<tag:yaml.org,2002:string> s\n"
                      "- !<xmark-foo> s\n"
                      "- !<foo> s\n"
                      "- !<!foo> s\n"
                      "- !<custom-foo> s\n"
                      "- !<custom-bar> s\n"
                      "");
    test_tree_resolve(yaml,
                      "%TAG ! xmark-\n"
                      "%TAG !c! custom-\n"
                      "---\n"
                      "- !!str s\n"
                      "- !<tag:yaml.org,2002:foo> s\n"
                      "- !<tag:yaml.org,2002:string> s\n"
                      "- !<xmark-foo> s\n"
                      "- !<foo> s\n"
                      "- !<!foo> s\n"
                      "- !<custom-foo> s\n"
                      "- !<custom-bar> s\n"
                      "",
                      false);
}


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
    EXPECT_EQ(t0.docref(2).val_tag(), "<tag:example.com,2000:app/foo>");
    TagCache tag_cache0;
    t0.resolve_tags(tag_cache0);
    EXPECT_EQ(t0.docref(0).val_tag(), "<!foo>");
    EXPECT_EQ(t0.docref(1).val_tag(), "<tag:example.com,2000:app/foo>");
    EXPECT_EQ(t0.docref(2).val_tag(), "<tag:example.com,2000:app/foo>");
    std::string emitted0 = emitrs_yaml<std::string>(t0);
    EXPECT_EQ(emitted0, R"(--- !<!foo> "bar"
...
%TAG ! tag:example.com,2000:app/
--- !<tag:example.com,2000:app/foo> "bar"
--- !<tag:example.com,2000:app/foo> "bar"
)");
    //
    Tree t1 = parse_in_arena(to_csubstr(emitted0));
    TagCache tag_cache1;
    t1.resolve_tags(tag_cache1);
    EXPECT_EQ(t1.docref(0).val_tag(), "<!foo>");
    EXPECT_EQ(t1.docref(1).val_tag(), "<tag:example.com,2000:app/foo>");
    EXPECT_EQ(t1.docref(2).val_tag(), "<tag:example.com,2000:app/foo>");
    std::string emitted1 = emitrs_yaml<std::string>(t1);
    EXPECT_EQ(emitted1, emitted0);
    //
    Tree t2 = parse_in_arena(to_csubstr(emitted1));
    TagCache tag_cache2;
    t2.resolve_tags(tag_cache2);
    EXPECT_EQ(t2.docref(0).val_tag(), "<!foo>");
    EXPECT_EQ(t2.docref(1).val_tag(), "<tag:example.com,2000:app/foo>");
    EXPECT_EQ(t2.docref(2).val_tag(), "<tag:example.com,2000:app/foo>");
    std::string emitted2 = emitrs_yaml<std::string>(t2);
    EXPECT_EQ(emitted2, emitted0);
    //
    Tree t3 = parse_in_arena(to_csubstr(emitted2));
    TagCache tag_cache3;
    t3.resolve_tags(tag_cache3);
    EXPECT_EQ(t3.docref(0).val_tag(), "<!foo>");
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
        EXPECT_TRUE(t.rootref().type().is_map());
        EXPECT_TRUE(t.rootref().type().has_val_tag());
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
        ASSERT_TRUE(t.rootref().type().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 5u);
        //
        EXPECT_TRUE(t[0].type().is_map());
        EXPECT_TRUE(!t[0].type().has_val_tag());
        EXPECT_EQ(t[0]["foo"].val(), csubstr("bar"));
        //
        EXPECT_TRUE(t[1].type().is_map());
        EXPECT_TRUE(!t[1].type().has_val_tag());
        EXPECT_EQ(t[1]["foo"].val(), csubstr("bar"));
        //
        EXPECT_TRUE(t[2].type().is_map());
        EXPECT_TRUE(!t[2].type().has_val_tag());
        EXPECT_EQ(t[2]["foo"].val(), csubstr("bar"));
        //
        EXPECT_TRUE(t[3].type().is_map());
        ASSERT_TRUE(t[3].type().has_val_tag());
        EXPECT_EQ(t[3].val_tag(), csubstr("!!map"));
        EXPECT_EQ(t[3]["foo"].val(), csubstr("bar"));
        //
        EXPECT_TRUE(t[4].type().is_map());
        ASSERT_TRUE(t[4].type().has_val_tag());
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
    EXPECT_EQ(t[0]["key2"].key_tag(), csubstr("<!key>"));
    EXPECT_EQ(t[0]["key2"].val_tag(), csubstr("<!val>"));
    EXPECT_EQ(t[0]["key3"].key_tag(), csubstr("<key>"));
    EXPECT_EQ(t[0]["key3"].val_tag(), csubstr("<val>"));
    EXPECT_EQ(t[0]["<!key> key4"].type().has_key_tag(), false);
    EXPECT_EQ(t[0]["<!key> key4"].type().has_val_tag(), false);
    EXPECT_EQ(t[0]["<!key> key4"].key(), csubstr("<!key> key4"));
    EXPECT_EQ(t[0]["<!key> key4"].val(), csubstr("<!val> val4"));
    EXPECT_EQ(t[1].val_tag(), csubstr("<tag:yaml.org,2002:map>"));
    EXPECT_EQ(t[1]["key1"].key_tag(), csubstr("!key"));
    EXPECT_EQ(t[1]["key1"].val_tag(), csubstr("!val"));
    EXPECT_EQ(t[1]["key1"].key(), csubstr("key1"));
    EXPECT_EQ(t[1]["key1"].val(), csubstr("val1"));
    EXPECT_EQ(t[2].val_tag(), csubstr("<tag:yaml.org,2002:seq>"));
    EXPECT_EQ(t[2][0].val_tag(), csubstr("!val"));
    EXPECT_EQ(t[2][1].val_tag(), csubstr("!str"));
    EXPECT_FALSE(t[2][2].type().has_val_tag()); // not tag
    EXPECT_EQ(t[2][2].val(), csubstr("<!str> val")); // not tag
    EXPECT_EQ(t[2][3].val_tag(), csubstr("<!str>"));
    EXPECT_EQ(t[2][4].val_tag(), csubstr("<!!str>"));
    EXPECT_EQ(t[2][5].val_tag(), csubstr("<tag:yaml.org,2002:str>"));
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
    t.set_map(rid);
    t.set_val_tag(rid, "!valtag");
    EXPECT_EQ(t.val_tag(rid), "!valtag");

    // a keymap
    {
        id_type child = t.append_child(rid);
        t.set_seq(child);
        t.set_key(child, "key2");
        t.set_key_tag(child, "!keytag");
        t.set_val_tag(child, "!valtag2");
        EXPECT_TRUE(t.type(child).has_key());
        EXPECT_FALSE(t.type(child).has_val());
        EXPECT_EQ(t.key(child), "key2");
        EXPECT_EQ(t.key_tag(child), "!keytag");
        EXPECT_EQ(t.val_tag(child), "!valtag2");
    }

    // a keyseq
    {
        id_type child = t.append_child(rid);
        t.set_seq(child);
        t.set_key(child, "key2");
        t.set_key_tag(child, "!keytag");
        t.set_val_tag(child, "!valtag2");
        EXPECT_TRUE(t.type(child).has_key());
        EXPECT_FALSE(t.type(child).has_val());
        EXPECT_EQ(t.key(child), "key2");
        EXPECT_EQ(t.key_tag(child), "!keytag");
        EXPECT_EQ(t.val_tag(child), "!valtag2");
    }

    // a keyval
    {
        id_type child = t.append_child(rid);
        t.set_key(child, "key");
        t.set_val(child, "val");
        t.set_key_tag(child, "!keytag");
        t.set_val_tag(child, "!valtag");
        EXPECT_TRUE(t.type(child).has_key());
        EXPECT_TRUE(t.type(child).has_val());
        EXPECT_EQ(t.key(child), "key");
        EXPECT_EQ(t.val(child), "val");
        EXPECT_EQ(t.key_tag(child), "!keytag");
        EXPECT_EQ(t.val_tag(child), "!valtag");
    }

    // a val
    {
        id_type seqid = t[1].id();
        ASSERT_TRUE(t.type(seqid).is_seq());
        id_type child = t.append_child(seqid);
        t.set_val(child, "val");
        t.set_val_tag(child, "!valtag");
        EXPECT_FALSE(t.type(child).has_key());
        EXPECT_TRUE(t.type(child).has_val());
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
    EXPECT_FALSE(t.type(empty_keyval).has_key_tag());
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.key_tag(empty_keyval), "");
    });
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.key_tag(keyval), "");
    });
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.key_tag(keymap), "");
    });
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.key_tag(keyseq), "");
    });
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.key_tag(val), "");
    });
    // cannot get val tag in a node that does not have a val tag
    EXPECT_FALSE(t.type(empty_val).has_val_tag());
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.val_tag(empty_val), "");
    });
    EXPECT_FALSE(t.type(empty_val).has_val_tag());
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.val_tag(empty_keyval), "");
    });
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.val_tag(keyval), "");
    });
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.val_tag(keymap), "");
    });
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.val_tag(keyseq), "");
    });
    ExpectError::check_assert_visit(&t, [&](){
        EXPECT_EQ(t.val_tag(val), "");
    });
    // cannot set key tag in a node that does not have a key
    EXPECT_FALSE(t.type(empty_keyval).has_key());
    ExpectError::check_assert_visit(&t, [&](){
        t.set_key_tag(empty_keyval, "!keytag");
    });
    EXPECT_FALSE(t.type(val).has_key_tag()); // must stay the same
    ExpectError::check_assert_visit(&t, [&](){
        t.set_key_tag(val, "!valtag");
    });
    EXPECT_FALSE(t.type(val).has_key_tag()); // must stay the same
    // cannot set val tag in a node that does not have a val
    EXPECT_FALSE(t.type(empty_val).has_val());
    ExpectError::check_assert_visit(&t, [&](){
        t.set_val_tag(empty_val, "!valtag");
    });
    EXPECT_FALSE(t.type(empty_val).has_val_tag()); // must stay the same
    EXPECT_FALSE(t.type(empty_keyval).has_val());
    ExpectError::check_assert_visit(&t, [&](){
        t.set_val_tag(empty_keyval, "!valtag");
    });
    EXPECT_FALSE(t.type(empty_val).has_val_tag()); // must stay the same
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
    EXPECT_EQ(t[0].val_tag(), "<foo bar>");
    EXPECT_EQ(t[0].val(), "val");
    EXPECT_EQ(t[1].val_tag(), "<foo>");
    EXPECT_EQ(t[1].val(), "bar> val");
    EXPECT_EQ(t[2].val_tag(), "<foo>");
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
        ASSERT_TRUE(t.rootref().type().has_val_tag());
        EXPECT_EQ(t.rootref().val_tag(), "!!map");
        ASSERT_TRUE(t["k"].type().has_val_tag());
        ASSERT_TRUE(t["j"].type().has_val_tag());
        EXPECT_EQ(t["k"].val_tag(), "!!seq");
        EXPECT_EQ(t["j"].val_tag(), "!!seq");
    }
    {
        Tree t = parse_in_arena(R"(!!seq [
  !!map { !!str k: v},
  !!map { !!str ? k: v}
])");
        ASSERT_TRUE(t.rootref().type().has_val_tag());
        EXPECT_EQ(t.rootref().val_tag(), "!!seq");
        ASSERT_TRUE(t[0].type().has_val_tag());
        ASSERT_TRUE(t[1].type().has_val_tag());
        EXPECT_EQ(t[0].val_tag(), "!!map");
        EXPECT_EQ(t[1].val_tag(), "!!map");
        ASSERT_TRUE(t[0]["k"].type().has_key_tag());
        ASSERT_TRUE(t[1]["k"].type().has_key_tag());
        EXPECT_EQ(t[0]["k"].key_tag(), "!!str");
        EXPECT_EQ(t[1]["k"].key_tag(), "!!str");
    }
}

TEST(tags, fuzzcrash0)
{
    Tree tree;
    ExpectError::check_error_parse(&tree, [&]{
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
        EXPECT_TRUE(t.rootref().type().is_stream());
        EXPECT_TRUE(t.docref(0).type().is_val());
        EXPECT_TRUE(t.docref(0).type().has_val_tag());
        EXPECT_EQ(t.docref(0).val_tag(), "!yamlscript/v0/bare");
        EXPECT_EQ(t.docref(0).val(), csubstr{});
        EXPECT_TRUE(t.docref(1).type().is_val());
        EXPECT_TRUE(t.docref(1).type().has_val_tag());
        EXPECT_EQ(t.docref(1).val_tag(), "!code");
        EXPECT_EQ(t.docref(1).val(), "42");
    });
}

TEST(tags, key_tag_error_json)
{
    Tree tree = parse_in_arena("{!!str key: val}");
    EXPECT_EQ(emitrs_json<std::string>(tree), "{\"key\": \"val\"}");
    ExpectError::check_error_visit(&tree, [&]{
        emitrs_json<std::string>(tree, EmitOptions{}.json_error_flags(EmitOptions::JSON_ERR_ON_TAG));
    });
}

TEST(tags, val_tag_error_json)
{
    Tree tree = parse_in_arena("{key: !!str val}");
    EXPECT_EQ(emitrs_json<std::string>(tree), "{\"key\": \"val\"}");
    ExpectError::check_error_visit(&tree, [&]{
        emitrs_json<std::string>(tree, EmitOptions{}.json_error_flags(EmitOptions::JSON_ERR_ON_TAG));
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
N(MFM, L{
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
N(MFM, L{
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
N(SFM, L{
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
    N(SFM, L{
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
N(MB, L{N(KP|SFM, "some_seq", TL("!!its_type", L{
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
N(MB, L{N(KP|SFM, "some_seq", TL("!its_type", L{
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
