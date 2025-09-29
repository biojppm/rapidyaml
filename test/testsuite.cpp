#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/detail/checks.hpp>
#endif
#include "test_lib/test_case.hpp"
#include "test_lib/test_engine.hpp"
#include "test_lib/test_events_ints_helpers.hpp"
#include "testsuite/testsuite_common.hpp"
#include "testsuite/testsuite_parts.hpp"
#include "testsuite/testsuite_events.hpp"
#include "c4/yml/extra/event_handler_testsuite.hpp"
#include "c4/yml/extra/event_handler_ints.hpp"
#include "c4/yml/extra/ints_to_testsuite.hpp"
#include <c4/fs/fs.hpp>
#include <c4/log/log.hpp>
#include <gtest/gtest.h>


/* Each case from the test suite contains:
 *
 *  - (awkward) input yaml (in_yaml)
 *  - (somewhat standard) output equivalent (out_yaml)
 *  - (when meaningful/possible) json equivalent (in_json)
 *  - yaml parsing events (events)
 *
 * Running a test consists of parsing the contents above into a data
 * structure, and then repeatedly parsing and emitting yaml in
 * sequence. Ie, (eg for in_yaml) parse in_yaml, emit corresponding
 * yaml, then parse this emitted yaml, and so on. Each parse/emit pair
 * is named a processing level in this test. */


C4_SUPPRESS_WARNING_MSVC_PUSH
C4_SUPPRESS_WARNING_MSVC(4702) // unreachable code

