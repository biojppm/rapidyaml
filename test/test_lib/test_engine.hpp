#ifndef _C4_YML_TEST_TEST_ENGINE_HPP_
#define _C4_YML_TEST_TEST_ENGINE_HPP_

#ifdef RYML_SINGLE_HEADER
#include "ryml_all.hpp"
#else
#include "c4/yml/event_handler_tree.hpp"
#include "c4/yml/parse_engine.hpp"
#include "c4/yml/emit.hpp"
#include "c4/yml/detail/print.hpp"
#endif
#include <gtest/gtest.h>
#include "./test_lib/test_case.hpp"
#include "./test_lib/test_events_ints_helpers.hpp"
#include "testsuite/testsuite_events.hpp"
#include "c4/yml/extra/ints_to_testsuite.hpp"
#include "c4/yml/extra/ints_utils.hpp"
#include "c4/yml/extra/event_handler_testsuite.hpp"
#include "c4/yml/extra/event_handler_ints.hpp"


namespace c4 {
namespace yml {


//-----------------------------------------------------------------------------

struct OptionalScalar
{
    csubstr val = {};
    bool was_set = false;
    operator csubstr() const { return get(); }
    operator bool() const { return was_set; }
    void operator= (csubstr v) { val = v; was_set = true; }
    csubstr get() const { _RYML_ASSERT_BASIC(was_set); return val; }
    csubstr maybe_get() const { return was_set ? val : csubstr(""); }
};

csubstr parse_anchor_and_tag(csubstr tokens, OptionalScalar *anchor, OptionalScalar *tag);

void test_compare_events(csubstr ref_evts,
                    csubstr emt_evts,
                    bool ignore_doc_style,
                    bool ignore_container_style,
                    bool ignore_scalar_style,
                    bool ignore_tag_normalization);



//-----------------------------------------------------------------------------

struct Separator {};
template<class Handler, class ArgTransformer>
struct EventTransformer
{
    Handler handler;
    ArgTransformer transformer{&handler, {}};

    void start_parse(const char* filename, c4::yml::detail::pfn_relocate_arena relocate_arena, void *relocate_arena_data)
    {
        handler.start_parse(filename, relocate_arena, relocate_arena_data);
    }

    #define fwd(evt) void evt() { handler.evt(); }
    #define fwds(evt) void evt(csubstr s) { handler.evt(transformer(s)); }
    #define fwdb(evt) void evt(bool v) { handler.evt(v); }

    fwd(begin_stream)
    fwd(end_stream)
    fwd(begin_doc)
    fwd(end_doc)
    fwd(begin_doc_expl)
    fwd(end_doc_expl)

    fwd(begin_map_key_flow)
    fwd(begin_map_key_block)
    fwd(begin_map_val_flow)
    fwd(begin_map_val_block)
    fwd(end_map_block)
    fwdb(end_map_flow)

    fwd(begin_seq_key_flow)
    fwd(begin_seq_key_block)
    fwd(begin_seq_val_flow)
    fwd(begin_seq_val_block)
    fwd(end_seq_block)
    fwdb(end_seq_flow)

    fwd(add_sibling)
    fwd(actually_val_is_first_key_of_new_map_flow)
    fwd(actually_val_is_first_key_of_new_map_block)

    fwd(mark_key_scalar_unfiltered)
    fwd(mark_val_scalar_unfiltered)

    fwd(set_key_scalar_plain_empty)
    fwd(set_val_scalar_plain_empty)

    fwds(set_key_scalar_plain)
    fwds(set_val_scalar_plain)
    fwds(set_key_scalar_dquoted)
    fwds(set_val_scalar_dquoted)
    fwds(set_key_scalar_squoted)
    fwds(set_val_scalar_squoted)
    fwds(set_key_scalar_literal)
    fwds(set_val_scalar_literal)
    fwds(set_key_scalar_folded)
    fwds(set_val_scalar_folded)

    fwds(set_key_anchor)
    fwds(set_val_anchor)
    fwds(set_key_ref)
    fwds(set_val_ref)

    fwds(set_key_tag)
    fwds(set_val_tag)

    fwds(add_directive)

