// This file shows a quick example of parsing YAML to an int events
// buffer. Since this functionality is meant to implement in other
// programming languages, the code is kept very simple, and using only
// C-like idioms.

// ryml can be used as a single header, or as a simple library:
#if defined(RYML_SINGLE_HEADER) // using the single header directly in the executable
    #define RYML_SINGLE_HDR_DEFINE_NOW
    #ifndef RYML_SINGLE_HEADER_INTS
        #include <ryml_all.hpp>
    #else
        #include <ryml_ints.hpp>
    #endif
#elif defined(RYML_SINGLE_HEADER_LIB) // using the single header from a library
    #ifndef RYML_SINGLE_HEADER_INTS
        #include <ryml_all.hpp>
    #else
        #include <ryml_ints.hpp>
    #endif
#else
#include <c4/yml/parse_engine.def.hpp>
#endif

#ifndef RYML_SINGLE_HEADER_INTS
#include <c4/yml/extra/event_handler_ints.hpp>
#endif


// NOLINTBEGIN(hicpp-signed-bitwise)

int main(int, const char *[])
{
    using namespace c4::yml::extra::ievt;
    auto PSTR_ = c4::yml::extra::ievt::PSTR; // PSTR does not work in windows
    // YAML code to be parsed in place
    char yaml[] = "do: a deer, a female deer\n"
                  "re: a drop of golden sun\n"
                  "mi: a name I call myself\n"
                  "fa: a long long way to run\n";
    // these are the event values we expect
    const int expected_events[] = {
        BSTR,
        BDOC,
        VAL_|BMAP|BLCK,
        //
        KEY_|SCLR|PLAI,        0,  2,  // "do"
        VAL_|SCLR|PLAI|PSTR_,  4, 21,  // "a deer, a female deer"
        //
        KEY_|SCLR|PLAI|PSTR_, 26,  2,  // "re"
        VAL_|SCLR|PLAI|PSTR_, 30, 20,  // "a drop of golden sun"
        //
        KEY_|SCLR|PLAI|PSTR_, 51,  2,  // "mi"
        VAL_|SCLR|PLAI|PSTR_, 55, 20,  // "a name I call myself"
        //
        KEY_|SCLR|PLAI|PSTR_, 76,  2,  // "fa"
        VAL_|SCLR|PLAI|PSTR_, 80, 22,  // "a long long way to run"
        //
        EMAP|PSTR_,
        EDOC,
        ESTR,
    };

    /* the output should be this:
     *
     * success! YAML requires event size 30, estimated=49
     * pos=0	event[0]:	0x1
     * pos=1	event[1]:	0x4
     * pos=2	event[2]:	0x110010
     * pos=3	event[3]:	0x80500 	str=(0,2)	'do'
     * pos=6	event[4]:	0x900500	str=(4,21)	'a deer, a female deer'
     * pos=9	event[5]:	0x880500	str=(26,2)	're'
     * pos=12	event[6]:	0x900500	str=(30,20)	'a drop of golden sun'
     * pos=15	event[7]:	0x880500	str=(51,2)	'mi'
     * pos=18	event[8]:	0x900500	str=(55,20)	'a name I call myself'
     * pos=21	event[9]:	0x880500	str=(76,2)	'fa'
     * pos=24	event[10]:	0x900500	str=(80,22)	'a long long way to run'
     * pos=27	event[11]:	0x800020
     * pos=28	event[12]:	0x8
     * pos=29	event[13]:	0x2
     */

    // buffer to where we will write the events
    constexpr const int events_size = 100;
    int events[events_size] = {};
    static_assert(events_size >= sizeof(expected_events)/sizeof(expected_events[0]), "buffer too small");
    // buffer for placing any scalars/tags that cannot be filtered
    // in-place
    char arena[100] = {};


    // ensure the estimation will succeed vs required size
    int estimated_size = c4::yml::extra::estimate_events_ints_size(yaml);
    if (estimated_size > events_size)
    {
        printf("the estimated size (%d) will not fit the events array (%d)\n", estimated_size, events_size); // LCOV_EXCL_LINE
        return 1; // LCOV_EXCL_LINE
    }

    // parse now. the parse should succeed (because the YAML above is
    // legit), but if there were would be a parse error, we would get
    // the default behavior which is abort on error, since we did not
    // set up the error callbacks
    c4::yml::extra::EventHandlerInts handler;
    c4::yml::ParseEngine<c4::yml::extra::EventHandlerInts> parser(&handler);
    handler.reset(yaml, arena, events, estimated_size); // note we pass the estimated size!
    parser.parse_in_place_ev("filename", yaml);

    // the YAML was successfully parsed, but it may happen that it
    // requires more events than may fit in the buffer. so we need to
    // check that it actually fits (this is mandatory):
    if(!handler.fits_buffers())
    {
        printf("error: buffers too small:   required_evt=%d actual_evt=%d\n   required_arena=%zu actual_arena=%zu\n",     // LCOV_EXCL_LINE
               handler.required_size_events(), estimated_size, handler.required_size_arena(), c4::to_csubstr(arena).len); // LCOV_EXCL_LINE
        // WATCHOUT: if you want to retry the parse, you need to set
        // up the source buffer again, because it is invalidated from
        // being parsed in place. refer to the doxygen documentation
        // for more details.
        return 1; // LCOV_EXCL_LINE
    }

    // done!
    printf("success! YAML requires event size %d, estimated=%d (required_arena=%zu actual=%zu)\n",                    // LCOV_EXCL_LINE
           handler.required_size_events(), estimated_size, handler.required_size_arena(), c4::to_csubstr(arena).len); // LCOV_EXCL_LINE

    // ensure the result is as expected
    bool compare = true;

    // example iterating through the events array: compare and print
    // the result
    for (int pos = 0, evt = 0; pos < handler.required_size_events(); ++pos, ++evt)
    {
        bool status = (events[pos] == expected_events[pos]);
        printf("pos=%d\tevent[%d]:\t0x%x", pos, evt, events[pos]);
        if(events[pos] & WSTR) // the event has a string following it
        {
            int offset = events[pos + 1];
            int length = events[pos + 2];
            bool in_arena = (events[pos] & AREN);
            // WATCHOUT! the string is NOT ZERO TERMINATED!
            const char *ptr = in_arena ? arena : yaml;
            const char *str = ptr + offset;
            printf("\tstr=(%d,%d)\t'%.*s'", offset, length, length, str);
            status = status && (offset == expected_events[pos + 1]);
            status = status && (length == expected_events[pos + 2]);
            pos += 2; // advance the two ints from the string
        }
        if(!status)
        {
            printf("  ... fail!"); // LCOV_EXCL_LINE
            compare = false;       // LCOV_EXCL_LINE
        }
        printf("\n");
    }

    return compare ? 0 : 1;
}

// NOLINTEND(hicpp-signed-bitwise)
