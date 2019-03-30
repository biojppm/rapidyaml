#include <c4/substr.hpp>
#include <c4/std/string.hpp>
#include <c4/fs/fs.hpp>
#include <c4/log/log.hpp>

#include <c4/yml/tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/detail/checks.hpp>

#include <gtest/gtest.h>

#define NLEVELS 4

enum : size_t { npos = c4::csubstr::npos };


struct ProcLevel
{
    c4::csubstr     filename;
    std::string     src;
    c4::yml::Parser parser;
    c4::yml::Tree   tree;
    std::string     emitted;

    bool            is_yaml_events = false;
    bool            immutable = false;
    bool            reuse = false;
    bool            was_parsed = false;
    bool            was_emitted = false;

    void init(c4::csubstr filename, c4::csubstr src_, bool immutable_, bool reuse_, bool is_yaml_events_)
    {
        src.assign(src_.begin(), src_.end());
        immutable = immutable_;
        reuse = reuse_;
        was_parsed = false;
        was_emitted = false;
    }

    void receive_src(ProcLevel & prev)
    {
        if(!prev.was_emitted)
        {
            prev.emit();
        }
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
        const char sep[] = "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
#ifdef RYML_DBG
        c4::log("{}:\n{}{}{}", context, sep, v, sep);
#endif
    }

    void parse()
    {
        if(was_parsed) return;
        log("parsing source", src);
        if(is_yaml_events)
        {
            C4_NOT_IMPLEMENTED();
        }
        else
        {
            if(reuse)
            {
                if(immutable)
                {
                    parser.parse(filename, c4::to_csubstr(src), &tree);
                }
                else
                {
                    parser.parse(filename, c4::to_substr(src), &tree);
                }
            }
            else
            {
                if(immutable)
                {
                    tree = c4::yml::parse(filename, c4::to_csubstr(src));
                }
                else
                {
                    tree = c4::yml::parse(filename, c4::to_substr(src));
                }
            }
        }
        was_parsed = true;
#ifdef RYML_DBG
        c4::yml::print_tree(tree);
#endif
    }

    void emit()
    {
        if(was_emitted) return;
        if(!was_parsed) parse();
        emitrs(tree, &emitted);
        auto ss = c4::to_csubstr(emitted);
        if(ss.ends_with("\n...\n"))
        {
            emitted.resize(emitted.size() - 4);
        }
        log("emitted YAML", emitted);
        was_emitted = true;
    }

    void compare_trees(ProcLevel const& prev)
    {
        if(!was_parsed) parse();
        // do it
    }

    void compare_emitted(ProcLevel const& prev)
    {
        if(!was_emitted) emit();
        EXPECT_EQ(emitted, prev.emitted);
    }
};


struct Approach
{
    ProcLevel levels[NLEVELS];

    void init(c4::csubstr filename, c4::csubstr src, bool immutable_, bool reuse_, bool is_yaml_events)
    {
        for(auto &l : levels)
        {
            l.init(filename, src, immutable_, reuse_, is_yaml_events);
            is_yaml_events = false; // only the first one
        }
    }

    void parse(size_t num, bool emit)
    {
        for(size_t i = 0; i < num; ++i)
        {
            levels[i].parse();
            if(emit) levels[i].emit();
            if(i + 1 < num)
            {
                levels[i+1].receive_src(levels[i]);
            }
        }
    }

    void compare_trees(size_t num)
    {
        for(size_t i = 1; i < num; ++i)
        {
            levels[i].compare_trees(levels[i-1]);
        }
    }
    void compare_trees(size_t num, Approach const& other)
    {
        for(size_t i = 0; i < num; ++i)
        {
            levels[i].compare_trees(other.levels[i]);
        }
    }

    void compare_emitted(size_t num)
    {
        for(size_t i = 1; i < num; ++i)
        {
            levels[i].compare_emitted(levels[i-1]);
        }
    }
    void compare_emitted(size_t num, Approach const& other)
    {
        for(size_t i = 0; i < num; ++i)
        {
            levels[i].compare_emitted(other.levels[i]);
        }
    }
};


struct Subject
{
    Approach ro;
    Approach ro_reuse;
    Approach rw;
    Approach rw_reuse;

