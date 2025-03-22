#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/std/std.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/common.hpp>
#include <c4/yml/error.def.hpp>
#endif
#include <c4/fs/fs.hpp>

#include <cstdio>
#include <chrono>

#ifdef C4_EXCEPTIONS
#include <stdexcept>
#else
#include <csetjmp>
std::jmp_buf jmp_env = {};
c4::csubstr jmp_msg = {};
#endif


C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4996) // This function or variable may be unsafe


//-----------------------------------------------------------------------------

using namespace c4;

// LCOV_EXCL_START

struct Args
{
    c4::csubstr filename = "-";
    c4::csubstr output = {};
    c4::yml::id_type reserve_size = false;
    bool resolve_refs = false;
    bool keep_refs = false;
    bool print_tree = false;
    bool quiet = false;
    bool emit_as_json = false;
    bool emit_to_string = false;
    bool timed_sections = false;
};
void print_usage(const char *exename)
{
    const Args defs = {};
    fprintf(stderr, R"(usage:

   %s <options> <path/to/file.yaml>

Parse yaml from file (or stdin when file is `-`) and emit to stdout.

Options:

  -h,--help              print this message
  -e [N],--reserve [N]   reserve tree size before parsing (default: N=%d):
                         0=do not reserve
                         1=reserve by estimating size
                         all other values=reserve with value
  -r,--resolve           resolve references (default: %s)
  -k,--keep-refs         keep refs and anchors after resolving (default: %s)
  -p,--print-tree        print parsed rapidyaml tree before emitting (default: %s)
  -q,--quiet             do not emit (default: %s)
  -j,--json              emit json instead of yaml (default: %s)
  -s,--string            emit to string before dumping to stdout/file.
                         otherwise, emit directly to stdout (default: %s)
  -t,--timed             time sections (print timings to stderr) (default: %s)
  -o,--output <filename> emit to the given filename (default: %s)

)",
            exename,
            (int)defs.reserve_size,
            defs.resolve_refs ? "resolve refs" : "do not resolve refs",
            defs.keep_refs ? "keep refs" : "remove refs",
            defs.print_tree ? "print tree" : "do not print tree",
            defs.quiet ? "do not emit" : "emit",
            defs.emit_as_json ? "emit as json" : "emit as yaml",
            defs.emit_to_string ? "emit to string" : "no",
            defs.timed_sections ? "show timings" : "no",
            defs.output.empty() ? "no" : defs.output.str
        );
}
bool timing_enabled = false;
bool parse_args(int argc, const char *argv[], Args &args)
{
    args = {};
    if(argc < 2)
    {
        print_usage(argv[0]);
        _RYML_ERR_BASIC("missing filename (use - to read from stdin)");
    }
    args.filename = c4::to_csubstr(argv[argc - 1]);
    for(int i = 1; i+1 < argc; ++i)
    {
        c4::csubstr arg = c4::to_csubstr(argv[i]);
        auto arg0_is = [&](c4::csubstr argshort, c4::csubstr arglong){
            return (arg == argshort) || (arg == arglong);
        };
        auto arg1_is = [&](c4::csubstr argshort, c4::csubstr arglong){
            if(arg0_is(argshort, arglong))
            {
                if(i + 1 >= argc)
                    _RYML_ERR_BASIC("missing argument value: {}", arg);
                return true;
            }
            return false;
        };
        if /**/(arg1_is("-e", "--reserve")) C4_CHECK(c4::from_chars(c4::to_csubstr(argv[++i]), &args.reserve_size));
        else if(arg1_is("-r", "--resolve")) args.resolve_refs = true;
        else if(arg1_is("-o", "--output")) args.output = c4::to_csubstr(argv[++i]);
        else if(arg0_is("-k", "--keep-refs")) args.keep_refs = true;
        else if(arg0_is("-p", "--print-tree")) args.print_tree = true;
        else if(arg0_is("-q", "--quiet")) args.quiet = true;
        else if(arg0_is("-j", "--json")) args.emit_as_json = true;
        else if(arg0_is("-s", "--string")) args.emit_to_string = true;
        else if(arg0_is("-t", "--timed")) args.timed_sections = true;
        else if(arg0_is("-h", "--help"))
        {
            print_usage(argv[0]);
            return false;
        }
        else if(i+1 < argc)
        {
            print_usage(argv[0]);
            _RYML_ERR_BASIC("unknown argument: {}", arg);
        }
    }
    timing_enabled = args.timed_sections;
    if(argc < 2)
    {
        print_usage(argv[0]);
        _RYML_ERR_BASIC("missing filename (use - to read from stdin)");
    }
    args.filename = c4::to_csubstr(argv[argc - 1]);
    return true;
}

void read_file(csubstr filename, std::string *buf)
{
    buf->clear();
    if(filename == "-") // read from stdin
    {
        for(int c = std::getchar(); c != EOF; c = std::getchar())
        {
            buf->push_back((char)c);
            if(buf->size() == buf->capacity())
                buf->reserve(2u * (buf->capacity() >= 128u ? buf->capacity() : 128u));
        }
    }
    else
    {
        _RYML_CHECK_BASIC_MSG(fs::path_exists(filename.str), "file not found: {} (cwd={})", filename, fs::cwd<std::string>());
        fs::file_get_contents<std::string>(filename.str, buf);
    }
}

