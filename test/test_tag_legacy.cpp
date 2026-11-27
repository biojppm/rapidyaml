#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"
#include "./test_group/test_case.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

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

ADD_CASE_TO_GROUP("tagged doc 8", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
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

ADD_CASE_TO_GROUP("tagged doc 9", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
--- !!set
? a
? b
)",
N(STREAM, L{
    N(DOC|MB, TL("!!set", L{N(KP|VN, "a", /*"~"*/{}), N(KP|VN, "b", /*"~"*/{})})),
})
);

ADD_CASE_TO_GROUP("ambiguous tag in map, std tag", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
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

ADD_CASE_TO_GROUP("ambiguous tag in map, usr tag", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
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


ADD_CASE_TO_GROUP("ambiguous tag in seq, std tag", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
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

ADD_CASE_TO_GROUP("ambiguous tag in seq, usr tag", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
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

} // CASE_GROUP()

} // namespace yml
} // namespace c4
