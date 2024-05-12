#include "./test_fuzz_common.hpp"

std::string fuzz_subject(c4::csubstr src)
{
    c4::yml::Tree tree(create_custom_callbacks());
    parse_in_arena(src, &tree);
    std::string dst = c4::yml::emitrs_yaml<std::string>(tree);
    return dst;
}
