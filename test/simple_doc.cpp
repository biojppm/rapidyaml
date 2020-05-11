#include "./test_group.hpp"

namespace c4 {
namespace yml {


#define SIMPLE_DOC_CASES                                                \
    "simple doc, empty docs",                                           \
    "simple doc, empty docs, term",                                     \
    "simple doc, plain scalar, multiple docs",                          \
    "simple doc, single scalar, implicit doc",                          \
    "simple doc, multi scalar, implicit doc",                           \
    "simple doc, single scalar, explicit doc, implicit termination",    \
    "simple doc, single scalar, explicit doc, explicit termination",    \
    "simple doc, multi doc, seq-map",                                   \
    "simple doc, multi doc, seq-map, no term",                          \
    "simple doc, multi doc, map-seq",                                   \
    "simple doc, multi doc, map-seq, no term",                          \
    "simple doc, multi doc, impl seq-map",                              \
    "simple doc, multi doc, impl seq-map, no term",                     \
    "simple doc, multi doc, impl map-seq",                              \
    "simple doc, multi doc, impl map-seq, no term"


CASE_GROUP(SIMPLE_DOC)
{
    APPEND_CASES(

C("simple doc, empty docs",
R"(---
---
---
---
)",
    N(STREAM, L{N(DOC), N(DOC), N(DOC), N(DOC)})
),

C("simple doc, empty docs, term",
R"(---
...


---
...
---
...
---
...
)",
    N(STREAM, L{N(DOC), N(DOC), N(DOC), N(DOC)})
),

C("simple doc, plain scalar, multiple docs", HAS_PARSE_ERROR,
R"(---
- a plain scalar
    with several lines
...
- a second plain scalar
    with several lines
)",
  LineCol(5, 1)
),

C("simple doc, single scalar, implicit doc",
R"(a scalar with some spaces inside
)",
    N(L{N("a scalar with some spaces inside")})
),

C("simple doc, multi scalar, implicit doc",
R"(a scalar with some spaces inside,
and yet another one with more spaces inside,
and it doesn't really stop
)",
    N(L{
     N("a scalar with some spaces inside"),
     N("and yet another one with more spaces inside"),
     N("and it doesn't really stop"),
   })
),

C("simple doc, single scalar, explicit doc, implicit termination",
R"(---
a scalar with some spaces inside
)",
    N(STREAM, L{N(DOCSEQ, L{N("a scalar with some spaces inside")})})
),

C("simple doc, single scalar, explicit doc, explicit termination",
R"(---
a scalar with some spaces inside
...
)",
    N(STREAM, L{N(DOCSEQ, L{N("a scalar with some spaces inside")})})
),

C("simple doc, multi doc, seq-map",
R"(---
- a
- b
- c
...
---
a: 0
b: 1
c: 2
)",
    N(STREAM, L{
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
),

C("simple doc, multi doc, seq-map, no term",
R"(---
- a
- b
- c
---
a: 0
b: 1
c: 2
)",
    N(STREAM, L{
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
),

C("simple doc, multi doc, map-seq",
R"(---
a: 0
b: 1
c: 2
...
---
- a
- b
- c
...
)",
    N(STREAM, L{
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
),

C("simple doc, multi doc, map-seq, no term",
R"(---
a: 0
b: 1
c: 2
---
- a
- b
- c
)",
    N(STREAM, L{
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
),

C("simple doc, multi doc, impl seq-map",
R"(---
[a, b, c]
...
---
{a: 0, b: 1, c: 2}
...
)",
    N(STREAM, L{
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
),

C("simple doc, multi doc, impl seq-map, no term",
R"(---
[a, b, c]
---
{a: 0, b: 1, c: 2}
)",
    N(STREAM, L{
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
),

C("simple doc, multi doc, impl map-seq",
R"(---
{a: 0, b: 1, c: 2}
...
---
[a, b, c]
...
)",
    N(STREAM, L{
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
),

C("simple doc, multi doc, impl map-seq, no term",
R"(---
{a: 0, b: 1, c: 2}
---
[a, b, c]
)",
    N(STREAM, L{
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
),
    );
}

INSTANTIATE_GROUP(SIMPLE_DOC)

} // namespace yml
} // namespace c4
