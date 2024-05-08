#include "./test_lib/test_group.hpp"
#include "test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {


CASE_GROUP(EMPTY_MAP)
{

ADD_CASE_TO_GROUP("empty map, explicit",
"{}",
    MFS
);


ADD_CASE_TO_GROUP("empty map, explicit, whitespace",
" {}",
    MFS
);


ADD_CASE_TO_GROUP("empty map, multiline",
R"({

}
)",
    MFS
);


ADD_CASE_TO_GROUP("empty map, multilines",
R"({
# ksjdfkjhsdfkjhsdfkjh


}
)",
    MFS
    );
}

} // namespace yml
} // namespace c4