void emit_json_docs(yml::Tree const& tree, std::string *dst=nullptr)
{
    auto emitnode = [&](yml::ConstNodeRef node){
        if(dst)
        {
            emitrs_json(node, dst, /*append*/true);
            *dst += '\n';
        }
        else
        {
            emit_json(node, stdout);
            printf("\n");
        }
    };
    yml::ConstNodeRef root = tree.rootref();
    if(!root.is_stream())
        emitnode(root);
    else
        for(yml::ConstNodeRef doc : root.children())
            emitnode(doc);
}

void emit_json_docs(yml::Tree const& tree, FILE *output)
{
    auto emitnode = [&](yml::ConstNodeRef node){
        emit_json(node, output);
        (void)fputc('\n', output);
    };
    yml::ConstNodeRef root = tree.rootref();
    if(!root.is_stream())
        emitnode(root);
    else
        for(yml::ConstNodeRef doc : root.children())
            emitnode(doc);
}

void dump2stderr(csubstr s)
{
    if(s.len)
    {
        fwrite(s.str, 1, s.len, stderr); // NOLINT
        fflush(stderr); // NOLINT
    }
}

[[noreturn]] void throwerr(csubstr msg)
{
    C4_IF_EXCEPTIONS(
        throw std::runtime_error({msg.str, msg.len});
        ,
        jmp_msg.assign(msg.str, msg.len);
        std::longjmp(jmp_env, 1);
        );
    C4_UNREACHABLE_AFTER_ERR();
}

yml::Callbacks create_custom_callbacks()
{
    return yml::Callbacks{}
        .set_error_basic([](csubstr msg, yml::ErrorDataBasic const& errdata, void *){
            yml::err_basic_format(dump2stderr, msg, errdata);
            throwerr(msg);
        })
        .set_error_parse([](csubstr msg, yml::ErrorDataParse const& errdata, void *){
            yml::err_parse_format(dump2stderr, msg, errdata);
            throwerr(msg);
        })
        .set_error_visit([](csubstr msg, yml::ErrorDataVisit const& errdata, void *){
            yml::err_visit_format(dump2stderr, msg, errdata);
            throwerr(msg);
        });
}

#define TS(name) timed_section C4_XCAT(__, C4_XCAT(name, __LINE__))(#name)
struct timed_section
{
    using myclock = std::chrono::steady_clock;
    using msecs = std::chrono::duration<double, std::milli>;

    csubstr name;
    myclock::time_point start;

    msecs since() const { return myclock::now() - start; }
    timed_section(csubstr n)
        : name(n)
        , start(timing_enabled ? myclock::now() : myclock::time_point{})
    {}
    ~timed_section()
    {
        if(timing_enabled)
        {
            fprintf(stderr, "%.6fms: %.*s\n", since().count(), (int)name.len, name.str); // NOLINT
            fflush(stderr); // NOLINT
        }
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void process_file(Args const& args, std::string *contents)
{
    TS(objects);
    yml::Tree tree(yml::get_callbacks());
    {
        TS(read_file);
        read_file(args.filename, contents);
    }
    if(args.reserve_size)
    {
        TS(tree_reserve);
        yml::id_type cap = args.reserve_size;
        if(args.reserve_size)
        {
            TS(estimate_capacity);
            cap = yml::estimate_tree_capacity(to_csubstr(*contents));
        }
        if(args.timed_sections)
            fprintf(stderr, "reserving capacity=%zu\n", (size_t)cap); // NOLINT
        tree.reserve(cap);
    }
    {
        TS(parse_yml);
        yml::parse_in_place(args.filename, to_substr(*contents), &tree);
    }
    if(args.print_tree)
    {
        print_tree(args.filename.str, tree); // safe because we are getting from argv which is zero-terminated
    }
    if(args.resolve_refs || args.emit_as_json)
    {
        TS(resolve_refs);
        tree.resolve();
        if(args.print_tree)
        {
            print_tree("resolved tree", tree);
        }
    }
    if(args.emit_to_string)
    {
        std::string output;
        {
            TS(emit_to_buffer);
            output.resize(contents->size()); // resize, not just reserve
            if(!args.emit_as_json)
                yml::emitrs_yaml(tree, &output);
            else
                emit_json_docs(tree, &output);
        }
        if(!args.quiet)
        {
            TS(print_stdout);
            fwrite(output.data(), 1, output.size(), stdout); // NOLINT
        }
    }
    else if(!args.quiet)
    {
        if(args.output.empty())
        {
            TS(emit_to_stdout);
            if(!args.emit_as_json)
                yml::emit_yaml(tree);
            else
                emit_json_docs(tree);
        }
        else
        {
            FILE *output = fopen(args.output.str, "wb");
            if (!output)
                _RYML_ERR_BASIC("could not open file: {}", args.output.str);
            {
                TS(emit_to_file);
                if(!args.emit_as_json)
                    yml::emit_yaml(tree, output);
                else
                    emit_json_docs(tree, output);
            }
            (void)fclose(output);
        }
    }
}
// LCOV_EXCL_STOP

int main(int argc, const char *argv[])
{
    Args args;
    if(!parse_args(argc, argv, args))
       return 0;
    TS(TOTAL);
    set_callbacks(create_custom_callbacks());
    TS(TOTAL);
    std::string yaml;
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        process_file(args, &yaml);
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const& exc), else) // LCOV_EXCL_LINE
    {
        C4_IF_EXCEPTIONS(
            dump2stderr(to_csubstr(exc.what()));  // LCOV_EXCL_LINE
        ,
            dump2stderr(to_csubstr(jmp_msg));  // LCOV_EXCL_LINE
        );
        return 1; // LCOV_EXCL_LINE
    }
    return 0;
}

C4_SUPPRESS_WARNING_GCC_CLANG_POP
C4_SUPPRESS_WARNING_MSVC_POP
