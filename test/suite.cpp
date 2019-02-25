#include <c4/substr.hpp>
#include <c4/std/string.hpp>
#include <c4/fs/fs.hpp>
#include <c4/log/log.hpp>

#include <c4/yml/tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>

#include <gtest/gtest.h>


enum : size_t { npos = c4::csubstr::npos };

struct Parsed
{
    bool was_parsed, was_emitted;
    c4::yml::Tree tree;
    std::string emitted;
};

struct Parsable
{
    c4::csubstr ro; // read-only  (copied to tree's arena before parsing)
    std::string rw; // read/write (parsed in situ)
    Parsed      ro_parsed;
    Parsed      rw_parsed;

    Parsable() { init(""); }

    void init(c4::csubstr contents)
    {
        ro = contents;
        rw.assign(ro.begin(), ro.end());
        ro_parsed.was_parsed = false;
        rw_parsed.was_parsed = false;
        ro_parsed.was_emitted = false;
        rw_parsed.was_emitted = false;
    }

    bool empty() const
    {
        return ro.empty();
    }

    void parse_ro()
    {
        if(empty()) return;
        c4::yml::parse(ro, &ro_parsed.tree);
        ro_parsed.was_parsed = true;
    }

    void parse_rw()
    {
        if(rw.empty()) return;
        c4::yml::parse(to_substr(rw), &rw_parsed.tree);
        rw_parsed.was_parsed = true;
    }

    void emit_ro()
    {
        if(empty()) return;
        if(!ro_parsed.was_parsed) parse_ro();
        c4::yml::emit_resize(ro_parsed.tree, &ro_parsed.emitted);
        ro_parsed.was_emitted = true;
    }

    void emit_rw()
    {
        if(rw.empty()) return;
        if(!rw_parsed.was_parsed) parse_rw();
        c4::yml::emit_resize(rw_parsed.tree, &rw_parsed.emitted);
        rw_parsed.was_emitted = true;
    }

    void compare_src()
    {
        if(empty()) return;
        EXPECT_EQ(std::string(ro.begin(), ro.end()), rw);
    }
    void compare_emitted()
    {
        if(empty()) return;
        if(!ro_parsed.was_parsed) parse_ro();
        if(!rw_parsed.was_parsed) parse_rw();
        if(!ro_parsed.was_emitted) emit_ro();
        if(!rw_parsed.was_emitted) emit_rw();
        EXPECT_EQ(ro_parsed.emitted, rw_parsed.emitted);
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

    return to_csubstr(*dst);
}


struct SuiteCase
{
    c4::csubstr filename;
    std::string file_contents;

    c4::csubstr desc;
    c4::csubstr from;
    c4::csubstr tags;

    std::string in_yaml_filtered;
    Parsable    in_yaml;
    Parsable    in_json;
    Parsable    out_yaml;

    c4::csubstr events;

    bool load(const char* filename_)
    {
        filename = c4::to_csubstr(filename_);

        // read the file
        c4::fs::file_get_contents(filename_, &file_contents);
        c4::csubstr contents = to_csubstr(file_contents);

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
        size_t first_after_yaml = std::min(begin_in_json, std::min(begin_out_yaml, begin_events));
        size_t first_after_json = std::min(begin_out_yaml, begin_events);

        // in_yaml
        C4_CHECK(begin_in_yaml != npos);
        begin_in_yaml = 1 + contents.find('\n', begin_in_yaml); // skip this line
        txt = contents.range(begin_in_yaml, first_after_yaml).trimr(ws);
        C4_ASSERT( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "---test-event"));
        in_yaml.init(txt);

        // in_json
        if(begin_in_json != npos)
        {
            begin_in_json = 1 + contents.find('\n', begin_in_json); // skip this line
            txt = contents.range(begin_in_json, first_after_json).trimr(ws);
            C4_ASSERT( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "---test-event"));
            in_json.init(txt);
        }

        // out_yaml
        if(begin_out_yaml != npos)
        {
            if(begin_in_json == npos) begin_in_json = begin_in_yaml;
            begin_out_yaml = 1 + contents.find('\n', begin_out_yaml); // skip this line
            txt = contents.range(begin_out_yaml, begin_events).trimr(ws);
            C4_ASSERT( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "---test-event"));
            out_yaml.init(txt);
        }

        // events
        C4_CHECK(begin_events != npos);
        begin_events = 1 + contents.find('\n', begin_events); // skip this line
        events = contents.sub(begin_events).trimr(ws);
        C4_ASSERT( ! events.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "---test-event"));

        // filter
        if(tags.find("whitespace") != npos)
        {
            c4::csubstr filtered = replace_all("<SPC>", " ", in_yaml.ro, &in_yaml_filtered);
            in_yaml.init(filtered);
        }

        return true;
    }

    void print() const
    {
        c4::dump("% desc   : "  , desc       , " %\n",
                 "% from   : "  , from       , " %\n",
                 "% tags   : "  , tags       , " %\n",
                 "% in_yaml:\n" , in_yaml.ro , " %\n",
                 "% in_json:\n" , in_json.ro , " %\n",
                 "% out_yaml:\n", out_yaml.ro, " %\n",
                 "% events :\n" , events     , " %\n");
    }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

SuiteCase g_suite_case;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(in_json_rw, parse)
{
    g_suite_case.in_json.parse_rw();
}
TEST(in_json_rw, emit)
{
    g_suite_case.in_json.emit_rw();
}

TEST(in_json_ro, parse)
{
    g_suite_case.in_json.parse_ro();
}
TEST(in_json_ro, emit)
{
    g_suite_case.in_json.emit_ro();
}


//-----------------------------------------------------------------------------
/*

TEST(out_yaml_rw, parse)
{
    g_suite_case.out_yaml.parse_rw();
}
TEST(out_yaml_rw, emit)
{
    g_suite_case.out_yaml.emit_rw();
}

TEST(out_yaml_ro, parse)
{
    g_suite_case.out_yaml.parse_ro();
}
TEST(out_yaml_ro, emit)
{
    g_suite_case.out_yaml.emit_ro();
}

TEST(out_yaml__ro_vs_rw, compare_src)
{
    g_suite_case.out_yaml.compare_src();
}

TEST(out_yaml__ro_vs_rw, compare_emitted)
{
    g_suite_case.out_yaml.compare_emitted();
}


//-----------------------------------------------------------------------------
TEST(in_yaml_rw, parse)
{
    g_suite_case.in_yaml.parse_rw();
}
TEST(in_yaml_rw, emit)
{
    g_suite_case.in_yaml.emit_rw();
}

TEST(in_yaml_ro, parse)
{
    g_suite_case.in_yaml.parse_ro();
}
TEST(in_yaml_ro, emit)
{
    g_suite_case.in_yaml.emit_ro();
}
*/


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
            if( ! g_suite_case.load(argv[1]))
            {
                return 0;
            }
            c4::print(g_suite_case.file_contents);
            g_suite_case.print();
        }
    }

    return RUN_ALL_TESTS();
}
