#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/std/std.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/event_handler_tree.hpp>
#include <c4/yml/parse_engine.def.hpp>
#endif
#include <c4/yml/extra/string.hpp>
#include <c4/yml/extra/scalar.hpp>
#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/event_handler_testsuite.hpp>
#include <c4/yml/extra/ints_utils.hpp>
#include <c4/yml/extra/ints_to_testsuite.hpp>
#include <testsuite/testsuite_events.hpp>
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

C4_SUPPRESS_WARNING_GCC("-Wold-style-cast")


//-----------------------------------------------------------------------------

using namespace c4;
using namespace c4::yml;

enum class evts_type
{
    testsuite_src,
    testsuite_ints,
    testsuite_tree,
    ryml_ints,
};

struct Args
{
    csubstr filename = "-";
    evts_type evts = evts_type::testsuite_src;
    int ints_size = -1; // estimate by default
    bool ints_size_force = false; // do not force the estimated size
    static bool parse(Args *args, int argc, const char *argv[], int *errcode);
};

const char usage[] = R"(usage:

ryml-yaml-events <command> <options> [-]        # read from stdin (default)
ryml-yaml-events <command> <options> <file>     # read from file

The command must be one of the following:

    testsuite_src,ts_src,tss
         emit test suite events directly from source: parse the YAML
         source, and directly emit events during the parse (ie, no
         ryml tree is created). This is the default behavior when the
         option is omitted.

    testsuite_tree,ts_tree,tst
         emit test suite events from tree: parse the YAML source,
         creating a ryml tree. Once the tree is completely created,
         emit the test suite events by iterating over the nodes of the
         created tree.

    testsuite_ints,ts_ints,tsi
         emit test suite events from the ryml int events handler:
         parse the YAML source to a container of ryml int events. Once
         this is completed, emit the corresponding test suite events.

    ryml_ints,ri
         emit ryml int events: parse the YAML source to a container of
         ryml int events. Once this is completed, print those same int
         events.

The following options influence the behavior of the program:

    --timings,--timing,-t
         print task timings and size information (to stderr)

The following options influence the behavior of testsuite_ints and ryml_ints:

    --ints-size <int-number>,-is <int-number>
         when using int events, set the int event buffer size from this
         value. use a negative value to force a conservative estimation
         from a first run over the YAML source, and then multiply the
         estimation by the symmetrical of that value. For example, -2
         will result in 2*estimation. Default is -1.

    --ints-size-force,-isf
         when using int events, force the initial int event buffer size
         to prevail: if this size is not large enough to accomodate the
         actual number of events required from the YAML source, exit
         with a nonzero error code. This is in contrast to the default
         behavior, which consists of expanding the buffer as needed,
         which requires two parses and two string copies of the
         original source buffer.

EXAMPLES:

  $ ryml-yaml-events ts_src            # parse stdin to test suite events, then print the events
  $ ryml-yaml-events ts_src -          # parse stdin to test suite events, then print the events
  $ ryml-yaml-events ts_src <file>     # parse file to test suite events, then print the events

  $ ryml-yaml-events ts_tree           # parse stdin to ryml tree, emit test suite events from created tree
  $ ryml-yaml-events ts_tree -         # parse stdin to ryml tree, emit test suite events from created tree
  $ ryml-yaml-events ts_tree <file>    # parse file to ryml tree, emit test suite events from created tree

  $ ryml-yaml-events ts_ints           # parse stdin to ryml int events, emit test suite events from ryml int events
  $ ryml-yaml-events ts_ints -         # parse stdin to ryml int events, emit test suite events from ryml int events
  $ ryml-yaml-events ts_ints <file>    # parse file to ryml int events, emit test suite events from ryml int events

  $ ryml-yaml-events ryml_ints         # parse stdin to ryml int events, emit ryml int events
  $ ryml-yaml-events ryml_ints -       # parse stdin to ryml int events, emit ryml int events
  $ ryml-yaml-events ryml_ints <file>  # parse file to ryml int events, emit ryml int events

)";


//-----------------------------------------------------------------------------

using IntEvents = std::vector<extra::ievt::DataType>;

std::string load_file(csubstr filename);
extra::string emit_testsuite_events(csubstr filename, substr filecontents);
std::string emit_testsuite_events_from_tree(csubstr filename, substr filecontents);
std::string emit_testsuite_events_from_ints(csubstr filename, substr filecontents, IntEvents &evts, bool fail_size);
void emit_ints_events(csubstr filename, substr filecontents, IntEvents &evts, bool fail_size);
int estimate_ints_size(csubstr filecontents, int size);
Callbacks create_custom_callbacks();


