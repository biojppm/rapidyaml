#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/std/std.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/event_handler_tree.hpp>
#include <c4/yml/parse_engine.def.hpp>
#endif
#include <test_suite/test_suite_events.hpp>
#include <test_suite/test_suite_event_handler.hpp>
#include <c4/fs/fs.hpp>
#include <cstdio>
#include <stdexcept>

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

using namespace c4;
using namespace c4::yml;


//-----------------------------------------------------------------------------

const char usage[] = R"(usage:

ryml-yaml-events [-s|-t] [-]        # read from stdin (default)
ryml-yaml-events [-s|-t] <file>     # read from file

The option can be one of the following:

    -s   events from source: parse the source, directly
         emitting events during the parse (ie, no ryml
         tree is created). This is the default behavior.

    -t   events from tree: parse the source, creating a
         ryml tree, and THEN emit the events from the
         created tree.

When the option is omitted, -s is assumed.


EXAMPLES:

$ ryml-yaml-events            # emit direct from stdin
$ ryml-yaml-events -          # emit direct from stdin
$ ryml-yaml-events -s -       # emit direct from stdin

$ ryml-yaml-events -t         # emit tree from stdin
$ ryml-yaml-events -t -       # emit tree from stdin

$ ryml-yaml-events <file>     # emit direct from file
$ ryml-yaml-events - <file>   # emit direct from file
$ ryml-yaml-events -s <file>  # emit direct from file

$ ryml-yaml-events -t <file>  # emit tree from stdin

)";

struct Args
{
    csubstr filename = "-";
    bool events_from_tree = false;
    static bool parse(Args *args, int argc, const char *argv[]);
};


//-----------------------------------------------------------------------------

std::string load_file(csubstr filename);
std::string emit_events_from_tree(csubstr filename, substr filecontents);
std::string emit_events_direct(csubstr filename, substr filecontents);
Callbacks create_custom_callbacks();


//-----------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    Args args = {};
    if(!Args::parse(&args, argc, argv))
        return 1;
    set_callbacks(create_custom_callbacks());
    try
    {
        std::string src = load_file(args.filename);
        const std::string events = args.events_from_tree ?
            emit_events_from_tree(args.filename, to_substr(src))
            :
            emit_events_direct(args.filename, to_substr(src));
        std::fwrite(events.data(), 1, events.size(), stdout);
    }
    catch(std::exception const&)
    {
        return 1;
    }
    return 0;
}


//-----------------------------------------------------------------------------

std::string emit_events_from_tree(csubstr filename, substr filecontents)
{
    Tree tree(create_custom_callbacks());
    tree.reserve(filecontents.count('\n'));
    parse_in_place(filename, filecontents, &tree);
    return emit_events_from_tree<std::string>(tree);
}

std::string emit_events_direct(csubstr filename, substr filecontents)
{
    using Handler = EventHandlerYamlStd;
    EventSink sink;
    Handler handler(&sink, create_custom_callbacks());
    ParseEngine<Handler> parser(&handler);
    parser.parse_in_place_ev(filename, filecontents);
    return sink.result;
}


//-----------------------------------------------------------------------------

bool Args::parse(Args *args, int argc, const char *argv[])
{
    if(argc > 3)
    {
        std::printf(usage);
        return false;
    }
    *args = {};
    if(argc == 3)
    {
        args->events_from_tree = (to_csubstr(argv[1]) == "-t");
        args->filename = to_csubstr(argv[2]);
    }
    else if(argc == 2)
    {
        csubstr a = to_csubstr(argv[1]);
        if(a == "-t")
            args->events_from_tree = true;
        else
            args->filename = a;
    }
    return true;
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
    if(!fs::path_exists(filename.str))
    {
        std::fprintf(stderr, "cannot find file: %s (cwd=%s)\n", filename.str, fs::cwd<std::string>().c_str());
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
    fprintf(f, "%.*s\n", (int)length, msg);
    fflush(f);
}

Callbacks create_custom_callbacks()
{
    Callbacks callbacks = {};
    callbacks.m_error = [](const char *msg, size_t msg_len, Location location, void *)
    {
        report_error(msg, msg_len, location, stderr);
        throw std::runtime_error({msg, msg_len});
    };
    return callbacks;
}

C4_SUPPRESS_WARNING_GCC_CLANG_POP
