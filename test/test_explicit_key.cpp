#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

TEST(explicit_key, test_suite_5WE3)
{
    csubstr yaml = R"(
? explicit key # Empty value
? |
  block key
: - one # Explicit compact
  - two # block value
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_NE(t.find_child(t.root_id(), "explicit key"), (size_t)NONE);
        ASSERT_NE(t.find_child(t.root_id(), "block key\n"), (size_t)NONE);
        EXPECT_EQ(t["explicit key"].val(), csubstr{});
        EXPECT_TRUE(t["block key\n"].is_seq());
        EXPECT_EQ(t["block key\n"][0], csubstr("one"));
        EXPECT_EQ(t["block key\n"][1], csubstr("two"));
    });
}

TEST(explicit_key, test_suite_652Z)
{
    csubstr yaml = R"(
?foo: bar  # not an explicit key in RUNK
?bar: 42   # not an explicit key in RMAP|RKEY
?baz:
   ?bat: 24  # not an explicit key in RMAP|RVAL
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ConstNodeRef r = t.rootref();
        ASSERT_TRUE(r.has_child("?foo"));
        ASSERT_TRUE(r.has_child("?bar"));
        ASSERT_TRUE(r.has_child("?baz"));
        EXPECT_EQ(r["?foo"].val(), "bar");
        EXPECT_EQ(r["?bar"].val(), "42");
        ASSERT_TRUE(r["?baz"].has_child("?bat"));
        ASSERT_EQ(r["?baz"]["?bat"], "24");
    });
}


TEST(explicit_key, test_suite_DFF7_v1)
{
    csubstr yaml = R"(
{
? explicit: entry,
implicit: entry,
?
}
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_EQ(t.rootref().num_children(), 3u);
        ASSERT_TRUE(t.rootref().has_child("explicit"));
        EXPECT_EQ(t["explicit"].val(), csubstr("entry"));
        ASSERT_TRUE(t.rootref().has_child("implicit"));
        EXPECT_EQ(t["explicit"].val(), csubstr("entry"));
        ASSERT_TRUE(t.rootref().has_child(csubstr{}));
        EXPECT_EQ(t[csubstr{}].val(), csubstr{});
    });
}

TEST(explicit_key, test_suite_DFF7_v2)
{
    csubstr yaml = R"(
{
?
  key on next line
:
  val on next line
,
?
  # no key
:
  val on next line
}
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_EQ(t.rootref().num_children(), 2u);
        ASSERT_TRUE(t.rootref().has_child("key on next line"));
        EXPECT_EQ(t[0].key(), "key on next line");
        EXPECT_EQ(t[0].val(), "val on next line");
        EXPECT_EQ(t[1].key(), csubstr{});
        EXPECT_EQ(t[1].val(), "val on next line");
    });
}


TEST(explicit_key, test_suite_FRK4)
{
    csubstr yaml = R"(
{
  ? foo :,
  : bar,
}
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("foo"));
        EXPECT_EQ(t["foo"].val(), csubstr{});
        ASSERT_TRUE(t.rootref().has_child(csubstr{}));
        EXPECT_EQ(t[csubstr{}].val(), csubstr("bar"));
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(EXPLICIT_KEY)
{

ADD_CASE_TO_GROUP("explicit key, last value missing",
R"(
? a
? b
?
--- !!set   # test that we do not add any last item
? a
? b
--- !!set   # test that we do add the last item
? a
? b
?
)",
N(STREAM, L{
        N(DOC|MB, L{
                N(KP|VP, "a", {}),
                N(KP|VP, "b", {}),
                N(KP|VP, "", {})
            }),
        N(DOC|MB, TL("!!set", L{
                    N(KP|VP, "a", {}),
                    N(KP|VP, "b", {}),
                })),
        N(DOC|MB, TL("!!set", L{
                    N(KP|VP, "a", {}),
                    N(KP|VP, "b", {}),
                    N(KP|VP, "", {})
                })),
    })
);

ADD_CASE_TO_GROUP("explicit key, all values missing",
R"(
?
?
?
)",
N(MB, L{
  N(KP|VP, "", {}),
  N(KP|VP, "", {}),
  N(KP|VP, "", {}),
})
);

ADD_CASE_TO_GROUP("explicit key, last value missing, end doc",
R"(
? a
? b
?
...
--- !!set   # test that we do not add any last item
? a
? b
...
--- !!set   # test that we do add the last item
? a
? b
?
...
)",
N(STREAM, L{
        N(DOC|MB, L{
                N(KP|VP, "a", {}),
                N(KP|VP, "b", {}),
                N(KP|VP, "", {})
            }),
        N(DOC|MB, TL("!!set", L{
                    N(KP|VP, "a", {}),
                    N(KP|VP, "b", {}),
                })),
        N(DOC|MB, TL("!!set", L{
                    N(KP|VP, "a", {}),
                    N(KP|VP, "b", {}),
                    N(KP|VP, "", {})
                })),
    })
);

