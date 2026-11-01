// This file shows a quick example of parsing YAML to an int events
// buffer. Since this functionality is meant to implement in other
// programming languages, the code is kept very simple, and using only
// C-like idioms (other than std::vector).


// ryml can be used as a single header, or as a simple library:
#if defined(RYML_SINGLE_HEADER) // using the single header directly in the executable
    #define RYML_SINGLE_HDR_DEFINE_NOW
    #include <ryml_ints.hpp>
#elif defined(RYML_SINGLE_HEADER_LIB) // using the single header from a library
    #include <ryml_ints.hpp>
#else
#include <c4/yml/parse_engine.def.hpp>
#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/ints_utils.hpp> // estimate size, print
#include <c4/std/vector.hpp>
#include <c4/yml/file.hpp>
#endif

#include <vector>

using namespace c4::yml::extra::ievt;


// holds the buffers necessary for getting the int events
struct IntsResult
{
    std::vector<char> yaml = {}; ///< source buffer
    std::vector<char> arena = {}; ///< arena to place out-of-source filtered scalars or tags
    std::vector<evt_bits> events = {}; ///< result buffer with events
    void reset_yaml(c4::csubstr yaml_)
    {
        yaml.assign(yaml_.begin(), yaml_.end());
    }
    void resize_buffers(c4::yml::extra::evt_size esz, size_t arena_size)
    {
        events.resize(static_cast<size_t>(esz));
        arena.resize(arena_size);
    }
    void resize_buffers(c4::yml::extra::EventHandlerInts const& handler)
    {
        resize_buffers(handler.required_size_events(), handler.required_size_arena());
    }
};


// a helper to parse the YAML
struct IntsParser
{
    c4::yml::extra::EventHandlerInts handler;
    c4::yml::ParseEngine<c4::yml::extra::EventHandlerInts> parser;

    IntsParser() noexcept: handler(), parser(&handler) {}

    bool parse_in_place(const char *filename, c4::substr yaml, IntsResult *result)
    {
        handler.reset(yaml, c4::to_substr(result->arena),
                      result->events.data(), static_cast<c4::yml::extra::evt_size>(result->events.size()));
        parser.parse_in_place_ev(filename, yaml); // we're parsing in place on the original YAML
        // the YAML was successfully parsed, but it may happen that it
        // requires more events than may fit in the buffers. so we
        // need to check that it actually fits (this is mandatory):
        if(handler.fits_buffers())
            result->resize_buffers(handler); // trim the events buffer to the required size
        return handler.fits_buffers();
    }

    bool parse_or_resize_and_then_parse(const char *filename, c4::csubstr yaml, IntsResult *result)
    {
        // the buffer is not mutable. we'll need to parse a copy
        result->reset_yaml(yaml);
        // attempt the parse
        if(this->parse_in_place(filename, c4::to_substr(result->yaml), result))
            return true;
        // events or arena buffers were too small. need to resize and parse again.
        result->resize_buffers(handler);
        // copy the source again! we need to do this because the
        // buffer was parsed in place, and may have been modified
        // during the parse.
        result->reset_yaml(yaml);
        // parse again!
        return this->parse_in_place(filename, c4::to_substr(result->yaml), result);
    }
};


