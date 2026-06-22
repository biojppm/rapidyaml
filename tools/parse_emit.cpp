#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/std/std.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/file.hpp>
#include <c4/yml/common.hpp>
#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/parse_engine.hpp>
#include <c4/yml/extra/ints_utils.hpp>
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


C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")
C4_SUPPRESS_WARNING_CLANG("-Wdeprecated-declarations") // fopen is deprecated
C4_SUPPRESS_WARNING_MSVC(4996) // This function or variable may be unsafe


//-----------------------------------------------------------------------------

using namespace c4;


struct Args
{
    c4::csubstr filename = "-";
    c4::csubstr output = {};
    c4::yml::id_type reserve_size = {};
    bool resolve_refs = false;
    bool keep_refs = false;
    bool print_tree = false;
    bool quiet = false;
    bool emit_as_json = false;
    bool emit_to_string = false;
    bool ints_parser = false;
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
  -e [N],--reserve [N]   reserve before parsing (default: N=%d):
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
  -i,--ints              use the ints parser, and print the int events (default: %s)

)",
            c4::to_csubstr(exename).basename().str,
            (int)defs.reserve_size,
            defs.resolve_refs ? "resolve refs" : "do not resolve refs",
            defs.keep_refs ? "keep refs" : "remove refs",
            defs.print_tree ? "print tree" : "do not print tree",
            defs.quiet ? "do not emit" : "emit",
            defs.emit_as_json ? "emit as json" : "emit as yaml",
            defs.emit_to_string ? "emit to string" : "no",
            defs.timed_sections ? "show timings" : "no",
            defs.output.empty() ? "no" : defs.output.str,
            defs.ints_parser ? "ints parser" : "tree parser"
        );
}


// Note about coverage: lcov reports some lines as uncovered, even
// though they /are/ covered (verified through debugger, printing and
// use of std::exit). Not really sure why this is happening.
// Suppressing those lines; see below.


bool timing_enabled = false;
bool is_arg0(c4::csubstr arg, c4::csubstr argshort, c4::csubstr arglong) noexcept
{
    return (arg == argshort) || (arg == arglong);
}
bool is_arg1(c4::csubstr arg, c4::csubstr argshort, c4::csubstr arglong, int i, int argc)
{
    if(is_arg0(arg, argshort, arglong))
    {
        if(i + 2 >= argc)
            _RYML_ERR_BASIC("missing argument value: {}", arg); // LCOV_EXCL_LINE --- lcov fail!
        return true;
    }
    return false;
}
template<class T>
void read_arg(c4::csubstr arg, c4::csubstr arglong, T *var)
{
    if(!c4::from_chars(arg, var))
        _RYML_ERR_BASIC("{}: could not read '{}'", arglong, arg); // LCOV_EXCL_LINE --- lcov fail!
}
bool parse_args(int argc, const char *argv[], Args &args)
{
    args = {};
    args.filename = c4::to_csubstr(argv[argc - 1]);
    for(int i = 1; i < argc; ++i)
    {
        c4::csubstr arg = c4::to_csubstr(argv[i]);
        if /**/(is_arg1(arg, "-e", "--reserve"   , i, argc)) read_arg(argv[++i], "--reserve", &args.reserve_size);
        else if(is_arg1(arg, "-o", "--output"    , i, argc)) args.output = argv[++i];
        else if(is_arg0(arg, "-r", "--resolve"            )) args.resolve_refs = true;
        else if(is_arg0(arg, "-k", "--keep-refs"          )) args.keep_refs = true;
        else if(is_arg0(arg, "-p", "--print-tree"         )) args.print_tree = true;
        else if(is_arg0(arg, "-q", "--quiet"              )) args.quiet = true;
        else if(is_arg0(arg, "-j", "--json"               )) args.emit_as_json = true;
        else if(is_arg0(arg, "-s", "--string"             )) args.emit_to_string = true;
        else if(is_arg0(arg, "-t", "--timed"              )) args.timed_sections = true;
        else if(is_arg0(arg, "-i", "--ints"               )) args.ints_parser = true;
        else if(is_arg0(arg, "-h", "--help"               ))
        {
            print_usage(argv[0]); return false; // LCOV_EXCL_LINE --- lcov fail!
        }
        else if(i+1 < argc)
        {
            print_usage(argv[0]); _RYML_ERR_BASIC("unknown argument: {}", arg); // LCOV_EXCL_LINE --- lcov fail!
        }
    }
    if(argc < 2)
    {
        print_usage(argv[0]); _RYML_ERR_BASIC("missing filename (use - to read from stdin)"); // LCOV_EXCL_LINE --- lcov fail!
    }
    timing_enabled = args.timed_sections;
    args.filename = c4::to_csubstr(argv[argc - 1]);
    return true;
}

