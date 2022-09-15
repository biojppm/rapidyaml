#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/std/std.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#endif
#include <c4/fs/fs.hpp>

#include <cstdio>
#include <chrono>


using namespace c4;


//-----------------------------------------------------------------------------

struct timed_section
{
    using myclock = std::chrono::steady_clock;
    using msecs = std::chrono::duration<double, std::milli>;

    csubstr name;
    myclock::time_point start;

    msecs since() const { return myclock::now() - start; }
    timed_section(csubstr n) : name(n), start(myclock::now()) {}
    ~timed_section()
    {
        fprintf(stderr, "%.6fms: %.*s\n", since().count(), (int)name.len, name.str);
        fflush(stderr);
    }
};

#define TS(name) timed_section name##__##__LINE__(#name)


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    bool print_emitted_to_stdout = true;
    csubstr file;
    // LCOV_EXCL_START
    auto show_usage = [argv]{
        printf("usage: %s [-s] <path/to/file.yaml>\n", argv[0]);
    };
    if(argc == 2)
    {
        file = to_csubstr(argv[1]);
    }
    else if(argc > 2)
    {
        file = to_csubstr(argv[2]);
        csubstr arg = to_csubstr(argv[1]);
        if(arg == "-s")
        {
            print_emitted_to_stdout = false;
        }
        else
        {
            show_usage();
            return 1;
        }
    }
    else
    {
        show_usage();
        return 1;
    }
    // LCOV_EXCL_STOP

    TS(TOTAL);

    C4_CHECK_MSG(fs::path_exists(file.str), "cannot find file: %s (cwd=%s)", file.str, fs::cwd<std::string>().c_str());

    {
        TS(objects);
        std::string contents, output;
        yml::Tree tree;
        {
            TS(read_file);
            fs::file_get_contents(file.str, &contents);
        }
        {
            TS(tree_reserve);
            size_t nlines;
            {
                TS(count_lines);
                nlines = to_csubstr(contents).count('\n');
            }
            fprintf(stderr, "reserving #lines=%zu\n", nlines);
            tree.reserve(nlines);
        }
        {
            TS(parse_yml);
            yml::parse_in_place(file, to_substr(contents), &tree);
        }
        {
            TS(emit_to_buffer);
            output.resize(contents.size()); // resize, not just reserve
            yml::emitrs_yaml(tree, &output);
        }
        if(print_emitted_to_stdout)
        {
            TS(print_stdout);
            fwrite(output.data(), 1, output.size(), stdout);
            putchar('\n');
        }
    }

    return 0;
}
