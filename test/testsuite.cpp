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
#include "c4/yml/extra/event_handler_ints.hpp"
#include "c4/yml/extra/emitter_ints.hpp"
#include "c4/yml/extra/emitter_ints.def.hpp"
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
        SCOPED_TRACE("compare events");
        EXPECT_FALSE(reference_events.empty());
        EXPECT_FALSE(emitted_events.empty());
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
    std::string         src_ints_resize;
    std::string         src_ints_resize_json;
    std::string         src_ints_noresize;
    std::string         src_ints_noresize_json;
    std::string         src_ints_noresize_orig;
    std::string         src_ints_noresize_orig_json;
    std::string         arena_ints_noresize;
    std::string         arena_ints_noresize_json;
    ParserOptions       parser_options;
    EventHandlerTree    evt_handler_tree;
    EventHandlerTree    evt_handler_tree_json;
    Parser              parser_tree;
    Parser              parser_tree_json;
    Tree                tree_parsed_from_src;
    Tree                tree_parsed_from_src_json;
    std::string         emitted_from_tree_parsed_from_src;
    std::string         emitted_from_tree_parsed_from_src_json;
    std::string         evts_test_suite_from_tree;
    std::string         evts_test_suite_from_tree_json;

    std::string         emitted_from_ints_resize_parsed_from_src;
    std::string         emitted_from_ints_resize_parsed_from_src_json;
    std::string         emitted_from_ints_noresize_parsed_from_src;
    std::string         emitted_from_ints_noresize_parsed_from_src_json;

    using EventHandlerIntsNoResize = extra::ievt::EventHandlerInts<false>;
    using EventHandlerIntsResize = extra::ievt::EventHandlerInts<true>;

    EventHandlerIntsNoResize evt_handler_ints_noresize;
    EventHandlerIntsNoResize evt_handler_ints_noresize_json;
    EventHandlerIntsResize evt_handler_ints_resize;
    EventHandlerIntsResize evt_handler_ints_resize_json;
    ParseEngine<EventHandlerIntsNoResize> parser_ints_noresize;
    ParseEngine<EventHandlerIntsNoResize> parser_ints_noresize_json;
    ParseEngine<EventHandlerIntsResize> parser_ints_resize;
    ParseEngine<EventHandlerIntsResize> parser_ints_resize_json;
    std::vector<extra::ievt::evt_bits> buffer_ints_noresize;
    std::vector<extra::ievt::evt_bits> buffer_ints_noresize_json;
    extra::ievt::Buffers ints_resize;
    extra::ievt::Buffers ints_resize_json;
    std::string evts_test_suite_from_ints_noresize;
    std::string evts_test_suite_from_ints_noresize_json;
    std::string evts_test_suite_from_ints_resize;
    std::string evts_test_suite_from_ints_resize_json;

    bool in_arena = false;
    bool reuse = false;
    bool tree_was_parsed = false;
    bool tree_was_parsed_json = false;
    bool tree_was_emitted = false;
    bool tree_was_emitted_json = false;
    bool ints_were_parsed = false;
    bool ints_were_parsed_json = false;
    bool ints_were_emitted = false;
    bool ints_were_emitted_json = false;

    TestSequenceLevel()
        : parser_options(ParserOptions{}.resolve_tags(true).resolve_tags_all(true))
        , evt_handler_tree()
        , evt_handler_tree_json()
        , parser_tree(&evt_handler_tree, parser_options)
        , parser_tree_json(&evt_handler_tree_json, parser_options)
        , evt_handler_ints_noresize()
        , evt_handler_ints_noresize_json()
        , evt_handler_ints_resize()
        , evt_handler_ints_resize_json()
        , parser_ints_noresize(&evt_handler_ints_noresize, parser_options)
        , parser_ints_noresize_json(&evt_handler_ints_noresize_json, parser_options)
        , parser_ints_resize(&evt_handler_ints_resize, parser_options)
        , parser_ints_resize_json(&evt_handler_ints_resize_json, parser_options)
        , buffer_ints_noresize()
        , buffer_ints_noresize_json()
        , ints_resize()
        , ints_resize_json()
        , evts_test_suite_from_ints_noresize()
        , evts_test_suite_from_ints_noresize_json()
        , evts_test_suite_from_ints_resize()
        , evts_test_suite_from_ints_resize_json()
    {
    }

    void init(size_t level_, TestSequenceLevel *prev_, csubstr filename_, csubstr src_, bool in_arena_, bool reuse_)
    {
        level = level_;
        prev = prev_;
        filename = filename_;
        src_tree.assign(src_.begin(), src_.end());
        src_orig = src_tree;
        src_ints_resize = src_tree;
        src_ints_noresize = src_tree;
        src_ints_noresize_orig = src_tree;
        in_arena = in_arena_;
        reuse = reuse_;
        tree_was_parsed = false;
        tree_was_parsed_json = false;
        tree_was_emitted = false;
        tree_was_emitted_json = false;
        ints_were_parsed = false;
        ints_were_emitted = false;
        ints_were_emitted_json = false;
    }

    // workaround for miscompilation in string assignment op
    static void cpstr(std::string &dst, std::string const& src) noexcept
    {
        dst.resize(src.size());
        if(src.size())
            memcpy(&dst[0], &src[0], src.size()); // NOLINT
    };

    void receive_src_tree(TestSequenceLevel const& prev_)
    {
        RYML_ASSERT_BASIC_(&prev_ == prev);
        ASSERT_TRUE(prev_.tree_was_parsed);
        ASSERT_TRUE(prev_.tree_was_emitted);
        if(src_tree != prev_.emitted_from_tree_parsed_from_src)
        {
            tree_was_parsed = false;
            tree_was_emitted = false;
            cpstr(src_tree, prev_.emitted_from_tree_parsed_from_src);
            cpstr(src_orig, prev_.emitted_from_tree_parsed_from_src);
        }
        // check problem with string assignment op
        ASSERT_EQ(src_tree, prev_.emitted_from_tree_parsed_from_src);
        ASSERT_EQ(src_orig, prev_.emitted_from_tree_parsed_from_src);
    }

    void receive_src_tree_json(TestSequenceLevel const& prev_)
    {
        RYML_ASSERT_BASIC_(&prev_ == prev);
        ASSERT_TRUE(prev_.tree_was_emitted_json);
        if(src_tree_json != prev_.emitted_from_tree_parsed_from_src_json)
        {
            tree_was_parsed_json = false;
            tree_was_emitted_json = false;
            src_tree_json = prev_.emitted_from_tree_parsed_from_src_json;
        }
    }

    void receive_src_ints(TestSequenceLevel const& prev_)
    {
        RYML_ASSERT_BASIC_(&prev_ == prev);
        ASSERT_TRUE(prev_.ints_were_parsed);
        ASSERT_TRUE(prev_.ints_were_emitted);
        if(src_ints_noresize != prev_.emitted_from_ints_noresize_parsed_from_src
           ||
           src_ints_resize != prev_.emitted_from_ints_resize_parsed_from_src)
        {
            ints_were_parsed = false;
            ints_were_emitted = false;
            cpstr(src_ints_resize, prev_.emitted_from_ints_resize_parsed_from_src);
            cpstr(src_ints_noresize, prev_.emitted_from_ints_noresize_parsed_from_src);
        }
        cpstr(src_ints_noresize_orig, prev_.emitted_from_ints_noresize_parsed_from_src);
        // check problem with string assignment op
        ASSERT_EQ(src_ints_resize, prev_.emitted_from_ints_resize_parsed_from_src);
        ASSERT_EQ(src_ints_noresize, prev_.emitted_from_ints_noresize_parsed_from_src);
        ASSERT_EQ(src_ints_noresize_orig, prev_.emitted_from_ints_noresize_parsed_from_src);
    }

    void receive_src_ints_json(TestSequenceLevel const& prev_)
    {
        RYML_ASSERT_BASIC_(&prev_ == prev);
        ASSERT_TRUE(prev_.ints_were_parsed_json);
        ASSERT_TRUE(prev_.ints_were_emitted_json);
        if(src_ints_noresize_json != prev_.emitted_from_ints_noresize_parsed_from_src_json
           ||
           src_ints_resize_json != prev_.emitted_from_ints_resize_parsed_from_src_json)
        {
            ints_were_parsed_json = false;
            ints_were_emitted_json = false;
            cpstr(src_ints_resize_json, prev_.emitted_from_ints_resize_parsed_from_src_json);
            cpstr(src_ints_noresize_json, prev_.emitted_from_ints_noresize_parsed_from_src_json);
        }
        cpstr(src_ints_noresize_orig_json, prev_.emitted_from_ints_noresize_parsed_from_src_json);
        // check problem with string assignment op
        ASSERT_EQ(src_ints_resize_json, prev_.emitted_from_ints_resize_parsed_from_src_json);
        ASSERT_EQ(src_ints_noresize_json, prev_.emitted_from_ints_noresize_parsed_from_src_json);
        ASSERT_EQ(src_ints_noresize_orig_json, prev_.emitted_from_ints_noresize_parsed_from_src_json);
    }

    void parse_yaml_to_tree()
    {
        if(tree_was_parsed)
            return;
        if(prev)
            receive_src_tree(*prev);
        _nfo_logf("level[{}]: parsing source:\n{}", level, src_tree);
        if(reuse)
        {
            tree_parsed_from_src.clear();
            tree_parsed_from_src.clear_arena();
            evt_handler_tree.m_stack.m_callbacks = get_callbacks();
            tree_parsed_from_src.m_callbacks = get_callbacks();
            if(in_arena)
                parse_in_arena(&parser_tree, filename, c4::to_csubstr(src_tree), &tree_parsed_from_src);
            else
                parse_in_place(&parser_tree, filename, c4::to_substr(src_tree), &tree_parsed_from_src);
        }
        else
        {
            if(in_arena)
                tree_parsed_from_src = parse_in_arena(filename, c4::to_csubstr(src_tree), parser_options);
            else
                tree_parsed_from_src = parse_in_place(filename, c4::to_substr(src_tree), parser_options);
        }
        _nfo_print_tree("PARSED", tree_parsed_from_src);
        if(tree_parsed_from_src.num_tag_directives())
        {
            TagCache tag_cache;
            tree_parsed_from_src.resolve_tags(tag_cache);
            _nfo_print_tree("RESOLVED TAGS", tree_parsed_from_src);
        }
        tree_parsed_from_src.normalize_tags_long();
        tree_was_parsed = true;
        emit_events_from_tree(&evts_test_suite_from_tree, tree_parsed_from_src);
    }

    void parse_json_to_tree()
    {
        if(tree_was_parsed_json)
            return;
        if(prev)
            receive_src_tree_json(*prev);
        _nfo_logf("level[{}]: parsing source:\n{}", level, src_tree);
        if(reuse)
        {
            tree_parsed_from_src_json.clear();
            evt_handler_tree_json.m_stack.m_callbacks = get_callbacks();
            tree_parsed_from_src_json.m_callbacks = get_callbacks();
            if(in_arena)
                parse_json_in_arena(&parser_tree_json, filename, c4::to_csubstr(src_tree_json), &tree_parsed_from_src_json);
            else
                parse_json_in_place(&parser_tree_json, filename, c4::to_substr(src_tree_json), &tree_parsed_from_src_json);
        }
        else
        {
            if(in_arena)
                tree_parsed_from_src_json = parse_json_in_arena(filename, c4::to_csubstr(src_tree_json), parser_options);
            else
                tree_parsed_from_src_json = parse_json_in_place(filename, c4::to_substr(src_tree_json), parser_options);
        }
        _nfo_print_tree("PARSED", tree_parsed_from_src_json);
        ASSERT_FALSE(tree_parsed_from_src_json.empty());
        tree_was_parsed_json = true;
        emit_events_from_tree(&evts_test_suite_from_tree_json, tree_parsed_from_src_json);
    }

    void parse_yaml_to_ints()
    {
        if(ints_were_parsed)
            return;
        using I = extra::ievt::evt_bits;
        if(prev)
            receive_src_ints(*prev);
        ints_were_emitted = false;
        {
            SCOPED_TRACE("resize");
            _nfo_logf("level[{}]: parsing source to ints [resize]:\n{}", level, src_ints_resize);
            evt_handler_ints_resize.m_stack.m_callbacks = get_callbacks();
            evt_handler_ints_resize.reset(to_substr(src_ints_resize), ints_resize.arena, ints_resize.evts.ptr, ints_resize.evts.len);
            parser_ints_resize.parse_in_place_ev(filename, to_substr(src_ints_resize));
            ints_resize = evt_handler_ints_resize.get_buffers();
            #ifdef RYML_DBG
            extra::ievt::events_ints_print(to_csubstr(src_ints_resize), ints_resize.arena, ints_resize.evts.ptr, ints_resize.evts.len);
            #endif
            extra::ievt::test_events_ints_invariants(to_csubstr(src_ints_resize), ints_resize.arena, ints_resize.evts.ptr, ints_resize.evts.len);
            EXPECT_GT(evt_handler_ints_resize.required_size_events(), 0);
            extra::ievt::events_ints_to_testsuite(to_csubstr(src_ints_resize), ints_resize.arena, ints_resize.evts.ptr, ints_resize.evts.len,
                                                  &evts_test_suite_from_ints_resize);
        }
        {
            _nfo_logf("level[{}]: parsing source to ints [noresize]:\n{}", level, src_ints_noresize);
            SCOPED_TRACE("noresize");
            buffer_ints_noresize.resize(32);
            int size_estimated = extra::ievt::estimate_events_size(to_csubstr(src_ints_noresize));
            evt_handler_ints_noresize.m_stack.m_callbacks = get_callbacks();
            evt_handler_ints_noresize.reset(to_substr(src_ints_noresize), to_substr(arena_ints_noresize), buffer_ints_noresize.data(), (I)buffer_ints_noresize.size());
            parser_ints_noresize.parse_in_place_ev(filename, to_substr(src_ints_noresize));
            EXPECT_GE(size_estimated, evt_handler_ints_noresize.required_size_events());
            size_t szi = (size_t)evt_handler_ints_noresize.required_size_events();
            size_t sza = evt_handler_ints_noresize.required_size_arena();
            if (!evt_handler_ints_noresize.fits_buffers())
            {
                buffer_ints_noresize.resize(szi);
                arena_ints_noresize.resize(sza);
                src_ints_noresize = src_ints_noresize_orig;
                evt_handler_ints_noresize.reset(to_substr(src_ints_noresize), to_substr(arena_ints_noresize), buffer_ints_noresize.data(), (I)buffer_ints_noresize.size());
                parser_ints_noresize.parse_in_place_ev(filename, to_substr(src_ints_noresize));
                size_t szi2 = (size_t)evt_handler_ints_noresize.required_size_events();
                size_t sza2 = evt_handler_ints_noresize.required_size_arena();
                ASSERT_EQ(szi2, szi);
                ASSERT_EQ(sza2, sza);
                szi = szi2;
                ASSERT_EQ((size_t)evt_handler_ints_noresize.required_size_events(), buffer_ints_noresize.size());
                ASSERT_EQ(evt_handler_ints_noresize.required_size_arena(), arena_ints_noresize.size());
                ASSERT_TRUE(evt_handler_ints_noresize.fits_buffers());
            }
            ASSERT_LE(szi, buffer_ints_noresize.size());
            buffer_ints_noresize.resize(szi);
            #ifdef RYML_DBG
            extra::ievt::events_ints_print(to_csubstr(src_ints_noresize), to_csubstr(arena_ints_noresize), buffer_ints_noresize.data(), (I)sz);
            #endif
            extra::ievt::test_events_ints_invariants(to_csubstr(src_ints_noresize), to_csubstr(arena_ints_noresize), buffer_ints_noresize.data(), (I)szi);
            EXPECT_GT(evt_handler_ints_noresize.required_size_events(), 0);
            extra::ievt::events_ints_to_testsuite(to_csubstr(src_ints_noresize), to_csubstr(arena_ints_noresize), buffer_ints_noresize.data(), (I)buffer_ints_noresize.size(),
                                                  &evts_test_suite_from_ints_noresize);
        }
        ints_were_parsed = true;
    }

    void parse_json_to_ints()
    {
        if(ints_were_parsed_json)
            return;
        using I = extra::ievt::evt_bits;
        if(prev)
            receive_src_ints_json(*prev);
        ints_were_emitted_json = false;
        {
            SCOPED_TRACE("resize");
            _nfo_logf("level[{}]: parsing source to ints [resize]:\n{}", level, src_ints_resize);
            evt_handler_ints_resize_json.m_stack.m_callbacks = get_callbacks();
            evt_handler_ints_resize_json.reset(to_substr(src_ints_resize_json), ints_resize_json.arena, ints_resize_json.evts.ptr, ints_resize_json.evts.len);
            parser_ints_resize_json.parse_json_in_place_ev(filename, to_substr(src_ints_resize_json));
            ints_resize_json = evt_handler_ints_resize_json.get_buffers();
            #ifdef RYML_DBG
            extra::ievt::events_ints_print(to_csubstr(src_ints_resize_json), ints_resize_json.arena, ints_resize_json.evts.ptr, ints_resize_json.evts.len);
            #endif
            extra::ievt::test_events_ints_invariants(to_csubstr(src_ints_resize_json), ints_resize_json.arena, ints_resize_json.evts.ptr, ints_resize_json.evts.len);
            EXPECT_GT(evt_handler_ints_resize_json.required_size_events(), 0);
            extra::ievt::events_ints_to_testsuite(to_csubstr(src_ints_resize_json), ints_resize_json.arena, ints_resize_json.evts.ptr, ints_resize_json.evts.len,
                                                  &evts_test_suite_from_ints_resize_json);
        }
        {
            _nfo_logf("level[{}]: parsing source to ints [noresize]:\n{}", level, src_ints_noresize);
            SCOPED_TRACE("noresize");
            buffer_ints_noresize_json.resize(32);
            int size_estimated = extra::ievt::estimate_events_size(to_csubstr(src_ints_noresize_json));
            evt_handler_ints_noresize_json.m_stack.m_callbacks = get_callbacks();
            evt_handler_ints_noresize_json.reset(to_substr(src_ints_noresize_json), to_substr(arena_ints_noresize_json), buffer_ints_noresize_json.data(), (I)buffer_ints_noresize_json.size());
            parser_ints_noresize_json.parse_json_in_place_ev(filename, to_substr(src_ints_noresize_json));
            EXPECT_GE(size_estimated, evt_handler_ints_noresize_json.required_size_events());
            size_t szi = (size_t)evt_handler_ints_noresize_json.required_size_events();
            size_t sza = evt_handler_ints_noresize_json.required_size_arena();
            if (!evt_handler_ints_noresize_json.fits_buffers())
            {
                buffer_ints_noresize_json.resize(szi);
                arena_ints_noresize_json.resize(sza);
                src_ints_noresize_json = src_ints_noresize_orig_json;
                evt_handler_ints_noresize_json.reset(to_substr(src_ints_noresize_json), to_substr(arena_ints_noresize_json), buffer_ints_noresize_json.data(), (I)buffer_ints_noresize_json.size());
                parser_ints_noresize_json.parse_json_in_place_ev(filename, to_substr(src_ints_noresize_json));
                size_t szi2 = (size_t)evt_handler_ints_noresize_json.required_size_events();
                size_t sza2 = evt_handler_ints_noresize_json.required_size_arena();
                ASSERT_EQ(szi2, szi);
                ASSERT_EQ(sza2, sza);
                szi = szi2;
                ASSERT_EQ((size_t)evt_handler_ints_noresize_json.required_size_events(), buffer_ints_noresize_json.size());
                ASSERT_EQ(evt_handler_ints_noresize_json.required_size_arena(), arena_ints_noresize_json.size());
                ASSERT_TRUE(evt_handler_ints_noresize_json.fits_buffers());
            }
            ASSERT_LE(szi, buffer_ints_noresize_json.size());
            buffer_ints_noresize_json.resize(szi);
            #ifdef RYML_DBG
            extra::ievt::events_ints_print(to_csubstr(src_ints_noresize_json), to_csubstr(arena_ints_noresize_json), buffer_ints_noresize_json.data(), (I)sz);
            #endif
            extra::ievt::test_events_ints_invariants(to_csubstr(src_ints_noresize_json), to_csubstr(arena_ints_noresize_json), buffer_ints_noresize_json.data(), (I)szi);
            EXPECT_GT(evt_handler_ints_noresize_json.required_size_events(), 0);
            extra::ievt::events_ints_to_testsuite(to_csubstr(src_ints_noresize_json), to_csubstr(arena_ints_noresize_json), buffer_ints_noresize_json.data(), (I)buffer_ints_noresize_json.size(),
                                                  &evts_test_suite_from_ints_noresize_json);
        }
        ints_were_parsed_json = true;
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
        emitrs_yaml(tree_parsed_from_src, &emitted_from_tree_parsed_from_src);
        csubstr ss = to_csubstr(emitted_from_tree_parsed_from_src);
        if(ss.ends_with("\n...\n"))
            emitted_from_tree_parsed_from_src.resize(emitted_from_tree_parsed_from_src.size() - 4);
        tree_was_emitted = true;
        _nfo_logf("EMITTED[tree]:\n{}", emitted_from_tree_parsed_from_src);
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
        _nfo_logf("EMITTED[treejson]:\n{}", emitted_from_tree_parsed_from_src_json);
    }

    void emit_ints()
    {
        if(ints_were_emitted)
            return;
        if(!ints_were_parsed)
        {
            _nfo_logf("level[{}] not parsed. parse!", level);
            parse_yaml_to_ints();
        }
        emit_ints2str(&emitted_from_ints_resize_parsed_from_src,
                      ints_resize.evts.ptr, ints_resize.evts.len,
                      to_csubstr(src_ints_resize),
                      ints_resize.arena,
                      EMIT_YAML);
        emit_ints2str(&emitted_from_ints_noresize_parsed_from_src,
                      buffer_ints_noresize.data(), (extra::ievt::evt_size)buffer_ints_noresize.size(),
                      to_csubstr(src_ints_noresize),
                      to_csubstr(arena_ints_noresize),
                      EMIT_YAML);
        ints_were_emitted = true;
        _nfo_logf("EMITTED[resize]:\n{}", emitted_from_ints_resize_parsed_from_src);
        _nfo_logf("EMITTED[noresize]:\n{}", emitted_from_ints_noresize_parsed_from_src);
    }

    void emit_ints_json()
    {
        if(ints_were_emitted_json)
            return;
        if(!ints_were_parsed_json)
        {
            _nfo_logf("level[{}] not parsed. parse!", level);
            parse_json_to_ints();
        }
        emit_ints2str(&emitted_from_ints_resize_parsed_from_src_json,
                      ints_resize_json.evts.ptr, ints_resize_json.evts.len,
                      to_csubstr(src_ints_resize_json),
                      ints_resize_json.arena,
                      EMIT_JSON);
        emit_ints2str(&emitted_from_ints_noresize_parsed_from_src_json,
                      buffer_ints_noresize_json.data(), (extra::ievt::evt_size)buffer_ints_noresize_json.size(),
                      to_csubstr(src_ints_noresize_json),
                      to_csubstr(arena_ints_noresize_json),
                      EMIT_JSON);
        ints_were_emitted_json = true;
        _nfo_logf("EMITTED[resize]:\n{}", emitted_from_ints_resize_parsed_from_src_json);
        _nfo_logf("EMITTED[noresize]:\n{}", emitted_from_ints_noresize_parsed_from_src_json);
    }

    static void emit_ints2str(std::string *s,
                              extra::ievt::evt_bits const* evts, extra::ievt::evt_size evts_size,
                              csubstr src, csubstr arena,
                              EmitType_e ety)
    {
        ASSERT_FALSE(src.overlaps(to_substr(*s)));
        ASSERT_FALSE(arena.overlaps(to_substr(*s)));
        auto doit = [&](std::string *ss) -> size_t {
            extra::ievt::EmitterInts<WriterBuf> em(EmitOptions{}, to_substr(*ss));
            em.emit_as(ety, evts, evts_size, src, arena);
            return em.m_pos;
        };
        size_t needed = doit(s);
        bool ok = needed <= s->size();
        s->resize(needed);
        if(!ok)
        {
            size_t needed2 = doit(s);
            ASSERT_EQ(needed2, needed);
        }
    }

    void compare_trees(TestSequenceLevel const& that)
    {
        SCOPED_TRACE("compare_trees");
        RYML_ASSERT_BASIC_(&that == prev);
        ASSERT_TRUE(that.tree_was_parsed);
        ASSERT_TRUE(that.tree_was_emitted);
        ASSERT_TRUE(this->tree_was_parsed);
        ASSERT_TRUE(this->tree_was_emitted);
        _nfo_print_tree("PREV_", that.tree_parsed_from_src);
        _nfo_print_tree("CURR", tree_parsed_from_src);
        EXPECT_FALSE(that.tree_parsed_from_src_json.empty());
        EXPECT_FALSE(tree_parsed_from_src_json.empty());
        test_compare(that.tree_parsed_from_src, tree_parsed_from_src);
    }

    void compare_trees_json(TestSequenceLevel const& that)
    {
        SCOPED_TRACE("compare_trees_json");
        RYML_ASSERT_BASIC_(&that == prev);
        ASSERT_TRUE(that.tree_was_parsed_json);
        ASSERT_TRUE(that.tree_was_emitted_json);
        ASSERT_TRUE(this->tree_was_parsed_json);
        ASSERT_TRUE(this->tree_was_emitted_json);
        _nfo_print_tree("PREV_", that.tree_parsed_from_src_json);
        _nfo_print_tree("CURR", tree_parsed_from_src_json);
        EXPECT_FALSE(that.tree_parsed_from_src_json.empty());
        EXPECT_FALSE(tree_parsed_from_src_json.empty());
        test_compare(that.tree_parsed_from_src_json, tree_parsed_from_src_json);
    }

    void compare_emitted_yaml(TestSequenceLevel const& that)
    {
        SCOPED_TRACE("compare_emitted_yaml");
        RYML_ASSERT_BASIC_(&that == prev);
        ASSERT_TRUE(that.tree_was_parsed);
        ASSERT_TRUE(that.tree_was_emitted);
        ASSERT_TRUE(this->tree_was_parsed);
        ASSERT_TRUE(this->tree_was_emitted);
        _nfo_logf("level[{}]: EMITTED:\n{}", that.level, that.emitted_from_tree_parsed_from_src);
        _nfo_logf("level[{}]: EMITTED:\n{}", level, emitted_from_tree_parsed_from_src);
        // workaround for lack of idempotency in tag normalization.
        if(this->emitted_from_tree_parsed_from_src != that.emitted_from_tree_parsed_from_src)
        {
            Tree from_prev = parse_in_arena(to_csubstr(that.emitted_from_tree_parsed_from_src), parser_options);
            Tree from_this = parse_in_arena(to_csubstr(emitted_from_tree_parsed_from_src), parser_options);
            TagCache tag_cache;
            from_prev.resolve_tags(tag_cache);
            from_this.resolve_tags(tag_cache);
            test_compare(from_prev, from_this);
        }
    }

    void compare_emitted_json(TestSequenceLevel const& that)
    {
        SCOPED_TRACE("compare emitted");
        RYML_ASSERT_BASIC_(&that == prev);
        ASSERT_TRUE(that.tree_was_parsed_json);
        ASSERT_TRUE(that.tree_was_emitted_json);
        ASSERT_TRUE(this->tree_was_parsed_json);
        ASSERT_TRUE(this->tree_was_emitted_json);
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

    void print_src_and_tree() const
    {
        printf("src_orig:\n~~~\n%s~~~\n", src_orig.c_str());
        if(tree_was_parsed)
            print_tree(tree_parsed_from_src);
        if(tree_was_emitted)
            printf("emitted:\n~~~\n%s~~~\n", emitted_from_tree_parsed_from_src.c_str());
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
                if(i)
                    levels[i-1].emit_parsed_tree();
                levels[i].parse_yaml_to_tree();
            }
            else
            {
                ExpectError::check_error_parse([&]{
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
                if(i)
                    levels[i-1].emit_parsed_tree_json();
                levels[i].parse_json_to_tree();
            }
            else
            {
                ExpectError::check_error_parse([&]{
                    levels[i].parse_json_to_tree();
                });
                break; // because we expect error,we cannot go on to the next
            }
        }
    }

    void parse_yaml_to_ints(size_t num)
    {
        SKIP_IF(allowed_failure);
        //SKIP_IF(has_container_keys); // DO IT!
        for(size_t i = 0; i < num; ++i)
        {
            if(!expect_error)
            {
                if(i)
                    levels[i-1].emit_ints();
                levels[i].parse_yaml_to_ints();
            }
            else
            {
                ExpectError::check_error_parse([&]{
                    levels[i].parse_yaml_to_ints();
                });
                break; // because we expect error,we cannot go on to the next
            }
        }
    }

    void parse_json_to_ints(size_t num)
    {
        SKIP_IF(allowed_failure);
        //SKIP_IF(has_container_keys); // DO IT!
        for(size_t i = 0; i < num; ++i)
        {
            if(!expect_error)
            {
                if(i)
                    levels[i-1].emit_ints_json();
                levels[i].parse_json_to_ints();
            }
            else
            {
                ExpectError::check_error_parse([&]{
                    levels[i].parse_json_to_ints();
                });
                break; // because we expect error,we cannot go on to the next
            }
        }
    }

    void emit_ints_parsed_from_src(size_t num)
    {
        SKIP_IF(allowed_failure);
        for(size_t i = 0; i < num; ++i)
        {
            if(!levels[i].ints_were_parsed)
                levels[i].parse_yaml_to_ints();
            if(!levels[i].ints_were_emitted)
                levels[i].emit_ints();
        }
    }

    void emit_ints_parsed_from_src_json(size_t num)
    {
        SKIP_IF(allowed_failure);
        for(size_t i = 0; i < num; ++i)
        {
            if(!levels[i].ints_were_parsed_json)
                levels[i].parse_json_to_ints();
            if(!levels[i].ints_were_emitted_json)
                levels[i].emit_ints_json();
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
            if(!levels[i].tree_was_emitted)
                levels[i].emit_parsed_tree();
        }
    }

    void emit_tree_parsed_from_src_json(size_t num)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        for(size_t i = 0; i < num; ++i)
        {
            if(!levels[i].tree_was_parsed_json)
                levels[i].parse_json_to_tree();
            levels[i].emit_parsed_tree_json();
        }
    }

    void compare_level_trees(size_t num)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        emit_tree_parsed_from_src(num);
        for(size_t i = 1; i < num; ++i)
        {
            RYML_TRACE_FMT("compare_level_trees: {} vs {}", i, i-1);
            levels[i].compare_trees(levels[i-1]);
            if(testing::Test::HasFailure())
            {
                for(size_t j = 0; j <= i; ++j)
                {
                    printf("level[%zu]:\n", j);
                    levels[j].print_src_and_tree();
                    printf("(endlevel[%zu])\n", j);
                }
                break;
            }
        }
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

    void compare_emitted_events_tree_str(size_t num, TestSuiteCaseEvents *events)
    {
        SKIP_IF(allowed_failure);
        SKIP_IF(has_container_keys);
        SCOPED_TRACE("compare_emitted_events_tree_str");
        events->parse_events(src());
        parse_yaml_to_tree(num);
        for(size_t i = 0; i < num; ++i)
        {
            RYML_TRACE_FMT("level={}", i);
            ASSERT_TRUE(levels[i].tree_was_parsed);
            events->compare_events(to_csubstr(levels[i].evts_test_suite_from_tree),
                                   /*ignore_container_style*/false,
                                   /*ignore_scalar_style*/(num>0));
        }
    }
    void compare_emitted_events_ints_str(size_t num, TestSuiteCaseEvents *events)
    {
        SKIP_IF(allowed_failure);
        SCOPED_TRACE("compare_emitted_events_ints_str");
        ASSERT_FALSE(events->reference_events.empty());
        parse_yaml_to_ints(num);
        for(size_t i = 0; i < num; ++i)
        {
            RYML_TRACE_FMT("level={}", i);
            ASSERT_TRUE(levels[i].ints_were_parsed);
            {
                SCOPED_TRACE("resize");
                events->compare_events(to_csubstr(levels[i].evts_test_suite_from_ints_resize),
                                       /*ignore_container_style*/false,
                                       /*ignore_scalar_style*/(num>0));
                if(testing::Test::HasFailure())
                {
                    extra::ievt::events_ints_print(to_csubstr(levels[i].src_ints_resize),
                                                   levels[i].ints_resize.arena,
                                                   levels[i].ints_resize.evts.ptr,
                                                   levels[i].ints_resize.evts.len);
                    break;
                }
            }
            {
                SCOPED_TRACE("noresize");
                events->compare_events(to_csubstr(levels[i].evts_test_suite_from_ints_noresize),
                                       /*ignore_container_style*/false,
                                       /*ignore_scalar_style*/(num>0));
                if(testing::Test::HasFailure())
                {
                    extra::ievt::events_ints_print(to_csubstr(levels[i].src_ints_noresize),
                                                   to_csubstr(levels[i].arena_ints_noresize),
                                                   levels[i].buffer_ints_noresize.data(),
                                                   (int)levels[i].buffer_ints_noresize.size());
                    break;
                }
            }
        }
    }

    bool m_expected_error_to_tree_checked = false;
    bool m_expected_error_to_ints_checked = false;
    bool m_expected_error_to_events_checked = false;
    void check_expected_error()
    {
        SKIP_IF(allowed_failure);
        //SKIP_IF(has_container_keys); // DO IT!
        if(m_expected_error_to_tree_checked)
            return;
        SCOPED_TRACE("check_expected_error");
        ExpectError::check_error_parse(&levels[0].tree_parsed_from_src, [this]{
            levels[0].parse_yaml_to_tree();
        });
        m_expected_error_to_tree_checked = true;
    }
    void check_expected_error_events_ints()
    {
        SKIP_IF(allowed_failure);
        //SKIP_IF(has_container_keys); // DO IT!
        if(m_expected_error_to_ints_checked)
            return;
        SCOPED_TRACE("check_expected_error_events");
        ExpectError::check_error_parse([this]{
            levels[0].parse_yaml_to_ints();
        });
        m_expected_error_to_ints_checked = true;
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
        RYML_CHECK_BASIC_(case_dir.find('\\') == yml::npos);
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
    SCOPED_TRACE("check_expected_error_src_to_tree");                   \
    g_suite_case->which.check_expected_error();                         \
}                                                                       \
                                                                        \
