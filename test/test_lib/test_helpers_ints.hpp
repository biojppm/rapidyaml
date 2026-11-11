#ifndef TEST_EVENTS_INTS_HPP_
#define TEST_EVENTS_INTS_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/writer_buf.hpp>
#include <c4/yml/writer_file.hpp>
#include <c4/yml/writer_ostream.hpp>
#include <c4/yml/parse_engine.hpp>
#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/ints_utils.hpp>
#include <c4/yml/extra/ints_to_testsuite.hpp>
#include <c4/span.hpp>
#endif

#include <test_lib/test_utils.hpp>
#include <test_lib/test_compare_events.hpp>
#include <gtest/gtest.h>

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {


//-----------------------------------------------------------------------------

// provide a structured input for the event integers, grouping the
// relevant data for the event in a single structure to simplify
// specifying events in tests
struct IntEventWithScalar
{
    evt_bits flags, str_start, str_len;
    csubstr scalar;
    bool needs_filter;
    IntEventWithScalar(evt_bits t, evt_bits start=0, evt_bits len=0, csubstr sclr={}, bool needs_filter_=false)
        : flags(t)
        , str_start(start)
        , str_len(len)
        , scalar(sclr)
        , needs_filter(needs_filter_)
    {
    }
    size_t required_size() const { return (flags & ievt::WSTR) ? 3u : 1u; } // NOLINT
};


size_t num_ints(IntEventWithScalar const *evt, size_t evt_sz);


void test_events_ints(IntEventWithScalar const* expected, size_t expected_sz,
                      evt_bits const* actual, size_t actual_sz,
                      csubstr yaml,
                      csubstr parsed_source,
                      csubstr arena);

void test_events_ints_compare(ievt::Buffers const& expected, ievt::Buffers const& actual,
                              evt_bits mask_ignore=0);


void test_events_ints_invariants(
    csubstr parsed_yaml,
    csubstr arena,
    evt_bits const* evts_ints,
    evt_bits evts_ints_sz);


void test_compare_emitted_yaml_ints(std::string const& emitted, std::string const& expected);


//-----------------------------------------------------------------------------

// test structural integrity and consistency of int buffers
void verify_visit_seq(evt_bits const* evts, evt_size evts_sz,
                      evt_size const* expected, evt_size exsz);

inline void verify_visit_seq(cspan<ievt::evt_bits> evts, cspan<evt_size> expected)
{
    verify_visit_seq(evts.data(), static_cast<evt_size>(evts.size()),
                     expected.data(), static_cast<evt_size>(expected.size()));
}

// check
void verify_visit_seq(evt_bits const* evts, evt_size evts_sz);
inline void verify_visit_seq(cspan<ievt::evt_bits> evts)
{
    verify_visit_seq(evts.data(), static_cast<evt_size>(evts.size()));
}


//-----------------------------------------------------------------------------

C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

struct TestBuffers : public ievt::Buffers
{
    Callbacks callbacks;
    bool owned;
    void destroy()
    {
        if(owned)
        {
            if(evts.ptr)
                RYML_CB_FREE_(callbacks, evts.ptr, evt_size, static_cast<size_t>(evts.len));
            if(arena.str)
                RYML_CB_FREE_(callbacks, arena.str, char, arena.len);
        }
        evts = {};
        src = {};
        arena = {};
        owned = false;
    }
    ~TestBuffers() noexcept
    {
        destroy();
    }
    TestBuffers() noexcept : ievt::Buffers(), callbacks(get_callbacks()), owned(false) {}
    TestBuffers(TestBuffers &&that) noexcept { mv_(&that); }
    TestBuffers(TestBuffers const&) = delete;
    TestBuffers& operator= (TestBuffers &&that) noexcept { if(&that != this) { destroy(); mv_(&that); } return *this; }
    TestBuffers& operator= (TestBuffers const&) = delete;

    void mv_(TestBuffers* that) noexcept
    {
        this->ievt::Buffers::operator=(*that);
        callbacks = that->callbacks;
        owned = that->owned;
        that->owned = false;
        that->destroy();
    }

public:

    void prepare_parse(EventHandlerInts &handler,
                       substr parsed_yaml,
                       evt_size evts_cap=-1, size_t arena_size=npos)
    {
        if(evts_cap == -1)
            evts_cap = estimate_events_ints_size(to_csubstr(parsed_yaml));
        if(arena_size == npos)
            arena_size = parsed_yaml.size();
        _c4dbgpf("ints: setting buffer sizes: src={} ints={} arena={}", parsed_yaml.size(), evts_cap, arena_size);
        resize_(evts_cap, arena_size, parsed_yaml);
        RYML_ASSERT_BASIC_CB_(callbacks, parsed_yaml.str == src.str);
        handler.reset(src, arena, evts.ptr, evts.cap);
    }

    bool resize_post_parse(EventHandlerInts &handler, substr parsed_yaml)
    {
        bool ret = false;
        if(!handler.fits_buffers())
        {
            resize_(handler.required_size_events(), handler.required_size_arena(), parsed_yaml);
            handler.reset(src, arena, evts.ptr, evts.cap);
            ret = true;
        }
        else
        {
            evts.len = handler.required_size_events();
        }
        return ret;
    }

    void resize_(evt_size evts_cap, size_t arena_size, substr parsed_yaml)
    {
        src = parsed_yaml;
        if(arena_size > arena.len)
        {
            arena = detail::resize(arena, arena_size, get_callbacks());
        }
        ASSERT_GE(arena.len, arena_size);
        if(evts_cap > evts.len)
        {
            evt_size prev_len = evts.len;
            evts = detail::resize(evts, evts_cap, get_callbacks());
            EXPECT_EQ(prev_len, evts.len);
        }
        ASSERT_GE(evts.cap, evts_cap);
        owned = true;
    }

public:

    void print(bool print_all=false) const
    {
        evt_size sz = print_all ? evts.cap : evts.len;
        events_ints_print(src, arena, evts.ptr, sz);
    }

    evt_size num_ints() const
    {
        return evts.len;
    }

public:

    void test_invariants() const
    {
        SCOPED_TRACE("test_invariants");
        ievt::test_events_ints_invariants(src, arena, evts.ptr, evts.len);
    }

    void test_compare(ievt::Buffers const& that, evt_bits mask_ignore=0) const
    {
        SCOPED_TRACE("test_compare");
        ievt::test_events_ints_compare(that, *this, mask_ignore);
    }

    void test_expected_evts(csubstr yaml, ievt::IntEventWithScalar const* expected, evt_size sz) const
    {
        SCOPED_TRACE("compare_ints");
        ievt::test_events_ints(expected, (size_t)sz,
                               evts.ptr, (size_t)evts.len,
                               yaml,
                               src,
                               arena);
    }

    void test_expected_testsuite(csubstr expected_events, bool ignore_doc_style=false) const
    {
        SCOPED_TRACE("compare_testsuite");
        std::string actual_events = events_ints_to_testsuite<std::string>(
            src, arena,
            evts.ptr, evts.len);
        _c4dbgpf("~~~\n{}~~~\n", actual_events);
        test_compare_events(expected_events,
                            to_csubstr(actual_events),
                            ignore_doc_style,
                            /*ignore_container_style*/false,
                            /*ignore_scalar_style*/false,
                            /*ignore_tag_normalization*/true);
    }
};


inline void parse_ints(substr src, TestBuffers *ints, ParserOptions const& opts={})
{
    using Handler = extra::EventHandlerInts;
    Handler handler;
    ParseEngine<Handler> parser(&handler, opts);
    ints->prepare_parse(handler, src);
    parser.parse_in_place_ev("(testyaml)", src);
    ASSERT_TRUE(handler.fits_buffers());
    ints->evts.len = handler.required_size_events();
}


//-----------------------------------------------------------------------------

// helpers to create TestBuffers from static evt data
inline void mkevts(TestBuffers * ints,
                   TestBuffers const& actual,
                   evt_bits const* evts, evt_size sz)
{
    ints->destroy();
    *ints = {};
    ints->src = actual.src;
    ints->arena = actual.arena;
    ints->evts.ptr = const_cast<evt_size*>(evts); // NOLINT
    ints->evts.len = sz;
    ints->owned = false;
}
inline void mkevts(TestBuffers * ints, TestBuffers const& actual, cspan<evt_size> evts)
{
    mkevts(ints, actual, evts.data(), (evt_size)evts.size());
}
inline TestBuffers mkevts(TestBuffers const& actual, cspan<evt_size> evts)
{
    TestBuffers ints;
    mkevts(&ints, actual, evts.data(), (evt_size)evts.size());
    return ints;
}

C4_SUPPRESS_WARNING_POP

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

#endif /* TEST_EVENTS_INTS_HPP_ */
