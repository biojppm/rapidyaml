#include "./test_group.hpp"

namespace c4 {
namespace yml {

TEST(to_tag, user)
{
    EXPECT_EQ(to_tag("!"), TAG_NONE);
    EXPECT_EQ(to_tag("!."), TAG_NONE);
    EXPECT_EQ(to_tag("!good_type"), TAG_NONE);
}

TEST(to_tag, double_exc_mark)
{
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

TEST(to_tag, with_namespace)
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
}

TEST(to_tag, with_namespace_bracket)
{
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:"          ), TAG_NONE);
    EXPECT_EQ(to_tag("<tag:yaml.org,2002:."         ), TAG_NONE);

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
}

TEST(from_tag, basic)
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
}

TEST(normalize_tag, basic)
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
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define TAG_PROPERTY_CASES \
    "user tag, empty, test suite 52DL",                \
    "tag property in implicit map",\
    "tag property in explicit map",\
    "tag property in implicit seq",\
    "tag property in explicit seq",\
    "tagged explicit sequence in map",\
    "tagged doc",\
    "ambiguous tag in map, std tag",\
    "ambiguous tag in map, usr tag",\
    "ambiguous tag in seq, std tag",\
    "ambiguous tag in seq, usr tag"


CASE_GROUP(TAG_PROPERTY)
{
    APPEND_CASES(

C("user tag, empty, test suite 52DL",
R"(! a)",
N(DOCVAL, TS("!", "a"))
),

C("tag property in implicit map",
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
    L{
      N("ivar", TS("!!int", "0")),
      N("svar", TS("!!str", "0")),
      N("fvar", TS("!!float", "0.1")),
      N(TS("!!int", "2"), TS("!!float", "3")),
      N(TS("!!float", "3"), TS("!!int", "3.4")),
      N(TS("!!str", "key"), TS("!!int", "val")),
      N("myObject", TL("!myClass", L{N("name", "Joe"), N("age", "15")})),
      N(QV, "picture", TS("!!binary", R"(R0lGODdhDQAIAIAAAAAAANn Z2SwAAAAADQAIAAACF4SDGQ ar3xxbJ9p0qa7R0YxwzaFME 1IAADs=)")),
    }
),

C("tag property in explicit map",
R"({
ivar: !!int 0,
svar: !!str 0,
!!str key: !!int val
}
)",
    L{
      N("ivar", TS("!!int", "0")),
      N("svar", TS("!!str", "0")),
      N(TS("!!str", "key"), TS("!!int", "val"))
    }
),

C("tag property in implicit seq",
R"(- !!int 0
- !!str 0
)",
    L{
      N(TS("!!int", "0")),
      N(TS("!!str", "0")),
    }
),

C("tag property in explicit seq",
R"([
!!int 0,
!!str 0
]
)",
    L{
      N(TS("!!int", "0")),
      N(TS("!!str", "0")),
    }
),

C("tagged explicit sequence in map",
R"(some_seq: !its_type [
!!int 0,
!!str 0
]
)",
    L{N("some_seq", TL("!its_type", L{
              N(TS("!!int", "0")),
              N(TS("!!str", "0")),
                  }))
          }
),

C("tagged doc",
R"(
--- !!map
a: 0
b: 1
--- !map
? a
: b
--- !!seq
- a
- b
--- !!str
a
 b
...
--- !!str a b
...
--- !!str a b
--- !!str
a: b
--- !!str a: b
---
!!str a: b
---
!!str a
 b
---
!!set
? a
? b
--- !!set
? a
? b
)",
N(STREAM, L{
    N(DOCMAP, TL("!!map", L{N("a", "0"), N("b", "1")})),
    N(DOCMAP, TL("!map", L{N("a", "b")})),
    N(DOCSEQ, TL("!!seq", L{N("a"), N("b")})),
    N(DOCVAL, TS("!!str", "a b")),
    N(DOCVAL, TS("!!str", "a b")),
    N(DOCVAL, TS("!!str", "a b")),
    N(DOCVAL, TS("!!str", "a: b")),
    N(DOCVAL, TS("!!str", "a: b")),
    N(DOCMAP, L{N(TS("!!str", "a"), "b")}),
    N(DOCVAL, TS("!!str", "a b")),
    N(DOCMAP, TL("!!set", L{N(KEYVAL, "a", /*"~"*/{}), N(KEYVAL, "b", /*"~"*/{})})),
    N(DOCMAP, TL("!!set", L{N(KEYVAL, "a", /*"~"*/{}), N(KEYVAL, "b", /*"~"*/{})})),
})),


