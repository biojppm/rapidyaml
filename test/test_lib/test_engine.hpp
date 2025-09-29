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
#include "c4/yml/extra/event_handler_testsuite.hpp"
#include "c4/yml/extra/event_handler_ints.hpp"


namespace c4 {
namespace yml {


struct ReferenceYaml
{
    ReferenceYaml(                                       std::string const& s                      ) : test_case_flags(), expected_error_location(), parsed(s), emitted(s), expected_ints(), expected_ints_enabled(false) {}
    ReferenceYaml(                                       std::string const& p, std::string const& e) : test_case_flags(), expected_error_location(), parsed(p), emitted(e), expected_ints(), expected_ints_enabled(false) {}
    ReferenceYaml(TestCaseFlags_e tf,                    std::string const& p, std::string const& e) : test_case_flags(tf), expected_error_location(), parsed(p), emitted(e), expected_ints(), expected_ints_enabled(false) {}
    ReferenceYaml(TestCaseFlags_e tf,                    std::string const& p                      ) : test_case_flags(tf), expected_error_location(), parsed(p), emitted(p), expected_ints(), expected_ints_enabled(false) {}
    ReferenceYaml(                    Location linecol_, std::string const& p                      ) : test_case_flags(), expected_error_location(linecol_), parsed(p), emitted(p), expected_ints(), expected_ints_enabled(false) { RYML_ASSERT(linecol_); }
    ReferenceYaml(TestCaseFlags_e tf, Location linecol_, std::string const& p, std::string const& e) : test_case_flags(tf), expected_error_location(linecol_), parsed(p), emitted(e), expected_ints(), expected_ints_enabled(false) { RYML_ASSERT(linecol_); }
    ReferenceYaml(TestCaseFlags_e tf, Location linecol_, std::string const& p                      ) : test_case_flags(tf), expected_error_location(linecol_), parsed(p), emitted(p), expected_ints(), expected_ints_enabled(false) { RYML_ASSERT(linecol_); }