struct Args
{
    const char *filename = nullptr;
    int events_size = -1;
    int arena_size = -1;
    bool retry = true;
    bool quiet = false;
};
void print_usage(c4::csubstr basename)
{
    Args defaults = {};
    printf(R"(usage: %.*s [options] [<filename>]

   if filename is missing, we default to a preset example.

options:

   -e <size>,--events-size <size>    set size of events buffer. -1 means
                                     estimate size. default=%d
   -a <size>,--arena-size <size>     set size of arena buffer. -1 means
                                     take the source size as the arena size.
                                     default=%d
   -n,--no-retry                     do not retry parsing if any buffer size
                                     was insufficient. default=%s
   -q,--quiet                        do not print events. default=%s
)",
           static_cast<int>(basename.len), basename.str,
           defaults.events_size,
           defaults.arena_size,
           defaults.retry ? "retry" : "do not retry",
           defaults.quiet ? "quiet" : "print events");
}
bool parse_args(int argc, const char **argv, Args *args, int &status)
{
    c4::csubstr basename = argv[0];
    // read left-to-right until we find an arg that is not an option
    int remaining = argc - 1;
    ++argv;
    auto advance = [&](int num) { argv += num; remaining -= num; };
    auto parse_arg = [&](const char *name, int *dst) {
        if(remaining > 1)
        {
            c4::csubstr val = argv[1];
            if(!c4::atoi(val, dst))
            {
                status = -1;
                printf("error: could not parse %s value: %.*s\n", name, static_cast<int>(val.len), val.str);
            }
            advance(2);
        }
        else
        {
            status = -1;
            printf("error: missing value: %s\n", name);
        }
        return true;
    };
    while(remaining > 0 && status == 0)
    {
        c4::csubstr arg = argv[0];
        if(arg == "-h" || arg == "--help")
        {
            print_usage(basename);
            status = 0;
            return false;
        }
        else if(arg == "-e" || arg == "--events-size")
        {
            parse_arg("--events-size", &args->events_size);
        }
        else if(arg == "-a" || arg == "--arena-size")
        {
            parse_arg("--arena-size", &args->arena_size);
        }
        else if(arg == "-n" || arg == "--no-retry")
        {
            args->retry = false;
            advance(1);
        }
        else if(arg == "-q" || arg == "--quiet")
        {
            args->quiet = false;
            advance(1);
        }
        else
            break;
    }
    if(remaining == 1)
    {
        if(status == 0)
            args->filename = argv[0];
    }
    else if(remaining > 1)
    {
        print_usage(basename);
        printf("error: too many arguments\n");
        status = -1;
    }
    return status == 0;
}


//-----------------------------------------------------------------------------

// NOLINTBEGIN(hicpp-signed-bitwise)