    void init(c4::csubstr filename, c4::csubstr src, bool is_yaml_event=false)
    {
        ro      .init(filename, src, /*immutable*/true , /*reuse*/false, is_yaml_event);
        ro_reuse.init(filename, src, /*immutable*/true , /*reuse*/true , is_yaml_event);
        rw      .init(filename, src, /*immutable*/false, /*reuse*/false, is_yaml_event);
        rw_reuse.init(filename, src, /*immutable*/false, /*reuse*/true , is_yaml_event);
    }
};


c4::csubstr replace_all(c4::csubstr pattern, c4::csubstr repl, c4::csubstr subject, std::string *dst)
{
    dst->clear();
    size_t b = 0;
    do {
        size_t e = subject.find(pattern, b);
        if(e == npos)
        {
            dst->append(&subject[b], subject.end());
            break;
        }
        dst->append(&subject[b], &subject[e]);
        dst->append(repl.begin(), repl.end());
        b = e + pattern.size();
    } while(b != npos);

    return c4::to_csubstr(*dst);
}


size_t find_first_after(size_t pos, std::initializer_list<size_t> candidates)
{
    size_t ret = npos;
    for(size_t s : candidates)
    {
        if(s > pos && s < ret) ret = s;
    }
    return ret;
}


struct SuiteCase
{
    c4::csubstr filename;
    std::string file_contents;

    c4::csubstr desc;
    c4::csubstr from;
    c4::csubstr tags;

    std::string in_yaml_filtered;
    Subject     in_yaml;
    Subject     in_json;
    Subject     out_yaml;
    Subject     events;

    static c4::csubstr src(Subject const& s)
    {
        return c4::to_csubstr(s.ro.levels[0].src);
    }

    bool load(const char* filename_)
    {
        filename = c4::to_csubstr(filename_);

        // read the file
        c4::fs::file_get_contents(filename_, &file_contents);
        c4::csubstr contents = c4::to_csubstr(file_contents);

        // now parse the file
        c4::csubstr ws = " \t\r\n";
        c4::csubstr txt;
        size_t b, e;

        // desc
        C4_CHECK(contents.begins_with("=== "));
        e = contents.find("--- from: ", 4);
        C4_CHECK(e != npos);
        desc = contents.range(4, e).trimr(ws);

        // from
        b = e + 4;
        e = contents.find("--- tags: ", b);
        C4_CHECK(e != npos);
        from = contents.range(b, e);
        C4_CHECK(from.begins_with("from: "));
        C4_CHECK(from.size() >= 6);
        from = from.sub(6).trimr(ws);

        // tags
        b = e + 4;
        e = contents.find("--- in-yaml", b);
        C4_CHECK(e != npos);
        tags = contents.range(b, e);
        C4_CHECK(tags.begins_with("tags: "));
        C4_CHECK(tags.size() >= 6);
        tags = tags.sub(6).trimr(ws);

        if(tags.find("error") != npos) return false; // MARKED WITH ERROR. SKIP THE REST!

        size_t end_tags = e;
        size_t begin_in_yaml  = contents.find("--- in-yaml"   , end_tags);
        size_t begin_in_json  = contents.find("--- in-json"   , end_tags);
        size_t begin_out_yaml = contents.find("--- out-yaml"  , end_tags);
        size_t begin_events   = contents.find("--- test-event", end_tags);
        std::initializer_list<size_t> all = {begin_in_yaml, begin_in_json, begin_out_yaml, begin_events, contents.size()};

        // in_yaml
        C4_CHECK(begin_in_yaml != npos);
        size_t first_after_in_yaml = find_first_after(begin_in_yaml, all);
        begin_in_yaml = 1 + contents.find('\n', begin_in_yaml); // skip this line
        txt = contents.range(begin_in_yaml, first_after_in_yaml).trimr(ws);
        C4_ASSERT( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "---test-event"));
        in_yaml.init(filename, txt);