ADD_CASE_TO_GROUP("explicit key, ambiguity 2EBW",
R"(
a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~: safe
?foo: safe question mark
:foo: safe colon
-foo: safe dash
this is#not: a comment
)",
N(MB, L{
  N(KP|VP, "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~", "safe"),
  N(KP|VP, "?foo", "safe question mark"),
  N(KP|VP, ":foo", "safe colon"),
  N(KP|VP, "-foo", "safe dash"),
  N(KP|VP, "this is#not", "a comment"),
})
);

ADD_CASE_TO_GROUP("explicit key, ambiguity 2EBW, block seq",
R"(
- a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~
- ?foo
- :foo
- -foo
- this is#not:a comment
)",
N(SB, L{
  N(VP, "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"),
  N(VP, "?foo"),
  N(VP, ":foo"),
  N(VP, "-foo"),
  N(VP, "this is#not:a comment"),
})
);

ADD_CASE_TO_GROUP("explicit key with line break in between",
R"(
? an explicit key
: its value
)",
N(MB, L{N(KP|VP, "an explicit key", "its value")})
);

ADD_CASE_TO_GROUP("explicit key 2nd, inside explicit map",
R"(
{
    a simple key: a value,
    ? an explicit key: another value,
}
)",
N(MFS, L{
      N(KP|VP, "a simple key", "a value"),
      N(KP|VP, "an explicit key", "another value"),
  })
);

ADD_CASE_TO_GROUP("explicit key 1st, inside explicit map",
R"(
{
    ? an explicit key: another value,
    a simple key: a value,
}
)",
N(MFS, L{
      N(KP|VP, "an explicit key", "another value"),
      N(KP|VP, "a simple key", "a value"),
  })
);

ADD_CASE_TO_GROUP("M2N8", EXPECT_PARSE_ERROR,
R"(
- ? : x
- ? : 
- ? :
)",
  LineCol(2, 5)
);

ADD_CASE_TO_GROUP("explicit key 2nd", EXPECT_PARSE_ERROR,
R"(
a simple key: a value
? an explicit key: another value
)",
  LineCol(3, 19)
);

ADD_CASE_TO_GROUP("explicit key 1st", EXPECT_PARSE_ERROR,
R"(
? an explicit key: another value
a simple key: a value
)",
  LineCol(2, 19)
);

ADD_CASE_TO_GROUP("explicit key nested in a map, 1st", EXPECT_PARSE_ERROR,
R"(
map:
  ? an explicit key: another value
  a simple key: a value
? an explicit key deindented: its value
)",
  LineCol(3, 21)
);

ADD_CASE_TO_GROUP("explicit key nested in a seq, 1st", EXPECT_PARSE_ERROR,
R"(
- ? an explicit key: another value
  a simple key: a value
- ? another explicit key: its value
)",
  LineCol(2, 21)
);

ADD_CASE_TO_GROUP("explicit block key, literal, clip",
R"(? |
    This is a key
    that has multiple lines
    
: and this is its value
)",
N(MB, L{
  N(KL|VP, "This is a key\nthat has multiple lines\n", "and this is its value")
   })
);

ADD_CASE_TO_GROUP("explicit block key, literal, keep",
R"(? |+
    This is a key
    that has multiple lines
    
: and this is its value
)",
N(MB, L{
      N(KL|VP, "This is a key\nthat has multiple lines\n\n", "and this is its value")
   })
);

ADD_CASE_TO_GROUP("explicit block key, literal, strip",
R"(? |-
    This is a key
    that has multiple lines
    
: and this is its value
)",
N(MB, L{
  N(KL|VP, "This is a key\nthat has multiple lines", "and this is its value")
   })
);

ADD_CASE_TO_GROUP("explicit block key, folded, clip",
R"(? >
    This is a key
    that has multiple lines
    
: and this is its value
)",
N(MB, L{
  N(KF|VP, "This is a key that has multiple lines\n", "and this is its value")
   })
);

ADD_CASE_TO_GROUP("explicit block key, folded, keep",
R"(? >+
    This is a key
    that has multiple lines
    
: and this is its value
)",
N(MB, L{
  N(KF|VP, "This is a key that has multiple lines\n\n", "and this is its value")
   })
);

ADD_CASE_TO_GROUP("explicit block key, folded, strip",
R"(? >-
    This is a key
    that has multiple lines
    
: and this is its value
)",
N(MB, L{
      N(KF|VP, "This is a key that has multiple lines", "and this is its value")
   })
);

ADD_CASE_TO_GROUP("explicit key, missing val 7W2P",
R"(
? a
? b
c:
? d
e:
)",
N(MB, L{
        N(KP|VP, "a", {}),
        N(KP|VP, "b", {}),
        N(KP|VP, "c", {}),
        N(KP|VP, "d", {}),
        N(KP|VP, "e", {}),
    })
);

ADD_CASE_TO_GROUP("explicit key, missing val ZWK4",
R"(
a: 1
? b
&anchor c: 3
? d
!!str e: 4
? f
)",
N(MB, L{
        N(KP|VP, "a", "1"),
        N(KP|VP, "b", {}),
        N(KP|VP, "c", AR(KEYANCH, "anchor"), "3"),
        N(KP|VP, "d", {}),
        N(KP|VP, TS("!!str", "e"), "4"),
        N(KP|VP, "f", {}),
    })
);

}

} // namespace yml
} // namespace c4
