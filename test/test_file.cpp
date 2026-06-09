#ifndef RYML_SINGLE_HEADER
#include "c4/yml/file.hpp"
#endif
#include "./test_lib/test_case.hpp"
#include <gtest/gtest.h>
#include <c4/fs/fs.hpp>

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

template<class Container>
std::string mkfile(Container const& expected)
{
    std::string name = c4::fs::tmpnam<std::string>();
    file_put_contents(expected, name.c_str());
    return name;
}
template<class Container>
std::string mkfile2(Container const& expected)
{
    std::string name = c4::fs::tmpnam<std::string>();
    detail::ScopedFILE sf(name.c_str(), "wb");
    file_put_contents(expected, sf.file, name.c_str());
    return name;
}

template<class Container>
void test_roundtrip_reads(Container const& expected, std::string const& name)
{
    const char *filename = name.c_str();
    Container actual = file_get_contents<Container>(filename);
    EXPECT_EQ(actual.size(), expected.size());
    EXPECT_EQ(actual, expected);
    if(!actual.empty())
    {
        size_t num_bytes = actual.size() * sizeof(typename Container::value_type);
        memset(&actual[0], 0, num_bytes);
        size_t ret = file_get_contents(filename, nullptr, 0);
        EXPECT_EQ(ret, num_bytes);
        EXPECT_NE(actual, expected);
        ret = file_get_contents(filename, &actual[0], num_bytes);
        EXPECT_EQ(ret, num_bytes);
        EXPECT_EQ(actual, expected);
    }
    {
        detail::ScopedFILE f(filename, "rb");
        Container other = stdin_get_contents<Container>(f.file);
        EXPECT_EQ(other.size(), expected.size());
        EXPECT_EQ(other, expected);
    }
}
template<class Container>
void test_roundtrip(Container const& expected)
{
    {
        SCOPED_TRACE("mk1");
        std::string name = mkfile(expected);
        test_roundtrip_reads<Container>(expected, name);
        c4::fs::rmfile(name.c_str());
    }
    {
        SCOPED_TRACE("mk2");
        std::string name = mkfile2(expected);
        test_roundtrip_reads<Container>(expected, name);
        c4::fs::rmfile(name.c_str());
    }
}
template<class T>
void testvec()
{
    std::vector<T> vec(100);
    {
        T i = 0;
        for(T &v : vec)
            v = i++;
    }
    {
        SCOPED_TRACE("full");
        test_roundtrip(vec);
    }
    size_t sz = sizeof(T);
    if(sz > 1)
    {
        csubstr s = {(const char*)vec.data(), (sz * (vec.size() - 1)) + 1};
        RYML_TRACE_FMT("s.len={} sz={}", s.len, sz);
        ASSERT_NE(s.len % sz, 0);
        std::string name = mkfile(std::string(s.begin(), s.end()));
        ExpectError::check_error_basic([&]{
            std::vector<T> out = file_get_contents<std::vector<T>>(name.c_str());
        });
        {
            detail::ScopedFILE f(name.c_str(), "rb");
            ExpectError::check_error_basic([&]{
                std::vector<T> out = stdin_get_contents<std::vector<T>>(f.file);
            });
        }
    }
}

TEST(file, err_on_cannot_open)
{
    const char *non_existent_path = "dlkjsdf/sdfsdflkj/sdfjhsdfkjh/file.noluck";
    {
        SCOPED_TRACE("file_get_contents");
        ExpectError::check_error_basic([&]{
            (void)file_get_contents<std::string>(non_existent_path);
        });
    }
    {
        SCOPED_TRACE("file_get_contents");
        ExpectError::check_error_basic([&]{
            file_put_contents(std::string("asdkjhasdkjh"), non_existent_path);
        });
    }
}

TEST(file, empty_stdstring)
{
    std::string str;
    test_roundtrip(str);
}
TEST(file, empty_vecchar)
{
    std::vector<char> vec;
    test_roundtrip(vec);
}
TEST(file, empty_vecint)
{
    std::vector<int> vec;
    test_roundtrip(vec);
}
TEST(file, empty_vecsize_t)
{
    std::vector<size_t> vec;
    test_roundtrip(vec);
}

TEST(file, stdstring)
{
    std::string str = "asdasdijasdlkj";
    test_roundtrip(str);
}
TEST(file, vecchar)
{
    testvec<char>();
}
TEST(file, vecint)
{
    testvec<int>();
}
TEST(file, vecsize_t)
{
    testvec<size_t>();
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