        // in_json
        if(begin_in_json != npos)
        {
            size_t first_after_in_json = find_first_after(begin_in_json, all);
            begin_in_json = 1 + contents.find('\n', begin_in_json); // skip this line
            txt = contents.range(begin_in_json, first_after_in_json).trimr(ws);
            C4_ASSERT( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "---test-event"));
            in_json.init(filename, txt);
        }

        // out_yaml
        if(begin_out_yaml != npos)
        {
            if(begin_in_json == npos) begin_in_json = begin_in_yaml;
            size_t first_after_out_yaml = find_first_after(begin_out_yaml, all);
            begin_out_yaml = 1 + contents.find('\n', begin_out_yaml); // skip this line
            txt = contents.range(begin_out_yaml, first_after_out_yaml).trimr(ws);
            C4_ASSERT( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "---test-event"));
            out_yaml.init(filename, txt);
        }

        // events
        C4_CHECK(begin_events != npos);
        size_t first_after_events = find_first_after(begin_events, all);
        begin_events = 1 + contents.find('\n', begin_events); // skip this line
        c4::csubstr src_events = contents.sub(begin_events).trimr(ws);
        C4_ASSERT( ! src_events.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "---test-event"));
        events.init(filename, src_events);


        // filter
        if(tags.find("whitespace") != npos)
        {
            c4::csubstr filtered = replace_all("<SPC>", " ", src(in_yaml), &in_yaml_filtered);
            in_yaml.init(filename, filtered);
        }

        return true;
    }

    void print() const
    {
        c4::dump("% file   : "  , filename     , " %\n",
                 "% desc   : "  , desc         , " %\n",
                 "% from   : "  , from         , " %\n",
                 "% tags   : "  , tags         , " %\n",
                 "% in_yaml:\n" , src(in_yaml ), " %\n",
                 "% in_json:\n" , src(in_json ), " %\n",
                 "% out_yaml:\n", src(out_yaml), " %\n",
                 "% events :\n" , src(events  ), " %\n");
    }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

SuiteCase g_suite_case;


#define DECLARE_TEST_CLASS(cls, pfx) \
\
class cls##_##pfx : public ::testing::TestWithParam<size_t> {};\
\
TEST_P(cls##_##pfx, parse)\
{\
    C4_ASSERT(GetParam() < NLEVELS);\
    g_suite_case.cls.pfx.parse(1 + GetParam(), false);\
}\
TEST_P(cls##_##pfx, compare_trees)\
{\
    C4_ASSERT(GetParam() < NLEVELS);\
    g_suite_case.cls.pfx.compare_trees(1 + GetParam());\
}\
TEST_P(cls##_##pfx, emit)\
{\
    C4_ASSERT(GetParam() < NLEVELS);\
    g_suite_case.cls.pfx.parse(1 + GetParam(), true);\
}\
TEST_P(cls##_##pfx, compare_emitted)\
{\
    C4_ASSERT(GetParam() < NLEVELS);\
    g_suite_case.cls.pfx.compare_emitted(1 + GetParam());\
}\
/**/\
/**/\
/**/\


#define DECLARE_TESTS(cls) \
\
DECLARE_TEST_CLASS(cls, ro)\
DECLARE_TEST_CLASS(cls, rw)\
DECLARE_TEST_CLASS(cls, ro_reuse)\
DECLARE_TEST_CLASS(cls, rw_reuse)\
\
INSTANTIATE_TEST_SUITE_P(_, cls##_ro      , testing::Range<size_t>(0, NLEVELS));\
INSTANTIATE_TEST_SUITE_P(_, cls##_rw      , testing::Range<size_t>(0, NLEVELS));\
INSTANTIATE_TEST_SUITE_P(_, cls##_ro_reuse, testing::Range<size_t>(0, NLEVELS));\
INSTANTIATE_TEST_SUITE_P(_, cls##_rw_reuse, testing::Range<size_t>(0, NLEVELS));


DECLARE_TESTS(out_yaml);
//DECLARE_TESTS(events);
//DECLARE_TESTS(in_json);
DECLARE_TESTS(in_yaml);



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    if(argc > 1)
    {
        const char *path = argv[1];
        if(strlen(path) < 2) return 1;
        if(path[0] != '-')
        {
            C4_CHECK(c4::fs::path_exists(argv[1]));
            auto fn = c4::to_csubstr(argv[1]);
            c4::log("testing suite case: {} ({})", fn.basename(), fn);
            if( ! g_suite_case.load(argv[1]))
            {
                return 0;
            }
            c4::print(g_suite_case.file_contents);
            g_suite_case.print();
        }
    }

    int status = RUN_ALL_TESTS();

    if(g_suite_case.filename.not_empty())
    {
        c4::log("\nTESTS {}: {} ({})\n",
            status == 0 ? "SUCCEEDED" : "FAILED",
            g_suite_case.filename.basename(),
            g_suite_case.filename);
    }

    return status;
}
