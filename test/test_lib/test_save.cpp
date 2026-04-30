#ifdef RYML_SAVE_TEST_YAML

#include <c4/yml/common.hpp>
#include <c4/yml/error.hpp>
#include <c4/error.hpp>
#include <c4/format.hpp>
#include <c4/std/vector.hpp>
#include <c4/std/string.hpp>
#include <c4/fs/fs.hpp>
#include <gtest/gtest.h>
#include <map>
#include <string>

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

namespace c4 {
namespace yml {

static char savedir[] = "./yamldump\0";

struct savehelper
{
    std::vector<std::string> sources;
    std::vector<char> basename;
    std::vector<char> fullname;
    size_t indexpos;
    void reset_from_notest(csubstr filename)
    {
        char buf[1024];
        ssize_t ret = readlink("/proc/self/exe", buf, sizeof(buf)-1);
        _RYML_CHECK_BASIC(ret > 0);
        _RYML_CHECK_BASIC(ret < (int)sizeof(buf));
        csubstr exe = {buf, (size_t)ret};
        exe = exe.basename();
        static size_t count = 0;
        if(filename.empty())
            filename = "nofilename";
        filename = filename.basename();
        c4::formatrs(&basename, "{}_{}_{}", exe, filename, c4::fmt::zpad(count, 3));
        to_substr(basename).replace('/', '_');
        ++count;
        prepare_fullname();
    }
    void reset_from_gtest(testing::TestInfo const* curr)
    {
        csubstr filename = to_csubstr(curr->file());
        csubstr suitename = to_csubstr(curr->test_suite_name());
        csubstr testname = to_csubstr(curr->name());
        // handle group tests differently:
        if(filename.ends_with("test_group.def.hpp"))
        {
            filename = suitename;
            size_t pos = filename.find('/');
            _RYML_ASSERT_BASIC(pos != npos);
            filename = filename.first(pos);
            suitename = suitename.sub(pos + 1);
            if(suitename.begins_with("YmlTestCase"))
            {
                pos = testname.last_of('/');
                _RYML_ASSERT_BASIC(pos != npos);
                suitename = testname.sub(pos+1);
            }
        }
        size_t pos = filename.find("test/");
        if(pos != npos)
            filename = filename.sub(pos + 5);
        if(filename.ends_with(".cpp") || filename.ends_with(".hpp"))
            filename = filename.offs(0, 4);
        c4::formatrs(&basename, "{}_{}-{}_{}", filename, fmt::zpad(curr->line(), 3), suitename, testname);
        to_substr(basename).replace('/', '_');
        prepare_fullname();
    }
    void prepare_fullname()
    {
        printf("new test! %.*s\n", (int)basename.size(), &basename.front());
        c4::formatrs(&fullname, "{}/{}--", savedir, basename);
        indexpos = fullname.size();
        fullname.resize(fullname.size() + 32);
        sources.clear();
    }
};

static void save_impl(csubstr filename, csubstr extension, csubstr src)
{
    static savehelper h = {};
    static testing::TestInfo const* prev = nullptr;
    testing::TestInfo const* curr = testing::UnitTest::GetInstance()->current_test_info();
    if(prev == nullptr)
        c4::fs::mkdirs(savedir);
    if(curr != prev || !curr)
    {
        prev = curr;
        if(curr)
            h.reset_from_gtest(curr);
        else
            h.reset_from_notest(filename);
    }
    if(to_csubstr(h.basename).find("FilterTest_filter") != npos)
        return; // this case is not interesting, and very noisy
    // have we already saved this source?
    for(std::string const& s : h.sources)
        if(to_csubstr(s) == src)
            return;
    // we'll place the source in this position:
    size_t index = h.sources.size(); // it is also the index we'll use for the savename
    h.sources.emplace_back(src.begin(), src.end());
    // now form the savename
    substr buf = to_substr(h.fullname).sub(h.indexpos);
    _RYML_ASSERT_BASIC(buf.len > 0);
    size_t len = c4::cat(buf, c4::fmt::zpad(index, 3), extension, '\0');
    _RYML_ASSERT_BASIC(len < buf.len);
    csubstr savename = to_substr(h.fullname).first(h.indexpos + len);
    // done! dump the file
    printf("saving %.*s\n", (int)savename.len, savename.str);
    c4::fs::file_put_contents(savename.str, src);
}

void ryml_save_test_yaml(csubstr filename, csubstr src)
{
    save_impl(filename, ".yaml", src);
}
void ryml_save_test_json(csubstr filename, csubstr src)
{
    save_impl(filename, ".json", src);
}
} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif // RYML_SAVE_TEST_YAML
