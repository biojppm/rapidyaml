#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/detail/checks.hpp>
#endif
#include "test_case.hpp"
#include "test_suite/test_suite_common.hpp"
#include "test_suite/test_suite_parts.hpp"
#include "test_suite/test_suite_events.hpp"
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
 * structure, and then repeatedly parsing and emitting yaml in a sort
 * of pipe. Ie, (eg for in_yaml) parse in_yaml, emit corresponding
 * yaml, then parse this emitted yaml, and so on. Each parse/emit pair
 * is named a processing level in this test. */


C4_SUPPRESS_WARNING_MSVC_PUSH
C4_SUPPRESS_WARNING_MSVC(4702) // unreachable code

#define NLEVELS 4

namespace c4 {
namespace yml {


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct Events
{
    csubstr filename = {};
    std::string src = {};
    std::string emitted_events = {};
    Tree    tree = {};
    mutable Tree adjusted_tree = {};
    mutable Tree tree_from_emitted_events = {};
    bool    was_parsed = false;
    bool    enabled = false;

    void init(csubstr filename_, csubstr src_)
    {
        filename = filename_;
        src.assign(src_.begin(), src_.end());
        tree.clear();
        tree.clear_arena();
        tree.reserve(10);
        was_parsed = false;
        enabled = true;
    }

    void compare_trees(csubstr actual_src, Tree const& actual_tree) const
    {
        if(actual_src.empty())
            GTEST_SKIP();
        _nfo_logf("SRC:\n{}", actual_src);
        _nfo_print_tree("EXPECTED", tree);
        _nfo_print_tree("ACTUAL", actual_tree);
        test_compare(actual_tree, tree);
    }

    void compare_emitted_events(csubstr actual_src, Tree const& tree_from_actual_src)
    {
        C4_UNUSED(actual_src);
        emit_events(&emitted_events, tree_from_actual_src);
        _nfo_logf("EXPECTED_EVENTS:\n{}", src);
        _nfo_logf("ACTUAL_EVENTS:\n{}", emitted_events);
        // we cannot directly compare the event strings,
        // so we create a tree from the emitted events,
        // and then compare the trees:
        tree_from_emitted_events.clear();
        tree_from_emitted_events.reserve(16);
        parser.parse(c4::to_csubstr(emitted_events), &tree_from_emitted_events);
        _nfo_logf("SRC:\n{}", actual_src);
        _nfo_print_tree("ACTUAL_FROM_SOURCE", tree_from_actual_src);
        _nfo_print_tree("ACTUAL_FROM_EMITTED_EVENTS", tree_from_emitted_events);
        _nfo_print_tree("EXPECTED_FROM_EVENTS", tree);
        test_compare(tree_from_emitted_events, tree);
    }

    EventsParser parser;
    void parse_events(csubstr actual_src)
    {
        if(was_parsed)
            return;
        if(actual_src.empty())
            GTEST_SKIP();
        parser.parse(c4::to_csubstr(src), &tree);
        if(tree.empty())
            tree.reserve(10);
        _nfo_print_tree("EXPECTED", tree);
        was_parsed = true;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** a processing level */
struct ProcLevel
{
    size_t          level;
    ProcLevel      *prev;
    csubstr         filename;
    std::string     src;
    c4::yml::Parser parser;
    c4::yml::Tree   tree;
    std::string     emitted;

    bool            immutable = false;
    bool            reuse = false;
    bool            was_parsed = false;
    bool            was_emitted = false;

    void init(size_t level_, ProcLevel *prev_, csubstr filename_, csubstr src_, bool immutable_, bool reuse_)
    {
        level = level_;
        prev = prev_;
        filename = filename_;
        src.assign(src_.begin(), src_.end());
        immutable = immutable_;
        reuse = reuse_;
        was_parsed = false;
        was_emitted = false;
    }

    void receive_src(ProcLevel & prev_)
    {
        RYML_ASSERT(&prev_ == prev);
        if(!prev_.was_emitted)
        {
            _nfo_logf("level[{}] not emitted. emit!", prev_.level);
            prev_.emit();
        }
        if(src != prev_.emitted)
        {
            was_parsed = false;
            was_emitted = false;
            src = prev_.emitted;
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

    void parse()
    {
        if(was_parsed)
            return;
        if(prev)
        {
            receive_src(*prev);
        }
        _nfo_logf("level[{}]: parsing source:\n{}", level, src);
        if(reuse)
        {
            tree.clear();
            if(immutable)
                parser.parse_in_arena(filename, c4::to_csubstr(src), &tree);
            else
                parser.parse_in_place(filename, c4::to_substr(src), &tree);
        }
        else
        {
            if(immutable)
                tree = parse_in_arena(filename, c4::to_csubstr(src));
            else
                tree = parse_in_place(filename, c4::to_substr(src));
        }
        _nfo_print_tree("PARSED", tree);
        tree.resolve_tags();
        _nfo_print_tree("RESOLVED TAGS", tree);
        was_parsed = true;
        //_resolve_if_needed();
    }

    void _resolve_if_needed()
    {
        ConstNodeRef root = tree.rootref();
        bool has_anchors_or_refs = root.visit([](ConstNodeRef const& node, size_t /*level*/){
            return (node.is_anchor() || node.is_ref());
        });
        if(has_anchors_or_refs)
        {
            tree.resolve();
            _nfo_print_tree("RESOLVED", tree);
        }
    }

    void emit()
    {
        if(was_emitted)
            return;
        if(!was_parsed)
        {
            _nfo_logf("level[{}] not parsed. parse!", level);
            parse();
        }
        emitrs(tree, &emitted);
        csubstr ss = to_csubstr(emitted);
        if(ss.ends_with("\n...\n"))
            emitted.resize(emitted.size() - 4);
        was_emitted = true;
        _nfo_logf("EMITTED:\n{}", emitted);
    }

    void compare_trees(ProcLevel & prev_)
    {
        RYML_ASSERT(&prev_ == prev);
        if(!prev_.was_parsed)
        {
            _nfo_logf("level[{}] not parsed. parse!", prev_.level);
            prev_.parse();
        }
        if(!was_parsed)
        {
            _nfo_logf("level[{}] not parsed. parse!", level);
            parse();
        }
        _nfo_print_tree("PREV_", prev_.tree);
        _nfo_print_tree("CURR", tree);
        test_compare(prev_.tree, tree);
    }

    void compare_emitted(ProcLevel & prev_)
    {
        RYML_ASSERT(&prev_ == prev);
        if(!prev_.was_emitted)
        {
            _nfo_logf("level[{}] not emitted. emit!", prev_.level);
            prev_.emit();
        }
        if(!was_emitted)
        {
            _nfo_logf("level[{}] not emitted. emit!", level);
            emit();
        }
        _nfo_logf("level[{}]: EMITTED:\n{}", prev_.level, prev_.emitted);
        _nfo_logf("level[{}]: EMITTED:\n{}", level, emitted);
        if(emitted != prev_.emitted)
        {
            // workaround for lack of idempotency in tag normalization.
            Tree from_prev = parse_in_arena(to_csubstr(prev_.emitted));
            Tree from_this = parse_in_arena(to_csubstr(emitted));
            from_prev.resolve_tags();
            from_this.resolve_tags();
            test_compare(from_prev, from_this);
        }
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** holds data for one particular test suite approach. */
struct Approach
{
    csubstr casename;
    csubstr filename;
    ProcLevel levels[NLEVELS] = {};
    AllowedFailure allowed_failure = {};
    bool enabled = false;
    bool expect_error = false;

    void init(csubstr casename_, csubstr filename_, csubstr src_, bool immutable_, bool reuse_, bool expect_error_)
    {
        casename = casename_;
        filename = filename_;
        allowed_failure = is_failure_expected(casename);
        size_t level_index = 0;
        ProcLevel *prev = nullptr;
        for(ProcLevel &l : levels)
        {
            l.init(level_index++, prev, filename, src_, immutable_, reuse_);
            prev = &l;
        }
        expect_error = expect_error_;
    }

    csubstr src() const { return c4::to_csubstr(levels[0].src); }
    bool skip() const { return allowed_failure; }

    void parse(size_t num, bool emit)
    {
        if(allowed_failure)
            GTEST_SKIP();
        for(size_t i = 0; i < num; ++i)
        {
            levels[i].parse();
            if(emit)
                levels[i].emit();
            if(i + 1 < num)
                levels[i+1].receive_src(levels[i]);
        }
    }

    void compare_trees(size_t num)
    {
        if(allowed_failure)
            GTEST_SKIP();
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_trees(levels[i-1]);
    }
    void compare_trees(size_t num, Approach & other)
    {
        if(allowed_failure)
            GTEST_SKIP();
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_trees(other.levels[i]);
    }

    void compare_emitted(size_t num)
    {
        if(allowed_failure)
            GTEST_SKIP();
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_emitted(levels[i-1]);
    }
    void compare_emitted(size_t num, Approach & other)
    {
        if(allowed_failure)
            GTEST_SKIP();
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_emitted(other.levels[i]);
    }

    void compare_events(Events *events)
    {
        if(allowed_failure || filename.ends_with(".json"))
            GTEST_SKIP();
        events->parse_events(src());
        parse(1, /*emit*/false);
        events->compare_trees(src(), levels[0].tree);
    }

    void compare_emitted_events(Events *events)
    {
        if(allowed_failure || filename.ends_with(".json"))
            GTEST_SKIP();
        events->parse_events(src());
        parse(1, /*emit*/false);
        events->compare_emitted_events(src(), levels[0].tree);
    }

    void check_expected_error()
    {
        if(allowed_failure)
            GTEST_SKIP();
        ExpectError::do_check(&levels[0].tree, [this]{
            levels[0].parse();
        });
    }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** Each approach can be read from mutable/immutable yaml source and
 * with/without reuse. */
struct Subject
{
    Approach unix_arena;
    Approach unix_arena_reuse;
    Approach unix_inplace;
    Approach unix_inplace_reuse;

    Approach windows_arena;
    Approach windows_arena_reuse;
    Approach windows_inplace;
    Approach windows_inplace_reuse;

    std::string unix_src;
    std::string windows_src;

    void init(csubstr casename, csubstr filename, csubstr src, bool expect_error)
    {
        src = replace_all("\r", "", src, &unix_src);

        unix_arena      .init(casename, filename, src, /*immutable*/true , /*reuse*/false, expect_error);
        unix_arena_reuse.init(casename, filename, src, /*immutable*/true , /*reuse*/true , expect_error);
        unix_inplace      .init(casename, filename, src, /*immutable*/false, /*reuse*/false, expect_error);
        unix_inplace_reuse.init(casename, filename, src, /*immutable*/false, /*reuse*/true , expect_error);

        src = replace_all("\n", "\r\n", src, &windows_src);

        windows_arena      .init(casename, filename, src, /*immutable*/true , /*reuse*/false, expect_error);
        windows_arena_reuse.init(casename, filename, src, /*immutable*/true , /*reuse*/true , expect_error);
        windows_inplace      .init(casename, filename, src, /*immutable*/false, /*reuse*/false, expect_error);
        windows_inplace_reuse.init(casename, filename, src, /*immutable*/false, /*reuse*/true , expect_error);
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// some utility functions, used below

/** all the ways to process a test case are available through this
 * class. Tests are defined below and use only one of these. */
struct SuiteCase
{
    csubstr     case_title;
    csubstr     case_dir;
    std::string filename;
    std::string file_contents;
    std::string events_filename;
    std::string events_file_contents;

    Subject input;
    Events  events;
    bool    expect_error;

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
        expect_error = fs::file_exists(errfile.c_str());

        fs::file_get_contents(filename.c_str(), &file_contents);
        input.init(case_title, to_csubstr(filename), to_csubstr(file_contents), expect_error);

        events_filename = catrs<std::string>(case_dir, "/test.event");
        C4_CHECK(fs::file_exists(events_filename.c_str()));
        fs::file_get_contents(events_filename.c_str(), &events_file_contents);
        events.init(to_csubstr(events_filename), to_csubstr(events_file_contents));

        dump("~~~ case: "      , case_title    , "~~~\n",
             "~~~ file: "      , filename      , "~~~\n",
             "~~~ input:\n"    , to_csubstr(input.unix_arena.levels[0].src), "~~~\n",
             "~~~ events:\n"   , events.src    , "~~~\n");
    }

};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


// a global holding the test case data
SuiteCase* g_suite_case = nullptr;
bool g_do_subcases = true;



#define DEFINE_TESTS(which)                                             \
                                                                        \
                                                                        \
struct which : public ::testing::TestWithParam<size_t>                  \
{                                                                       \
};                                                                      \
                                                                        \
                                                                        \
TEST_P(which, parse)                                                    \
{                                                                       \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    if(g_suite_case->expect_error)                                      \
        GTEST_SKIP();                                                   \
    g_suite_case->input.which.parse(1 + GetParam(), false);             \
}                                                                       \
                                                                        \
                                                                        \
TEST_P(which, compare_trees)                                            \
{                                                                       \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    if(g_suite_case->expect_error)                                      \
        GTEST_SKIP();                                                   \
    g_suite_case->input.which.compare_trees(1 + GetParam());            \
}                                                                       \
                                                                        \
                                                                        \
TEST_P(which, emit)                                                     \
{                                                                       \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    if(g_suite_case->expect_error)                                      \
        GTEST_SKIP();                                                   \
    g_suite_case->input.which.parse(1 + GetParam(), true);              \
}                                                                       \
                                                                        \
                                                                        \
TEST_P(which, compare_emitted)                                          \
{                                                                       \
    RYML_CHECK(GetParam() < NLEVELS);                                   \
    if(g_suite_case->expect_error)                                      \
        GTEST_SKIP();                                                   \
    g_suite_case->input.which.compare_emitted(1 + GetParam());          \
}                                                                       \
                                                                        \
/*-----------------------------------------------*/                     \
                                                                        \
TEST(which##_events, compare)                                           \
{                                                                       \
    if(g_suite_case->expect_error)                                      \
        GTEST_SKIP();                                                   \
    g_suite_case->input.which.compare_events(&g_suite_case->events);    \
}                                                                       \
                                                                        \
TEST(which##_events, emit_events)                                       \
{                                                                       \
    if(g_suite_case->expect_error)                                      \
        GTEST_SKIP();                                                   \
    g_suite_case->input.which.compare_emitted_events(&g_suite_case->events); \
}                                                                       \
                                                                        \
/*-----------------------------------------------*/                     \
                                                                        \
TEST(which##_errors, check_expected_error)                              \
{                                                                       \
    if(!g_suite_case->expect_error)                                     \
        GTEST_SKIP();                                                   \
    g_suite_case->input.which.check_expected_error();                   \
}                                                                       \
                                                                        \
                                                                        \
INSTANTIATE_TEST_SUITE_P(_, which, testing::Range<size_t>(0, NLEVELS))


DEFINE_TESTS(unix_arena);
DEFINE_TESTS(unix_inplace);
DEFINE_TESTS(unix_arena_reuse);
DEFINE_TESTS(unix_inplace_reuse);
DEFINE_TESTS(windows_arena);
DEFINE_TESTS(windows_inplace);
DEFINE_TESTS(windows_arena_reuse);
DEFINE_TESTS(windows_inplace_reuse);


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

    return  RUN_ALL_TESTS();
}

C4_SUPPRESS_WARNING_MSVC_PUSH
