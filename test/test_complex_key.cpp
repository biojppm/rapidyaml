#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define COMPLEX_KEY_CASES                       \
"complex key, ambiguity 2EBW",                  \
"complex key, ambiguity 2EBW, expl",            \
"complex key, ambiguity 2EBW, impl seq",        \
"complex key, ambiguity 2EBW, expl seq",        \
"complex key with line break in between",       \
"complex key 2nd, inside explicit map",         \
"complex key 1st, inside explicit map",         \
"complex key 2nd",                              \
"complex key 1st",                              \
"complex key nested in a map, 1st",             \
"complex key nested in a seq, 1st",             \
"complex block key, literal, clip",             \
"complex block key, literal, keep",             \
"complex block key, literal, strip",            \
"complex block key, folded, clip",              \
"complex block key, folded, keep",              \
"complex block key, folded, strip",             \
"complex key, missing val 7W2P ZWK4"            \


CASE_GROUP(COMPLEX_KEY)
{
    APPEND_CASES(

C("complex key, ambiguity 2EBW",
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

C("complex key, ambiguity 2EBW, expl",
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

C("complex key, ambiguity 2EBW, impl seq",
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

C("complex key, ambiguity 2EBW, expl seq",
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

C("complex key with line break in between",
R"(
? a complex key
: its value
)",
  L{N("a complex key", "its value")}
),

C("complex key 2nd, inside explicit map",
R"(
{
    a simple key: a value,
    ? a complex key: another value,
}
)",
  L{
      N("a simple key", "a value"),
      N("a complex key", "another value"),
  }
),

C("complex key 1st, inside explicit map",
R"(
{
    ? a complex key: another value,
    a simple key: a value,
}
)",
  L{
      N("a complex key", "another value"),
      N("a simple key", "a value"),
  }
),

C("complex key 2nd",
R"(
a simple key: a value
? a complex key: another value
)",
  L{
      N("a simple key", "a value"),
      N("a complex key", "another value"),
  }
),

C("complex key 1st",
R"(
? a complex key: another value
a simple key: a value
)",
  L{
      N("a complex key", "another value"),
      N("a simple key", "a value"),
  }
),

C("complex key nested in a map, 1st",
R"(
map:
  ? a complex key: another value
  a simple key: a value
? a complex key deindented: its value
)",
  L{
      N("map", L{
          N("a complex key", "another value"),
          N("a simple key", "a value"),
      }),
      N("a complex key deindented", "its value")
   }
),

C("complex key nested in a seq, 1st",
R"(
- ? a complex key: another value
  a simple key: a value
- ? another complex key: its value
)",
  L{
      N(L{
          N("a complex key", "another value"),
          N("a simple key", "a value"),
      }),
      N(L{N("another complex key", "its value")})
   }
),

C("complex block key, literal, clip",
R"(? |
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key\nthat has multiple lines\n", "and this is its value")
   }
),

C("complex block key, literal, keep",
R"(? |+
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key\nthat has multiple lines\n\n", "and this is its value")
   }
),

C("complex block key, literal, strip",
R"(? |-
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key\nthat has multiple lines", "and this is its value")
   }
),

C("complex block key, folded, clip",
R"(? >
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key that has multiple lines\n", "and this is its value")
   }
),

C("complex block key, folded, keep",
R"(? >+
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key that has multiple lines\n\n", "and this is its value")
   }
),

C("complex block key, folded, strip",
R"(? >-
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N(QK, "This is a key that has multiple lines", "and this is its value")
   }
),

C("complex key, missing val 7W2P ZWK4",
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

INSTANTIATE_GROUP(COMPLEX_KEY)

} // namespace yml
} // namespace c4