#define NLEVELS 4
#define SKIP_IF(condition)                              \
    do {                                                \
        if(condition) {                                 \
            printf("%s:%d: skipping: %s\n",             \
                   __FILE__, __LINE__, #condition);     \
            /*GTEST_SKIP();*/                           \
            return;                                     \
        }                                               \
    } while(0)

namespace c4 {
namespace yml {



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct TestSuiteCaseEvents
{
    csubstr filename = {};
    std::string reference_events = {}; ///< the events in the test suite case
    Tree    tree_parsed_from_events_src = {};
    bool    was_parsed = false;

    void init(csubstr filename_, csubstr src_)
    {
        filename = filename_;
        reference_events.assign(src_.begin(), src_.end());
        tree_parsed_from_events_src.clear();
        tree_parsed_from_events_src.clear_arena();
        tree_parsed_from_events_src.reserve(10);
        was_parsed = false;
    }

    void parse_events(csubstr actual_src)
    {
        if(was_parsed)
            return;
        SKIP_IF(actual_src.empty());
        parse_events_to_tree(c4::to_csubstr(reference_events), &tree_parsed_from_events_src);
        if(tree_parsed_from_events_src.empty())
            tree_parsed_from_events_src.reserve(10);
        _nfo_print_tree("EXPECTED", tree_parsed_from_events_src);
        was_parsed = true;
    }

    void compare_actual_tree_to_events_src_tree(csubstr actual_src, Tree const& actual_tree) const
    {
        SKIP_IF(actual_src.empty());
        SCOPED_TRACE("compare trees");
        _nfo_logf("SRC:\n{}", actual_src);
        _nfo_print_tree("EXPECTED", tree_parsed_from_events_src);
        _nfo_print_tree("ACTUAL", actual_tree);
        test_compare(actual_tree, tree_parsed_from_events_src);
    }

    /** compare the tree parsed from the events source with the tree
     * parsed from the actual source */
    void compare_event_tree_to_src_tree(csubstr actual_src, Tree const& tree_from_actual_src)
    {
        SKIP_IF(actual_src.empty());
        SCOPED_TRACE("compare emitted events");
        C4_UNUSED(actual_src);
        emit_events_from_tree(&tmp_events_emitted_from_parsed_tree, tree_from_actual_src);
        _nfo_logf("EXPECTED_EVENTS:\n{}", reference_events);
        _nfo_logf("ACTUAL_EVENTS:\n{}", tmp_events_emitted_from_parsed_tree);
        // we cannot directly compare the event strings,
        // so we create a tree from the emitted events,
        // and then compare the trees:
        tmp_tree_from_emitted_events.clear();
        tmp_tree_from_emitted_events.reserve(16);
        parse_events_to_tree(c4::to_csubstr(tmp_events_emitted_from_parsed_tree), &tmp_tree_from_emitted_events);
        _nfo_logf("SRC:\n{}", actual_src);
        _nfo_print_tree("ACTUAL_FROM_SOURCE", tree_from_actual_src);
        _nfo_print_tree("ACTUAL_FROM_EMITTED_EVENTS", tmp_tree_from_emitted_events);
        _nfo_print_tree("EXPECTED_FROM_EVENTS", tree_parsed_from_events_src);
        test_compare(tmp_tree_from_emitted_events, tree_parsed_from_events_src);
    }
    mutable Tree tmp_tree_from_emitted_events = {};
    std::string tmp_events_emitted_from_parsed_tree = {};

    void compare_events(csubstr emitted_events, bool ignore_container_style, bool ignore_scalar_style)
    {
        test_compare_events(to_csubstr(reference_events),
                            emitted_events,
                            /*ignore_doc_style*/true,
                            ignore_container_style,
                            ignore_scalar_style,
                            /*ignore_tag*/true);
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** a processing level */
struct TestSequenceLevel
{
    size_t              level;
    TestSequenceLevel   *prev;
    csubstr             filename;
    std::string         src_orig;
    std::string         src_tree;
    std::string         src_tree_json;
    std::string         src_evts;
    std::string         src_evts_ints;
    std::string         arena_evts_ints;
    EventHandlerTree    evt_handler_tree;
    EventHandlerTree    evt_handler_tree_json;
    Parser              parser_tree;
    Parser              parser_tree_json;
    Tree                tree_parsed_from_src;
    Tree                tree_parsed_from_src_json;
    std::string         emitted_from_tree_parsed_from_src;
    std::string         emitted_from_tree_parsed_from_src_json;

    extra::EventHandlerTestSuite::EventSink evt_str_sink;
    extra::EventHandlerTestSuite evt_handler_str;
    ParseEngine<extra::EventHandlerTestSuite> parser_str;

    extra::EventHandlerInts evt_handler_ints;
    ParseEngine<extra::EventHandlerInts> parser_ints;
    std::vector<extra::EventHandlerInts::value_type> buffer_ints;
    std::string evts_test_suite_from_ints;

    bool immutable = false;
    bool reuse = false;
    bool tree_was_parsed = false;
    bool tree_was_parsed_json = false;
    bool tree_was_emitted = false;
    bool tree_was_emitted_json = false;
    bool events_were_generated = false;
    bool events_ints_were_generated = false;

    TestSequenceLevel()
        : evt_handler_tree()
        , evt_handler_tree_json()
        , parser_tree(&evt_handler_tree)
        , parser_tree_json(&evt_handler_tree_json)
        , evt_str_sink()
        , evt_handler_str(&evt_str_sink)
        , parser_str(&evt_handler_str)
        , evt_handler_ints()
        , parser_ints(&evt_handler_ints)
        , buffer_ints()
    {
    }

    void init(size_t level_, TestSequenceLevel *prev_, csubstr filename_, csubstr src_, bool immutable_, bool reuse_)
    {
        level = level_;
        prev = prev_;
        filename = filename_;
        src_tree.assign(src_.begin(), src_.end());
        src_orig = src_tree;
        src_evts = src_tree;
        src_evts_ints = src_tree;
        immutable = immutable_;
        reuse = reuse_;
        tree_was_parsed = false;
        tree_was_parsed_json = false;
        tree_was_emitted = false;
        tree_was_emitted_json = false;
        events_were_generated = false;
        events_ints_were_generated = false;
    }

    void receive_src(TestSequenceLevel & prev_)
    {
        RYML_ASSERT(&prev_ == prev);
        if(!prev_.tree_was_emitted)
        {
            _nfo_logf("level[{}] not emitted. emit!", prev_.level);
            prev_.emit_parsed_tree();
        }
        if(src_tree != prev_.emitted_from_tree_parsed_from_src)
        {
            tree_was_parsed = false;
            tree_was_emitted = false;
            events_were_generated = false;
            events_ints_were_generated = false;
            src_tree = prev_.emitted_from_tree_parsed_from_src;
            src_evts = src_tree;
            src_evts_ints = src_tree;
        }
    }

    void receive_src_json(TestSequenceLevel & prev_)
    {
        RYML_ASSERT(&prev_ == prev);
        if(!prev_.tree_was_emitted_json)
        {
            _nfo_logf("level[{}] not emitted. emit!", prev_.level);
            prev_.emit_parsed_tree_json();
        }
        if(src_tree_json != prev_.emitted_from_tree_parsed_from_src_json)
        {
            tree_was_parsed_json = false;
            tree_was_emitted_json = false;
            src_tree_json = prev_.emitted_from_tree_parsed_from_src_json;
        }
    }

    void parse_yaml_to_tree()
    {
        if(tree_was_parsed)
            return;
        if(prev)
            receive_src(*prev);
        _nfo_logf("level[{}]: parsing source:\n{}", level, src_tree);
        if(reuse)
        {
            tree_parsed_from_src.clear();
            evt_handler_tree.m_stack.m_callbacks = get_callbacks();
            tree_parsed_from_src.m_callbacks = get_callbacks();
            if(immutable)
                parse_in_arena(&parser_tree, filename, c4::to_csubstr(src_tree), &tree_parsed_from_src);
            else
                parse_in_place(&parser_tree, filename, c4::to_substr(src_tree), &tree_parsed_from_src);
        }
        else
        {
            if(immutable)
                tree_parsed_from_src = parse_in_arena(filename, c4::to_csubstr(src_tree));
            else
                tree_parsed_from_src = parse_in_place(filename, c4::to_substr(src_tree));
        }
        _nfo_print_tree("PARSED", tree_parsed_from_src);
        if(tree_parsed_from_src.num_tag_directives())
        {
            tree_parsed_from_src.resolve_tags();
            _nfo_print_tree("RESOLVED TAGS", tree_parsed_from_src);
        }
        tree_parsed_from_src.normalize_tags_long();
        tree_was_parsed = true;
    }

    void parse_json_to_tree()
    {
        if(tree_was_parsed_json)
            return;
        if(prev)
            receive_src_json(*prev);
        _nfo_logf("level[{}]: parsing source:\n{}", level, src_tree);
        if(reuse)
        {
            tree_parsed_from_src_json.clear();
            evt_handler_tree_json.m_stack.m_callbacks = get_callbacks();
            tree_parsed_from_src_json.m_callbacks = get_callbacks();
            if(immutable)
            {
                parse_json_in_arena(&parser_tree_json, filename, c4::to_csubstr(src_tree_json), &tree_parsed_from_src_json);
            }
            else
            {
                parse_json_in_place(&parser_tree_json, filename, c4::to_substr(src_tree_json), &tree_parsed_from_src_json);
            }
        }
        else
        {
            if(immutable)
            {
                tree_parsed_from_src_json = parse_json_in_arena(filename, c4::to_csubstr(src_tree_json));
            }
            else
            {
                tree_parsed_from_src_json = parse_json_in_place(filename, c4::to_substr(src_tree_json));
            }
        }
        _nfo_print_tree("PARSED", tree_parsed_from_src_json);
        ASSERT_FALSE(tree_parsed_from_src_json.empty());
        tree_was_parsed_json = true;
    }

    void parse_yaml_to_events()
    {
        if(events_were_generated)
            return;
        if(prev)
            receive_src(*prev);
        _nfo_logf("level[{}]: parsing source to events:\n{}", level, src_evts);
        evt_str_sink.clear();
        evt_handler_str.reset();
        evt_handler_str.m_stack.m_callbacks = get_callbacks();
        parser_str.parse_in_place_ev(filename, to_substr(src_evts));
        EXPECT_NE(evt_str_sink.size(), 0);
        events_were_generated = true;
    }

    void parse_yaml_to_events_ints()
    {
        using I = extra::ievt::DataType;
        if(events_ints_were_generated)
            return;
        if(prev)
            receive_src(*prev);
        _nfo_logf("level[{}]: parsing source to ints:\n{}", level, src_evts_ints);
        buffer_ints.resize(32);
        int size_estimated = extra::estimate_events_ints_size(to_csubstr(src_evts_ints));
        evt_handler_ints.m_stack.m_callbacks = get_callbacks();
        evt_handler_ints.reset(to_substr(src_evts_ints), to_substr(arena_evts_ints), buffer_ints.data(), (I)buffer_ints.size());
        parser_ints.parse_in_place_ev(filename, to_substr(src_evts_ints));
        EXPECT_GE(size_estimated, evt_handler_ints.required_size_events());
        size_t sz = (size_t)evt_handler_ints.required_size_events();
        if (!evt_handler_ints.fits_buffers())
        {
            buffer_ints.resize(sz);
            arena_evts_ints.resize(evt_handler_ints.required_size_arena());
            src_evts_ints = src_orig;
            evt_handler_ints.reset(to_substr(src_evts_ints), to_substr(arena_evts_ints), buffer_ints.data(), (I)buffer_ints.size());
            parser_ints.parse_in_place_ev(filename, to_substr(src_evts_ints));
            size_t sz2 = (size_t)evt_handler_ints.required_size_events();
            ASSERT_EQ(sz2, sz);
            sz = sz2;
        }
        ASSERT_LE(sz, buffer_ints.size());
        buffer_ints.resize(sz);
        #ifdef RYML_DBG
        extra::events_ints_print(to_csubstr(src_evts_ints), to_substr(arena_evts_ints), buffer_ints.data(), (I)sz);
        #endif
        extra::test_events_ints_invariants(to_csubstr(src_evts_ints), to_substr(arena_evts_ints), buffer_ints.data(), (I)sz);
        EXPECT_GT(evt_handler_ints.required_size_events(), 0);
        extra::events_ints_to_testsuite(to_csubstr(src_evts_ints), to_substr(arena_evts_ints), buffer_ints.data(), (I)buffer_ints.size(), &evts_test_suite_from_ints);
        events_ints_were_generated = true;
    }

    void emit_parsed_tree()
    {
        if(tree_was_emitted)
            return;
        if(!tree_was_parsed)
        {
            _nfo_logf("level[{}] not parsed. parse!", level);
            parse_yaml_to_tree();
        }
        if(!tree_was_parsed_json)
        {
            _nfo_logf("level[{}] json not parsed. parse!", level);
            parse_json_to_tree();
        }
        emitrs_yaml(tree_parsed_from_src, &emitted_from_tree_parsed_from_src);
        emitrs_json(tree_parsed_from_src_json, &emitted_from_tree_parsed_from_src_json);
        csubstr ss = to_csubstr(emitted_from_tree_parsed_from_src);
        if(ss.ends_with("\n...\n"))
            emitted_from_tree_parsed_from_src.resize(emitted_from_tree_parsed_from_src.size() - 4);
        tree_was_emitted = true;
        _nfo_logf("EMITTED:\n{}", emitted_from_tree_parsed_from_src);
    }

    void emit_parsed_tree_json()
    {
        if(tree_was_emitted_json)
            return;
        if(!tree_was_parsed_json)
        {
            _nfo_logf("level[{}] json not parsed. parse!", level);
            parse_json_to_tree();
        }
        emitrs_json(tree_parsed_from_src_json, &emitted_from_tree_parsed_from_src_json);
        tree_was_emitted_json = true;
        _nfo_logf("EMITTED:\n{}", emitted_from_tree_parsed_from_src_json);
    }

    void compare_trees(TestSequenceLevel & that)
    {
        SCOPED_TRACE("compare trees");
        RYML_ASSERT(&that == prev);
        if(!that.tree_was_parsed)
        {
            _nfo_logf("level[{}] not parsed. parse!", that.level);
            that.parse_yaml_to_tree();
        }
        if(!this->tree_was_parsed)
        {
            _nfo_logf("level[{}] not parsed. parse!", level);
            this->parse_yaml_to_tree();
        }
        _nfo_print_tree("PREV_", that.tree_parsed_from_src);
        _nfo_print_tree("CURR", tree_parsed_from_src);
        EXPECT_FALSE(that.tree_parsed_from_src_json.empty());
        EXPECT_FALSE(tree_parsed_from_src_json.empty());
        test_compare(that.tree_parsed_from_src, tree_parsed_from_src);
    }

    void compare_trees_json(TestSequenceLevel & that)
    {
        SCOPED_TRACE("compare trees");
        RYML_ASSERT(&that == prev);
        if(!that.tree_was_parsed_json)
        {
            _nfo_logf("level[{}] not parsed. parse!", that.level);
            that.parse_json_to_tree();
        }
        if(!this->tree_was_parsed_json)
        {
            _nfo_logf("level[{}] not parsed. parse!", level);
            this->parse_json_to_tree();
        }
        _nfo_print_tree("PREV_", that.tree_parsed_from_src_json);
        _nfo_print_tree("CURR", tree_parsed_from_src_json);
        EXPECT_FALSE(that.tree_parsed_from_src_json.empty());
        EXPECT_FALSE(tree_parsed_from_src_json.empty());
        test_compare(that.tree_parsed_from_src_json, tree_parsed_from_src_json);
    }

    void compare_emitted_yaml(TestSequenceLevel & that)
    {
        SCOPED_TRACE("compare emitted");
        RYML_ASSERT(&that == prev);
        if(!that.tree_was_emitted)
        {
            _nfo_logf("level[{}] not emitted. emit!", that.level);
            that.emit_parsed_tree();
        }
        if(!this->tree_was_emitted)
        {
            _nfo_logf("level[{}] not emitted. emit!", level);
            this->emit_parsed_tree();
        }
        _nfo_logf("level[{}]: EMITTED:\n{}", that.level, that.emitted_from_tree_parsed_from_src);
        _nfo_logf("level[{}]: EMITTED:\n{}", level, emitted_from_tree_parsed_from_src);
        if(this->emitted_from_tree_parsed_from_src != that.emitted_from_tree_parsed_from_src)
        {
            // workaround for lack of idempotency in tag normalization.
            Tree from_prev = parse_in_arena(to_csubstr(that.emitted_from_tree_parsed_from_src));
            Tree from_this = parse_in_arena(to_csubstr(emitted_from_tree_parsed_from_src));
            from_prev.resolve_tags();
            from_this.resolve_tags();
            test_compare(from_prev, from_this);
        }
    }

    void compare_emitted_json(TestSequenceLevel & that)
    {
        SCOPED_TRACE("compare emitted");
        RYML_ASSERT(&that == prev);
        if(!that.tree_was_emitted_json)
        {
            _nfo_logf("level[{}] not emitted. emit!", that.level);
            that.emit_parsed_tree_json();
        }
        if(!this->tree_was_emitted_json)
        {
            _nfo_logf("level[{}] not emitted. emit!", level);
            this->emit_parsed_tree_json();
        }
        _nfo_logf("level[{}]: EMITTED:\n{}", that.level, that.emitted_from_tree_parsed_from_src_json);
        _nfo_logf("level[{}]: EMITTED:\n{}", level, emitted_from_tree_parsed_from_src_json);
        if(this->emitted_from_tree_parsed_from_src_json != that.emitted_from_tree_parsed_from_src_json)
        {
            // workaround for lack of idempotency in tag normalization.
            Tree from_prev = parse_json_in_arena(to_csubstr(that.emitted_from_tree_parsed_from_src_json));
            Tree from_this = parse_json_in_arena(to_csubstr(emitted_from_tree_parsed_from_src_json));
            test_compare(from_prev, from_this);
        }
    }

    template<class T>
    void log(const char* context, T const& v)
    {
        C4_UNUSED(context);
        C4_UNUSED(v);
        #if RYML_NFO
        constexpr const char sep[] = "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
        c4::log("{}:\n{}{}{}", context, sep, v, sep);
        #endif
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** holds the sequential data for one particular test suite approach. */
struct TestSequenceData
{
    csubstr           casename;
    csubstr           filename;
    TestSequenceLevel levels[NLEVELS] = {};
    AllowedFailure    allowed_failure = {};
    AllowedFailure    has_container_keys = {};
    bool              expect_error = false;

    void init(csubstr casename_, csubstr filename_, csubstr src_, bool immutable_, bool reuse_, bool expect_error_)
    {
        casename = casename_;
        filename = filename_;
        allowed_failure = is_failure_expected(casename);
        has_container_keys = case_has_container_keys(casename);
        size_t level_index = 0;
        TestSequenceLevel *prev = nullptr;
        for(TestSequenceLevel &l : levels)
        {
            l.init(level_index++, prev, filename, src_, immutable_, reuse_);
            prev = &l;
        }
        expect_error = expect_error_;
    }

    csubstr src() const { return c4::to_csubstr(levels[0].src_tree); }
    bool skip() const { return allowed_failure; }

    void parse_yaml_to_tree(size_t num)
    {
        SKIP_IF(allowed_failure);
        for(size_t i = 0; i < num; ++i)
        {
            if(!has_container_keys && !expect_error)
            {
                levels[i].parse_yaml_to_tree();
            }
            else
            {
                ExpectError::check_error([&]{
                    levels[i].parse_yaml_to_tree();
                });
                break; // because we expect error,we cannot go on to the next
            }
        }
    }

    void parse_json_to_tree(size_t num)
    {
        SKIP_IF(allowed_failure);
        for(size_t i = 0; i < num; ++i)
        {
            if(!has_container_keys && !expect_error)
            {
                levels[i].parse_json_to_tree();
            }
            else
            {
                ExpectError::check_error([&]{
                    levels[i].parse_json_to_tree();
                });
                break; // because we expect error,we cannot go on to the next
            }
        }
    }

    void parse_yaml_to_events(size_t num)
    {
        SKIP_IF(allowed_failure);
        //SKIP_IF(has_container_keys); // DO IT!
        for(size_t i = 0; i < num; ++i)
        {
            if(!expect_error)
            {
                levels[i].parse_yaml_to_events();
                if(has_container_keys)
                    break;
            }
            else
            {
                ExpectError::check_error([&]{
                    levels[i].parse_yaml_to_events();
                });
                break; // because we expect error, we cannot go on to the next
            }
        }
    }

    void parse_yaml_to_events_ints(size_t num)
    {
        SKIP_IF(allowed_failure);
        //SKIP_IF(has_container_keys); // DO IT!
        for(size_t i = 0; i < num; ++i)
        {
            if(!expect_error)
            {
                levels[i].parse_yaml_to_events_ints();
                if(has_container_keys)
                    break;
            }
            else
            {
                ExpectError::check_error([&]{
                    levels[i].parse_yaml_to_events_ints();
                });
                break; // because we expect error,we cannot go on to the next
            }
        }
    }

    void emit_tree_parsed_from_src(size_t num)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 0; i < num; ++i)
        {
            if(!levels[i].tree_was_parsed)
                levels[i].parse_yaml_to_tree();
            levels[i].emit_parsed_tree();
            if(i + 1 < num)
                levels[i+1].receive_src(levels[i]);
        }
    }

    void emit_tree_parsed_from_src_json(size_t num)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 0; i < num; ++i)
        {
            if(!levels[i].tree_was_parsed)
                levels[i].parse_json_to_tree();
            levels[i].emit_parsed_tree_json();
            if(i + 1 < num)
                levels[i+1].receive_src_json(levels[i]);
        }
    }

    void compare_level_trees(size_t num)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_trees(levels[i-1]);
    }
    void compare_level_trees_json(size_t num)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_trees_json(levels[i-1]);
    }
    void compare_subject_trees(size_t num, TestSequenceData & other)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_trees(other.levels[i]);
    }
    void compare_subject_trees_json(size_t num, TestSequenceData & other)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_trees_json(other.levels[i]);
    }

    void compare_level_emitted(size_t num)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_emitted_yaml(levels[i-1]);
    }
    void compare_level_emitted_json(size_t num)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_emitted_json(levels[i-1]);
    }
    void compare_subject_emitted(size_t num, TestSequenceData & other)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_emitted_yaml(other.levels[i]);
    }
    void compare_subject_emitted_json(size_t num, TestSequenceData & other)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_emitted_json(other.levels[i]);
    }

    void compare_actual_tree_to_events_tree(TestSuiteCaseEvents *events)
    {
        SKIP_IF(allowed_failure || filename.ends_with(".json"));
        SKIP_IF(has_container_keys);
        events->parse_events(src());
        parse_yaml_to_tree(1);
        events->compare_actual_tree_to_events_src_tree(src(), levels[0].tree_parsed_from_src);
    }

    void compare_event_tree_to_src_tree(TestSuiteCaseEvents *events)
    {
        SKIP_IF(allowed_failure || filename.ends_with(".json"));
        SKIP_IF(has_container_keys);
        events->parse_events(src());
        parse_yaml_to_tree(1);
        events->compare_event_tree_to_src_tree(src(), levels[0].tree_parsed_from_src);
    }
    void compare_emitted_events_str(size_t num, TestSuiteCaseEvents *events)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        events->parse_events(src());
        parse_yaml_to_tree(num);
        for(size_t i = 0; i < num; ++i)
        {
            levels[i].parse_yaml_to_events();
            events->compare_events(levels[i].evt_str_sink,
                                   /*ignore_container_style*/false,
                                   /*ignore_scalar_style*/(num>0));
        }
    }
    void compare_events_str(size_t num, TestSuiteCaseEvents *events)
    {
        ASSERT_EQ(num, 1); // FIXME
        SKIP_IF(allowed_failure);
        parse_yaml_to_events(1);
        events->compare_events(levels[0].evt_str_sink,
                               /*ignore_container_style*/false,
                               /*ignore_scalar_style*/(num>0));
    }
    void compare_events_ints_str(size_t num, TestSuiteCaseEvents *events)
    {
        ASSERT_EQ(num, 1); // FIXME
        SKIP_IF(allowed_failure);
        parse_yaml_to_events_ints(1);
        events->compare_events(to_csubstr(levels[0].evts_test_suite_from_ints),
                               /*ignore_container_style*/false,
                               /*ignore_scalar_style*/false);
    }

    bool m_expected_error_to_tree_checked = false;
    bool m_expected_error_to_events_checked = false;
    bool m_expected_error_to_events_ints_checked = false;
    void check_expected_error()
    {
        SKIP_IF(allowed_failure);
        //SKIP_IF(has_container_keys); // DO IT!
        if(m_expected_error_to_tree_checked)
            return;
        ExpectError::check_error(&levels[0].tree_parsed_from_src, [this]{
            levels[0].parse_yaml_to_tree();
        });
        m_expected_error_to_tree_checked = true;
    }
    void check_expected_error_events()
    {
        SKIP_IF(allowed_failure);
        //SKIP_IF(has_container_keys); // DO IT!
        if(m_expected_error_to_events_checked)
            return;
        ExpectError::check_error([this]{
            levels[0].parse_yaml_to_events();
        });
        m_expected_error_to_events_checked = true;
    }
    void check_expected_error_events_ints()
    {
        SKIP_IF(allowed_failure);
        //SKIP_IF(has_container_keys); // DO IT!
        if(m_expected_error_to_events_ints_checked)
            return;
        ExpectError::check_error([this]{
            levels[0].parse_yaml_to_events_ints();
        });
        m_expected_error_to_events_ints_checked = true;
    }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** This contains the full information parsed from a test in the test suite,
 * and the data required to run the tests.
 *
 * Each test sequence can be made:
 *   - from mutable/immutable yaml source
 *   - with/without parser+tree reuse
 *   - with unix/windows line endings
 */