bool timing_enabled = false;
double src_size = 0;
namespace stdc = std::chrono;
struct stopwatch
{
    using clock_type = stdc::steady_clock;
    const char *name;
    clock_type::time_point start;
    stopwatch(const char *name_)
    {
        if(!timing_enabled) return;
        name = name_;
        stack.emplace_back(this);
        start = clock_type::now();
    }
    ~stopwatch()
    {
        if(!timing_enabled) return;
        stdc::duration<double, std::milli> delta = clock_type::now() - start;
        for(stopwatch const* sw : stack)
            fprintf(stderr, "%s:", sw->name);
        fprintf(stderr, " %.6fms (%.3fMB/s)\n", delta.count(), src_size / delta.count() * 1.e-3);
        stack.resize(stack.size()-1);
    }
    static std::vector<stopwatch*> stack;

};
std::vector<stopwatch*> stopwatch::stack;
#define STOPWATCH(name) stopwatch C4_XCAT(timer, __LINE__){name}


//-----------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    Args args = {};
    {
        int errcode = 0;
        if(!Args::parse(&args, argc, argv, &errcode))
            return errcode;
    }
    set_callbacks(create_custom_callbacks());
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        std::string src;
        {
            STOPWATCH("load_file");
            src = load_file(args.filename);
            src_size = (double)src.size();
            if(timing_enabled) fprintf(stderr, "src_size=%zuB\n", src.size());
        }
        STOPWATCH("process");
        switch(args.evts)
        {
        case evts_type::testsuite_src:
        {
            extra::string evts;
            {
                STOPWATCH("testsuite_src");
                evts = emit_testsuite_events(args.filename, to_substr(src));
            }
            {
                STOPWATCH("print");
                std::fwrite(evts.data(), 1, evts.size(), stdout);
            }
            break;
        }
        case evts_type::testsuite_tree:
        {
            std::string evts;
            {
                STOPWATCH("testsuite_tree");
                evts = emit_testsuite_events_from_tree(args.filename, to_substr(src));
            }
            {
                STOPWATCH("print");
                std::fwrite(evts.data(), 1, evts.size(), stdout);
            }
            break;
        }
        case evts_type::testsuite_ints:
        {
            substr ssrc = to_substr(src);
            int estimated_size = estimate_ints_size(ssrc, args.ints_size);
            IntEvents int_evts((size_t)estimated_size);
            std::string ts_evts;
            {
                STOPWATCH("testsuite_ints");
                ts_evts = emit_testsuite_events_from_ints(args.filename, to_substr(src), int_evts, args.ints_size_force);
            }
            {
                STOPWATCH("print");
                std::fwrite(ts_evts.data(), 1, ts_evts.size(), stdout);
            }
            break;
        }
        case evts_type::ryml_ints:
        {
            substr ssrc = to_substr(src);
            int estimated_size = estimate_ints_size(ssrc, args.ints_size);
            IntEvents int_evts((size_t)estimated_size);
            {
                STOPWATCH("ryml_ints");
                emit_ints_events(args.filename, to_substr(src), int_evts, args.ints_size_force);
            }
            break;
        }
        }
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        return 1;
    }
    return 0;
}


//-----------------------------------------------------------------------------

std::string emit_testsuite_events_from_tree(csubstr filename, substr filecontents)
{
    Tree tree(create_custom_callbacks());
    {
        STOPWATCH("tree_reserve");
        tree.reserve(estimate_tree_capacity(filecontents));
    }
    {
        STOPWATCH("parse");
        parse_in_place(filename, filecontents, &tree);
    }
    {
        STOPWATCH("emit_events");
        std::string result = emit_events_from_tree<std::string>(tree);
        return result;
    }
}

extra::string emit_testsuite_events(csubstr filename, substr filecontents)
{
    extra::EventHandlerTestSuite::EventSink sink = {};
    extra::EventHandlerTestSuite handler(&sink, create_custom_callbacks());
    ParseEngine<extra::EventHandlerTestSuite> parser(&handler);
    {
        STOPWATCH("parse");
        parser.parse_in_place_ev(filename, filecontents);
    }
    return sink;
}

csubstr parse_events_ints(csubstr filename, substr filecontents, std::string &parsed, std::string &arena, IntEvents &evts, bool fail_size)
{
    using I = extra::ievt::DataType;
    using Handler = extra::EventHandlerInts;
    Handler handler(create_custom_callbacks());
    ParseEngine<Handler> parser(&handler);
    substr src = filecontents;
    if(!fail_size)
    {
        STOPWATCH("copy_src");
        parsed.assign(filecontents.str, filecontents.len);
        src = to_substr(parsed);
    }
    arena.resize(src.len);
    handler.reset(src, to_substr(arena), evts.data(), (I)evts.size());
    {
        STOPWATCH("parse");
        parser.parse_in_place_ev(filename, src);
    }
    size_t sz = (size_t)handler.required_size_events();
    if(timing_enabled) fprintf(stderr, "current_size=%zu vs needed_size=%zu. arena_size=%zu\n", evts.size(), sz, arena.size());
    if (!handler.fits_buffers())
    {
        RYML_CHECK(!fail_size);
        {
            STOPWATCH("resize");
            evts.resize(sz);
            arena.resize(handler.required_size_arena());
        }
        {
            STOPWATCH("redo_copy_src");
            parsed.assign(filecontents.str, filecontents.len);
            src = to_substr(parsed);
        }
        handler.reset(src, to_substr(arena), evts.data(), (I)evts.size());
        {
            STOPWATCH("redo_parse");
            parser.parse_in_place_ev(filename, src);
        }
        RYML_CHECK((size_t)handler.m_evt_pos == sz);
    }
    evts.resize(sz);
    return src;
}

