#include "./test_fuzz_common.hpp"
#include <atomic>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *str, size_t len)
{
    static std::atomic<uint32_t> case_number{0};
    c4::csubstr src = {reinterpret_cast<const char*>(str), len};
    return c4::yml::fuzztest_yaml_events(case_number++, src);
}
