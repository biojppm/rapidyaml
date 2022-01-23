#include <c4/yml/std/std.hpp>
#include <c4/yml/parse.hpp>
#include <test_suite/test_suite_events.hpp>
#include <c4/fs/fs.hpp>
#include <cstdio>

using namespace c4;
using namespace c4::yml;

void usage(const char *exename);
std::string load_file(csubstr filename);
void report_error_impl(const char* msg, size_t length, Location loc, FILE *f);


int main(int argc, const char *argv[])
{
    if(argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    // do not abort on error
    Callbacks callbacks = {};
    int exit_status = 0;
    callbacks.m_user_data = &exit_status;
    callbacks.m_error = [](const char *msg, size_t msg_len, Location location, void *user_data)
    {
        report_error_impl(msg, msg_len, location, stderr);
        *(int*)user_data = 1;
    };

    Tree tree(callbacks);
    csubstr filename = to_csubstr(argv[1]);
    std::string buf = load_file(filename);
    std::string events;
    tree.reserve(to_substr(buf).count('\n'));
    parse_in_place(filename, to_substr(buf), &tree);
    if(exit_status)
        return exit_status;
    emit_events(&events, tree);
    if(exit_status)
        return exit_status;
    std::fwrite(events.data(), 1, events.size(), stdout);
    return 0;
}


//-----------------------------------------------------------------------------

void usage(const char *exename)
{
    std::printf(R"(usage:
%s -          # read from stdin
%s <file>     # read from file
)", exename, exename);
}


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

void report_error_impl(const char* msg, size_t length, Location loc, FILE *f)
{
    if(loc)
    {
        if(!loc.name.empty())
        {
            std::fwrite(loc.name.str, 1, loc.name.len, f);
            std::fputc(':', f);
        }
        std::fprintf(f, "%zu:", loc.line);
        if(loc.col)
            std::fprintf(f, "%zu:", loc.col);
        if(loc.offset)
            std::fprintf(f, " (%zuB):", loc.offset);
    }
    std::fprintf(f, "ERROR: %.*s\n", (int)length, msg);
    std::fflush(f);
}
