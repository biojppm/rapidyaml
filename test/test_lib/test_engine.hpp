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
#include "c4/yml/extra/ints_to_testsuite.hpp"
#include "c4/yml/extra/string.hpp"
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
    void operator= (csubstr v) { val = v; was_set = true; } // NOLINT
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

struct EngineEvtTestCase
{
    const char *file;
    int line;
    extra::string fileline;
    ParserOptions opts;
    TestCaseFlags_e test_case_flags;
    Location expected_error_location;
    std::string yaml;
    std::string expected_emitted;
    std::string expected_events;
    std::vector<extra::IntEventWithScalar> expected_ints;
    bool expected_ints_enabled;

public:

    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_,                                        std::string s               , std::string ev) : file(file_), line(line_), opts(opts_), test_case_flags(  ), expected_error_location(        ), yaml(s           ), expected_emitted(std::move(s)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_,                                        std::string p, std::string e, std::string ev) : file(file_), line(line_), opts(opts_), test_case_flags(  ), expected_error_location(        ), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_, TestCaseFlags_e tf,                    std::string p, std::string e, std::string ev) : file(file_), line(line_), opts(opts_), test_case_flags(tf), expected_error_location(        ), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_, TestCaseFlags_e tf,                    std::string p               , std::string ev) : file(file_), line(line_), opts(opts_), test_case_flags(tf), expected_error_location(        ), yaml(p           ), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_,                     Location linecol_, std::string p               , std::string ev) : file(file_), line(line_), opts(opts_), test_case_flags(  ), expected_error_location(linecol_), yaml(p           ), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_, TestCaseFlags_e tf, Location linecol_, std::string p, std::string e, std::string ev) : file(file_), line(line_), opts(opts_), test_case_flags(tf), expected_error_location(linecol_), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_, TestCaseFlags_e tf, Location linecol_, std::string p               , std::string ev) : file(file_), line(line_), opts(opts_), test_case_flags(tf), expected_error_location(linecol_), yaml(p           ), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(), expected_ints_enabled(false) { _mkfileline(); }

    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_,                                        std::string s               , std::string ev, std::vector<extra::IntEventWithScalar> ints) : file(file_), line(line_), opts(opts_), test_case_flags(  ), expected_error_location(        ), yaml(s           ), expected_emitted(std::move(s)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_,                                        std::string p, std::string e, std::string ev, std::vector<extra::IntEventWithScalar> ints) : file(file_), line(line_), opts(opts_), test_case_flags(  ), expected_error_location(        ), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_, TestCaseFlags_e tf,                    std::string p, std::string e, std::string ev, std::vector<extra::IntEventWithScalar> ints) : file(file_), line(line_), opts(opts_), test_case_flags(tf), expected_error_location(        ), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_, TestCaseFlags_e tf,                    std::string p               , std::string ev, std::vector<extra::IntEventWithScalar> ints) : file(file_), line(line_), opts(opts_), test_case_flags(tf), expected_error_location(        ), yaml(p           ), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_,                     Location linecol_, std::string p               , std::string ev, std::vector<extra::IntEventWithScalar> ints) : file(file_), line(line_), opts(opts_), test_case_flags(  ), expected_error_location(linecol_), yaml(p           ), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_, TestCaseFlags_e tf, Location linecol_, std::string p, std::string e, std::string ev, std::vector<extra::IntEventWithScalar> ints) : file(file_), line(line_), opts(opts_), test_case_flags(tf), expected_error_location(linecol_), yaml(std::move(p)), expected_emitted(std::move(e)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) { _mkfileline(); }
    EngineEvtTestCase(const char *file_, int line_, ParserOptions opts_, TestCaseFlags_e tf, Location linecol_, std::string p               , std::string ev, std::vector<extra::IntEventWithScalar> ints) : file(file_), line(line_), opts(opts_), test_case_flags(tf), expected_error_location(linecol_), yaml(           p), expected_emitted(std::move(p)), expected_events(std::move(ev)), expected_ints(std::move(ints)), expected_ints_enabled(true) { _mkfileline(); }

private:
    void _mkfileline() { c4::formatrs(&fileline, "{}:{}: (testyaml)", file, line); }
};


//-----------------------------------------------------------------------------

