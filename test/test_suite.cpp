#include <gtest/gtest.h>

#include <c4/yml/std/std.hpp>

#include <c4/fs/fs.hpp>
#include <c4/log/log.hpp>

#include <c4/yml/tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/detail/checks.hpp>

#include "test_case.hpp"
#include "test_suite/test_suite_common.hpp"
#include "test_suite/test_suite_parts.hpp"
#include "test_suite/test_suite_events.hpp"


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
    csubstr filename;
    std::string src;
    Tree    tree;
    mutable Tree adjusted_tree;
    bool    was_parsed = false;

    void init(csubstr filename_, csubstr src_)
    {
        filename = filename_;
        src.assign(src_.begin(), src_.end());
        tree.clear();
        tree.clear_arena();
        was_parsed = false;
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

    EventsParser parser;
    void parse_events(csubstr actual_src)
    {
        if(was_parsed)
            return;
        if(actual_src.empty())
            GTEST_SKIP();
        parser.parse(c4::to_csubstr(src), &tree);
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
                parser.parse(filename, c4::to_csubstr(src), &tree);
            else
                parser.parse(filename, c4::to_substr(src), &tree);
        }
        else
        {
            if(immutable)
                tree = c4::yml::parse(filename, c4::to_csubstr(src));
            else
                tree = c4::yml::parse(filename, c4::to_substr(src));
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
        return skip_part() | allowed_failure_events.skip(events_part(case_part));
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

    void check_expected_error()
    {
        if(skip_error())
            GTEST_SKIP();
        ExpectError::do_check([this]{
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
    Approach unix_ro;
    Approach unix_ro_reuse;
    Approach unix_rw;
    Approach unix_rw_reuse;

    Approach windows_ro;
    Approach windows_ro_reuse;
    Approach windows_rw;
    Approach windows_rw_reuse;

    std::string unix_src;
    std::string windows_src;

    void init(csubstr filename, csubstr src, CasePart_e case_part, bool expect_error)
    {
        if(src.ends_with("\r\n"))
            src = src.offs(0, 2);
        else if(src.ends_with('\n'))
            src = src.offs(0, 1);

        src = replace_all("\r", "", src, &unix_src);

        unix_ro      .init(filename, src, /*immutable*/true , /*reuse*/false, case_part, expect_error);
        unix_ro_reuse.init(filename, src, /*immutable*/true , /*reuse*/true , case_part, expect_error);
        unix_rw      .init(filename, src, /*immutable*/false, /*reuse*/false, case_part, expect_error);
        unix_rw_reuse.init(filename, src, /*immutable*/false, /*reuse*/true , case_part, expect_error);

        src = replace_all("\n", "\r\n", src, &windows_src);

        windows_ro      .init(filename, src, /*immutable*/true , /*reuse*/false, case_part, expect_error);
        windows_ro_reuse.init(filename, src, /*immutable*/true , /*reuse*/true , case_part, expect_error);
        windows_rw      .init(filename, src, /*immutable*/false, /*reuse*/false, case_part, expect_error);
        windows_rw_reuse.init(filename, src, /*immutable*/false, /*reuse*/true , case_part, expect_error);
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// some utility functions, used below

size_t find_first_after(size_t pos, std::initializer_list<size_t> candidates)
{
    size_t ret = npos;
    for(size_t s : candidates)
        if(s > pos && s < ret)
            ret = s;
    return ret;
}

csubstr filter_out_indentation(csubstr src, std::string *dst)
{
    if( ! src.begins_with("    "))
    {
        dst->assign(src.begin(), src.end());
        return c4::to_csubstr(*dst);
    }
    auto has_meta_comments = [](csubstr s) {
        for(csubstr line : s.split('\n'))
            if(line.begins_with('#'))
                return true;
        return false;
    };
    std::string tmp;
    if(has_meta_comments(src))
    {
        tmp.reserve(src.size());
        for(csubstr line : src.split('\n'))
        {
            if(line.begins_with('#'))
                continue;
            tmp.append(line.begin(), line.end());
            tmp += '\n';
        }
        src = c4::to_csubstr(tmp);
    }
    return replace_all("\n    ", "\n", src.sub(4), dst);
}


/** all the ways that a test case can be processed are
 * available through this class. Tests are defined below and use only
 * one of these. */
struct SuiteCase
{
    csubstr     filename;
    std::string file_contents;

    csubstr desc;
    csubstr from;
    csubstr tags;
    bool    expect_error;

    Subject in_yaml;
    Subject in_json;
    Subject out_yaml;
    Subject emit_yaml;

    Events  events;

    static csubstr src(Subject const& s)
    {
        return c4::to_csubstr(s.unix_ro.levels[0].src);
    }

    /** loads the several types of tests from an input test suite
     * template file (tml)*/
    SuiteCase(const char* filename_)
    {
        filename = c4::to_csubstr(filename_);

        // read the file
        c4::fs::file_get_contents(filename_, &file_contents);
        csubstr contents = c4::to_csubstr(file_contents);

        // now parse the file
        csubstr ws = " \t\r\n";
        csubstr txt;
        size_t b, e;

        // desc
        RYML_CHECK(contents.begins_with("=== "));
        e = contents.find("--- from: ", 4);
        RYML_CHECK(e != npos);
        desc = contents.range(4, e).trimr(ws);

        // from
        b = e + 4;
        e = contents.find("--- tags: ", b);
        RYML_CHECK(e != npos);
        from = contents.range(b, e);
        RYML_CHECK(from.begins_with("from: "));
        RYML_CHECK(from.size() >= 6);
        from = from.sub(6).trimr(ws);

        // tags
        b = e + 4;
        e = contents.find("--- in-yaml", b);
        RYML_CHECK(e != npos);
        tags = contents.range(b, e);
        RYML_CHECK(tags.begins_with("tags: "));
        RYML_CHECK(tags.size() >= 6);
        tags = tags.sub(6).trimr(ws);

        expect_error = (tags.find("error") != npos);

        bool has_whitespace = tags.find("whitespace");

        size_t end_tags        = e;
        size_t begin_in_yaml   = contents.find("--- in-yaml"   , end_tags);
        size_t begin_error     = contents.find("--- error"     , end_tags);
        size_t begin_in_json   = contents.find("--- in-json"   , end_tags);
        size_t begin_out_yaml  = contents.find("--- out-yaml"  , end_tags);
        size_t begin_emit_yaml = contents.find("--- emit-yaml" , end_tags);
        size_t begin_events    = contents.find("--- test-event", end_tags);
        size_t lex_token       = contents.find("--- lex-token" , end_tags);
        auto did_not_slurp_other_tml_tokens = [](csubstr part){
            csubstr tokens[] = {"--- in-yaml", "--- error", "--- in-json", "--- out-yaml", "--- emit-yaml", "---test-event", "--- lex-token"};
            return ! part.first_of_any_iter(std::begin(tokens), std::end(tokens));
        };
        std::initializer_list<size_t> all = {
            begin_in_yaml,
            begin_error,
            begin_in_json,
            begin_out_yaml,
            begin_emit_yaml,
            begin_events,
            lex_token,
            contents.size()
        };

        // some of the examples have their code indented,
        // so we need these workspaces for deindenting
        std::string tmpa;
        std::string tmpb;

        // in_yaml
        RYML_CHECK(begin_in_yaml != npos);
        size_t first_after_in_yaml = find_first_after(begin_in_yaml, all);
        begin_in_yaml = 1 + contents.find('\n', begin_in_yaml); // skip this line
        txt = contents.range(begin_in_yaml, first_after_in_yaml);
        RYML_CHECK(did_not_slurp_other_tml_tokens(txt));
        txt = filter_out_indentation(txt, &tmpa);
        if(has_whitespace)
        {
            txt = replace_all("<SPC>", " ", txt, &tmpb);
            txt = replace_all("<TAB>", "\t", txt, &tmpa);
        }
        in_yaml.init(filename, txt, CPART_IN_YAML, expect_error);

        // error
        if(begin_error != npos)
        {
            size_t first_after = find_first_after(begin_error, all);
            begin_error = 1 + contents.find('\n', begin_error); // skip this line
            txt = contents.range(begin_error, first_after);
            RYML_CHECK(did_not_slurp_other_tml_tokens(txt));
            txt = filter_out_indentation(txt, &tmpa);
        }

        // in_json
        if(begin_in_json != npos)
        {
            size_t first_after = find_first_after(begin_in_json, all);
            begin_in_json = 1 + contents.find('\n', begin_in_json); // skip this line
            txt = contents.range(begin_in_json, first_after);
            RYML_CHECK(did_not_slurp_other_tml_tokens(txt));
            in_json.init(filename, txt, CPART_IN_JSON, expect_error);
        }

        // out_yaml
        if(begin_out_yaml != npos)
        {
            size_t first_after = find_first_after(begin_out_yaml, all);
            begin_out_yaml = 1 + contents.find('\n', begin_out_yaml); // skip this line
            txt = contents.range(begin_out_yaml, first_after);
            RYML_CHECK(did_not_slurp_other_tml_tokens(txt));
            txt = filter_out_indentation(txt, &tmpa);
            if(has_whitespace)
            {
                txt = replace_all("<SPC>", " ", txt, &tmpb);
                txt = replace_all("<TAB>", "\t", txt, &tmpa);
            }
            out_yaml.init(filename, txt, CPART_OUT_YAML, expect_error);
        }

        // emit_yaml
        if(begin_emit_yaml != npos)
        {
            size_t first_after = find_first_after(begin_emit_yaml, all);
            begin_emit_yaml = 1 + contents.find('\n', begin_emit_yaml); // skip this line
            txt = contents.range(begin_emit_yaml, first_after);
            RYML_CHECK(did_not_slurp_other_tml_tokens(txt));
            txt = filter_out_indentation(txt, &tmpa);
            if(has_whitespace)
            {
                txt = replace_all("<SPC>", " ", txt, &tmpb);
                txt = replace_all("<TAB>", "\t", txt, &tmpa);
            }
            emit_yaml.init(filename, txt, CPART_EMIT_YAML, expect_error);
        }

        // events
        {
            RYML_CHECK(begin_events != npos);
            size_t first_after = find_first_after(begin_events, all);
            begin_events = 1 + contents.find('\n', begin_events); // skip this line
            txt = contents.range(begin_events, first_after);
            RYML_CHECK(did_not_slurp_other_tml_tokens(txt));
            if(has_whitespace)
            {
                txt = replace_all("<SPC>", " ", txt, &tmpb);
                txt = replace_all("<TAB>", "\t", txt, &tmpa);
            }
            events.init(filename, txt);
        }

        // lex-token
        {
            // don't really care
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



#define DEFINE_EVENT_TESTS(cls, pfx)                                \
                                                                    \
TEST(cls##_##pfx##_events, compare)                                 \
{                                                                   \
    if(!g_suite_case || !g_suite_case->cls.pfx.enabled)             \
        GTEST_SKIP();                                               \
    if(g_suite_case->expect_error)                                  \
        GTEST_SKIP();                                               \
    g_suite_case->cls.pfx.compare_events(&g_suite_case->events);    \
}



#define DEFINE_ERROR_TESTS(pfx)                         \
                                                        \
TEST(in_yaml_##pfx##_errors, check_expected_error)      \
{                                                       \
    if(!g_suite_case->expect_error)                     \
        GTEST_SKIP();                                   \
    g_suite_case->in_yaml.pfx.check_expected_error();   \
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
    if(g_suite_case->expect_error)                              \
        GTEST_SKIP();                                           \
    get_test_case()->parse(1 + GetParam(), false);              \
}                                                               \
                                                                \
                                                                \
TEST_P(cls##_##pfx, compare_trees)                              \
{                                                               \
    if(!get_test_case())                                        \
        GTEST_SKIP();                                           \
    if(g_suite_case->expect_error)                              \
        GTEST_SKIP();                                           \
    get_test_case()->compare_trees(1 + GetParam());             \
}                                                               \
                                                                \
                                                                \
TEST_P(cls##_##pfx, emit)                                       \
{                                                               \
    if(!get_test_case())                                        \
        GTEST_SKIP();                                           \
    if(g_suite_case->expect_error)                              \
        GTEST_SKIP();                                           \
    get_test_case()->parse(1 + GetParam(), true);               \
}                                                               \
                                                                \
                                                                \
TEST_P(cls##_##pfx, compare_emitted)                            \
{                                                               \
    if(!get_test_case())                                        \
        GTEST_SKIP();                                           \
    if(g_suite_case->expect_error)                              \
        GTEST_SKIP();                                           \
    get_test_case()->compare_emitted(1 + GetParam());           \
}                                                               \
\
INSTANTIATE_TEST_SUITE_P(_, cls##_##pfx, testing::Range<size_t>(0, NLEVELS))


// in-json
DEFINE_TEST_CLASS(in_json, unix_ro);
DEFINE_TEST_CLASS(in_json, unix_rw);
DEFINE_TEST_CLASS(in_json, unix_ro_reuse);
DEFINE_TEST_CLASS(in_json, unix_rw_reuse);
DEFINE_TEST_CLASS(in_json, windows_ro);
DEFINE_TEST_CLASS(in_json, windows_rw);
DEFINE_TEST_CLASS(in_json, windows_ro_reuse);
DEFINE_TEST_CLASS(in_json, windows_rw_reuse);


// out-yaml
DEFINE_TEST_CLASS(out_yaml, unix_ro);
DEFINE_TEST_CLASS(out_yaml, unix_rw);
DEFINE_TEST_CLASS(out_yaml, unix_ro_reuse);
DEFINE_TEST_CLASS(out_yaml, unix_rw_reuse);
DEFINE_TEST_CLASS(out_yaml, windows_ro);
DEFINE_TEST_CLASS(out_yaml, windows_rw);
DEFINE_TEST_CLASS(out_yaml, windows_ro_reuse);
DEFINE_TEST_CLASS(out_yaml, windows_rw_reuse);

DEFINE_EVENT_TESTS(out_yaml, unix_ro)
DEFINE_EVENT_TESTS(out_yaml, unix_rw)
DEFINE_EVENT_TESTS(out_yaml, unix_ro_reuse)
DEFINE_EVENT_TESTS(out_yaml, unix_rw_reuse)
DEFINE_EVENT_TESTS(out_yaml, windows_ro)
DEFINE_EVENT_TESTS(out_yaml, windows_rw)
DEFINE_EVENT_TESTS(out_yaml, windows_ro_reuse)
DEFINE_EVENT_TESTS(out_yaml, windows_rw_reuse)


// emit-yaml
DEFINE_TEST_CLASS(emit_yaml, unix_ro);
DEFINE_TEST_CLASS(emit_yaml, unix_rw);
DEFINE_TEST_CLASS(emit_yaml, unix_ro_reuse);
DEFINE_TEST_CLASS(emit_yaml, unix_rw_reuse);
DEFINE_TEST_CLASS(emit_yaml, windows_ro);
DEFINE_TEST_CLASS(emit_yaml, windows_rw);
DEFINE_TEST_CLASS(emit_yaml, windows_ro_reuse);
DEFINE_TEST_CLASS(emit_yaml, windows_rw_reuse);

DEFINE_EVENT_TESTS(emit_yaml, unix_ro)
DEFINE_EVENT_TESTS(emit_yaml, unix_rw)
DEFINE_EVENT_TESTS(emit_yaml, unix_ro_reuse)
DEFINE_EVENT_TESTS(emit_yaml, unix_rw_reuse)
DEFINE_EVENT_TESTS(emit_yaml, windows_ro)
DEFINE_EVENT_TESTS(emit_yaml, windows_rw)
DEFINE_EVENT_TESTS(emit_yaml, windows_ro_reuse)
DEFINE_EVENT_TESTS(emit_yaml, windows_rw_reuse)


// in-yaml: this is the hardest one
DEFINE_TEST_CLASS(in_yaml, unix_ro);
DEFINE_TEST_CLASS(in_yaml, unix_rw);
DEFINE_TEST_CLASS(in_yaml, unix_ro_reuse);
DEFINE_TEST_CLASS(in_yaml, unix_rw_reuse);
DEFINE_TEST_CLASS(in_yaml, windows_ro);
DEFINE_TEST_CLASS(in_yaml, windows_rw);
DEFINE_TEST_CLASS(in_yaml, windows_ro_reuse);
DEFINE_TEST_CLASS(in_yaml, windows_rw_reuse);

DEFINE_EVENT_TESTS(in_yaml, unix_ro)
DEFINE_EVENT_TESTS(in_yaml, unix_rw)
DEFINE_EVENT_TESTS(in_yaml, unix_ro_reuse)
DEFINE_EVENT_TESTS(in_yaml, unix_rw_reuse)
DEFINE_EVENT_TESTS(in_yaml, windows_ro)
DEFINE_EVENT_TESTS(in_yaml, windows_rw)
DEFINE_EVENT_TESTS(in_yaml, windows_ro_reuse)
DEFINE_EVENT_TESTS(in_yaml, windows_rw_reuse)

DEFINE_ERROR_TESTS(unix_ro)
DEFINE_ERROR_TESTS(unix_rw)
DEFINE_ERROR_TESTS(unix_ro_reuse)
DEFINE_ERROR_TESTS(unix_rw_reuse)
DEFINE_ERROR_TESTS(windows_ro)
DEFINE_ERROR_TESTS(windows_rw)
DEFINE_ERROR_TESTS(windows_ro_reuse)
DEFINE_ERROR_TESTS(windows_rw_reuse)



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
