// This file shows a quick example of parsing YAML to an int events
// buffer. Since this functionality is meant to implement in other
// programming languages, the code is kept very simple, and using only
// C-like idioms.

// ryml can be used as a single header, or as a simple library:
#if defined(RYML_SINGLE_HEADER) // using the single header directly in the executable
    #define RYML_SINGLE_HDR_DEFINE_NOW
    #include <ryml_ints.hpp>
#elif defined(RYML_SINGLE_HEADER_LIB) // using the single header from a library
    #include <ryml_ints.hpp>
#else
    #include <ryml.hpp>
    #include <c4/yml/extra/event_handler_ints.hpp>
#endif


int main(int, const char *[])
{
    // setup a YAML buffer to be parsed in place
    char yaml_buf[] = "doe: a deer, a female deer\n"
                      "ray: a drop of golden sun\n"
                      "me: a name I call myself\n"
                      "far: a long long way to run\n";
    c4::substr yaml = yaml_buf;
    // add a buffer for placing scalars/tags that cannot be filtered
    // in-place
    char arena[100];

    // setup a buffer to where we should write the events
    constexpr const int events_size = 100;
    int events[events_size];

    // ensure the estimation will succeed vs required size
    int estimated_size = c4::yml::extra::estimate_events_ints_size(yaml);
    if (estimated_size > events_size)
    {
        printf("the estimated size (%d) will not fit the events array (%d)\n",
               estimated_size, events_size);
        return 1;
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
        printf("error: buffers too small:"
               "   required_evt=%d actual_evt=%d\n"
               "   required_arena=%zu actual_arena=%zu\n",
               handler.required_size_events(), estimated_size,
               handler.required_size_arena(), c4::to_csubstr(arena).len);
        // WATCHOUT: if you want to retry the parse, you need to set
        // up the source buffer again, because it is invalidated from
        // being parsed in place. refer to the doxygen documentation
        // for more details.
        return 1;
    }

    // done!
    printf("success! YAML requires event size %d, estimated=%d (required_arena=%zu actual=%zu)\n",
           handler.required_size_events(), estimated_size,
           handler.required_size_arena(), c4::to_csubstr(arena).len);

    // example iterating through the events array: print the result
    for (int pos = 0, evt = 0; pos < handler.required_size_events(); ++pos, ++evt)
    {
        using namespace c4::yml::extra::ievt;
        printf("pos=%d\tevent[%d]:\t0x%x", pos, evt, events[pos]);
        if(events[pos] & WSTR) // the event has a string following it
        {
            int offset = events[pos + 1];
            int length = events[pos + 2];
            bool in_arena = (events[pos] & AREN);
            // NOT ZERO TERMINATED!
            const char *str = !in_arena ?
                yaml.str + offset
                :
                arena + offset;
            printf("\tstr=(%d,%d)\t'%.*s'", offset, length, length, str);
            pos += 2; // advance the two ints from the string
        }
        printf("\n");
    }

    /* the output should be this:
     *
     * success! YAML requires event size 30, estimated=49
     * pos=0	event[0]:	0x1
     * pos=1	event[1]:	0x4
     * pos=2	event[2]:	0x110010
     * pos=3	event[3]:	0x80500 	str=(0,3)	'doe'
     * pos=6	event[4]:	0x500500	str=(5,21)	'a deer, a female deer'
     * pos=9	event[5]:	0x480500	str=(27,3)	'ray'
     * pos=12	event[6]:	0x500500	str=(32,20)	'a drop of golden sun'
     * pos=15	event[7]:	0x480500	str=(53,2)	'me'
     * pos=18	event[8]:	0x500500	str=(57,20)	'a name I call myself'
     * pos=21	event[9]:	0x480500	str=(78,3)	'far'
     * pos=24	event[10]:	0x500500	str=(83,22)	'a long long way to run'
     * pos=27	event[11]:	0x400020
     * pos=28	event[12]:	0x8
     * pos=29	event[13]:	0x2
     */

    return 0;
}
