#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define SIMPLE_DOC_CASES                                        \
    "single scalar, implicit doc",                              \
        "multi scalar, implicit doc",                           \
        "single scalar, explicit doc, implicit termination",    \
        "single scalar, explicit doc, explicit termination"

CASE_GROUP(SIMPLE_DOC)
{
    APPEND_CASES(
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
    );
}

INSTANTIATE_GROUP(SIMPLE_DOC);

} // namespace yml
} // namespace c4