#define ENGINE_TEST_ERR(name, refyaml)                              ENGINE_TEST_ERRLOC__(name, ParserOptions{}, TestCaseFlags_e{}, Location{}, refyaml)
#define ENGINE_TEST_ERR_(name, testcaseflags, refyaml)              ENGINE_TEST_ERRLOC__(name, ParserOptions{}, testcaseflags    , Location{}, refyaml)
#define ENGINE_TEST_ERRLOC(name, location, refyaml)                 ENGINE_TEST_ERRLOC__(name, ParserOptions{}, TestCaseFlags_e{}, location  , refyaml)
#define ENGINE_TEST_ERRLOC_(name, testcaseflags, location, refyaml) ENGINE_TEST_ERRLOC__(name, ParserOptions{}, testcaseflags    , location  , refyaml)
#define ENGINE_TEST_ERROPT(name, parseoptions, refyaml)             ENGINE_TEST_ERRLOC__(name, parseoptions   , TestCaseFlags_e{}, Location{}, refyaml)
#define ENGINE_TEST_ERROPT_(name, parseoptions, location, refyaml)  ENGINE_TEST_ERRLOC__(name, parseoptions   , TestCaseFlags_e{}, location  , refyaml)
#define ENGINE_TEST_ERRLOC__(name, parseoptions, testcaseflags, location, refyaml) \
                                                                        \
                                                                        \
static const EngineEvtTestCase test_case_err_##name(                    \
    __FILE__, __LINE__,                                                 \
    parseoptions,                                                       \
    testcaseflags,                                                      \
    location,                                                           \
    refyaml,                                                            \
    "");                                                                \
                                                                        \
                                                                        \
