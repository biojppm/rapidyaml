#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define SIMPLE_SET_CASES                     \
"doc as set, implicit",                      \
"doc as set"                                 \
                                             \

CASE_GROUP(SIMPLE_SET)
{
    APPEND_CASES(

C("doc as set, implicit",
R"(!!set
? a
? b
)",
N(TL("!!set", L{N("a", "~"), N("b", "~")}))
),

C("doc as set",
R"(--- !!set
? aa
? bb
? cc
)",
N(STREAM, L{N(DOCMAP, TL("!!set", L{N("aa", "~"), N("bb", "~"), N("cc", "~")}))})
),

    )
}

INSTANTIATE_GROUP(SIMPLE_SET)

} // namespace yml
} // namespace c4
