#pragma once
#ifndef TEST_FUZZ_SRLZ_HPP_
#define TEST_FUZZ_SRLZ_HPP_

#include "./test_fuzz_common.hpp"

namespace c4 {
namespace yml {

namespace {

template<class T>
inline bool fuzztest_serialize_cleanup(csubstr src, csubstr *filtered)
{
    if C4_IF_CONSTEXPR (std::is_arithmetic<T>::value)
    {
        src = src.triml(" \t\n\r+");
        if C4_IF_CONSTEXPR (std::is_unsigned<T>::value)
        {
            src = src.triml("-");
        }
    }
    *filtered = src;
    return true;
}


template<class T>
inline int fuzztest_serialize(std::string *workspace, csubstr src, T val={})
{
    bool ok = fuzztest_serialize_cleanup<T>(src, &src);
    if(!ok)
        return false;
    ok = scalar_deserialize(src, &val);
    if(ok)
    {
        size_t sz = 0;
    again:
        scalar_serialize(c4::to_substr(*workspace), val);
        if(sz > workspace->size())
        {
            workspace->resize(sz);
            goto again; // NOLINT
        }
    }
    return 0;
}
} // namespace

inline int fuzztest_serialize(uint32_t case_number, csubstr src)
{
    std::string workspace;
    C4_UNUSED(case_number);
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        _if_dbg(dbg_printf_("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
        fuzztest_serialize<uint8_t>(&workspace, src);
        fuzztest_serialize<uint16_t>(&workspace, src);
        fuzztest_serialize<uint32_t>(&workspace, src);
        fuzztest_serialize<uint64_t>(&workspace, src);
        fuzztest_serialize<int8_t>(&workspace, src);
        fuzztest_serialize<int16_t>(&workspace, src);
        fuzztest_serialize<int32_t>(&workspace, src);
        fuzztest_serialize<int64_t>(&workspace, src);
        fuzztest_serialize<float>(&workspace, src);
        fuzztest_serialize<double>(&workspace, src);
        fuzztest_serialize<std::string>(&workspace, src);
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        // if an exception leaks from here, it is likely because of a greedy noexcept
        _if_dbg(fprintf(stdout, "err\n"); fflush(NULL));
        return 1;
    }
    return 0;
}


} // namespace yml
} // namespace c4

#endif /* TEST_FUZZ_SRLZ_HPP_ */