TEST(which##_errors, check_expected_error_src_to_events_ints)           \
{                                                                       \
    SKIP_IF(!g_suite_case->test_case_expects_error);                    \
    SCOPED_TRACE("check_expected_error_src_to_events_ints");            \
    g_suite_case->which.check_expected_error_events_ints();             \
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
TEST_P(which, 0_parse_yaml_to_ints)                                     \
{                                                                       \
    /*ALWAYS COMPARE.~SKIP_IF(g_suite_case->test_case_expects_error);*/ \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.parse_yaml_to_ints(1 + GetParam());             \
}                                                                       \
TEST_P(which, 0_parse_json_to_ints)                                     \
{                                                                       \
    SKIP_IF( ! g_suite_case->test_case_is_json);                        \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.parse_json_to_ints(1 + GetParam());             \
}                                                                       \
TEST_P(which, 0_parse_yaml_to_tree)                                     \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.parse_yaml_to_tree(1 + GetParam());             \
}                                                                       \
TEST_P(which, 0_parse_json_to_tree)                                     \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    SKIP_IF( ! g_suite_case->test_case_is_json);                        \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.parse_json_to_tree(1 + GetParam());             \
}                                                                       \
                                                                        \
                                                                        \
TEST_P(which, 1_compare_emitted_events_tree_to_ref_events)              \
{                                                                       \
    /*ALWAYS COMPARE.~SKIP_IF(g_suite_case->test_case_expects_error);*/ \
    SKIP_IF(g_suite_case->test_case_is_json);                           \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.compare_emitted_events_tree_str(1 + GetParam(), &g_suite_case->events); \
}                                                                       \
TEST_P(which, 1_compare_emitted_events_ints_to_ref_events)              \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_is_json);                           \
    /*ALWAYS COMPARE.~SKIP_IF(g_suite_case->test_case_expects_error);*/ \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.compare_emitted_events_ints_str(1 + GetParam(), &g_suite_case->events); \
}                                                                       \
                                                                        \
                                                                        \
