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

#ifdef C4_EXCEPTIONS
#include <stdexcept>
#else
#include <csetjmp>
std::jmp_buf jmp_env = {};
c4::csubstr jmp_msg = {};
#endif


//-----------------------------------------------------------------------------

const char usage[] = R"(usage:

ryml-yaml-events [-s|-t] [-]        # read from stdin (default)
ryml-yaml-events [-s|-t] <file>     # read from file

The option can be one of the following:

    -s   emit events from source: parse the YAML source,
         and directly emit events during the parse (ie, no
         ryml tree is created). This is the default behavior
         when the option is omitted.

    -t   events from tree: parse the YAML source, creating a
         ryml tree. Once the tree is completely created, emit
         the events from the created tree.

When the option is omitted, -s is assumed.


EXAMPLES:

$ ryml-yaml-events            # emit events direct from stdin
$ ryml-yaml-events -          # emit events direct from stdin
$ ryml-yaml-events -s -       # emit events direct from stdin

$ ryml-yaml-events -t         # parse stdin to tree, emit events from created tree
$ ryml-yaml-events -t -       # parse stdin to tree, emit events from created tree

$ ryml-yaml-events <file>     # emit events direct from file
$ ryml-yaml-events - <file>   # emit events direct from file
$ ryml-yaml-events -s <file>  # emit events direct from file

$ ryml-yaml-events -t <file>  # parse file to tree, emit events from created tree

)";

using namespace c4;
using namespace c4::yml;

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
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        std::string src = load_file(args.filename);
        const std::string events = args.events_from_tree ?
            emit_events_from_tree(args.filename, to_substr(src))
            :
            emit_events_direct(args.filename, to_substr(src));
        std::fwrite(events.data(), 1, events.size(), stdout);
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        return 1;
    }
    return 0;
}


//-----------------------------------------------------------------------------

std::string emit_events_from_tree(csubstr filename, substr filecontents)
{
    Tree tree(create_custom_callbacks());
    tree.reserve(estimate_tree_capacity(filecontents));
    parse_in_place(filename, filecontents, &tree);
    return emit_events_from_tree<std::string>(tree);
}

std::string emit_events_direct(csubstr filename, substr filecontents)
{
    EventHandlerYamlStd::EventSink sink = {};
    EventHandlerYamlStd handler(&sink, create_custom_callbacks());
    ParseEngine<EventHandlerYamlStd> parser(&handler);
    parser.parse_in_place_ev(filename, filecontents);
    csubstr result = sink;
    return std::string(result.str, result.len);
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
        else if(a == "-s")
            args->events_from_tree = false;
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
            buf.push_back(static_cast<char>(c));
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