TEST(name, err_ints_from_yaml)                                          \
{                                                                       \
    SCOPED_TRACE(#name ".err_ints_from_yaml");                          \
    test_expected_error_ints_from_yaml(test_case_err_##name,            \
                                       ExpectedErrorType::err_parse);   \
}                                                                       \
                                                                        \
                                                                        \
TEST(name, err_tree_from_yaml)                                          \
{                                                                       \
    SCOPED_TRACE(#name ".err_tree_from_yaml");                          \
    test_expected_error_tree_from_yaml(test_case_err_##name,            \
                                       ExpectedErrorType::err_parse);   \
}


//-----------------------------------------------------------------------------

#define ENGINE_TEST_DECLARE_CASE(name, ...)                     \
                                                                \
                                                                \
static const EngineEvtTestCase test_case_##name(                \
    __FILE__, __LINE__,                                         \
    __VA_ARGS__);                                               \
                                                                \
                                                                \
/* declare a function producing the sequence of events */       \
template<class EvtHandlerClass>                                 \
void events_##name(EvtHandlerClass &handler);



#define ENGINE_TEST_DEFINE_CASE(name)                                   \
                                                                        \
                                                                        \
/* explicitly instantiate the event producers */                        \
template void events_##name<EventHandlerIntsTr>(EventHandlerIntsTr&);   \
template void events_##name<EventHandlerTree>(EventHandlerTree&);       \
                                                                        \
                                                                        \
/* define the event producer template */                                \
template<class EvtHandlerClass>                                         \
void events_##name(EvtHandlerClass &ps)



//-----------------------------------------------------------------------------



/* declare a parse engine test for the existing event handlers.
 * The extra arguments are for the ctor of EngineEvtTestCase */
#define ENGINE_TEST(name, ...) ENGINE_TEST_(name, ParserOptions{}, __VA_ARGS__)


/* declare a parse engine test for the existing event handlers.
 * The extra arguments are for the ctor of EngineEvtTestCase */
#define ENGINE_TEST_(name, ...)                                         \
                                                                        \
                                                                        \
ENGINE_TEST_DECLARE_CASE(name, __VA_ARGS__)                             \
                                                                        \
                                                                        \
                                                                        \
TEST(name, ints_from_events)                                            \
{                                                                       \
    SCOPED_TRACE(#name ".ints_from_events");                            \
    test_engine_ints_from_events(test_case_##name,                      \
                                 &events_##name<EventHandlerIntsTr>);   \
}                                                                       \
                                                                        \
TEST(name, tree_from_events)                                            \
{                                                                       \
    SCOPED_TRACE(#name ".tree_from_events");                            \
    test_engine_tree_from_events(test_case_##name,                      \
                                 &events_##name<EventHandlerTree>);     \
}                                                                       \
                                                                        \
                                                                        \
                                                                        \
TEST(name, ints_from_yaml)                                              \
{                                                                       \
    SCOPED_TRACE(#name ".event_ints_from_yaml");                        \
    test_engine_ints_from_yaml(test_case_##name);                       \
}                                                                       \
                                                                        \
TEST(name, tree_from_yaml)                                              \
{                                                                       \
    SCOPED_TRACE(#name ".tree_from_yaml");                              \
    test_engine_tree_from_yaml(test_case_##name);                       \
}                                                                       \
                                                                        \
                                                                        \
                                                                        \
TEST(name, roundtrip_from_events)                                       \
{                                                                       \
    SCOPED_TRACE(#name ".roundtrip_from_events");                       \
    test_engine_roundtrip_from_events(test_case_##name,                 \
                                      &events_##name<EventHandlerTree>); \
}                                                                       \
                                                                        \
TEST(name, roundtrip_from_yaml)                                         \
{                                                                       \
    SCOPED_TRACE(#name ".tree_from_yaml");                              \
    test_engine_roundtrip_from_yaml(test_case_##name);                  \
}                                                                       \
                                                                        \
                                                                        \
                                                                        \
TEST(name, ints_from_yaml_with_comments)                                \
{                                                                       \
    SCOPED_TRACE(#name ".ints_from_yaml_with_comments");                \
    test_engine_ints_from_yaml_with_comments(test_case_##name);         \
}                                                                       \
                                                                        \
TEST(name, tree_from_yaml_with_comments)                                \
{                                                                       \
    SCOPED_TRACE(#name ".tree_from_yaml");                              \
    test_engine_tree_from_yaml_with_comments(test_case_##name);         \
}                                                                       \
                                                                        \
TEST(name, roundtrip_from_yaml_with_comments)                           \
{                                                                       \
    SCOPED_TRACE(#name ".roundtrip_from_yaml");                         \
    test_engine_roundtrip_from_yaml_with_comments(test_case_##name);    \
}                                                                       \
                                                                        \
                                                                        \
ENGINE_TEST_DEFINE_CASE(name)                                           \



//-----------------------------------------------------------------------------

template<class Handler, class ArgTransformer> struct EventTransformer;
template<class Handler> struct TransformToSourceBufferOrArena;
using EventHandlerIntsTr = EventTransformer<extra::EventHandlerInts, TransformToSourceBufferOrArena<extra::EventHandlerInts>>;

using EventProducerInts = void (*)(EventHandlerIntsTr &);
using EventProducerTree = void (*)(EventHandlerTree &);



//-----------------------------------------------------------------------------


void test_engine_ints_from_events(EngineEvtTestCase const& test_case, EventProducerInts evts);
void test_engine_error_ints_from_events(const EngineEvtTestCase& test_case, EventProducerInts fn);
void test_engine_ints_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml);
inline void test_engine_ints_from_yaml(EngineEvtTestCase const& test_case) { test_engine_ints_from_yaml(test_case, test_case.yaml); }
void test_engine_ints_from_yaml_with_comments(EngineEvtTestCase const& test_case);
void test_expected_error_ints_from_yaml(EngineEvtTestCase const& test_case, ExpectedErrorType errtype);


//-----------------------------------------------------------------------------

void test_engine_tree_from_events(EngineEvtTestCase const& test_case, EventProducerTree evts);
void test_engine_error_tree_from_events(const EngineEvtTestCase& test_case, EventProducerTree evts);
void test_engine_tree_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml);
inline void test_engine_tree_from_yaml(EngineEvtTestCase const& test_case) { test_engine_tree_from_yaml(test_case, test_case.yaml); }
void test_engine_tree_from_yaml_with_comments(EngineEvtTestCase const& test_case);
void test_expected_error_tree_from_yaml(EngineEvtTestCase const& test_case, ExpectedErrorType errtype);


//-----------------------------------------------------------------------------

void test_engine_roundtrip_from_events(EngineEvtTestCase const& test_case, EventProducerTree evts);
void test_engine_roundtrip_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml);
inline void test_engine_roundtrip_from_yaml(EngineEvtTestCase const& test_case) { test_engine_roundtrip_from_yaml(test_case, test_case.yaml); }
void test_engine_roundtrip_from_yaml_with_comments(EngineEvtTestCase const& test_case);


//-----------------------------------------------------------------------------

#if !defined(RYML_DBG)
#define ___(stmt) stmt
#else

#define ___(stmt)                                           \
    do                                                      \
    {                                                       \
       stmt;                                                \
       _print_handler_info(ps, #stmt, __FILE__, __LINE__);  \
    } while(0)


void _print_handler_info(EventHandlerTree const& ps, csubstr stmt, const char *file, int line);
void _print_handler_info(extra::EventHandlerInts const& ps, csubstr stmt, const char *file, int line);

template<class Handler, class ArgTransformer>
struct EventTransformer;
template<class Handler, class Transformer>
void _print_handler_info(EventTransformer<Handler, Transformer> const& ps, csubstr stmt, const char *file, int line)
{
    _print_handler_info(ps.handler, stmt, file, line);
}

#endif


//-----------------------------------------------------------------------------

/** Helper to transform int event strings in bare event tests.
 *
 * The ints event handler needs to have the event strings either in
 * the source or in the arena. That happens automatically when parsing
 * from source. But the tests emitting bare events have no source, so
 * we use this tranformer to ensure the event strings are placed in
 * the arena. */
template<class Handler, class ArgTransformer>
struct EventTransformer
{
    Handler handler;
    ArgTransformer transformer{&handler, {}};

    void start_parse(const char* filename, substr ymlsrc) { handler.start_parse(filename, ymlsrc); }

    void begin_stream() { handler.begin_stream(); }
    void end_stream() { handler.end_stream(); }
    void begin_doc() { handler.begin_doc(); }
    void end_doc() { handler.end_doc(); }
    void begin_doc_expl() { handler.begin_doc_expl(); }
    void end_doc_expl() { handler.end_doc_expl(); }

    void begin_map_key_flow() { handler.begin_map_key_flow(); }
    void begin_map_key_block() { handler.begin_map_key_block(); }
    void begin_map_val_flow() { handler.begin_map_val_flow(); }
    void begin_map_val_block() { handler.begin_map_val_block(); }
    void end_map_block() { handler.end_map_block(); }
    void end_map_flow(bool v) { handler.end_map_flow(v); }

    void begin_seq_key_flow() { handler.begin_seq_key_flow(); }
    void begin_seq_key_block() { handler.begin_seq_key_block(); }
    void begin_seq_val_flow() { handler.begin_seq_val_flow(); }
    void begin_seq_val_block() { handler.begin_seq_val_block(); }
    void end_seq_block() { handler.end_seq_block(); }
    void end_seq_flow(bool v) { handler.end_seq_flow(v); }

    void add_sibling() { handler.add_sibling(); }
    void actually_val_is_first_key_of_new_map_flow() { handler.actually_val_is_first_key_of_new_map_flow(); }
    void actually_val_is_first_key_of_new_map_block() { handler.actually_val_is_first_key_of_new_map_block(); }

    void mark_key_scalar_unfiltered() { handler.mark_key_scalar_unfiltered(); }
    void mark_val_scalar_unfiltered() { handler.mark_val_scalar_unfiltered(); }

    void set_key_scalar_plain_empty() { handler.set_key_scalar_plain_empty(); }
    void set_val_scalar_plain_empty() { handler.set_val_scalar_plain_empty(); }

    void set_key_scalar_plain(csubstr s) { handler.set_key_scalar_plain(transformer(s)); }
    void set_val_scalar_plain(csubstr s) { handler.set_val_scalar_plain(transformer(s)); }
    void set_key_scalar_dquoted(csubstr s) { handler.set_key_scalar_dquoted(transformer(s)); }
    void set_val_scalar_dquoted(csubstr s) { handler.set_val_scalar_dquoted(transformer(s)); }
    void set_key_scalar_squoted(csubstr s) { handler.set_key_scalar_squoted(transformer(s)); }
    void set_val_scalar_squoted(csubstr s) { handler.set_val_scalar_squoted(transformer(s)); }
    void set_key_scalar_literal(csubstr s) { handler.set_key_scalar_literal(transformer(s)); }
    void set_val_scalar_literal(csubstr s) { handler.set_val_scalar_literal(transformer(s)); }
    void set_key_scalar_folded(csubstr s) { handler.set_key_scalar_folded(transformer(s)); }
    void set_val_scalar_folded(csubstr s) { handler.set_val_scalar_folded(transformer(s)); }

    void set_key_anchor(csubstr s) { handler.set_key_anchor(transformer(s)); }
    void set_val_anchor(csubstr s) { handler.set_val_anchor(transformer(s)); }
    void set_key_ref(csubstr s) { handler.set_key_ref(transformer(s)); }
    void set_val_ref(csubstr s) { handler.set_val_ref(transformer(s)); }

    void set_key_tag(csubstr s) { handler.set_key_tag(transformer(s)); }
    void set_val_tag(csubstr s) { handler.set_val_tag(transformer(s)); }

    void add_directive_yaml(csubstr version) { handler.add_directive_yaml(transformer(version)); }
    void add_directive_tag(csubstr handle, csubstr prefix) { handler.add_directive_tag(transformer(handle), transformer(prefix)); }
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
        _RYML_CHECK_BASIC(dst.str != nullptr && dst.len == s.len);
        if(s.len)
            memcpy(dst.str, s.str, s.len);
        return dst;
    }
};

} // namespace yml
} // namespace c4


#endif // _C4_YML_TEST_TEST_ENGINE_HPP_
