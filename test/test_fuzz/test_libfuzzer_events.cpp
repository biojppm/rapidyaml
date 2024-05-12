#include "./test_fuzz_common.hpp"

std::string fuzz_subject(c4::csubstr src_)
{
    std::string src(src_.begin(), src_.end());
    c4::yml::EventHandlerYamlStd::EventSink sink = {};
    c4::yml::EventHandlerYamlStd handler(&sink, create_custom_callbacks());
    c4::yml::ParseEngine<c4::yml::EventHandlerYamlStd> parser(&handler);
    parser.parse_in_place_ev("input", c4::to_substr(src));
    return sink.result;
}
