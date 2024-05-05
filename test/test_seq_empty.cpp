#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

CASE_GROUP(EMPTY_SEQ)
{

ADD_CASE_TO_GROUP("empty seq, explicit",
"[]",
    SFS
);


ADD_CASE_TO_GROUP("empty seq, explicit, whitespace",
" []",
    SFS
);


ADD_CASE_TO_GROUP("empty seq, multiline",
R"([
]
)",
    SFS
);

ADD_CASE_TO_GROUP("empty seq, multilines",
R"([
# ksjdfkjhsdfkjhsdfkjh


]
)",
    SFS
);
}

} // namespace yml
} // namespace c4