    #undef fwd
    #undef fwds
    #undef fwdb
};
template<class Handler>
struct TransformToSourceBufferOrArena
{
    Handler *handler;
    csubstr src;
    csubstr operator() (csubstr s) const
    {
        size_t pos = src.find(s);
        if(pos != csubstr::npos)
        {
            _RYML_ASSERT_BASIC(pos + s.len <= src.len);
            return src.sub(pos, s.len);
        }
        substr dst = handler->alloc_arena(s.len);
        #if defined(__GNUC__) && (__GNUC__ >= 6)
        C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wnull-dereference")
        #endif
        if(s.len) memcpy(dst.str, s.str, s.len);
        #if defined(__GNUC__) && (__GNUC__ >= 6)
        C4_SUPPRESS_WARNING_GCC_POP
        #endif
        return dst;
    }
};


//-----------------------------------------------------------------------------

struct EngineEvtTestCase
{
    ParserOptions opts;
    TestCaseFlags_e test_case_flags;
    Location expected_error_location;
    std::string yaml;
    std::string expected_emitted;
    std::string expected_events;
    std::vector<extra::IntEventWithScalar> expected_ints;
    bool expected_ints_enabled;

public:

    EngineEvtTestCase(ParserOptions opts_,                                        std::string s               , std::string ev) : opts(opts_), test_case_flags(  ), expected_error_location(        ), yaml(s           ), expected_emitted(std::move(s)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) {}
    EngineEvtTestCase(ParserOptions opts_,                                        std::string p, std::string e, std::string ev) : opts(opts_), test_case_flags(  ), expected_error_location(        ), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) {}
    EngineEvtTestCase(ParserOptions opts_, TestCaseFlags_e tf,                    std::string p, std::string e, std::string ev) : opts(opts_), test_case_flags(tf), expected_error_location(        ), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) {}
    EngineEvtTestCase(ParserOptions opts_, TestCaseFlags_e tf,                    std::string p               , std::string ev) : opts(opts_), test_case_flags(tf), expected_error_location(        ), yaml(p           ), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) {}
    EngineEvtTestCase(ParserOptions opts_,                     Location linecol_, std::string p               , std::string ev) : opts(opts_), test_case_flags(  ), expected_error_location(linecol_), yaml(p           ), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) { _RYML_ASSERT_BASIC(linecol_); }
    EngineEvtTestCase(ParserOptions opts_, TestCaseFlags_e tf, Location linecol_, std::string p, std::string e, std::string ev) : opts(opts_), test_case_flags(tf), expected_error_location(linecol_), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) { _RYML_ASSERT_BASIC(linecol_); }
    EngineEvtTestCase(ParserOptions opts_, TestCaseFlags_e tf, Location linecol_, std::string p               , std::string ev) : opts(opts_), test_case_flags(tf), expected_error_location(linecol_), yaml(p           ), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) { _RYML_ASSERT_BASIC(linecol_); }

    EngineEvtTestCase(ParserOptions opts_,                                        std::string s               , std::string ev, std::vector<extra::IntEventWithScalar> ints) : opts(opts_), test_case_flags(  ), expected_error_location(        ), yaml(s           ), expected_emitted(std::move(s)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) {}
    EngineEvtTestCase(ParserOptions opts_,                                        std::string p, std::string e, std::string ev, std::vector<extra::IntEventWithScalar> ints) : opts(opts_), test_case_flags(  ), expected_error_location(        ), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) {}
    EngineEvtTestCase(ParserOptions opts_, TestCaseFlags_e tf,                    std::string p, std::string e, std::string ev, std::vector<extra::IntEventWithScalar> ints) : opts(opts_), test_case_flags(tf), expected_error_location(        ), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) {}
    EngineEvtTestCase(ParserOptions opts_, TestCaseFlags_e tf,                    std::string p               , std::string ev, std::vector<extra::IntEventWithScalar> ints) : opts(opts_), test_case_flags(tf), expected_error_location(        ), yaml(p           ), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) {}
    EngineEvtTestCase(ParserOptions opts_,                     Location linecol_, std::string p               , std::string ev, std::vector<extra::IntEventWithScalar> ints) : opts(opts_), test_case_flags(  ), expected_error_location(linecol_), yaml(p           ), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) { _RYML_ASSERT_BASIC(linecol_); }
    EngineEvtTestCase(ParserOptions opts_, TestCaseFlags_e tf, Location linecol_, std::string p, std::string e, std::string ev, std::vector<extra::IntEventWithScalar> ints) : opts(opts_), test_case_flags(tf), expected_error_location(linecol_), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) { _RYML_ASSERT_BASIC(linecol_); }
    EngineEvtTestCase(ParserOptions opts_, TestCaseFlags_e tf, Location linecol_, std::string p               , std::string ev, std::vector<extra::IntEventWithScalar> ints) : opts(opts_), test_case_flags(tf), expected_error_location(linecol_), yaml(           p), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) { _RYML_ASSERT_BASIC(linecol_); }
};


