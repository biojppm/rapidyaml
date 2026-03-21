#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

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
    SFM
);

ADD_CASE_TO_GROUP("empty seq, multilines",
R"([
# ksjdfkjhsdfkjhsdfkjh


]
)",
    SFM
);
}

} // namespace yml
} // namespace c4