struct SuiteCase
{
    csubstr     case_title;
    csubstr     case_dir;
    std::string filename;
    std::string file_contents;
    std::string events_filename;
    std::string events_file_contents;

    bool    test_case_expects_error;
    bool    test_case_is_json;

    TestSuiteCaseEvents  events;

    std::string unix_src;
    std::string windows_src;

    TestSequenceData unix_in_arena;
    TestSequenceData unix_in_arena_reuse;
    TestSequenceData unix_in_place;
    TestSequenceData unix_in_place_reuse;

    TestSequenceData windows_in_arena;
    TestSequenceData windows_in_arena_reuse;
    TestSequenceData windows_in_place;
    TestSequenceData windows_in_place_reuse;

    /** loads the several types of tests from an input test suite file */
    SuiteCase(const char *case_title_, const char* case_dir_, const char *input_file)
    {
        using namespace c4;
        using c4::to_csubstr;

        if(to_csubstr(input_file) == "error")
            input_file = "in.yaml";

        case_title = to_csubstr(case_title_);

        case_dir = to_csubstr(case_dir_);
        RYML_CHECK(case_dir.find('\\') == yml::npos);
        C4_CHECK_MSG(fs::dir_exists(case_dir.str), "dir not found: '%s'", case_dir);

        filename = catrs<std::string>(case_dir, '/', to_csubstr(input_file));
        C4_CHECK_MSG(fs::file_exists(filename.c_str()), "file not found: '%s'", filename.c_str());
        log("testing suite case: {} {} ({})", case_title, filename, case_dir);

        std::string errfile = catrs<std::string>(to_csubstr(case_dir_), "/error");
        test_case_expects_error = fs::file_exists(errfile.c_str());
        test_case_is_json = (npos != to_csubstr(input_file).find("in.json"));

        fs::file_get_contents(filename.c_str(), &file_contents);
        _init_seq_data(case_title, to_csubstr(filename), to_csubstr(file_contents), test_case_expects_error);

        events_filename = catrs<std::string>(case_dir, "/test.event");
        C4_CHECK(fs::file_exists(events_filename.c_str()));
        {
            std::string tmp;
            fs::file_get_contents(events_filename.c_str(), &tmp);
            replace_all("\r", "", to_csubstr(tmp), &events_file_contents);
        }
        events.init(to_csubstr(events_filename), to_csubstr(events_file_contents));

        dump("~~~ case: "      , case_title    , "~~~\n",
             "~~~ file: "      , filename      , "~~~\n",
             "~~~ input:\n"    , to_csubstr(unix_in_arena.levels[0].src_tree), "~~~\n",
             "~~~ events:\n"   , events.reference_events    , "~~~\n");
    }


