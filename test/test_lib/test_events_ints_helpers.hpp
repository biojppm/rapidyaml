#ifndef TEST_EVENTS_INTS_HPP_
#define TEST_EVENTS_INTS_HPP_

#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/ints_utils.hpp>
#include <c4/yml/extra/emitter_ints.hpp>
#include <c4/yml/extra/emitter_ints.def.hpp>
#include <c4/yml/extra/ints_to_testsuite.hpp>
#include <c4/yml/writer_buf.hpp>
#include <testsuite/testsuite_events.hpp>
#include <gtest/gtest.h>

namespace c4 {
namespace yml {
namespace extra {

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

void test_events_ints_invariants(
    csubstr parsed_yaml,
    csubstr arena,
    evt_bits const* evts_ints,
    evt_bits evts_ints_sz);


//-----------------------------------------------------------------------------

C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")


struct foo {

public:

    void prepare_parse(extra::EventHandlerInts &handler,
                       std::string const& parsed_yaml,
                       extra::evt_size ints_size=-1, size_t arena_size=npos)
    {
        if(ints_size == -1)
            ints_size = extra::estimate_events_ints_size(to_csubstr(parsed_yaml));
        if(arena_size == npos)
            arena_size = parsed_yaml.size();
        _c4dbgpf("ints: setting buffer sizes: src={} emitted={} ints={} arena={}", parsed_yaml.size(), test_case.expected_emitted.size(), ints_size, arena_size);
        ints.resize((size_t)ints_size);
        arena.resize(arena_size);
        src.assign(parsed_yaml.begin(), parsed_yaml.end());
        handler.reset(to_substr(src), to_substr(arena), ints.data(), (int)ints.size());
    }
    bool resize_post_parse(extra::EventHandlerInts &handler, std::string const& parsed_yaml)
    {
        bool ret = false;
        ints.resize((size_t)handler.required_size_events());
        if(!handler.fits_buffers())
        {
            ints.resize((size_t)handler.required_size_events());
            arena.resize(handler.required_size_arena());
            src.assign(parsed_yaml.begin(), parsed_yaml.end());
            handler.reset(to_substr(src), to_substr(arena), ints.data(), (int)ints.size());
            ret = true;
        }
        return ret;
    }

public:

    void print(bool print_all=true) const
    {
        size_t sz = print_all ? ints.size() : num_ints();
        extra::events_ints_print(to_csubstr(src), to_csubstr(arena), ints.data(), (int)sz);
    }

    size_t num_ints() const
    {
        size_t sz = ints.size();
        for(size_t i = 0; i < sz; ++i)
        {
            if(ints[i] & extra::ievt::WSTR) // NOLINT
                i += 2; // NOLINT
            else if(!ints[i])
                return i;
        }
        return sz;
    }

public:

    void test_invariants() const
    {
        SCOPED_TRACE("test_invariants");
        extra::test_events_ints_invariants(to_csubstr(src), to_csubstr(arena), ints.data(), (int)ints.size());
    }

    void test_expected_evts(csubstr yaml, extra::IntEventWithScalar *expected, evt_size sz) const
    {
        SCOPED_TRACE("compare_ints");
        extra::test_events_ints(expected, (size_t)sz,
                                ints.data(), ints.size(),
                                yaml,
                                to_csubstr(src),
                                to_csubstr(arena));
    }

    void test_expected_testsuite(csubstr expected_events) const
    {
        std::string actual_events = extra::events_ints_to_testsuite<std::string>(
            to_csubstr(src), to_csubstr(arena),
            ints.data(), (int)ints.size());
        _c4dbgpf("~~~\n{}~~~\n", actual_events);
        test_compare_events(expected_events,
                            to_csubstr(actual_events),
                            /*ignore_doc_style*/false,
                            /*ignore_container_style*/false,
                            /*ignore_scalar_style*/false,
                            /*ignore_tag_normalization*/true);
    }

public:

    size_t emit_yaml(substr yaml, EmitOptions const& opts={}) const
    {
        extra::EmitterInts<WriterBuf> emitter(opts, yaml);
        emitter.emit_as(EMIT_YAML, ints.data(), (extra::evt_size)ints.size(), to_csubstr(src), to_csubstr(arena));
        return emitter.get_result(/*error_on_excess*/false).len;
    }
    size_t emit_json(substr json, EmitOptions const& opts={}) const
    {
        extra::EmitterInts<WriterBuf> emitter(opts, json);
        emitter.emit_as(EMIT_JSON, ints.data(), (extra::evt_size)ints.size(), to_csubstr(src), to_csubstr(arena));
        return emitter.get_result(/*error_on_excess*/false).len;
    }

    template<class CharContainer>
    void emit_yaml(CharContainer *cont, EmitOptions const& opts={}) const
    {
        size_t sz = cont->capacity();
        cont->resize(sz);
        size_t len = this->emit_yaml(to_substr(*cont), opts);
        cont->resize(len);
        if(len > sz)
            this->emit_yaml(to_substr(*cont), opts);
    }
    template<class CharContainer>
    void emit_json(CharContainer *cont, EmitOptions const& opts={}) const
    {
        size_t sz = cont->capacity();
        cont->resize(sz);
        size_t len = this->emit_json(to_substr(*cont), opts);
        cont->resize(len);
        if(len > sz)
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

} // namespace extra
} // namespace yml
} // namespace c4

#endif /* TEST_EVENTS_INTS_HPP_ */
