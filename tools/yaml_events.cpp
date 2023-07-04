#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/std/std.hpp>
#include <c4/yml/parse.hpp>
#endif
#include <test_suite/test_suite_events.hpp>
#include <c4/fs/fs.hpp>
#include <cstdio>
#ifdef C4_EXCEPTIONS
#include <stdexcept>
#else
#include <csetjmp>
std::jmp_buf jmp_env = {};
c4::csubstr jmp_msg = {};
#endif

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

using namespace c4;
using namespace c4::yml;

void usage(const char *exename);
std::string load_file(csubstr filename);
void report_error(const char* msg, size_t length, Location loc, FILE *f);


int main(int argc, const char *argv[])
{
    if(argc < 2)
    {
        usage(argv[0]);
        return 1;
    }
    Callbacks callbacks = {};
    callbacks.m_error = [](const char *msg, size_t msg_len, Location location, void *)
    {
        report_error(msg, msg_len, location, stderr);
        C4_IF_EXCEPTIONS(
            throw std::runtime_error({msg, msg_len});
            ,
            jmp_msg.assign(msg, msg_len);
            std::longjmp(jmp_env, 1);
        );
    };
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        Tree tree(callbacks);
        csubstr filename = to_csubstr(argv[1]);
        std::string evt, src = load_file(filename);
        tree.reserve(to_substr(src).count('\n'));
        parse_in_place(filename, to_substr(src), &tree);
        emit_events(&evt, tree);
        std::fwrite(evt.data(), 1, evt.size(), stdout);
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        return 1;
    }
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
    if(filename == "-") // read from stdin
    {
        std::string buf;
        for(int c = std::getchar(); c != EOF; c = std::getchar())
        {
            buf.push_back((char)c);
            if(buf.size() == buf.capacity())
                buf.reserve(2u * (buf.capacity() >= 128u ? buf.capacity() : 128u));
        }
        return buf;
    }
    C4_CHECK_MSG(fs::path_exists(filename.str), "cannot find file: %s (cwd=%s)", filename.str, fs::cwd<std::string>().c_str());
    return fs::file_get_contents<std::string>(filename.str);
}

void report_error(const char* msg, size_t length, Location loc, FILE *f)
{
    if(!loc.name.empty())
    {
        fwrite(loc.name.str, 1, loc.name.len, f);
        fputc(':', f);
    }
    fprintf(f, "%zu:", loc.line);
    if(loc.col)
        fprintf(f, "%zu:", loc.col);
    if(loc.offset)
        fprintf(f, " (%zuB):", loc.offset);
    fputc(' ', f);
    fprintf(f, "%.*s\n", (int)length, msg);
    fflush(f);
}

C4_SUPPRESS_WARNING_GCC_CLANG_POP