C("ambiguous tag in map, std tag",
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
TL("!!map", L{
  N(TS("!!str", "a0"), TS("!!xxx", "b0")),
  N(TS("!!str", "fooz"), TL("!!map", L{N("k1", TS("!!float", "1.0")), N("k3", TS("!!float", "2.0"))})),
  N(TS("!!str", "foo"), TL("!!map", L{N(TS("!!int", "1"), TS("!!float", "20.0")), N(TS("!!int", "3"), TS("!!float", "40.0"))})),
  N("bar", TL("!!map", L{N("10", TS("!!str", "2")), N("30", TS("!!str", "4"))})),
  N(TS("!!str", "baz"), L{N(TS("!!int", "10"), TS("!!float", "20")), N(TS("!!int", "30"), TS("!!float", "40"))}),
})),

C("ambiguous tag in map, usr tag",
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
TL("!map", L{
  N(TS("!str", "a0"), TS("!xxx", "b0")),
  N(TS("!str", "fooz"), TL("!map", L{N("k1", TS("!float", "1.0")), N("k3", TS("!float", "2.0"))})),
  N(TS("!str", "foo"), TL("!map", L{N(TS("!int", "1"), TS("!float", "20.0")), N(TS("!int", "3"), TS("!float", "40.0"))})),
  N("bar", TL("!map", L{N("10", TS("!str", "2")), N("30", TS("!str", "4"))})),
  N(TS("!str", "baz"), L{N(TS("!int", "10"), TS("!float", "20")), N(TS("!int", "30"), TS("!float", "40"))}),
})),


C("ambiguous tag in seq, std tag",
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
TL("!!seq", L{
  N(L{N(TS("!!str", "k1"), "v1"), N(TS("!!str", "k2"), "v2"), N(TS("!!str", "k3"), "v3"), }),
  N(TL("!!map", L{N(TS("!!str", "k4"), "v4"), N(TS("!!str", "k5"), "v5"), N(TS("!!str", "k6"), "v6"), })),
  N(TL("!!map", L{N("k7", "v7"), N("k8", "v8"), N("k9", "v9"), })),
  N(L{N(TS("!!str", "v10")), N(TS("!!str", "v20")), N(TS("!!str", "v30"))}),
  N(TL("!!seq", L{N(TS("!!str", "v40")), N(TS("!!str", "v50")), N(TS("!!str", "v60"))})),
  N(TL("!!seq", L{N("v70"), N("v80"), N("v90")})),
})),

C("ambiguous tag in seq, usr tag",
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
TL("!seq", L{
  N(L{N(TS("!str", "k1"), "v1"), N(TS("!str", "k2"), "v2"), N(TS("!str", "k3"), "v3"), }),
  N(TL("!map", L{N(TS("!str", "k4"), "v4"), N(TS("!str", "k5"), "v5"), N(TS("!str", "k6"), "v6"), })),
  N(TL("!map", L{N("k7", "v7"), N("k8", "v8"), N("k9", "v9"), })),
  N(L{N(TS("!str", "v10")), N(TS("!str", "v20")), N(TS("!str", "v30"))}),
  N(TL("!seq", L{N(TS("!str", "v40")), N(TS("!str", "v50")), N(TS("!str", "v60"))})),
  N(TL("!seq", L{N("v70"), N("v80"), N("v90")})),
})),

    )
}

INSTANTIATE_GROUP(TAG_PROPERTY)

} // namespace yml
} // namespace c4
