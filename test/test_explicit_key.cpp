#include "./test_group.hpp"

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


TEST(explicit_key, test_suite_M2N8)
{
    csubstr yaml = R"(
- ? : x
- ? : 
- ? :
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 3u);
        ASSERT_EQ(t[0].num_children(), 1u);
        EXPECT_EQ(t[0][0].key(), csubstr{});
        EXPECT_EQ(t[0][0].val(), "x");
        ASSERT_EQ(t[1].num_children(), 1u);
        EXPECT_EQ(t[1][0].key(), csubstr{});
        EXPECT_EQ(t[1][0].val(), csubstr{});
        ASSERT_EQ(t[2].num_children(), 1u);
        EXPECT_EQ(t[2][0].key(), csubstr{});
        EXPECT_EQ(t[2][0].val(), csubstr{});
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(EXPLICIT_KEY)
{
//
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
        N(DOCMAP, L{
                N(KEYVAL, "a", {}),
                N(KEYVAL, "b", {}),
                N(KEYVAL, "", {})
            }),
        N(DOCMAP, TL("!!set", L{
                    N(KEYVAL, "a", {}),
                    N(KEYVAL, "b", {}),
                })),
        N(DOCMAP, TL("!!set", L{
                    N(KEYVAL, "a", {}),
                    N(KEYVAL, "b", {}),
                    N(KEYVAL, "", {})
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
L{
  N("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~", "safe"),
  N("?foo", "safe question mark"),
  N(":foo", "safe colon"),
  N("-foo", "safe dash"),
  N("this is#not", "a comment"),
});

ADD_CASE_TO_GROUP("explicit key, ambiguity 2EBW, expl",
R"({
  a!"#$%&'()*+-./09:;<=>?@AZ[\]^_`az{|~: safe,
  ?foo: safe question mark,
  :foo: safe colon,
  -foo: safe dash,
  this is#not: a comment,
})",
L{
  N("a!\"#$%&'()*+-./09:;<=>?@AZ[\\]^_`az{|~", "safe"),
  N("?foo", "safe question mark"),
  N(":foo", "safe colon"),
  N("-foo", "safe dash"),
  N("this is#not", "a comment"),
});

ADD_CASE_TO_GROUP("explicit key, ambiguity 2EBW, impl seq",
R"(
- a!"#$%&'()*+,-./09:;<=>?@AZ[\]^_`az{|}~
- ?foo
- :foo
- -foo
- this is#not:a comment
)",
L{
  N("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"),
  N("?foo"),
  N(":foo"),
  N("-foo"),
  N("this is#not:a comment"),
});

ADD_CASE_TO_GROUP("explicit key, ambiguity 2EBW, expl seq",
R"([
  a!"#$%&'()*+-./09:;<=>?@AZ[\^_`az{|}~,
  ?foo,
  :foo,
  -foo,
  this is#not:a comment,
])",
L{
  N("a!\"#$%&'()*+-./09:;<=>?@AZ[\\^_`az{|}~"),
  N("?foo"),
  N(":foo"),
  N("-foo"),
  N("this is#not:a comment"),
});

ADD_CASE_TO_GROUP("explicit key with line break in between",
R"(
? an explicit key
: its value
)",
  L{N("an explicit key", "its value")}
);

ADD_CASE_TO_GROUP("explicit key 2nd, inside explicit map",
R"(
{
    a simple key: a value,
    ? an explicit key: another value,
}
)",
  L{
      N("a simple key", "a value"),
      N("an explicit key", "another value"),
  }
);

ADD_CASE_TO_GROUP("explicit key 1st, inside explicit map",
R"(
{
    ? an explicit key: another value,
    a simple key: a value,
}
)",
  L{
      N("an explicit key", "another value"),
      N("a simple key", "a value"),
  }
);

ADD_CASE_TO_GROUP("explicit key 2nd",
R"(
a simple key: a value
? an explicit key: another value
)",
  L{
      N("a simple key", "a value"),
      N("an explicit key", "another value"),
  }
);

ADD_CASE_TO_GROUP("explicit key 1st",
R"(
? an explicit key: another value
a simple key: a value
)",
  L{
      N("an explicit key", "another value"),
      N("a simple key", "a value"),
  }
);

ADD_CASE_TO_GROUP("explicit key nested in a map, 1st",
R"(
map:
  ? an explicit key: another value
  a simple key: a value
? an explicit key deindented: its value
)",
  L{
      N("map", L{
          N("an explicit key", "another value"),
          N("a simple key", "a value"),
      }),
      N("an explicit key deindented", "its value")
   }
);

ADD_CASE_TO_GROUP("explicit key nested in a seq, 1st",
R"(
- ? an explicit key: another value
  a simple key: a value
- ? another explicit key: its value
)",
  L{
      N(L{
          N("an explicit key", "another value"),
          N("a simple key", "a value"),
      }),
      N(L{N("another explicit key", "its value")})
   }
);

ADD_CASE_TO_GROUP("explicit block key, literal, clip",
R"(? |
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key\nthat has multiple lines\n", "and this is its value")
   }
);

ADD_CASE_TO_GROUP("explicit block key, literal, keep",
R"(? |+
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key\nthat has multiple lines\n\n", "and this is its value")
   }
);

ADD_CASE_TO_GROUP("explicit block key, literal, strip",
R"(? |-
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key\nthat has multiple lines", "and this is its value")
   }
);

ADD_CASE_TO_GROUP("explicit block key, folded, clip",
R"(? >
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key that has multiple lines\n", "and this is its value")
   }
);

ADD_CASE_TO_GROUP("explicit block key, folded, keep",
R"(? >+
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key that has multiple lines\n\n", "and this is its value")
   }
);

ADD_CASE_TO_GROUP("explicit block key, folded, strip",
R"(? >-
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key that has multiple lines", "and this is its value")
   }
);

ADD_CASE_TO_GROUP("explicit key, missing val 7W2P",
R"(
? a
? b
c:
? d
e:
)",
N(MAP, L{
        N(KEYVAL, "a", {}),
        N(KEYVAL, "b", {}),
        N(KEYVAL, "c", {}),
        N(KEYVAL, "d", {}),
        N(KEYVAL, "e", {}),
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
N(MAP, L{
        N("a", "1"),
        N(KEYVAL, "b", {}),
        N("c", AR(KEYANCH, "anchor"), "3"),
        N(KEYVAL, "d", {}),
        N(TS("!!str", "e"), "4"),
        N(KEYVAL, "f", {}),
    })
);

}

} // namespace yml
} // namespace c4
