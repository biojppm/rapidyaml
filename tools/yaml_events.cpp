#include <c4/yml/std/std.hpp>
#include <c4/yml/parse.hpp>
#include <test_suite/test_suite_events.hpp>
#include <c4/fs/fs.hpp>
#include <cstdio>

using namespace c4;
using namespace c4::yml;

std::string load_file(csubstr filename)
{
    std::string buf;
    if(filename == "-") // read from stdin
    {
        for(int c = std::getchar(); c != EOF; c = std::getchar())
        {
            buf.push_back((char)c);
            if(buf.size() == buf.capacity())
            {
                size_t cap = 2u * (buf.capacity() >= 128u ? buf.capacity() : 128u); // at least 256
                buf.reserve(cap);
            }
        }
    }
    else
    {
        C4_CHECK_MSG(fs::path_exists(filename.str), "cannot find file: %s (cwd=%s)", filename.str, fs::cwd<std::string>().c_str());
        fs::file_get_contents(filename.str, &buf);
    }
    return buf;
}


int main(int argc, const char *argv[])
{
    if(argc != 2)
    {
        std::printf(R"(usage:
%s -          # read from stdin
%s <file>     # read from file
)", argv[0], argv[0]);
        return 1;
    }
    Tree tree;
    csubstr filename = to_csubstr(argv[1]);
    std::string buf = load_file(filename);
    tree.reserve(to_substr(buf).count('\n'));
    parse_in_place(filename, to_substr(buf), &tree);
    std::string events;
    emit_events(&events, tree);
    std::fwrite(events.data(), 1, events.size(), stdout);
    return 0;
}