void read_file(csubstr filename, std::vector<char> *buf)
{
    buf->clear();
    if(filename == "-" || filename == "stdin") // read from stdin
    {
        yml::stdin_get_contents(buf); // LCOV_EXCL_LINE --- lcov fail!
    }
    else
    {
        if(!fs::path_exists(filename.str))
            _RYML_ERR_BASIC("file not found: {} (cwd={})", filename, fs::cwd<std::string>()); // LCOV_EXCL_LINE --- lcov fail!
        yml::file_get_contents(buf, filename.str);
    }
}

void dump2stderr(csubstr s)
{
    if(s.len)
    {
        fwrite(s.str, 1, s.len, stderr); // NOLINT
        fflush(stderr); // NOLINT
    }
}

void throwerr(csubstr msg)
{
    C4_IF_EXCEPTIONS(
        throw std::runtime_error({msg.str, msg.len});
        ,
        jmp_msg.assign(msg.str, msg.len);
        std::longjmp(jmp_env, 1);
        );
}

yml::Callbacks create_custom_callbacks()
{
    return yml::Callbacks{}
        .set_allocate([](size_t len, void* , void *){
            return malloc(len);
        })
        .set_free([](void* mem, size_t, void *){
            free(mem);
        })
        .set_error_basic([](csubstr msg, yml::ErrorDataBasic const& errdata, void *){
            yml::err_basic_format(dump2stderr, msg, errdata);
            throwerr(msg);
        })                          // LCOV_EXCL_LINE --- lcov fail!
        .set_error_parse([](csubstr msg, yml::ErrorDataParse const& errdata, void *){
            yml::err_parse_format(dump2stderr, msg, errdata);
            throwerr(msg);
        })                          // LCOV_EXCL_LINE --- lcov fail!
        .set_error_visit([](csubstr msg, yml::ErrorDataVisit const& errdata, void *){
            yml::err_visit_format(dump2stderr, msg, errdata);
            throwerr(msg);
        });
}


#define TS(name) timed_section C4_XCAT(__, C4_XCAT(name, __LINE__))(#name)
#define TSB(name, numbytes) timed_section C4_XCAT(__, C4_XCAT(name, __LINE__))(#name, numbytes)
struct timed_section
{
    using myclock = std::chrono::steady_clock;
    using msecs = std::chrono::duration<double, std::milli>;

    csubstr name;
    size_t num_bytes;
    myclock::time_point start;

