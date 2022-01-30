#include "./test_group.hpp"

namespace c4 {
namespace yml {

CASE_GROUP(EMPTY_SEQ)
{

ADD_CASE_TO_GROUP("empty seq, explicit",
"[]",
    SEQ
);


ADD_CASE_TO_GROUP("empty seq, explicit, whitespace",
" []",
    SEQ
);


ADD_CASE_TO_GROUP("empty seq, multiline",
R"([
]
)",
    SEQ
);

ADD_CASE_TO_GROUP("empty seq, multilines",
R"([
# ksjdfkjhsdfkjhsdfkjh


]
)",
    SEQ
);
}

} // namespace yml
} // namespace c4