    ReferenceYaml(                                       std::string const& s                      , std::vector<extra::IntEventWithScalar> ints) : test_case_flags(), expected_error_location(), parsed(s), emitted(s), expected_ints(std::move(ints)), expected_ints_enabled(true) {}
    ReferenceYaml(                                       std::string const& p, std::string const& e, std::vector<extra::IntEventWithScalar> ints) : test_case_flags(), expected_error_location(), parsed(p), emitted(e), expected_ints(std::move(ints)), expected_ints_enabled(true) {}
    ReferenceYaml(TestCaseFlags_e tf,                    std::string const& p, std::string const& e, std::vector<extra::IntEventWithScalar> ints) : test_case_flags(tf), expected_error_location(), parsed(p), emitted(e), expected_ints(std::move(ints)), expected_ints_enabled(true) {}
    ReferenceYaml(TestCaseFlags_e tf,                    std::string const& p                      , std::vector<extra::IntEventWithScalar> ints) : test_case_flags(tf), expected_error_location(), parsed(p), emitted(p), expected_ints(std::move(ints)), expected_ints_enabled(true) {}
    ReferenceYaml(                    Location linecol_, std::string const& p                      , std::vector<extra::IntEventWithScalar> ints) : test_case_flags(), expected_error_location(linecol_), parsed(p), emitted(p), expected_ints(std::move(ints)), expected_ints_enabled(true) { RYML_ASSERT(linecol_); }
    ReferenceYaml(TestCaseFlags_e tf, Location linecol_, std::string const& p, std::string const& e, std::vector<extra::IntEventWithScalar> ints) : test_case_flags(tf), expected_error_location(linecol_), parsed(p), emitted(e), expected_ints(std::move(ints)), expected_ints_enabled(true) { RYML_ASSERT(linecol_); }
    ReferenceYaml(TestCaseFlags_e tf, Location linecol_, std::string const& p                      , std::vector<extra::IntEventWithScalar> ints) : test_case_flags(tf), expected_error_location(linecol_), parsed(p), emitted(p), expected_ints(std::move(ints)), expected_ints_enabled(true) { RYML_ASSERT(linecol_); }
    TestCaseFlags_e test_case_flags;
    Location expected_error_location;
    std::string parsed;
    std::string emitted;
    std::vector<extra::IntEventWithScalar> expected_ints;
    bool expected_ints_enabled;
};


template<template<class> class EventProducerFn>
C4_NO_INLINE void test_engine_str_from_testsuite(std::string const& expected_events)
{
    extra::EventHandlerTestSuite::EventSink sink;
    extra::EventHandlerTestSuite handler(&sink);
    handler.reset();
    EventProducerFn<extra::EventHandlerTestSuite> event_producer;
    event_producer(handler);
    csubstr result = sink;
    _c4dbgpf("~~~\n{}~~~\n", result);
    EXPECT_EQ(std::string(result.str, result.len), expected_events);
}

template<template<class> class EventProducerFn>
C4_NO_INLINE void test_engine_tree_from_testsuite(ReferenceYaml const& yaml)
{
    if(yaml.test_case_flags & HAS_CONTAINER_KEYS)
    {
        ExpectError::check_error([&]{
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
        #ifdef RYML_DBG
        print_tree(tree);
        #endif
        std::string actual = emitrs_yaml<std::string>(tree);
        _c4dbgpf("~~~\n{}~~~\n", actual);
        EXPECT_EQ(actual, yaml.emitted);
    }
}

void test_engine_testsuite_from_yaml(ReferenceYaml const& yaml, std::string const& expected_events, ParserOptions opts);
void test_engine_ints_from_yaml(ReferenceYaml const& yaml, std::string const& expected_events, ParserOptions opts);
void test_engine_tree_from_yaml(ReferenceYaml const& yaml, ParserOptions opts);

void test_engine_testsuite_from_yaml_with_comments(ReferenceYaml const& yaml, std::string const& expected_events, ParserOptions opts);
void test_engine_ints_from_yaml_with_comments(ReferenceYaml const& yaml, std::string const& expected_events, ParserOptions opts);
void test_engine_tree_from_yaml_with_comments(ReferenceYaml const& yaml, ParserOptions opts);

void test_expected_error_testsuite_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location={});
void test_expected_error_ints_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location={});
void test_expected_error_tree_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location={});


//-----------------------------------------------------------------------------

struct OptionalScalar
{
    csubstr val = {};
    bool was_set = false;
    operator csubstr() const { return get(); }
    operator bool() const { return was_set; }
    void operator= (csubstr v) { val = v; was_set = true; }
    csubstr get() const { RYML_ASSERT(was_set); return val; }
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

#ifdef RYML_DBG
#define _RYML_SHOWFILELINE(name) printf("%s:%d: " #name "\n", __FILE__, __LINE__)
#else
#define _RYML_SHOWFILELINE(name)
#endif


//-----------------------------------------------------------------------------

#define ENGINE_TEST_ERRLOC(name, location, refyaml)             \
                                                                \
                                                                \
TEST(EngineTest, name##_err_testsuite_from_yaml)                \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_testsuite_from_yaml");             \
    test_expected_error_testsuite_from_yaml(refyaml, location); \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
TEST(EngineTest, name##_err_ints_from_yaml)                     \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_ints_from_yaml");                  \
    test_expected_error_ints_from_yaml(refyaml, location);      \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
TEST(EngineTest, name##_err_tree_from_yaml)                     \
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
TEST(EngineTest, name##_err_testsuite_from_yaml)        \
{                                                       \
    _RYML_SHOWFILELINE(name);                           \
    SCOPED_TRACE(#name "_err_testsuite_from_yaml");     \
    test_expected_error_testsuite_from_yaml(refyaml);   \
    _RYML_SHOWFILELINE(name);                           \
}                                                       \
                                                        \
                                                        \
TEST(EngineTest, name##_err_ints_from_yaml)             \
{                                                       \
    _RYML_SHOWFILELINE(name);                           \
    SCOPED_TRACE(#name "_err_ints_from_yaml");          \
    test_expected_error_ints_from_yaml(refyaml);        \
    _RYML_SHOWFILELINE(name);                           \
}                                                       \
                                                        \
                                                        \
TEST(EngineTest, name##_err_tree_from_yaml)             \
{                                                       \
    _RYML_SHOWFILELINE(name);                           \
    SCOPED_TRACE(#name "_err_tree_from_yaml");          \
    test_expected_error_tree_from_yaml(refyaml);        \
    _RYML_SHOWFILELINE(name);                           \
}


//-----------------------------------------------------------------------------

/* declare a parse engine test for the existing event handlers */
#define ENGINE_TEST(name, refyaml, events)                          \
    ENGINE_TEST_(name, ParserOptions{}, refyaml, events)

#define ENGINE_TEST_(name, opts, refyaml, events)                       \
                                                                        \
                                                                        \
/* declare a function that will produce a                               \
   sequence of events */                                                \
template<class Ps>                                                      \
void name##_impl(Ps &ps);                                               \
                                                                        \
                                                                        \
/* package the function into a class */                                 \
template<class Ps>                                                      \
struct name                                                             \
{                                                                       \
    void operator() (Ps &ps)                                            \
    {                                                                   \
        name##_impl(ps);                                                \
    }                                                                   \
};                                                                      \
                                                                        \
                                                                        \
TEST(EngineTest, name##_str_from_testsuite)                             \
{                                                                       \
    _RYML_SHOWFILELINE(name);                                           \
    SCOPED_TRACE(#name "_str_from_testsuite");                          \
    test_engine_str_from_testsuite<name>(events);                       \
    _RYML_SHOWFILELINE(name);                                           \
}                                                                       \
                                                                        \
TEST(EngineTest, name##_tree_from_testsuite)                            \
{                                                                       \
    _RYML_SHOWFILELINE(name);                                           \
    SCOPED_TRACE(#name "_tree_from_testsuite");                         \
    ReferenceYaml yaml refyaml;                                         \
    test_engine_tree_from_testsuite<name>(yaml);                        \
    _RYML_SHOWFILELINE(name);                                           \
}                                                                       \
                                                                        \
TEST(EngineTest, name##_testsuite_from_yaml)                            \
{                                                                       \
    _RYML_SHOWFILELINE(name);                                           \
    SCOPED_TRACE(#name "_testsuite_from_yaml");                         \
    ReferenceYaml yaml refyaml;                                         \
    test_engine_testsuite_from_yaml(yaml, events, opts);                \
    _RYML_SHOWFILELINE(name);                                           \
}                                                                       \
                                                                        \
TEST(EngineTest, name##_ints_from_yaml)                                 \
{                                                                       \
    _RYML_SHOWFILELINE(name);                                           \
    SCOPED_TRACE(#name "_event_ints_from_yaml");                        \
    ReferenceYaml yaml refyaml;                                         \
    test_engine_ints_from_yaml(yaml, events, opts);                     \
    _RYML_SHOWFILELINE(name);                                           \
}                                                                       \
                                                                        \
TEST(EngineTest, name##_tree_from_yaml)                                 \
{                                                                       \
    _RYML_SHOWFILELINE(name);                                           \
    SCOPED_TRACE(#name "_tree_from_yaml");                              \
    ReferenceYaml yaml refyaml;                                         \
    test_engine_tree_from_yaml(yaml, opts);                             \
    _RYML_SHOWFILELINE(name);                                           \
}                                                                       \
                                                                        \
TEST(EngineTest, name##_testsuite_from_yaml_with_comments)              \
{                                                                       \
    _RYML_SHOWFILELINE(name);                                           \
    SCOPED_TRACE(#name "_testsuite_from_yaml_with_comments");           \
    ReferenceYaml yaml refyaml;                                         \
    test_engine_testsuite_from_yaml_with_comments(yaml, events, opts);  \
}                                                                       \
                                                                        \
TEST(EngineTest, name##_ints_from_yaml_with_comments)                   \
{                                                                       \
    _RYML_SHOWFILELINE(name);                                           \
    SCOPED_TRACE(#name "_ints_from_yaml_with_comments");                \
    ReferenceYaml yaml refyaml;                                         \
    test_engine_ints_from_yaml_with_comments(yaml, events, opts);       \
}                                                                       \
                                                                        \
TEST(EngineTest, name##_tree_from_yaml_with_comments)                   \
{                                                                       \
    _RYML_SHOWFILELINE(name);                                           \
    SCOPED_TRACE(#name "_tree_from_yaml");                              \
    ReferenceYaml yaml refyaml;                                         \
    test_engine_tree_from_yaml_with_comments(yaml, opts);               \
    _RYML_SHOWFILELINE(name);                                           \
}                                                                       \
                                                                        \
                                                                        \
/* define the function that will produce the                            \
 * sequence of events */                                                \
template<class Ps>                                                      \
void name##_impl(Ps &ps)



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
    _dbg_printf("{}:{}: {}", file, line, stmt);
    (void)ps;
}
#endif

} // namespace yml
} // namespace c4


#endif // _C4_YML_TEST_TEST_ENGINE_HPP_