    msecs since() const { return myclock::now() - start; }
    timed_section(csubstr name_, size_t num_bytes_=0)
        : name(name_)
        , num_bytes(num_bytes_)
        , start(timing_enabled ? myclock::now() : myclock::time_point{})
    {}
    ~timed_section()
    {
        if(timing_enabled)
        {
            const auto time = since().count();
            fprintf(stderr, "%.6fms: %.*s", time, (int)name.len, name.str); // NOLINT
            if(num_bytes)
                fprintf(stderr, " %zuB %.6fMB/s", num_bytes, ((double)num_bytes * 1.e-6) / (time * 1.e3)); // NOLINT
            fprintf(stderr, "\n"); //NOLINT
            fflush(stderr); // NOLINT
        }
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void process_file_tree(Args const& args, c4::substr contents)
{
    TSB(process_file_tree, contents.len);
    yml::Tree tree(yml::get_callbacks());
    if(args.reserve_size)
    {
        TSB(tree_reserve, contents.len);
        yml::id_type cap = args.reserve_size;
        if(args.reserve_size == 1)
        {
            TSB(estimate_capacity, contents.len);
            cap = yml::estimate_tree_capacity(to_csubstr(contents));
        }
        tree.reserve(cap);
        if(args.timed_sections)
            fprintf(stderr, "reserving capacity=%zu\n", (size_t)cap); // NOLINT
    }
    {
        TSB(parse_yml, contents.len);
        yml::parse_in_place(args.filename, to_substr(contents), &tree);
    }
    if(args.print_tree)
    {
        TSB(print_tree_original, contents.len);
        print_tree(args.filename.str, tree); // safe because we are getting from argv which is zero-terminated
    }
    if(args.resolve_refs || args.emit_as_json)
    {
        TSB(resolve_refs, contents.len);
        tree.resolve();
        if(args.print_tree)
        {
            TS(print_tree_resolved);
            print_tree("resolved tree", tree);
        }
    }
    if(args.emit_to_string)
    {
        std::string output;
        {
            TSB(emit_to_buffer, contents.len);
            output.resize(contents.size()); // resize, not just reserve
            if(!args.emit_as_json)
                yml::emitrs_yaml(tree, &output);
            else
                yml::emitrs_json(tree, &output);
        }
        if(!args.quiet)
        {
            TSB(print_buf_stdout, contents.len);
            fwrite(output.data(), 1, output.size(), stdout); // NOLINT
        }
    }
    else if(!args.quiet)
    {
        if(args.output.empty())
        {
            TSB(emit_to_stdout, contents.len);
            if(!args.emit_as_json)
                yml::emit_yaml(tree);
            else
                yml::emit_json(tree);
        }
        else
        {
            FILE *output = fopen(args.output.str, "wb"); // NOLINT
            if (!output)
                _RYML_ERR_BASIC("could not open file: {}", args.output.str);
            {
                TSB(emit_to_file, contents.len);
                if(!args.emit_as_json)
                    yml::emit_yaml(tree, output);
                else
                    yml::emit_json(tree, output);
            }
            (void)fclose(output);
        }
    }
}


void process_file_ints(Args const& args, c4::csubstr contents)
{
    TS(process_file_ints);
    using evt_type = yml::extra::ievt::DataType;
    using Handler = yml::extra::EventHandlerInts;
    using Parser = yml::ParseEngine<Handler>;
    Handler handler;
    Parser parser(&handler);
    std::vector<evt_type> events;
    std::vector<char> src;
    std::vector<char> arena;
    if(args.reserve_size)
    {
        TSB(reserve, contents.len);
        yml::id_type cap = args.reserve_size;
        if(args.reserve_size == 1)
        {
            TSB(estimate_size, contents.len);
            cap = (yml::id_type)yml::extra::estimate_events_ints_size(to_csubstr(contents));
        }
        events.reserve(cap);
        arena.resize(contents.len);
        if(args.timed_sections)
            fprintf(stderr, "reserving capacity=%zu\n", (size_t)cap); // NOLINT
    }
 again:
    {
        TS(reset);
        {
            TSB(copy_src, contents.len);
            src.assign(contents.begin(), contents.end());
        }
        {
            TS(handler);
            handler.reset(to_substr(src), to_substr(arena), events.data(), (evt_type)events.size());
        }
    }
    {
        TSB(parse_yml, contents.len);
        parser.parse_in_place_ev(args.filename, to_substr(src));
    }
    events.resize((size_t)handler.required_size_events());
    if(!handler.fits_buffers())
    {
        arena.resize(handler.required_size_arena());
        goto again; // NOLINT
    }
    if(!args.quiet || args.print_tree)
    {
        TS(print_events);
        yml::extra::events_ints_print(to_csubstr(src), to_csubstr(arena), events.data(), (evt_type)events.size());
    }
}


void process_file(Args const& args)
{
    std::vector<char> contents;
    {
        TS(read_file);
        read_file(args.filename, &contents);
    }
    if(args.ints_parser)
        process_file_ints(args, to_substr(contents));
    else
        process_file_tree(args, to_substr(contents));
}


int main(int argc, const char *argv[])
{
    set_callbacks(create_custom_callbacks());
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        Args args;
        if (!parse_args(argc, argv, args))
            return 0;
        {
            TS(TOTAL);
            process_file(args);
        }
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

C4_SUPPRESS_WARNING_POP
