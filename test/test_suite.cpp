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
        test_compare(tree, actual_tree);
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
    csubstr         filename;
    std::string     src;
    c4::yml::Parser parser;
    c4::yml::Tree   tree;
    std::string     emitted;

    bool            immutable = false;
    bool            reuse = false;
    bool            was_parsed = false;
    bool            was_emitted = false;

    void init(csubstr filename_, csubstr src_, bool immutable_, bool reuse_)
    {
        filename = filename_;
        src.assign(src_.begin(), src_.end());
        immutable = immutable_;
        reuse = reuse_;
        was_parsed = false;
        was_emitted = false;
    }

    void receive_src(ProcLevel & prev)
    {
        if(!prev.was_emitted)
            prev.emit();
        if(src != prev.emitted)
        {
            was_parsed = false;
            was_emitted = false;
            src = prev.emitted;
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
        log("parsing source", src);
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
                tree = c4::yml::parse_in_arena(filename, c4::to_csubstr(src));
            else
                tree = c4::yml::parse_in_place(filename, c4::to_substr(src));
        }
        was_parsed = true;
        #if RYML_NFO
        c4::yml::print_tree(tree);
        #endif
        //_resolve_if_needed();
    }

    void _resolve_if_needed()
    {
        const NodeRef root = tree.rootref();
        bool has_anchors_or_refs = root.visit([](NodeRef const* node, size_t /*level*/){
            return (node->is_anchor() || node->is_ref());
        });
        if(has_anchors_or_refs)
        {
            tree.resolve();
            #if RYML_NFO
            c4::yml::print_tree(tree);
            #endif
        }
    }

    void emit()
    {
        if(was_emitted)
            return;
        if(!was_parsed)
            parse();
        emitrs(tree, &emitted);
        auto ss = c4::to_csubstr(emitted);
        if(ss.ends_with("\n...\n"))
            emitted.resize(emitted.size() - 4);
        log("emitted YAML", emitted);
        was_emitted = true;
        #if RYML_NFO
        c4::log("EMITTED:\n{}", emitted);
        #endif
    }

    void compare_trees(ProcLevel const& prev)
    {
        if(!was_parsed)
            parse();
        #if RYML_NFO
        c4::print("PREV:"); print_tree(prev.tree);
        c4::print("CURR:"); print_tree(tree);
        #endif
        test_compare(tree, prev.tree);
    }

    void compare_emitted(ProcLevel const& prev)
    {
        if(!was_emitted)
            emit();
        #if RYML_NFO
        c4::log("PREV:\n{}", prev.emitted);
        c4::log("CURR:\n{}", emitted);
        #endif
        EXPECT_EQ(emitted, prev.emitted);
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** holds data for one particular test suite approach. */
struct Approach
{
    ProcLevel levels[NLEVELS] = {};
    CasePart_e case_part = CPART_NONE;
    AllowedFailure allowed_failure = {};
    AllowedFailure allowed_failure_events = {};
    AllowedFailure allowed_failure_errors = {};
    bool enabled = false;
    bool expect_error = false;

    void init(csubstr filename, csubstr src_, bool immutable_, bool reuse_, CasePart_e case_part_, bool expect_error_)
    {
        RYML_ASSERT((case_part & CPART_EVENTS) == 0u);
        case_part = case_part_;
        allowed_failure = is_failure_expected(filename, case_part);
        allowed_failure_events = is_failure_expected(filename, events_part(case_part));
        allowed_failure_errors = is_failure_expected(filename, CPART_IN_YAML_ERRORS);
        enabled = !allowed_failure.skip(case_part);
        for(ProcLevel &l : levels)
            l.init(filename, src_, immutable_, reuse_);
        expect_error = expect_error_;
    }

    csubstr src() const { return c4::to_csubstr(levels[0].src); }
    bool skip_part() const { return expect_error || src().empty() || allowed_failure.skip(case_part); }
    bool skip_error() const { return !expect_error || allowed_failure_errors.skip(CPART_IN_YAML_ERRORS); }
    bool skip_events() const
    {
        RYML_ASSERT(events_part(CPART_IN_YAML) == CPART_IN_YAML_EVENTS);
        RYML_ASSERT(events_part(CPART_OUT_YAML) == CPART_OUT_YAML_EVENTS);
        RYML_ASSERT(events_part(CPART_EMIT_YAML) == CPART_EMIT_YAML_EVENTS);
        // use bit-or to ensure calling both, so that both report a skip
        return (bool)((int)skip_part() | (int)allowed_failure_events.skip(events_part(case_part)));
    }

    void parse(size_t num, bool emit)
    {
        if(skip_part())
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
        if(skip_part())
            GTEST_SKIP();
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_trees(levels[i-1]);
    }
    void compare_trees(size_t num, Approach const& other)
    {
        if(skip_part())
            GTEST_SKIP();
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_trees(other.levels[i]);
    }

    void compare_emitted(size_t num)
    {
        if(skip_part())
            GTEST_SKIP();
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_emitted(levels[i-1]);
    }
    void compare_emitted(size_t num, Approach const& other)
    {
        if(skip_part())
            GTEST_SKIP();
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_emitted(other.levels[i]);
    }

    void compare_events(Events *events)
    {
        if(skip_events())
            GTEST_SKIP();
        events->parse_events(src());
        parse(1, /*emit*/false);
        events->compare_trees(src(), levels[0].tree);
    }

    void compare_emitted_events(Events *events)
    {
        if(skip_events())
            GTEST_SKIP();
        events->parse_events(src());
        parse(1, /*emit*/false);
        events->compare_emitted_events(src(), levels[0].tree);
    }

    void check_expected_error()
    {
        if(skip_error())
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

    void init(csubstr filename, csubstr src, CasePart_e case_part, bool expect_error)
    {
        src = replace_all("\r", "", src, &unix_src);

        unix_arena      .init(filename, src, /*immutable*/true , /*reuse*/false, case_part, expect_error);
        unix_arena_reuse.init(filename, src, /*immutable*/true , /*reuse*/true , case_part, expect_error);
        unix_inplace      .init(filename, src, /*immutable*/false, /*reuse*/false, case_part, expect_error);
        unix_inplace_reuse.init(filename, src, /*immutable*/false, /*reuse*/true , case_part, expect_error);

        src = replace_all("\n", "\r\n", src, &windows_src);

        windows_arena      .init(filename, src, /*immutable*/true , /*reuse*/false, case_part, expect_error);
        windows_arena_reuse.init(filename, src, /*immutable*/true , /*reuse*/true , case_part, expect_error);
        windows_inplace      .init(filename, src, /*immutable*/false, /*reuse*/false, case_part, expect_error);
        windows_inplace_reuse.init(filename, src, /*immutable*/false, /*reuse*/true , case_part, expect_error);
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// some utility functions, used below

void check_known_keys(csubstr filename, NodeRef const spec)
{
    for(auto node : spec.children())
    {
        csubstr k = node.key();
        if(k == "name")
            ;
        else if(k == "from")
            ;
        else if(k == "yaml")
            ;
        else if(k == "tags")
            ;
        else if(k == "tree")
            ;
        else if(k == "json")
            ;
        else if(k == "dump")
            ;
        else if(k == "emit")
            ;
        else if(k == "fail")
            ;
        else if(k == "toke")
            ;
        else if(k == "skip")
            ;
        else
            C4_ERROR("%.*s: unknown tag '%.*s'",
                     (int)filename.len, filename.str,
                     (int)k.len, k.str);
    }
}

struct SpecialCharsFilter
{
    std::string tmpa;
    std::string tmpb;
    std::string *current = &tmpa;
    csubstr _do_replace(csubstr pattern, csubstr repl, csubstr subject)
    {
        subject = replace_all(pattern, repl, subject, current);
        if(current == &tmpa)
            current = &tmpb;
        else
            current = &tmpa;
        return subject;
    }
    // https://github.com/yaml/yaml-test-suite#special-characters
    csubstr replace_normal(csubstr txt)
    {
        txt = _do_replace("␣", " ", txt);
        txt = _do_replace("————»", "\t", txt);
        txt = _do_replace("———»", "\t", txt);
        txt = _do_replace("——»", "\t", txt);
        txt = _do_replace("—»", "\t", txt);
        txt = _do_replace("»", "\t", txt);
        txt = _do_replace("↵", "\n", txt);
        txt = _do_replace("←", "\r", txt);
        txt = _do_replace("∎", "", txt);
        txt = _do_replace("⇔", "\xef\xbb\xbf", txt); // byte order mark 0xef 0xbb 0xbf
        return txt;
    }
    csubstr replace_events(csubstr txt)
    {
        txt = replace_normal(txt);
        txt = _do_replace("<SPC>", " ", txt);
        txt = _do_replace("<TAB>", "\t", txt);
        return txt;
    }
};


/** all the ways that a test case can be processed are
 * available through this class. Tests are defined below and use only
 * one of these. */
struct SuiteCase
{
    csubstr     filename;
    std::string file_contents;

    // this is the tree parsed from the test suite file for this case
    Tree spec_tree;

    // a single case from the test suite may contain several subcases
    csubstr desc;
    csubstr from;
    csubstr tags;
    bool    expect_error;
    bool    skip;

    Subject in_yaml;
    Subject in_json;
    Subject out_yaml;
    Subject emit_yaml;

    Events  events;

    struct SubCase
    {
        csubstr tags;
        bool    expect_error;
        bool    skip;
        Subject in_yaml;
        Subject in_json;
        Subject out_yaml;
        Subject emit_yaml;
        Events  events;
        SubCase() = default;
    };
    std::vector<SubCase> extra_subcases;

    static csubstr src(Subject const& s)
    {
        return c4::to_csubstr(s.unix_arena.levels[0].src);
    }

    static bool parse_spec_error_directives(const NodeRef spec, bool case_value)
    {
        bool expect_error = case_value;
        if(spec.has_child("tags"))
            expect_error = (spec["tags"].val().find("error") != npos);
        if(spec.has_child("fail"))
        {
            if(!expect_error)
            {
                spec["fail"] >> expect_error;
            }
            else
            {
                bool tmp = false;
                spec["fail"] >> tmp;
                C4_CHECK(tmp == expect_error);
            }
        }
        return expect_error;
    };
    static bool parse_spec_skip_directives(const NodeRef spec, bool case_value)
    {
        bool skip = case_value;
        if(spec.has_child("skip"))
            spec["skip"] >> skip;
        return skip;
    };

    /** loads the several types of tests from an input test suite file */
    SuiteCase(const char* filename_)
    {
        filename = c4::to_csubstr(filename_);

        // read the file
        c4::fs::file_get_contents(filename_, &file_contents);
        csubstr contents = c4::to_csubstr(file_contents);
        #if RYML_NFO
        _nfo_logf("contents:\n~~~{}~~~", contents);
        #endif

        // now parse the file
        RYML_CHECK(contents.begins_with("---"));
        parse_in_arena(filename, contents, &spec_tree);
        #if RYML_NFO
        c4::print("parsed:"); print_tree(spec_tree);
        #endif
        RYML_CHECK(spec_tree.rootref().num_children() == 1u);
        const NodeRef doc = spec_tree.docref(0);
        RYML_CHECK(doc.num_children() >= 1u);
        new ((void*)&extra_subcases) std::vector<SubCase>(doc.num_children() - 1u);
        size_t isubcase = 0;
        SpecialCharsFilter filter;
        SubCase *sc = nullptr;
        for(const NodeRef spec : doc.children())
        {
            check_known_keys(filename, spec);
            if(isubcase == 0)
            {
                desc = spec["name"].val();
                from = spec["from"].val();
                tags = spec["tags"].val();
                expect_error = parse_spec_error_directives(spec, false);
                skip = parse_spec_skip_directives(spec, false);
            }
            else
            {
                // not really sure, so check for now
                RYML_CHECK(!spec.has_child("name"));
                RYML_CHECK(!spec.has_child("from"));
                sc = &extra_subcases[isubcase - 1u];
                sc->expect_error = parse_spec_error_directives(spec, expect_error);
                sc->skip = parse_spec_skip_directives(spec, skip);
            }

            // in_yaml
            if(isubcase == 0u)
            {
                csubstr txt = spec["yaml"].val();
                txt = filter.replace_normal(txt);
                in_yaml.init(filename, txt, CPART_IN_YAML, expect_error);
            }
            else if(spec.has_child("yaml"))
            {
                csubstr txt = spec["yaml"].val();
                txt = filter.replace_normal(txt);
                sc->in_yaml.init(filename, txt, CPART_IN_YAML, sc->expect_error);
            }

            // in_json
            if(isubcase == 0u)
            {
                if(spec.has_child("json"))
                {
                    csubstr txt = spec["json"].val();
                    in_json.init(filename, txt, CPART_IN_JSON, expect_error);
                }
            }
            else if(spec.has_child("json"))
            {
                csubstr txt = spec["json"].val();
                in_json.init(filename, txt, CPART_IN_JSON, expect_error);
                sc->in_json.init(filename, txt, CPART_IN_YAML, sc->expect_error);
            }

            // out_yaml
            if(isubcase == 0u)
            {
                if(spec.has_child("dump"))
                {
                    csubstr txt = spec["dump"].val();
                    txt = filter.replace_normal(txt);
                    out_yaml.init(filename, txt, CPART_OUT_YAML, expect_error);
                }
            }
            else if(spec.has_child("dump"))
            {
                csubstr txt = spec["dump"].val();
                in_json.init(filename, txt, CPART_OUT_YAML, expect_error);
                sc->out_yaml.init(filename, txt, CPART_OUT_YAML, sc->expect_error);
            }

            // emit_yaml
            if(isubcase == 0u)
            {
                if(spec.has_child("emit"))
                {
                    csubstr txt = spec["emit"].val();
                    txt = filter.replace_normal(txt);
                    emit_yaml.init(filename, txt, CPART_EMIT_YAML, expect_error);
                }
            }
            else if(spec.has_child("emit"))
            {
                csubstr txt = spec["emit"].val();
                txt = filter.replace_normal(txt);
                sc->emit_yaml.init(filename, txt, CPART_EMIT_YAML, sc->expect_error);
            }

            // events
            if(isubcase == 0u)
            {
                C4_CHECK(spec.has_child("tree"));
                csubstr txt = spec["tree"].val();
                txt = filter.replace_events(txt);
                events.init(filename, txt);
            }
            else if(spec.has_child("tree"))
            {
                csubstr txt = spec["tree"].val();
                txt = filter.replace_events(txt);
                sc->events.init(filename, txt);
            }

            ++isubcase;
        }
    }

    void print() const
    {
        c4::dump("~~~ file: "      , filename      , "~~~\n",
                 "~~~ desc: "      , desc          , "~~~\n",
                 "~~~ from: "      , from          , "~~~\n",
                 "~~~ tags: "      , tags          , "~~~\n",
                 "~~~ in-yaml:\n"  , src(in_yaml  ), "~~~\n",
                 "~~~ in-json:\n"  , src(in_json  ), "~~~\n",
                 "~~~ out-yaml:\n" , src(out_yaml ), "~~~\n",
                 "~~~ emit-yaml:\n", src(emit_yaml), "~~~\n",
                 "~~~ events:\n"   , events.src    , "~~~\n");
    }

};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


// a global holding the test case data
SuiteCase* g_suite_case = nullptr;
bool g_do_subcases = true;

#define ANNOUNCE_SUBCASE(cls, pfx, i) \
    c4::dump("~~~~~~~~~~~~~~~~~~~~~~~ "                   \
             #cls "." #pfx ": SUBCASE ", i,               \
             " ~~~~~~~~~~~~~~~~~~~~~~~\n");               \
    SCOPED_TRACE(i)                                       \



#define DEFINE_EVENT_TESTS(cls, pfx)                                \
                                                                    \
TEST(cls##_##pfx##_events, compare)                                 \
{                                                                   \
    if(!g_suite_case || !g_suite_case->cls.pfx.enabled)             \
        GTEST_SKIP();                                               \
    if(g_suite_case->skip || g_suite_case->expect_error)            \
        GTEST_SKIP();                                               \
    g_suite_case->cls.pfx.compare_events(&g_suite_case->events);    \
    size_t i_extra_subcase = 0;                                     \
    for(auto &sc : g_suite_case->extra_subcases)                    \
    {                                                               \
        if(!g_do_subcases || !sc.cls.pfx.enabled || !sc.events.enabled) \
            continue;                                               \
        ANNOUNCE_SUBCASE(cls, pfx ## events, i_extra_subcase);      \
        sc.cls.pfx.compare_events(&sc.events);                      \
        ++i_extra_subcase;                                          \
    }                                                               \
}                                                                   \
                                                                    \
TEST(cls##_##pfx##_events, emit_events)                             \
{                                                                   \
    if(!g_suite_case || !g_suite_case->cls.pfx.enabled)             \
        GTEST_SKIP();                                               \
    if(g_suite_case->skip || g_suite_case->expect_error)            \
        GTEST_SKIP();                                               \
    g_suite_case->cls.pfx.compare_emitted_events(&g_suite_case->events); \
    size_t i_extra_subcase = 0;                                     \
    for(auto &sc : g_suite_case->extra_subcases)                    \
    {                                                               \
        if(!g_do_subcases || !sc.cls.pfx.enabled || !sc.events.enabled) \
            continue;                                               \
        ANNOUNCE_SUBCASE(cls, pfx ## events, i_extra_subcase);      \
        sc.cls.pfx.compare_emitted_events(&sc.events);              \
        ++i_extra_subcase;                                          \
    }                                                               \
}



#define DEFINE_ERROR_TESTS(pfx)                                     \
                                                                    \
TEST(in_yaml_##pfx##_errors, check_expected_error)                  \
{                                                                   \
    if(g_suite_case->skip || !g_suite_case->expect_error)           \
        GTEST_SKIP();                                               \
    g_suite_case->in_yaml.pfx.check_expected_error();               \
    size_t i_extra_subcase = 0;                                     \
    for(auto &sc : g_suite_case->extra_subcases)                    \
    {                                                               \
        if(!g_do_subcases || !sc.in_yaml.pfx.enabled)               \
            continue;                                               \
        ANNOUNCE_SUBCASE(in_yaml, pfx ## errors, i_extra_subcase);  \
        sc.in_yaml.pfx.check_expected_error();                      \
        ++i_extra_subcase;                                          \
    }                                                               \
}



#define DEFINE_TEST_CLASS(cls, pfx)                             \
                                                                \
                                                                \
struct cls##_##pfx : public ::testing::TestWithParam<size_t>    \
{                                                               \
    Approach* get_test_case() const                             \
    {                                                           \
        RYML_CHECK(GetParam() < NLEVELS);                       \
        if(g_suite_case && g_suite_case->cls.pfx.enabled)       \
            return &g_suite_case->cls.pfx;                      \
        c4::dump(#cls "." #pfx ": no input for this case\n");   \
        return nullptr;                                         \
    }                                                           \
};                                                              \
                                                                \
                                                                \
TEST_P(cls##_##pfx, parse)                                      \
{                                                               \
    if(!get_test_case())                                        \
        GTEST_SKIP();                                           \
    if(g_suite_case->skip || g_suite_case->expect_error)        \
        GTEST_SKIP();                                           \
    get_test_case()->parse(1 + GetParam(), false);              \
    size_t i_extra_subcase = 0;                                 \
    for(auto &sc : g_suite_case->extra_subcases)                \
    {                                                           \
        if(!g_do_subcases || !sc.cls.pfx.enabled)               \
            continue;                                           \
        ANNOUNCE_SUBCASE(cls, pfx, i_extra_subcase);            \
        sc.cls.pfx.parse(1 + GetParam(), false);                \
        ++i_extra_subcase;                                      \
    }                                                           \
}                                                               \
                                                                \
                                                                \
TEST_P(cls##_##pfx, compare_trees)                              \
{                                                               \
    if(!get_test_case())                                        \
        GTEST_SKIP();                                           \
    if(g_suite_case->skip || g_suite_case->expect_error)        \
        GTEST_SKIP();                                           \
    get_test_case()->compare_trees(1 + GetParam());             \
    size_t i_extra_subcase = 0;                                 \
    for(auto &sc : g_suite_case->extra_subcases)                \
    {                                                           \
        if(!g_do_subcases || !sc.cls.pfx.enabled)               \
            continue;                                           \
        ANNOUNCE_SUBCASE(cls, pfx, i_extra_subcase);            \
        sc.cls.pfx.compare_trees(1 + GetParam());               \
        ++i_extra_subcase;                                      \
    }                                                           \
}                                                               \
                                                                \
                                                                \
TEST_P(cls##_##pfx, emit)                                       \
{                                                               \
    if(!get_test_case())                                        \
        GTEST_SKIP();                                           \
    if(g_suite_case->skip || g_suite_case->expect_error)        \
        GTEST_SKIP();                                           \
    get_test_case()->parse(1 + GetParam(), true);               \
    size_t i_extra_subcase = 0;                                 \
    for(auto &sc : g_suite_case->extra_subcases)                \
    {                                                           \
        if(!g_do_subcases || !sc.cls.pfx.enabled)               \
            continue;                                           \
        ANNOUNCE_SUBCASE(cls, pfx, i_extra_subcase);            \
        sc.cls.pfx.parse(1 + GetParam(), true);                 \
        ++i_extra_subcase;                                      \
    }                                                           \
}                                                               \
                                                                \
                                                                \
TEST_P(cls##_##pfx, compare_emitted)                            \
{                                                               \
    if(!get_test_case())                                        \
        GTEST_SKIP();                                           \
    if(g_suite_case->skip || g_suite_case->expect_error)        \
        GTEST_SKIP();                                           \
    get_test_case()->compare_emitted(1 + GetParam());           \
    size_t i_extra_subcase = 0;                                 \
    for(auto &sc : g_suite_case->extra_subcases)                \
    {                                                           \
        if(!g_do_subcases || !sc.cls.pfx.enabled)               \
            continue;                                           \
        ANNOUNCE_SUBCASE(cls, pfx, i_extra_subcase);            \
        sc.cls.pfx.compare_emitted(1 + GetParam());             \
        ++i_extra_subcase;                                      \
    }                                                           \
}                                                               \
\
INSTANTIATE_TEST_SUITE_P(_, cls##_##pfx, testing::Range<size_t>(0, NLEVELS))


// in-json
DEFINE_TEST_CLASS(in_json, unix_arena);
DEFINE_TEST_CLASS(in_json, unix_inplace);
DEFINE_TEST_CLASS(in_json, unix_arena_reuse);
DEFINE_TEST_CLASS(in_json, unix_inplace_reuse);
DEFINE_TEST_CLASS(in_json, windows_arena);
DEFINE_TEST_CLASS(in_json, windows_inplace);
DEFINE_TEST_CLASS(in_json, windows_arena_reuse);
DEFINE_TEST_CLASS(in_json, windows_inplace_reuse);


// out-yaml
DEFINE_TEST_CLASS(out_yaml, unix_arena);
DEFINE_TEST_CLASS(out_yaml, unix_inplace);
DEFINE_TEST_CLASS(out_yaml, unix_arena_reuse);
DEFINE_TEST_CLASS(out_yaml, unix_inplace_reuse);
DEFINE_TEST_CLASS(out_yaml, windows_arena);
DEFINE_TEST_CLASS(out_yaml, windows_inplace);
DEFINE_TEST_CLASS(out_yaml, windows_arena_reuse);
DEFINE_TEST_CLASS(out_yaml, windows_inplace_reuse);

DEFINE_EVENT_TESTS(out_yaml, unix_arena)
DEFINE_EVENT_TESTS(out_yaml, unix_inplace)
DEFINE_EVENT_TESTS(out_yaml, unix_arena_reuse)
DEFINE_EVENT_TESTS(out_yaml, unix_inplace_reuse)
DEFINE_EVENT_TESTS(out_yaml, windows_arena)
DEFINE_EVENT_TESTS(out_yaml, windows_inplace)
DEFINE_EVENT_TESTS(out_yaml, windows_arena_reuse)
DEFINE_EVENT_TESTS(out_yaml, windows_inplace_reuse)


// emit-yaml
DEFINE_TEST_CLASS(emit_yaml, unix_arena);
DEFINE_TEST_CLASS(emit_yaml, unix_inplace);
DEFINE_TEST_CLASS(emit_yaml, unix_arena_reuse);
DEFINE_TEST_CLASS(emit_yaml, unix_inplace_reuse);
DEFINE_TEST_CLASS(emit_yaml, windows_arena);
DEFINE_TEST_CLASS(emit_yaml, windows_inplace);
DEFINE_TEST_CLASS(emit_yaml, windows_arena_reuse);
DEFINE_TEST_CLASS(emit_yaml, windows_inplace_reuse);

DEFINE_EVENT_TESTS(emit_yaml, unix_arena)
DEFINE_EVENT_TESTS(emit_yaml, unix_inplace)
DEFINE_EVENT_TESTS(emit_yaml, unix_arena_reuse)
DEFINE_EVENT_TESTS(emit_yaml, unix_inplace_reuse)
DEFINE_EVENT_TESTS(emit_yaml, windows_arena)
DEFINE_EVENT_TESTS(emit_yaml, windows_inplace)
DEFINE_EVENT_TESTS(emit_yaml, windows_arena_reuse)
DEFINE_EVENT_TESTS(emit_yaml, windows_inplace_reuse)


// in-yaml: this is the hardest one
DEFINE_TEST_CLASS(in_yaml, unix_arena);
DEFINE_TEST_CLASS(in_yaml, unix_inplace);
DEFINE_TEST_CLASS(in_yaml, unix_arena_reuse);
DEFINE_TEST_CLASS(in_yaml, unix_inplace_reuse);
DEFINE_TEST_CLASS(in_yaml, windows_arena);
DEFINE_TEST_CLASS(in_yaml, windows_inplace);
DEFINE_TEST_CLASS(in_yaml, windows_arena_reuse);
DEFINE_TEST_CLASS(in_yaml, windows_inplace_reuse);

DEFINE_EVENT_TESTS(in_yaml, unix_arena)
DEFINE_EVENT_TESTS(in_yaml, unix_inplace)
DEFINE_EVENT_TESTS(in_yaml, unix_arena_reuse)
DEFINE_EVENT_TESTS(in_yaml, unix_inplace_reuse)
DEFINE_EVENT_TESTS(in_yaml, windows_arena)
DEFINE_EVENT_TESTS(in_yaml, windows_inplace)
DEFINE_EVENT_TESTS(in_yaml, windows_arena_reuse)
DEFINE_EVENT_TESTS(in_yaml, windows_inplace_reuse)

DEFINE_ERROR_TESTS(unix_arena)
DEFINE_ERROR_TESTS(unix_inplace)
DEFINE_ERROR_TESTS(unix_arena_reuse)
DEFINE_ERROR_TESTS(unix_inplace_reuse)
DEFINE_ERROR_TESTS(windows_arena)
DEFINE_ERROR_TESTS(windows_inplace)
DEFINE_ERROR_TESTS(windows_arena_reuse)
DEFINE_ERROR_TESTS(windows_inplace_reuse)


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // make gtest parse its args
    ::testing::InitGoogleTest(&argc, argv);

    // now we have only our args to consider
    if(argc != 2)
    {
        c4::log("usage:\n{} <test-suite-file>", c4::to_csubstr(argv[0]));
        return 1;
    }
    else if(c4::to_csubstr(argv[1]) == "-h" || c4::to_csubstr(argv[1]) == "--help")
    {
        return 0;
    }

    // load the test case from the suite file
    auto path = c4::to_substr(argv[1]);
    path.replace('\\', '/');
    RYML_CHECK(path.len > 0);
    RYML_CHECK(path[0] != '-');
    C4_CHECK_MSG(c4::fs::file_exists(path.str), "file not found: '%.*s'", (int)path.len, path.str);
    c4::log("testing suite case: {} ({})", path.basename(), path);

    c4::yml::SuiteCase suite_case(path.str);
    c4::print(suite_case.file_contents);
    suite_case.print();
    c4::yml::g_suite_case = &suite_case;

    // run all tests!
    int status = RUN_ALL_TESTS();

    // a terminating message
    c4::log("\n{}: TESTS {}: {}\n",
            suite_case.filename.basename(),
            status == 0 ? "SUCCEEDED" : "FAILED",
            suite_case.filename);

    return status;
}

C4_SUPPRESS_WARNING_MSVC_PUSH
