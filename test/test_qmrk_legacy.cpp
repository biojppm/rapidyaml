#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

CASE_GROUP(QMRK)
{

ADD_CASE_TO_GROUP("explicit key, last value missing", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
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
                N(KP|VN, "a", {}),
                N(KP|VN, "b", {}),
                N(KN|VN, "", {})
            }),
        N(DOC|MB, TL("!!set", L{
                    N(KP|VN, "a", {}),
                    N(KP|VN, "b", {}),
                })),
        N(DOC|MB, TL("!!set", L{
                    N(KP|VN, "a", {}),
                    N(KP|VN, "b", {}),
                    N(KN|VN, "", {})
                })),
    })
);

ADD_CASE_TO_GROUP("explicit key, all values missing", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
?
?
?
)",
N(MB, L{
  N(KN|VN, "", {}),
  N(KN|VN, "", {}),
  N(KN|VN, "", {}),
})
);

ADD_CASE_TO_GROUP("explicit key, last value missing, end doc", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
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
                N(KP|VN, "a", {}),
                N(KP|VN, "b", {}),
                N(KN|VN, "", {})
            }),
        N(DOC|MB, TL("!!set", L{
                    N(KP|VN, "a", {}),
                    N(KP|VN, "b", {}),
                })),
        N(DOC|MB, TL("!!set", L{
                    N(KP|VN, "a", {}),
                    N(KP|VN, "b", {}),
                    N(KN|VN, "", {})
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
N(MFM, L{
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
N(MFM, L{
      N(KP|VP, "an explicit key", "another value"),
      N(KP|VP, "a simple key", "a value"),
  })
);

ADD_CASE_TO_GROUP("M2N8", HAS_CONTAINER_KEYS,
R"(
- ? : x
- ? : 
- ? :
)",
  Location(2, 5)
);

ADD_CASE_TO_GROUP("explicit key 2nd", HAS_CONTAINER_KEYS,
R"(
a simple key: a value
? an explicit key: another value
)",
  Location(3, 19+1)
);

ADD_CASE_TO_GROUP("explicit key 1st", HAS_CONTAINER_KEYS,
R"(
? an explicit key: another value
a simple key: a value
)",
  Location(2, 19+1)
);

ADD_CASE_TO_GROUP("explicit key nested in a map, 1st", HAS_CONTAINER_KEYS,
R"(
map:
  ? an explicit key: another value
  a simple key: a value
? an explicit key deindented: its value
)",
  Location(3, 21+1)
);

ADD_CASE_TO_GROUP("explicit key nested in a seq, 1st", HAS_CONTAINER_KEYS,
R"(
- ? an explicit key: another value
  a simple key: a value
- ? another explicit key: its value
)",
  Location(2, 21+1)
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

ADD_CASE_TO_GROUP("explicit key, missing val 7W2P", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
? a
? b
c:
? d
e:
)",
N(MB, L{
        N(KP|VN, "a", {}),
        N(KP|VN, "b", {}),
        N(KP|VN, "c", {}),
        N(KP|VN, "d", {}),
        N(KP|VN, "e", {}),
    })
);

ADD_CASE_TO_GROUP("explicit key, missing val ZWK4", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
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
        N(KP|VN, "b", {}),
        N(KP|VP, "c", AR(KEYANCH, "anchor"), "3"),
        N(KP|VN, "d", {}),
        N(KP|VP, TS("!!str", "e"), "4"),
        N(KP|VN, "f", {}),
    })
);

} // CASE_GROUP()

} // namespace yml
} // namespace c4