    void _init_seq_data(csubstr casename, csubstr filename_, csubstr src, bool expect_error_)
    {
        src = replace_all("\r", "", src, &unix_src);

        unix_in_arena      .init(casename, filename_, src, /*immutable*/true , /*reuse*/false, expect_error_);
        unix_in_arena_reuse.init(casename, filename_, src, /*immutable*/true , /*reuse*/true , expect_error_);
        unix_in_place      .init(casename, filename_, src, /*immutable*/false, /*reuse*/false, expect_error_);
        unix_in_place_reuse.init(casename, filename_, src, /*immutable*/false, /*reuse*/true , expect_error_);

        src = replace_all("\n", "\r\n", src, &windows_src);

        windows_in_arena      .init(casename, filename_, src, /*immutable*/true , /*reuse*/false, expect_error_);
        windows_in_arena_reuse.init(casename, filename_, src, /*immutable*/true , /*reuse*/true , expect_error_);
        windows_in_place      .init(casename, filename_, src, /*immutable*/false, /*reuse*/false, expect_error_);
        windows_in_place_reuse.init(casename, filename_, src, /*immutable*/false, /*reuse*/true , expect_error_);
    }

};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


// globals holding the test case data. We're using these globals to
// avoid repeating the same read/init task thousands of times per run,
// which was making problems in embedded platforms. The downside is
// that the data structures need to take care of initialization/setup
// status.
SuiteCase* g_suite_case = nullptr;
bool g_do_subcases = true;