//-----------------------------------------------------------------------------


template<template<class> class EventProducerFn>
C4_NO_INLINE void test_engine_testsuite_from_events(const EngineEvtTestCase& test_case)
{
    extra::EventHandlerTestSuite::EventSink sink;
    extra::EventHandlerTestSuite handler(&sink);
    handler.reset();
    EventProducerFn<extra::EventHandlerTestSuite> event_producer;
    event_producer(handler);
    csubstr result = sink;
    _c4dbgpf("~~~\n{}~~~\n", result);
    EXPECT_EQ(std::string(result.str, result.len), test_case.expected_events);
}

template<template<class> class EventProducerFn>
C4_NO_INLINE void test_engine_error_testsuite_from_events(const EngineEvtTestCase& tc)
{
    ExpectError::check_error_parse([&]{
        test_engine_testsuite_from_events<EventProducerFn>(tc);
    });
}


//-----------------------------------------------------------------------------

template<template<class> class EventProducerFn>
C4_NO_INLINE void test_engine_ints_from_events(EngineEvtTestCase const& test_case)
{
    SCOPED_TRACE("ints_from_events");
    using Helper = EventTransformer<extra::EventHandlerInts, TransformToSourceBufferOrArena<extra::EventHandlerInts>>;
    Helper helper;
    extra::EventHandlerInts &handler = helper.handler;
    std::string src = test_case.yaml;
    std::string arena;
    std::vector<int> ints((size_t)extra::estimate_events_ints_size(to_csubstr(src)));
    arena.resize(src.size());
    handler.reset(to_substr(src), to_substr(arena), ints.data(), (int)ints.size());
    helper.transformer.src = to_csubstr(src);
    EventProducerFn<Helper> event_producer;
    event_producer(helper);
    ASSERT_LE(handler.required_size_events(), (int)ints.size());
    if(!handler.fits_buffers())
    {
        // must be because the arena is too small
        ASSERT_GT(handler.required_size_arena(), (int)arena.size());
        arena.resize(handler.required_size_arena());
        // try again
        src.assign(test_case.yaml.begin(), test_case.yaml.end());
        helper.transformer.src = to_csubstr(src);
        handler.reset(to_substr(src), to_substr(arena), ints.data(), (int)ints.size());
        event_producer(helper);
    }
    ASSERT_TRUE(handler.fits_buffers());
    ints.resize((size_t)handler.required_size_events());
    #ifdef RYML_DBG
    extra::events_ints_print(to_csubstr(src), to_csubstr(arena), ints.data(), (int)ints.size());
    #endif
    {
        SCOPED_TRACE("test_invariants");
        extra::test_events_ints_invariants(to_csubstr(src), to_csubstr(arena), ints.data(), (int)ints.size());
    }
    if(test_case.expected_ints_enabled)
    {
        SCOPED_TRACE("compare_ints");
        extra::test_events_ints(test_case.expected_ints.data(), test_case.expected_ints.size(),
                                ints.data(), ints.size(),
                                to_csubstr(test_case.yaml),
                                to_csubstr(src),
                                to_csubstr(arena));
    }
    {
        std::string actual_testsuite_events = extra::events_ints_to_testsuite<std::string>(to_csubstr(src), to_csubstr(arena), ints.data(), (int)ints.size());
        _c4dbgpf("~~~\n{}~~~\n", actual_testsuite_events);
        test_compare_events(to_csubstr(test_case.expected_events),
                            to_csubstr(actual_testsuite_events),
                            /*ignore_doc_style*/false,
                            /*ignore_container_style*/false,
                            /*ignore_scalar_style*/false,
                            /*ignore_tag_normalization*/true);
    }
}

template<template<class> class EventProducerFn>
C4_NO_INLINE void test_engine_error_ints_from_events(const EngineEvtTestCase& test_case)
{
    ExpectError::check_error_parse([&]{
        test_engine_ints_from_events<EventProducerFn>(test_case);
    });
}


//-----------------------------------------------------------------------------