TEST_P(which, 2_emit_ints_parsed_from_src)                              \
{                                                                       \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.emit_ints_parsed_from_src(1 + GetParam());      \
}                                                                       \
TEST_P(which, 2_emit_ints_parsed_from_src_json)                         \
{                                                                       \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    SKIP_IF( ! g_suite_case->test_case_is_json);                        \
    g_suite_case->which.emit_ints_parsed_from_src_json(1 + GetParam()); \
}                                                                       \
TEST_P(which, 2_emit_tree_parsed_from_src)                              \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.emit_tree_parsed_from_src(1 + GetParam());      \
}                                                                       \
TEST_P(which, 2_emit_tree_parsed_from_src_json)                         \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    SKIP_IF( ! g_suite_case->test_case_is_json);                        \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.emit_tree_parsed_from_src_json(1 + GetParam()); \
}                                                                       \
                                                                        \
TEST_P(which, 3_compare_level_trees)                                    \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.compare_level_trees(1 + GetParam());            \
}                                                                       \
TEST_P(which, 3_compare_level_trees_json)                               \
{                                                                       \
    SKIP_IF( ! g_suite_case->test_case_is_json);                        \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.compare_level_trees_json(1 + GetParam());       \
}                                                                       \
                                                                        \
TEST_P(which, 4_compare_emitted_yaml)                                   \
{                                                                       \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
    g_suite_case->which.compare_level_emitted(1 + GetParam());          \
}                                                                       \
TEST_P(which, 4_compare_emitted_json)                                   \
{                                                                       \
    SKIP_IF( ! g_suite_case->test_case_is_json);                        \
    SKIP_IF(g_suite_case->test_case_expects_error);                     \
    RYML_CHECK_BASIC_(GetParam() < NLEVELS);                            \
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
    c4::yml::ensure_callbacks(); // needed when RYML_NO_DEFAULT_CALLBACKS
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