#define DEFINE_TESTS(which)                                             \
                                                                        \
                                                                        \
TEST(which##_errors, check_expected_error_src_to_tree)                  \
{                                                                       \
    SKIP_IF(!g_suite_case->test_case_expects_error);                    \
    g_suite_case->which.check_expected_error();                         \
}                                                                       \
                                                                        \
TEST(which##_errors, check_expected_error_src_to_events)                \
{                                                                       \
    SKIP_IF(!g_suite_case->test_case_expects_error);                    \
    g_suite_case->which.check_expected_error_events();                  \
}                                                                       \
                                                                        \
TEST(which##_errors, check_expected_error_src_to_events_ints)           \
{                                                                       \
    SKIP_IF(!g_suite_case->test_case_expects_error);                    \
    g_suite_case->which.check_expected_error_events_ints();             \
}                                                                       \
                                                                        \
                                                                        \
/*-----------------------------------------------*/                     \
                                                                        \
TEST(which##_events_from_src, parse_yaml_to_events)                     \
{                                                                       \
    g_suite_case->which.parse_yaml_to_events(1);                        \
}                                                                       \
                                                                        \
TEST(which##_events_from_src, compare_events_to_ref_events)             \
{                                                                       \
    g_suite_case->which.compare_events_ints_str(1, &g_suite_case->events); \
}                                                                       \
                                                                        \
                                                                        \
/*-----------------------------------------------*/                     \
                                                                        \
TEST(which##_events_ints_from_src, parse_yaml_to_events_ints)           \
{                                                                       \
    g_suite_case->which.parse_yaml_to_events_ints(1);                   \
}                                                                       \
                                                                        \
TEST(which##_events_ints_from_src, compare_events_ints_to_ref_events)   \
{                                                                       \
    g_suite_case->which.compare_events_ints_str(1, &g_suite_case->events); \
}                                                                       \
                                                                        \
                                                                        \
/*-----------------------------------------------*/                     \
                                                                        \
TEST(which##_events_from_tree, compare_actual_tree_to_events_tree)      \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    g_suite_case->which.compare_actual_tree_to_events_tree(&g_suite_case->events); \
}                                                                       \
                                                                        \
TEST(which##_events_from_tree, emit_events)                             \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    g_suite_case->which.compare_event_tree_to_src_tree(&g_suite_case->events); \
}                                                                       \
                                                                        \
                                                                        \
/*-----------------------------------------------*/                     \
                                                                        \
struct which : public ::testing::TestWithParam<size_t>                  \
{                                                                       \
};                                                                      \
                                                                        \
TEST_P(which, 0_parse_yaml_to_events)                                   \
{                                                                       \
    /*ALWAYS COMPARE.~SKIP_IF(g_suite_case->test_case_expects_error);*/ \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    g_suite_case->which.parse_yaml_to_events(1 + GetParam());           \
}                                                                       \
                                                                        \
TEST_P(which, 0_parse_yaml_to_tree)                                     \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    g_suite_case->which.parse_yaml_to_tree(1 + GetParam());             \
}                                                                       \
TEST_P(which, 0_parse_json_to_tree)                                     \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    SKIP_IF( ! g_suite_case->test_case_is_json);                        \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    g_suite_case->which.parse_json_to_tree(1 + GetParam());             \
}                                                                       \
                                                                        \
TEST_P(which, 1_compare_emitted_events_to_ref_events)                   \
{                                                                       \
    /*ALWAYS COMPARE.~SKIP_IF(g_suite_case->test_case_expects_error);*/ \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    g_suite_case->which.compare_emitted_events_str(1 + GetParam(), &g_suite_case->events); \
}                                                                       \
                                                                        \
TEST_P(which, 2_emit_tree_parsed_from_src)                              \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    g_suite_case->which.emit_tree_parsed_from_src(1 + GetParam());      \
}                                                                       \
TEST_P(which, 2_emit_tree_parsed_from_src_json)                         \
{                                                                       \
    SKIP_IF( ! g_suite_case->test_case_is_json);                        \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    g_suite_case->which.emit_tree_parsed_from_src_json(1 + GetParam()); \
}                                                                       \
                                                                        \
TEST_P(which, 3_compare_level_trees)                                    \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    g_suite_case->which.compare_level_trees(1 + GetParam());            \
}                                                                       \
TEST_P(which, 3_compare_level_trees_json)                               \
{                                                                       \
    SKIP_IF( ! g_suite_case->test_case_is_json);                        \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    g_suite_case->which.compare_level_trees_json(1 + GetParam());       \
}                                                                       \
                                                                        \
TEST_P(which, 4_compare_emitted_yaml)                                   \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    g_suite_case->which.compare_level_emitted(1 + GetParam());          \
}                                                                       \
TEST_P(which, 4_compare_emitted_yaml_json)                              \
{                                                                       \
    SKIP_IF( ! g_suite_case->test_case_is_json);                        \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    g_suite_case->which.compare_level_emitted_json(1 + GetParam());     \
}                                                                       \
                                                                        \
                                                                        \
INSTANTIATE_TEST_SUITE_P(_, which, testing::Range<size_t>(0, NLEVELS))

DEFINE_TESTS(unix_in_arena);
DEFINE_TESTS(unix_in_place);
DEFINE_TESTS(unix_in_arena_reuse);
DEFINE_TESTS(unix_in_place_reuse);
DEFINE_TESTS(windows_in_arena);
DEFINE_TESTS(windows_in_place);
DEFINE_TESTS(windows_in_arena_reuse);
DEFINE_TESTS(windows_in_place_reuse);


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/) { return nullptr; }

} // namespace yml
} // namespace c4



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    c4::dump("$");
    for(int i = 0; i < argc; ++i)
        c4::dump(' ', c4::to_csubstr(argv[i]));
    c4::dump("\n");

    // make gtest parse its args
    testing::InitGoogleTest(&argc, argv);

    // now we have only our args to consider
    if(argc != 4)
    {
        log("usage:\n{} <test_name> <test-dir> <input-file>", c4::to_csubstr(argv[0]));
        return 1;
    }

    // load the test case from the suite file
    c4::yml::SuiteCase suite_case(argv[1], argv[2], argv[3]);
    c4::yml::g_suite_case = &suite_case;

    return RUN_ALL_TESTS();
}

C4_SUPPRESS_WARNING_MSVC_PUSH
