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
#include "./test_suite/test_suite_event_handler.hpp"


namespace c4 {
namespace yml {


struct ReferenceYaml
{
    ReferenceYaml(                                       std::string const& s                      ) : test_case_flags(), expected_error_location(), parsed(s), emitted(s) {}
    ReferenceYaml(                                       std::string const& p, std::string const& e) : test_case_flags(), expected_error_location(), parsed(p), emitted(e) {}
    ReferenceYaml(TestCaseFlags_e tf,                    std::string const& p, std::string const& e) : test_case_flags(tf), expected_error_location(), parsed(p), emitted(e) {}
    ReferenceYaml(TestCaseFlags_e tf,                    std::string const& p                      ) : test_case_flags(tf), expected_error_location(), parsed(p), emitted(p) {}
    ReferenceYaml(                    Location linecol_, std::string const& p                      ) : test_case_flags(), expected_error_location(linecol_), parsed(p), emitted(p) { RYML_ASSERT(linecol_); }
    ReferenceYaml(TestCaseFlags_e tf, Location linecol_, std::string const& p, std::string const& e) : test_case_flags(tf), expected_error_location(linecol_), parsed(p), emitted(e) { RYML_ASSERT(linecol_); }
    ReferenceYaml(TestCaseFlags_e tf, Location linecol_, std::string const& p                      ) : test_case_flags(tf), expected_error_location(linecol_), parsed(p), emitted(p) { RYML_ASSERT(linecol_); }
    TestCaseFlags_e test_case_flags;
    Location expected_error_location;
    std::string parsed;
    std::string emitted;
};


template<template<class> class EventProducerFn>
C4_NO_INLINE void test_new_parser_str_from_events(std::string const& expected_events)
{
    EventHandlerYamlStd::EventSink sink;
    EventHandlerYamlStd handler(&sink);
    handler.reset();
    EventProducerFn<EventHandlerYamlStd> event_producer;
    event_producer(handler);
    csubstr result = sink;
    _c4dbgpf("~~~\n{}~~~\n", result);
    EXPECT_EQ(std::string(result.str, result.len), expected_events);
}

template<template<class> class EventProducerFn>
C4_NO_INLINE void test_new_parser_tree_from_events(ReferenceYaml const& yaml)
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

void test_new_parser_events_from_yaml(ReferenceYaml const& yaml, std::string const& expected_events);
void test_new_parser_tree_from_yaml(ReferenceYaml const& yaml);
void test_new_parser_events_from_yaml_with_comments(ReferenceYaml const& yaml, std::string const& expected_events);
void test_new_parser_tree_from_yaml_with_comments(ReferenceYaml const& yaml);
void test_expected_error_events_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location={});
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
TEST(EngineTest, name##_err_events_from_yaml)                   \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_events_from_yaml");                \
    test_expected_error_events_from_yaml(refyaml, location);    \
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

#define ENGINE_TEST_ERR(name, refyaml)                          \
                                                                \
                                                                \
TEST(EngineTest, name##_err_events_from_yaml)                   \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_events_from_yaml");                \
    test_expected_error_events_from_yaml(refyaml);              \
    _RYML_SHOWFILELINE(name);                                   \
}                                                               \
                                                                \
                                                                \
TEST(EngineTest, name##_err_tree_from_yaml)                     \
{                                                               \
    _RYML_SHOWFILELINE(name);                                   \
    SCOPED_TRACE(#name "_err_tree_from_yaml");                  \
    test_expected_error_tree_from_yaml(refyaml);                \
    _RYML_SHOWFILELINE(name);                                   \
}


//-----------------------------------------------------------------------------


/* declare a common parser test for the existing event handlers */  \
#define ENGINE_TEST(name, refyaml, events)                          \
                                                                    \
                                                                    \
/* declare a function that will produce a                           \
   sequence of events */                                            \
template<class Ps>                                                  \
void name##_impl(Ps &ps);                                           \
                                                                    \
                                                                    \
/* package the function into a class */                             \
template<class Ps>                                                  \
struct name                                                         \
{                                                                   \
    void operator() (Ps &ps)                                        \
    {                                                               \
        name##_impl(ps);                                            \
    }                                                               \
};                                                                  \
                                                                    \
                                                                    \
TEST(EngineTest, name##_str_from_events)                            \
{                                                                   \
    _RYML_SHOWFILELINE(name);                                       \
    SCOPED_TRACE(#name "_str_from_events");                         \
    test_new_parser_str_from_events<name>(events);                  \
    _RYML_SHOWFILELINE(name);                                       \
}                                                                   \
                                                                    \
TEST(EngineTest, name##_tree_from_events)                           \
{                                                                   \
    _RYML_SHOWFILELINE(name);                                       \
    SCOPED_TRACE(#name "_wtree_from_events");                       \
    ReferenceYaml yaml refyaml;                                     \
    test_new_parser_tree_from_events<name>(yaml);                   \
    _RYML_SHOWFILELINE(name);                                       \
}                                                                   \
                                                                    \
TEST(EngineTest, name##_events_from_yaml)                           \
{                                                                   \
    _RYML_SHOWFILELINE(name);                                       \
    SCOPED_TRACE(#name "_events_from_yaml");                        \
    ReferenceYaml yaml refyaml;                                     \
    test_new_parser_events_from_yaml(yaml, events);                 \
    _RYML_SHOWFILELINE(name);                                       \
}                                                                   \
                                                                    \
TEST(EngineTest, name##_tree_from_yaml)                             \
{                                                                   \
    _RYML_SHOWFILELINE(name);                                       \
    SCOPED_TRACE(#name "_wtree_from_yaml");                         \
    ReferenceYaml yaml refyaml;                                     \
    test_new_parser_tree_from_yaml(yaml);                           \
    _RYML_SHOWFILELINE(name);                                       \
}                                                                   \
                                                                    \
TEST(EngineTest, name##_events_from_yaml_with_comments)             \
{                                                                   \
    _RYML_SHOWFILELINE(name);                                       \
    SCOPED_TRACE(#name "_events_from_yaml_with_comments");          \
    ReferenceYaml yaml refyaml;                                     \
    test_new_parser_events_from_yaml_with_comments(yaml, events);   \
}                                                                   \
                                                                    \
TEST(EngineTest, name##_tree_from_yaml_with_comments)               \
{                                                                   \
    _RYML_SHOWFILELINE(name);                                       \
    SCOPED_TRACE(#name "_tree_from_yaml");                          \
    ReferenceYaml yaml refyaml;                                     \
    test_new_parser_tree_from_yaml_with_comments(yaml);             \
    _RYML_SHOWFILELINE(name);                                       \
}                                                                   \
                                                                    \
                                                                    \
/* define the function that will produce the                        \
 * sequence of events */                                            \
template<class Ps>                                                  \
void name##_impl(Ps &ps)



//-----------------------------------------------------------------------------

#if !defined(RYML_DBG)
#define ___(stmt) stmt
#else
inline void _print_handler_info(EventHandlerYamlStd const& ps, csubstr stmt, const char *file, int line)
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
inline void _print_handler_info(EventHandlerTree const& ps, csubstr stmt, const char *file, int line)
{
    if(ps.m_parent)
        _dbg_printf("{}:{}: parent.id={} curr.id={}  {}\n",
                    file, line, ps.m_parent->node_id, ps.m_curr->node_id, stmt);
    else
        _dbg_printf("{}:{}: parent.id=-- curr.id={}  {}\n",
                    file, line, ps.m_curr->node_id, stmt);
    print_tree(*ps.m_tree);
}
#define ___(stmt)                       \
    do                                  \
    {                                   \
       stmt;                            \
       _print_handler_info(ps, #stmt, __FILE__, __LINE__);  \
    } while(0)
#endif

} // namespace yml
} // namespace c4


#endif // _C4_YML_TEST_TEST_ENGINE_HPP_
