#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define EMPTY_FILE_CASES "empty0-nochars", "empty0-multiline"

CASE_GROUP(EMPTY_FILE)
{
    APPEND_CASES(
C("empty0-nochars",
"",
NOTYPE),

C("empty0-multiline", R"(



)", NOTYPE),
        );
}

INSTANTIATE_GROUP(EMPTY_FILE);

} // namespace yml
} // namespace c4