template<template<class> class EventProducerFn>
C4_NO_INLINE void test_engine_tree_from_events(EngineEvtTestCase const& test_case)
{
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
    {
        ExpectError::check_error_parse([&]{
            Tree tree = {};
            EventHandlerTree handler(&tree, tree.root_id());
            EventProducerFn<EventHandlerTree> event_producer;
            event_producer(handler);
        });
    }
    else
    {
        Tree tree = {};
        EventHandlerTree handler(&tree, tree.root_id());
        EventProducerFn<EventHandlerTree> event_producer;
        event_producer(handler);
        test_invariants(tree);
        #ifdef RYML_DBG
        print_tree(tree);
        #endif
        std::string actual = emitrs_yaml<std::string>(tree);
        _c4dbgpf("~~~\n{}~~~\n", actual);
        EXPECT_EQ(test_case.expected_emitted, actual);
    }
}

template<template<class> class EventProducerFn>
C4_NO_INLINE void test_engine_error_tree_from_events(const EngineEvtTestCase& test_case)
{
    ExpectError::check_error_parse([&]{
        test_engine_tree_from_events<EventProducerFn>(test_case);
    });
}

//-----------------------------------------------------------------------------
template<template<class> class EventProducerFn>
C4_NO_INLINE void test_engine_roundtrip_from_events(EngineEvtTestCase const& test_case)
{
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    SCOPED_TRACE("roundtrip_from_events");
    Tree event_tree = {};
    EventHandlerTree handler(&event_tree, event_tree.root_id());
    EventProducerFn<EventHandlerTree> event_producer;
    event_producer(handler);
    {
        SCOPED_TRACE("test_invariants_orig");
        test_invariants(event_tree);
    }
    std::string emitted0 = emitrs_yaml<std::string>(event_tree);
    EXPECT_EQ(test_case.expected_emitted, emitted0);
    std::string copy = emitted0;
    Tree after_roundtrip = parse_in_place(to_substr(copy), test_case.opts);
    {
        SCOPED_TRACE("test_invariants_after_roundtrip");
        test_invariants(after_roundtrip);
    }
    {
        SCOPED_TRACE("compare_trees");
        test_compare(after_roundtrip, event_tree, "after_roundtrip", "event_tree");
    }
    std::string emitted1 = emitrs_yaml<std::string>(after_roundtrip);
    EXPECT_EQ(test_case.expected_emitted, emitted1);
    if(testing::Test::HasFailure())
    {
        printf("source: ~~~\n%.*s~~~\n", (int)test_case.yaml.size(), test_case.yaml.data());
        print_tree("event_tree", event_tree);
        printf("event_tree_emitted: ~~~\n%.*s~~~\n", (int)emitted0.size(), emitted0.data());
        print_tree("after_roundtrip", after_roundtrip);
        printf("after_roundtrip_emitted: ~~~\n%.*s~~~\n", (int)emitted1.size(), emitted1.data());
    }
}


//-----------------------------------------------------------------------------

void test_engine_testsuite_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml);
void test_engine_ints_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml);
void test_engine_tree_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml);
void test_engine_roundtrip_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml);

inline void test_engine_testsuite_from_yaml(EngineEvtTestCase const& test_case) { test_engine_testsuite_from_yaml(test_case, test_case.yaml); }
inline void test_engine_ints_from_yaml(EngineEvtTestCase const& test_case) { test_engine_ints_from_yaml(test_case, test_case.yaml); }
inline void test_engine_tree_from_yaml(EngineEvtTestCase const& test_case) { test_engine_tree_from_yaml(test_case, test_case.yaml); }
inline void test_engine_roundtrip_from_yaml(EngineEvtTestCase const& test_case) { test_engine_roundtrip_from_yaml(test_case, test_case.yaml); }

void test_engine_testsuite_from_yaml_with_comments(EngineEvtTestCase const& test_case);
void test_engine_ints_from_yaml_with_comments(EngineEvtTestCase const& test_case);
void test_engine_tree_from_yaml_with_comments(EngineEvtTestCase const& test_case);
void test_engine_roundtrip_from_yaml_with_comments(EngineEvtTestCase const& test_case);

void test_expected_error_testsuite_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location={});
void test_expected_error_ints_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location={});
void test_expected_error_tree_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location={});


//-----------------------------------------------------------------------------

#ifdef RYML_DBG
#define _RYML_SHOWFILELINE(name) printf("%s:%d: " #name "\n", __FILE__, __LINE__)
#else
#define _RYML_SHOWFILELINE(name)
#endif



//-----------------------------------------------------------------------------


#define ENGINE_TEST_ERRLOC(name, location, refyaml)             \
                                                                \
                                                                \
TEST(name, err_testsuite_from_yaml)                             \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_testsuite_from_yaml");             \
    test_expected_error_testsuite_from_yaml(refyaml, location); \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
