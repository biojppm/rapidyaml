#include "./test_group.hpp"

namespace c4 {
namespace yml {


CASE_GROUP(EMPTY_MAP)
{

ADD_CASE_TO_GROUP("empty map, explicit",
"{}",
    MAP
);


ADD_CASE_TO_GROUP("empty map, explicit, whitespace",
" {}",
    MAP
);


ADD_CASE_TO_GROUP("empty map, multiline",
R"({

}
)",
    MAP
);


ADD_CASE_TO_GROUP("empty map, multilines",
R"({
# ksjdfkjhsdfkjhsdfkjh


}
)",
    MAP
    );
}

} // namespace yml
} // namespace c4