int main(int argc, const char *argv[])
{
    Args args = {};
    int status = 0;
    if(!parse_args(argc, argv, &args, status))
        return status;

    // if no filename is given, we run a demo,
    // and show the result is as expected.
    bool is_demo = args.filename == nullptr;

    // YAML code to be parsed in place
    char demo_yaml[] = ""
        "doe: a deer, a female deer\n"
        "ray: a drop of golden sun\n"
        "me : a name I call myself\n"
        "far: a long long way to run\n"
        "";

    // these are the events we expect from parsing the demo YAML
    auto STR = c4::yml::extra::ievt::PSTR; // PSTR does not work in windows
    const int demo_expected[] = {
        BSTR,
        BDOC,
        VAL_|BMAP|BLCK,
        KEY_|SCLR|PLAI,      0,  3,  // "doe"
        VAL_|SCLR|PLAI|STR,  5, 21,  // "a deer, a female deer"
        KEY_|SCLR|PLAI|STR, 27,  3,  // "ray"
        VAL_|SCLR|PLAI|STR, 32, 20,  // "a drop of golden sun"
        KEY_|SCLR|PLAI|STR, 53,  2,  // "me"
        VAL_|SCLR|PLAI|STR, 58, 20,  // "a name I call myself"
        KEY_|SCLR|PLAI|STR, 79,  3,  // "far"
        VAL_|SCLR|PLAI|STR, 84, 22,  // "a long long way to run"
        EMAP|STR,
        EDOC,
        ESTR,
    };

    /* the output should be this:
     *
     * pos=0	event[0]:	                BSTR = 0x00000001
     * pos=1	event[1]:	                BDOC = 0x00000004
     * pos=2	event[2]:	      VAL_|BMAP|BLCK = 0x00140010
     * pos=3	event[3]:	      KEY_|SCLR|PLAI = 0x00081100	str=(0,3)	'doe'
     * pos=6	event[4]:	 VAL_|SCLR|PLAI|PSTR = 0x04101100	str=(5,21)	'a deer, a female deer'
     * pos=9	event[5]:	 KEY_|SCLR|PLAI|PSTR = 0x04081100	str=(27,3)	'ray'
     * pos=12	event[6]:	 VAL_|SCLR|PLAI|PSTR = 0x04101100	str=(32,20)	'a drop of golden sun'
     * pos=15	event[7]:	 KEY_|SCLR|PLAI|PSTR = 0x04081100	str=(53,2)	'me'
     * pos=18	event[8]:	 VAL_|SCLR|PLAI|PSTR = 0x04101100	str=(58,20)	'a name I call myself'
     * pos=21	event[9]:	 KEY_|SCLR|PLAI|PSTR = 0x04081100	str=(79,3)	'far'
     * pos=24	event[10]:	 VAL_|SCLR|PLAI|PSTR = 0x04101100	str=(84,22)	'a long long way to run'
     * pos=27	event[11]:	           EMAP|PSTR = 0x04000020
     * pos=28	event[12]:	                EDOC = 0x00000008
     * pos=29	event[13]:	                ESTR = 0x00000002
     */

    IntsParser parser;
    IntsResult result;

    const char *filename = args.filename;
    std::vector<char> yaml_file;
    c4::substr yaml;

    if(is_demo)
    {
        yaml = demo_yaml;
        filename = "demo";
    }
    else
    {
        c4::csubstr filename_ = c4::to_csubstr(filename);
        if(filename_ == "-" || filename_ == "stdin")
            c4::yml::stdin_get_contents(&yaml_file); // LCOV_EXCL_LINE lcov fail
        else
            c4::yml::file_get_contents(&yaml_file, args.filename);
        yaml = c4::to_substr(yaml_file);
    }

    if(args.events_size == -1)
        args.events_size = c4::yml::extra::estimate_events_ints_size(yaml);
    if(args.arena_size == -1)
        args.arena_size = static_cast<int>(yaml.size());

    result.resize_buffers(args.events_size, static_cast<size_t>(args.arena_size));

    // parse now. since we did not set up the error callbacks, if
    // there is a parse error, we will get the default behavior, which
    // is abort on error
    if( ! args.retry)
    {
        if( ! parser.parse_in_place(filename, yaml, &result))
            return 1;
    }
    else
    {
        if( ! parser.parse_or_resize_and_then_parse(filename, yaml, &result))
            return 1; // this is really not expected   LCOV_EXCL_LINE
    }

    if( ! is_demo)
    {
        if( ! args.quiet)
        {
            c4::csubstr actual_buffer = args.retry ? yaml : c4::to_csubstr(result.yaml);
            c4::yml::extra::events_ints_print(actual_buffer, c4::to_csubstr(result.arena),
                                              result.events.data(),
                                              static_cast<c4::yml::extra::evt_size>(result.events.size()));
        }
        return 0;
    }

    // we have the demo. ensure the result is as expected
    bool success = true;

    // example iterating through the events array: compare and print
    // the result
    char flags[100]; // buffer for converting event flags to string
    for (size_t pos = 0, evt = 0, sz = static_cast<size_t>(parser.handler.required_size_events());
         pos < sz;
         ++pos, ++evt)
    {
        bool ok = (result.events[pos] == demo_expected[pos]);
        // print the event
        if( ! args.quiet)
        {
            // let's format the event flags to print them as string.
            size_t len = c4::yml::extra::ievt::to_str(flags, result.events[pos]);
            printf("pos=%zu\tevent[%zu]:\t%.*s = 0x%08x", pos, evt, static_cast<int>(len), flags, result.events[pos]);
        }
        if(result.events[pos] & WSTR) // the event has a string following it
        {
            int offset = result.events[pos + 1];
            int length = result.events[pos + 2];
            bool in_arena = (result.events[pos] & AREN);
            // WATCHOUT! the string is NOT ZERO TERMINATED!
            const char *ptr = in_arena ? result.arena.data() : yaml.str;
            const char *str = ptr + offset;
            if( ! args.quiet)
                printf("\tstr=(%d,%d)\t'%.*s'", offset, length, length, str);
            ok = ok && (offset == demo_expected[pos + 1]);
            ok = ok && (length == demo_expected[pos + 2]);
            pos += 2; // advance the two ints from the string
        }
        if(!ok)
        {
            if( ! args.quiet)          // LCOV_EXCL_LINE
                printf("  ... fail!"); // LCOV_EXCL_LINE
            success = false;           // LCOV_EXCL_LINE
        }
        if( ! args.quiet)          // LCOV_EXCL_LINE
            printf("\n");
    }

    return success ? 0 : 1;
}

// NOLINTEND(hicpp-signed-bitwise)