TEST(name, err_ints_from_yaml)                                  \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_ints_from_yaml");                  \
    test_expected_error_ints_from_yaml(refyaml, location);      \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
TEST(name, err_tree_from_yaml)                                  \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_tree_from_yaml");                  \
    test_expected_error_tree_from_yaml(refyaml, location);      \
    _RYML_SHOWFILELINE(name);                                   \
}


//-----------------------------------------------------------------------------

#define ENGINE_TEST_ERR(name, refyaml)                  \
                                                        \
                                                        \
TEST(name, err_testsuite_from_yaml)                     \
{                                                       \
    _RYML_SHOWFILELINE(name);                           \
    SCOPED_TRACE(#name "_err_testsuite_from_yaml");     \
    test_expected_error_testsuite_from_yaml(refyaml);   \
    _RYML_SHOWFILELINE(name);                           \
}                                                       \
                                                        \
                                                        \
TEST(name, err_ints_from_yaml)                          \
{                                                       \
    _RYML_SHOWFILELINE(name);                           \
    SCOPED_TRACE(#name "_err_ints_from_yaml");          \
    test_expected_error_ints_from_yaml(refyaml);        \
    _RYML_SHOWFILELINE(name);                           \
}                                                       \
                                                        \
                                                        \
TEST(name, err_tree_from_yaml)                          \
{                                                       \
    _RYML_SHOWFILELINE(name);                           \
    SCOPED_TRACE(#name "_err_tree_from_yaml");          \
    test_expected_error_tree_from_yaml(refyaml);        \
    _RYML_SHOWFILELINE(name);                           \
}


//-----------------------------------------------------------------------------

#define ENGINE_TEST_DECLARE_CASE(name, ...)                     \
                                                                \
                                                                \
static const EngineEvtTestCase test_case_##name(__VA_ARGS__);   \
                                                                \
                                                                \
/* declare a function that will produce a                       \
   sequence of events */                                        \
template<class EvtHandlerClass>                                 \
static void stream_events_##name(EvtHandlerClass &handler);     \
                                                                \
                                                                \
/* package the function into a class */                         \
template<class EvtHandlerClass>                                 \
struct name                                                     \
{                                                               \
    void operator() (EvtHandlerClass &handler)                  \
    {                                                           \
        stream_events_##name(handler);                          \
    }                                                           \
};                                                              \
                                                                \



#define ENGINE_TEST_DEFINE_CASE(name)                           \
                                                                \
/* finally, define the function that will produce the           \
 * sequence of events */                                        \
template<class EvtHandlerClass>                                 \
void stream_events_##name(EvtHandlerClass &ps)



//-----------------------------------------------------------------------------


#define ENGINE_TEST_ERR_FROM_EVENTS(name, ...)                  \
                                                                \
                                                                \
ENGINE_TEST_DECLARE_CASE(name, __VA_ARGS__)                     \
                                                                \
                                                                \
TEST(name, engine_err_testsuite_from_yaml)                      \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_testsuite_from_events");           \
    auto const &tc = test_case_##name;                          \
    test_engine_error_testsuite_from_events<name>(tc);          \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
TEST(name, engine_err_ints_from_yaml)                           \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_ints_from_events");                \
    auto const &tc = test_case_##name;                          \
    test_engine_error_ints_from_events<name>(tc);               \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
TEST(name, engine_err_tree_from_yaml)                           \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_tree_from_events");                \
    auto const &tc = test_case_##name;                          \
    test_engine_error_tree_from_events<name>(tc);               \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
ENGINE_TEST_DEFINE_CASE(name)                                   \
                                                                \
                                                                \


//-----------------------------------------------------------------------------


/* declare a parse engine test for the existing event handlers.
 * The extra arguments are for the ctor of EngineEvtTestCase */
#define ENGINE_TEST(name, ...) ENGINE_TEST_(name, ParserOptions{}, __VA_ARGS__)


/* declare a parse engine test for the existing event handlers.
 * The extra arguments are for the ctor of EngineEvtTestCase */
#define ENGINE_TEST_(name, ...)                                 \
                                                                \
                                                                \
ENGINE_TEST_DECLARE_CASE(name, __VA_ARGS__)                     \
                                                                \
                                                                \
TEST(name, testsuite_from_events)                               \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_testsuite_from_events");               \
    auto const &tc = test_case_##name;                          \
    test_engine_testsuite_from_events<name>(tc);                \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
TEST(name, ints_from_events)                                    \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_ints_from_events");                    \
    auto const &tc = test_case_##name;                          \
    test_engine_ints_from_events<name>(tc);                     \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
TEST(name, tree_from_events)                                    \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_tree_from_events");                    \
    auto const &tc = test_case_##name;                          \
    test_engine_tree_from_events<name>(tc);                     \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
TEST(name, roundtrip_from_events)                               \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_roundtrip_from_events");               \
    auto const &tc = test_case_##name;                          \
    test_engine_roundtrip_from_events<name>(tc);                \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
                                                                \
TEST(name, testsuite_from_yaml)                                 \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_testsuite_from_yaml");                 \
    auto const &tc = test_case_##name;                          \
    test_engine_testsuite_from_yaml(tc);                        \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
TEST(name, ints_from_yaml)                                      \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_event_ints_from_yaml");                \
    auto const &tc = test_case_##name;                          \
    test_engine_ints_from_yaml(tc);                             \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
TEST(name, tree_from_yaml)                                      \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_tree_from_yaml");                      \
    auto const &tc = test_case_##name;                          \
    test_engine_tree_from_yaml(tc);                             \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
TEST(name, roundtrip_from_yaml)                                 \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_tree_from_yaml");                      \
    auto const &tc = test_case_##name;                          \
    test_engine_roundtrip_from_yaml(tc);                        \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
                                                                \
TEST(name, testsuite_from_yaml_with_comments)                   \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_testsuite_from_yaml_with_comments");   \
    auto const &tc = test_case_##name;                          \
    test_engine_testsuite_from_yaml_with_comments(tc);          \
}                                                               \
                                                                \
TEST(name, ints_from_yaml_with_comments)                        \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_ints_from_yaml_with_comments");        \
    auto const &tc = test_case_##name;                          \
    test_engine_ints_from_yaml_with_comments(tc);               \
}                                                               \
                                                                \
TEST(name, tree_from_yaml_with_comments)                        \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_tree_from_yaml");                      \
    auto const &tc = test_case_##name;                          \
    test_engine_tree_from_yaml_with_comments(tc);               \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
TEST(name, roundtrip_from_yaml_with_comments)                   \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_roundtrip_from_yaml");                 \
    auto const &tc = test_case_##name;                          \
    test_engine_roundtrip_from_yaml_with_comments(tc);          \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
ENGINE_TEST_DEFINE_CASE(name)                                   \
                                                                \
                                                                \



//-----------------------------------------------------------------------------

#if !defined(RYML_DBG)
#define ___(stmt) stmt
#else

#define ___(stmt)                       \
    do                                  \
    {                                   \
       stmt;                            \
       _print_handler_info(ps, #stmt, __FILE__, __LINE__);  \
    } while(0)

C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wattributes")

inline C4_NO_INLINE void _print_handler_info(EventHandlerTree const& ps, csubstr stmt, const char *file, int line)
{
    if(ps.m_parent)
        _dbg_printf("{}:{}: parent.id={} curr.id={}  {}\n",
                    file, line, ps.m_parent->node_id, ps.m_curr->node_id, stmt);
    else
        _dbg_printf("{}:{}: parent.id=-- curr.id={}  {}\n",
                    file, line, ps.m_curr->node_id, stmt);
    print_tree(*ps.m_tree);
}

inline C4_NO_INLINE void _print_handler_info(extra::EventHandlerTestSuite const& ps, csubstr stmt, const char *file, int line)
{
    _dbg_printf("{}:{}: {}", file, line, stmt);
    auto indent = [](id_type n){
        for(id_type level = 0; level < n; ++level)
        {
            _dbg_printf("  ");
        }
    };
    for(id_type i = 0; i < ps.m_stack.size(); ++i)
    {
        csubstr const& str = ps._buf_(i);
        indent(i);
        _dbg_printf("[{}]\n", i);
        for(csubstr ln : str.split('\n'))
        {
            indent(i);
            _dbg_printf("{}\n", ln);
        }
    }
}

inline C4_NO_INLINE void _print_handler_info(extra::EventHandlerInts const& ps, csubstr stmt, const char *file, int line)
{
    _dbg_printf("{}:{}: {}\n", file, line, stmt);
    (void)ps;
}

template<class Handler, class Transformer>
inline C4_NO_INLINE void _print_handler_info(EventTransformer<Handler, Transformer> const& ps, csubstr stmt, const char *file, int line)
{
    _print_handler_info(ps.handler, stmt, file, line);
}

C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wattributes")

#endif

} // namespace yml
} // namespace c4


#endif // _C4_YML_TEST_TEST_ENGINE_HPP_
