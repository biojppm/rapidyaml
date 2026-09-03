#ifndef TEST_EVENTS_INTS_HPP_
#define TEST_EVENTS_INTS_HPP_

#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/ints_utils.hpp>
#include <c4/yml/extra/emitter_ints.hpp>
#include <c4/yml/extra/emitter_ints.def.hpp>
#include <c4/yml/extra/ints_to_testsuite.hpp>
#include <c4/yml/writer_buf.hpp>
#include <c4/yml/writer_file.hpp>
#include <c4/yml/writer_ostream.hpp>
#include <test_lib/test_compare_events.hpp>
#include <gtest/gtest.h>

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

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

void test_events_ints_compare(ievt::Buffers const& expected, ievt::Buffers const& actual);

void test_events_ints_invariants(
    csubstr parsed_yaml,
    csubstr arena,
    evt_bits const* evts_ints,
    evt_bits evts_ints_sz);


//-----------------------------------------------------------------------------

C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")


struct TestBuffers : public ievt::Buffers
{
public:

    template<bool resize_buffers>
    void prepare_parse(ievt::EventHandlerInts<resize_buffers> &handler,
                       std::string const& parsed_yaml,
                       evt_size evts_cap=-1, size_t arena_size=npos)
    {
        if(evts_cap == -1)
            evts_cap = ievt::estimate_events_size(to_csubstr(parsed_yaml));
        if(arena_size == npos)
            arena_size = parsed_yaml.size();
        _c4dbgpf("ints: setting buffer sizes: src={} ints={} arena={}", parsed_yaml.size(), evts_cap, arena_size);
        callbacks = handler.m_stack.m_callbacks;
        resize_(evts_cap, arena_size, parsed_yaml);
        handler.reset(*this);
    }

    template<bool resize_buffers>
    bool resize_post_parse(ievt::EventHandlerInts<resize_buffers> &handler, std::string const& parsed_yaml)
    {
        bool ret = false;
        if C4_IF_CONSTEXPR (resize_buffers)
        {
            EXPECT_TRUE(handler.fits_buffers());
        }
        else
        {
            if(!handler.fits_buffers())
            {
                resize_(handler.required_size_events(), handler.required_size_arena(), parsed_yaml);
                handler.reset(*this);
                ret = true;
            }
        }
        return ret;
    }

    void resize_(evt_size evts_cap, size_t arena_size, std::string const& parsed_yaml)
    {
        if(parsed_yaml.size() > src.len)
        {
            src = yml::detail::resize(src, parsed_yaml.size(), callbacks);
        }
        ASSERT_GE(src.len, parsed_yaml.size());
        memcpy(src.str, parsed_yaml.data(), parsed_yaml.size());
        if(arena_size > arena.len)
        {
            arena = yml::detail::resize(arena, arena_size, callbacks);
        }
        ASSERT_GE(arena.len, arena_size);
        if(evts_cap > evts.len)
        {
            evt_size prev_len = evts.len;
            evts = yml::detail::resize(evts, evts_cap, callbacks);
            EXPECT_EQ(prev_len, evts.len);
        }
        ASSERT_GE(evts.cap, evts_cap);
    }

public:

    void print(bool print_all=false) const
    {
        evt_size sz = print_all ? evts.cap : num_ints();
        ievt::events_ints_print(to_csubstr(src), to_csubstr(arena), evts.ptr, sz);
    }

    evt_size num_ints() const
    {
        evt_size sz = evts.len;
        for(evt_size i = 0; i < sz; ++i)
        {
            if(evts.ptr[i] & extra::ievt::WSTR) // NOLINT
                i += 2; // NOLINT
            else if(!evts.ptr[i])
                return i;
        }
        return sz;
    }

public:

    void test_invariants() const
    {
        SCOPED_TRACE("test_invariants");
        ievt::test_events_ints_invariants(src, arena, evts.ptr, evts.len);
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
        std::string actual_events = ievt::events_ints_to_testsuite<std::string>(
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

public:

    void emit_yaml(FILE* file, EmitOptions const& opts={}) const
    {
        ievt::EmitterInts<WriterFile> emitter(opts, file);
        emitter.emit_as(EMIT_YAML, evts.ptr, evts.len, src, arena);
    }
    void emit_json(FILE* file, EmitOptions const& opts={}) const
    {
        ievt::EmitterInts<WriterFile> emitter(opts, file);
        emitter.emit_as(EMIT_JSON, evts.ptr, evts.len, src, arena);
    }

    template<class Stream>
    void emit_yaml_stream(Stream &stream, EmitOptions const& opts={}) const
    {
        ievt::EmitterInts<WriterOStream<Stream>> emitter(opts, stream);
        emitter.emit_as(EMIT_YAML, evts.ptr, evts.len, src, arena);
    }
    template<class Stream>
    void emit_json_stream(Stream &stream, EmitOptions const& opts={}) const
    {
        ievt::EmitterInts<WriterOStream<Stream>> emitter(opts, stream);
        emitter.emit_as(EMIT_JSON, evts.ptr, evts.len, src, arena);
    }

    size_t emit_yaml(substr yaml, EmitOptions const& opts={}) const
    {
        ievt::EmitterInts<WriterBuf> emitter(opts, yaml);
        emitter.emit_as(EMIT_YAML, evts.ptr, evts.len, src, arena);
        return emitter.get_result(/*error_on_excess*/false).len;
    }
    size_t emit_json(substr json, EmitOptions const& opts={}) const
    {
        ievt::EmitterInts<WriterBuf> emitter(opts, json);
        emitter.emit_as(EMIT_JSON, evts.ptr, evts.len, src, arena);
        return emitter.get_result(/*error_on_excess*/false).len;
    }

    template<class CharContainer>
    void emit_yaml(CharContainer *cont, EmitOptions const& opts={}) const
    {
        const size_t cap = cont->capacity();
        cont->resize(cap);
        const size_t len = this->emit_yaml(to_substr(*cont), opts);
        cont->resize(len);
        if(len > cap)
            this->emit_yaml(to_substr(*cont), opts);
    }
    template<class CharContainer>
    void emit_json(CharContainer *cont, EmitOptions const& opts={}) const
    {
        const size_t cap = cont->capacity();
        cont->resize(cap);
        const size_t len = this->emit_json(to_substr(*cont), opts);
        cont->resize(len);
        if(len > cap)
            this->emit_json(to_substr(*cont), opts);
    }

    template<class CharContainer>
    CharContainer emit_yaml(EmitOptions const& opts={}) const
    {
        CharContainer c;
        this->emit_yaml(&c, opts);
        return c;
    }
    template<class CharContainer>
    CharContainer emit_json(EmitOptions const& opts={}) const
    {
        CharContainer c;
        this->emit_json(&c, opts);
        return c;
    }

};

C4_SUPPRESS_WARNING_POP

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

#endif /* TEST_EVENTS_INTS_HPP_ */
