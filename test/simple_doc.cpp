#include "./test_group.hpp"

namespace c4 {
namespace yml {


#define SIMPLE_DOC_CASES                                \
    "empty docs",                                       \
    "empty docs, term",                                 \
    "single scalar, implicit doc",                      \
    "multi scalar, implicit doc",                       \
    "single scalar, explicit doc, implicit termination",\
    "single scalar, explicit doc, explicit termination",\
    "multi doc, seq-map",\
    "multi doc, seq-map, no term",\
    "multi doc, map-seq",\
    "multi doc, map-seq, no term",\
    "multi doc, impl seq-map",\
    "multi doc, impl seq-map, no term",\
    "multi doc, impl map-seq",\
    "multi doc, impl map-seq, no term"


CASE_GROUP(SIMPLE_DOC)
{
    APPEND_CASES(

C("empty docs",
R"(---
---
---
---
)",
    N(STREAM, L{N(DOC), N(DOC), N(DOC), N(DOC)})
),

C("empty docs, term",
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

C("single scalar, implicit doc",
R"(a scalar with some spaces inside
)",
    N(L{N("a scalar with some spaces inside")})
),

C("multi scalar, implicit doc",
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

C("single scalar, explicit doc, implicit termination",
R"(---
a scalar with some spaces inside
)",
    N(STREAM, L{N(DOCSEQ, L{N("a scalar with some spaces inside")})})
),

C("single scalar, explicit doc, explicit termination",
R"(---
a scalar with some spaces inside
...
)",
    N(STREAM, L{N(DOCSEQ, L{N("a scalar with some spaces inside")})})
),

C("multi doc, seq-map",
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

C("multi doc, seq-map, no term",
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

C("multi doc, map-seq",
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

C("multi doc, map-seq, no term",
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

C("multi doc, impl seq-map",
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

C("multi doc, impl seq-map, no term",
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

C("multi doc, impl map-seq",
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

C("multi doc, impl map-seq, no term",
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
