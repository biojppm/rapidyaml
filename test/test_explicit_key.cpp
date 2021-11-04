#include "./test_group.hpp"

namespace c4 {
namespace yml {


#ifdef TEST_SUITE_WIP
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
        ASSERT_NE(t.find_child(t.root_id(), "explicit key"), NONE);
        ASSERT_NE(t.find_child(t.root_id(), "block key\n"), NONE);
        EXPECT_EQ(t["explicit key"].val(), csubstr{});
        EXPECT_TRUE(t["block key\n"].is_seq());
        EXPECT_EQ(t["block key\n"][0], csubstr("one"));
        EXPECT_EQ(t["block key\n"][1], csubstr("two"));
    });
}


TEST(explicit_key, test_suite_DFF7)
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
        ASSERT_TRUE(t.rootref().has_child("explicit"));
        EXPECT_EQ(t["explicit"].val(), csubstr("empty"));
        ASSERT_TRUE(t.rootref().has_child("implicit"));
        EXPECT_EQ(t["explicit"].val(), csubstr("empty"));
        ASSERT_TRUE(t.rootref().has_child(csubstr{}));
        EXPECT_EQ(t[csubstr{}].val(), csubstr{});
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


TEST(explicit_key, test_suite_NJ66)
{
    csubstr yaml = R"(
- { single line: value}
- { multi
  line: value}
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 2);
        ASSERT_TRUE(t[0].has_child("single line"));
        EXPECT_EQ(t[0]["single line"].val(), csubstr("value"));
        ASSERT_TRUE(t[1].has_child("multi line"));
        EXPECT_EQ(t[1]["multi line"].val(), csubstr("value"));
    });
}
#endif


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define EXPLICIT_KEY_CASES                       \
"explicit key, ambiguity 2EBW",                  \
"explicit key, ambiguity 2EBW, expl",            \
"explicit key, ambiguity 2EBW, impl seq",        \
"explicit key, ambiguity 2EBW, expl seq",        \
"explicit key with line break in between",       \
"explicit key 2nd, inside explicit map",         \
"explicit key 1st, inside explicit map",         \
"explicit key 2nd",                              \
"explicit key 1st",                              \
"explicit key nested in a map, 1st",             \
"explicit key nested in a seq, 1st",             \
"explicit block key, literal, clip",             \
"explicit block key, literal, keep",             \
"explicit block key, literal, strip",            \
"explicit block key, folded, clip",              \
"explicit block key, folded, keep",              \
"explicit block key, folded, strip",             \
"explicit key, missing val 7W2P ZWK4"            \


CASE_GROUP(EXPLICIT_KEY)
{
    APPEND_CASES(

C("explicit key, ambiguity 2EBW",
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
}),

C("explicit key, ambiguity 2EBW, expl",
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
}),

C("explicit key, ambiguity 2EBW, impl seq",
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
}),

C("explicit key, ambiguity 2EBW, expl seq",
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
}),

C("explicit key with line break in between",
R"(
? an explicit key
: its value
)",
  L{N("an explicit key", "its value")}
),

C("explicit key 2nd, inside explicit map",
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
),

C("explicit key 1st, inside explicit map",
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
),

C("explicit key 2nd",
R"(
a simple key: a value
? an explicit key: another value
)",
  L{
      N("a simple key", "a value"),
      N("an explicit key", "another value"),
  }
),

C("explicit key 1st",
R"(
? an explicit key: another value
a simple key: a value
)",
  L{
      N("an explicit key", "another value"),
      N("a simple key", "a value"),
  }
),

C("explicit key nested in a map, 1st",
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
),

C("explicit key nested in a seq, 1st",
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
),

C("explicit block key, literal, clip",
R"(? |
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key\nthat has multiple lines\n", "and this is its value")
   }
),

C("explicit block key, literal, keep",
R"(? |+
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key\nthat has multiple lines\n\n", "and this is its value")
   }
),

C("explicit block key, literal, strip",
R"(? |-
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key\nthat has multiple lines", "and this is its value")
   }
),

C("explicit block key, folded, clip",
R"(? >
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key that has multiple lines\n", "and this is its value")
   }
),

C("explicit block key, folded, keep",
R"(? >+
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key that has multiple lines\n\n", "and this is its value")
   }
),

C("explicit block key, folded, strip",
R"(? >-
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key that has multiple lines", "and this is its value")
   }
),

C("explicit key, missing val 7W2P ZWK4",
R"(--- # 7W2P
? a
? b
c:
? d
e:
--- # ZWK4
a: 1
? b
&anchor c: 3
? d
!!str e: 4
? f
)",
  N(STREAM, L{
      N(DOCMAP, L{
              N(KEYVAL, "a", {}),
              N(KEYVAL, "b", {}),
              N(KEYVAL, "c", {}),
              N(KEYVAL, "d", {}),
              N(KEYVAL, "e", {}),
          }),
      N(DOCMAP, L{
              N("a", "1"),
              N(KEYVAL, "b", {}),
              N("c", AR(KEYANCH, "anchor"), "3"),
              N(KEYVAL, "d", {}),
              N(TS("!!str", "e"), "4"),
              N(KEYVAL, "f", {}),
          }),
  })
    )

  )
}

INSTANTIATE_GROUP(EXPLICIT_KEY)

} // namespace yml
} // namespace c4