std::string emit_testsuite_events_from_ints(csubstr filename, substr filecontents, IntEvents &evts, bool fail_size)
{
    using I = extra::ievt::DataType;
    std::string buf;
    std::string arena;
    csubstr parsed;
    {
        STOPWATCH("events");
        parsed = parse_events_ints(filename, filecontents, buf, arena, evts, fail_size);
    }
    std::string result;
    {
        STOPWATCH("emit");
        extra::events_ints_to_testsuite(parsed, to_substr(arena), evts.data(), (I)evts.size(), &result);
    }
    return result;
}

void emit_ints_events(csubstr filename, substr filecontents, IntEvents &evts, bool fail_size)
{
    using I = extra::ievt::DataType;
    std::string buf;
    std::string arena;
    csubstr parsed;
    {
        STOPWATCH("events");
        parsed = parse_events_ints(filename, filecontents, buf, arena, evts, fail_size);
    }
    {
        STOPWATCH("emit");
        extra::events_ints_print(parsed, to_substr(arena), evts.data(), (I)evts.size());
    }
}

int estimate_ints_size(csubstr filecontents, int size)
{
    if(size < 0)
    {
        STOPWATCH("estimate_size");
        int est = extra::estimate_events_ints_size(filecontents);
        if(timing_enabled) fprintf(stderr, "estimated_size=%d*%d=%d\n", -size, est, -size * est);
        size = -size * est;
    }
    return size;
}


//-----------------------------------------------------------------------------

// return true if the program should continue (eg -h should exit)
bool Args::parse(Args *args, int argc, const char *argv[], int *errcode)
{
    *errcode = 0;
    auto argerr = [&](const char *msg){
        std::printf(usage);
        std::printf("ERROR: %s\n", msg);
        *errcode = 1;
        return false;
    };
    // set defaults
    *args = {};
    // parse the command argument
    if(argc < 2)
        return argerr("must provide a command");  // LCOV_EXCL_LINE
    bool has_cmd = false;
    {
        csubstr s = to_csubstr(argv[1]);
        if(s == "testsuite_src" || s == "ts_src" || s == "tss")
        {
            args->evts = evts_type::testsuite_src;
            has_cmd = true;
        }
        else if(s == "testsuite_tree" || s == "ts_tree" || s == "tst")
        {
            args->evts = evts_type::testsuite_tree;
            has_cmd = true;
        }
        else if(s == "testsuite_ints" || s == "ts_ints" || s == "tsi")
        {
            args->evts = evts_type::testsuite_ints;
            has_cmd = true;
        }
        else if(s == "ryml_ints" || s == "ri")
        {
            args->evts = evts_type::ryml_ints;
            has_cmd = true;
        }
        else if(s == "--help" || s == "-h")
        {
            std::printf(usage);
            return false;
        }
    }
    if(!has_cmd)
        return argerr("unknown command");
    // parse other args
    int i = 2; // cmd is mandatory at i=1, so start after that at i=2
    for(; i < argc; ++i)
    {
        csubstr arg = to_csubstr(argv[i]);
        if(arg == "--help" || arg == "-h")
        {
            std::printf(usage);
            return false;
        }
        else if(arg == "--timings" || arg == "--timing" || arg == "-t")
        {
            timing_enabled = true;
        }
        else if(arg == "--ints-size-force" || arg == "-isf")
        {
            args->ints_size_force = true;
        }
        else if(arg == "--ints-size" || arg == "-is")
        {
            if(i + 1 >= argc)
                return argerr("ints-size value not given");  // LCOV_EXCL_LINE
            else if(!atoi(to_csubstr(argv[i+1]), &args->ints_size))
                return argerr("ints-size value fails to parse");  // LCOV_EXCL_LINE
            ++i; // shift 1
        }
        else
        {
            args->filename = arg;
        }
    }
    return true;
}

std::string load_file(csubstr filename)
{
    if(filename == "-") // read from stdin
    {   // LCOV_EXCL_START
        std::string buf;
        buf.reserve(128);
        for(int c = std::getchar(); c != EOF; c = std::getchar())
            buf.push_back(static_cast<char>(c));
        return buf;    // LCOV_EXCL_STOP
    }
    else if(!fs::path_exists(filename.str))
    {
        std::fprintf(stderr, "%s: file not found (cwd=%s)\n", filename.str, fs::cwd<std::string>().c_str());
        error("file not found");
    }
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
    fprintf(f, "%.*s\n", static_cast<int>(length), msg);
    fflush(f);
}

Callbacks create_custom_callbacks()
{
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
    return callbacks;
}
